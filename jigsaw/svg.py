
import numpy as np


class Style:

    def trace_edge(self, data, edge, origin, forward, right):
        raise NotImplementedError("trace_edge")

    def trace_edges(self, data, edges, origin, forward, right, *, margin=0.0):

        # Note: by design, `edges` belongs to the piece on the left

        # Move to the beginning of the line
        if margin > 0.0:
            data.append("M {} {}".format(*(origin - forward * margin)))
            data.append("L {} {}".format(*origin))
        else:
            data.append("M {} {}".format(*origin))

        # Render edges, one-by-one
        for k, edge in enumerate(edges):
            self.trace_edge(data, edge, origin + k * forward, forward, right)

        # Append some margin, if requested
        if margin > 0.0:
            data.append("L {} {}".format(*(origin + forward * (len(edges) + margin))))

    def trace(self, data, horizontal_edges, vertical_edges, origin, ex, ey, *, margin=0.0):

        H, _ = horizontal_edges.shape
        _, W = vertical_edges.shape

        assert horizontal_edges.shape == (H, W + 1)
        assert vertical_edges.shape == (H + 1, W)

        # Note: origin is usually defined as the upper-left corner, with X-axis going right and Y-axis going down

        # Draw edges between left and right pieces
        for j in range(W + 1):
            self.trace_edges(data, horizontal_edges[:, j], origin + ex * j, ey, -ex, margin=margin)

        # Draw edges between top and bottom pieces
        for i in range(H + 1):
            self.trace_edges(data, vertical_edges[i, :], origin + ey * i, ex, ey, margin=margin)


class CircleStyle(Style):

    def trace_flat(self, data, origin, forward, right):

        # Control points
        a = origin + forward

        # Single line to target
        data.append("L {} {}".format(*a))

    def trace_single(self, data, origin, forward, right, *, unit=0.25, offset=0.0, shift=0.0):

        # Control points
        k = np.sqrt(2)
        a = origin + forward * (0.5 + shift - 0.4 * unit) + right * offset
        b = origin + forward * (0.5 + shift - 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        c = origin + forward * (0.5 + shift + 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        d = origin + forward * (0.5 + shift + 0.4 * unit) + right * offset
        e = origin + forward

        # Radii
        # TODO handle when forward and right have different lengths
        r = np.sqrt((forward ** 2).sum())
        r1 = 0.2 * unit * r
        r2 = 0.4 * unit * r

        # Which side to choose
        cross = forward[0] * right[1] - forward[1] * right[0]
        is_system_mirrored = cross < 0
        if is_system_mirrored:
            sweep = 0
        else:
            sweep = 1

        # Slope to hook
        if offset == 0:
            data.append("L {} {}".format(*a))
        else:
            q1 = origin + forward * (0.25 + shift * 0.5 - 0.2 * unit)
            q2 = q1 + right * offset
            data.append("C {} {}, {} {}, {} {}".format(*q1, *q2, *a))

        # Draw hook
        data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *b))
        data.append("A {} {} 0 1 {} {} {}".format(r2, r2, 1 - sweep, *c))
        data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *d))

        # Slope back to corner
        if offset == 0:
            data.append("L {} {}".format(*e))
        else:
            q1 = origin + forward * (0.75 + shift * 0.5 + 0.2 * unit)
            q2 = q1 + right * offset
            data.append("C {} {}, {} {}, {} {}".format(*q2, *q1, *e))

    def trace_double(self, data, origin, forward, right, *, unit=0.25, bias=0.2, offset=0.0, twisted=False):

        # Control points
        k = np.sqrt(2)
        twisted_right = -right if twisted else right
        a1 = origin + forward * (0.5 - bias - 0.4 * unit) + twisted_right * offset
        b1 = origin + forward * (0.5 - bias - 0.4 / k * unit) + twisted_right * (0.6 * unit - 0.4 / k * unit + offset)
        c1 = origin + forward * (0.5 - bias + 0.4 / k * unit) + twisted_right * (0.6 * unit - 0.4 / k * unit + offset)
        d1 = origin + forward * (0.5 - bias + 0.4 * unit) + twisted_right * offset
        a2 = origin + forward * (0.5 + bias - 0.4 * unit) + right * offset
        b2 = origin + forward * (0.5 + bias - 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        c2 = origin + forward * (0.5 + bias + 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        d2 = origin + forward * (0.5 + bias + 0.4 * unit) + right * offset
        e = origin + forward

        # Radii
        # TODO handle when forward and right have different lengths
        r = np.sqrt((forward ** 2).sum())
        r1 = 0.2 * unit * r
        r2 = 0.4 * unit * r

        # Which side to choose
        cross = forward[0] * right[1] - forward[1] * right[0]
        is_system_mirrored = cross < 0
        if is_system_mirrored:
            sweep = 0
        else:
            sweep = 1
        twisted_sweep = 1 - sweep if twisted else sweep

        # Slope to hook
        if offset == 0:
            data.append("L {} {}".format(*a1))
        else:
            q1 = origin + forward * (0.25 - bias * 0.5 - 0.2 * unit)
            q2 = q1 + twisted_right * offset
            data.append("C {} {}, {} {}, {} {}".format(*q1, *q2, *a1))

        # Draw first hook
        data.append("A {} {} 0 0 {} {} {}".format(r1, r1, twisted_sweep, *b1))
        data.append("A {} {} 0 1 {} {} {}".format(r2, r2, 1 - twisted_sweep, *c1))
        data.append("A {} {} 0 0 {} {} {}".format(r1, r1, twisted_sweep, *d1))

        # Slope to second hook
        if twisted and offset != 0:
            q1 = origin + forward * 0.5 + twisted_right * offset
            q2 = origin + forward * 0.5 + right * offset
            data.append("C {} {}, {} {}, {} {}".format(*q1, *q2, *a2))
        else:
            data.append("L {} {}".format(*a2))

        # Draw second hook
        data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *b2))
        data.append("A {} {} 0 1 {} {} {}".format(r2, r2, 1 - sweep, *c2))
        data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *d2))

        # Slope back to corner
        if offset == 0:
            data.append("L {} {}".format(*e))
        else:
            q1 = origin + forward * (0.75 + bias * 0.5 + 0.2 * unit)
            q2 = q1 + right * offset
            data.append("C {} {}, {} {}, {} {}".format(*q2, *q1, *e))

    def trace_edge(self, data, edge, origin, forward, right):

        # Flat
        if edge == 0:
            self.trace_flat(data, origin, forward, right)
            return

        # All others comes in both male and female flavors
        if edge % 2 == 0:
            right = -right
        index = (edge - 1) // 2
        unit = [0.45, 0.35, 0.25][index % 3]
        offset = -0.2 * unit

        # Centered
        if 0 <= index < 3:
            self.trace_single(data, origin, forward, right, unit=unit, offset=offset)
            return

        # Right
        if 3 <= index < 6:
            self.trace_single(data, origin, forward, right, unit=unit, offset=offset, shift=-0.15)
            return

        # Left
        if 6 <= index < 9:
            self.trace_single(data, origin, forward, right, unit=unit, offset=offset, shift=0.15)
            return

        # Double
        # Note: medium and large are typically too big...
        if 9 <= index < 12:
            self.trace_double(data, origin, forward, right, unit=unit, bias=unit * 0.6, offset=offset)
            return

        # Twisted
        # Note: medium and large are typically too big...
        if 12 <= index < 15:
            self.trace_double(data, origin, forward, right, unit=unit, bias=unit * 0.6, offset=offset, twisted=True)
            return

        raise KeyError(edge)


# TODO capsule style


def get_style(style=None):
    if isinstance(style, Style):
        return style
    if style is None:
        style = "circle"
    if style == "circle":
        return CircleStyle()
    raise KeyError(style)


def grid_to_svg(horizontal_edges, vertical_edges, *, style=None, scale=64.0, margin=0.25):

    H, _ = horizontal_edges.shape
    _, W = vertical_edges.shape
    assert horizontal_edges.shape == (H, W + 1)
    assert vertical_edges.shape == (H + 1, W)

    # Use simple coordinate system
    origin = np.array([0.0, 0.0])
    ex = np.array([scale, 0.0])
    ey = np.array([0.0, scale])

    # Use style to generate path
    style = get_style(style)
    data = []
    style.trace(data, horizontal_edges, vertical_edges, origin, ex, ey, margin=margin)
    data = " ".join(data)

    # Wrap as SVG code
    path = f'<path fill="none" stroke="black" d="{data}"/>'
    # TODO clean the whole margin&viewport handling
    height = int(scale * (H + 1))
    width = int(scale * (W + 1))
    group = f'<g transform="translate({scale*0.5} {scale*0.5})">{path}</g>'
    code = f'<svg width="{width}" height="{height}" xmlns="http://www.w3.org/2000/svg">{group}</svg>'
    return code


# TODO pieces_to_svg


def display_grid(horizontal_edges, vertical_edges, *, style=None, scale=64.0, margin=0.25):
    from IPython.core.display import HTML, display
    svg = grid_to_svg(horizontal_edges, vertical_edges, style=style, scale=scale, margin=margin)
    widget = HTML(f'<div style="padding:1em">{svg}<div>')
    display(widget)


# TODO display_pieces
