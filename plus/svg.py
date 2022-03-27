
import numpy as np


class Buffer:
    def __init__(self, offset=0.2):
        self.offset = offset
        self.data = []
        self.height = -1
        self.width = -1

    def segment(self, type, origin, forward, right):

        # Type 0 is flat
        if type == 0:
            a = origin + forward
            self.data.append("L {} {}".format(*a))
            return

        # Odd types are female
        if type % 2 == 0:
            right = -right

        # Choose scale
        index = (type - 1) // 2
        unit = [0.35, 0.25][index]
        offset = -self.offset * unit

        # Control points
        k = np.sqrt(2)
        a = origin + forward * (0.5 - 0.4 * unit) + right * offset
        b = origin + forward * (0.5 - 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        c = origin + forward * (0.5 + 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        d = origin + forward * (0.5 + 0.4 * unit) + right * offset
        e = origin + forward

        # Radii
        r1 = 0.2 * unit
        r2 = 0.4 * unit

        # Which side to choose
        cross = forward[0] * right[1] - forward[1] * right[0]
        is_system_mirrored = cross < 0
        if is_system_mirrored:
            sweep = 0
        else:
            sweep = 1

        # Slope to hook
        if offset == 0:
            self.data.append("L {} {}".format(*a))
        else:
            q1 = origin + forward * (0.5 - 0.4 * unit) / 2
            q2 = q1 + right * offset
            self.data.append("C {} {}, {} {}, {} {}".format(*q1, *q2, *a))

        # Draw hook
        self.data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *b))
        self.data.append("A {} {} 0 1 {} {} {}".format(r2, r2, 1 - sweep, *c))
        self.data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *d))

        # Slope back to corner
        if offset == 0:
            self.data.append("L {} {}".format(*e))
        else:
            q1 = origin + forward * (1.5 + 0.4 * unit) / 2
            q2 = q1 + right * offset
            self.data.append("C {} {}, {} {}, {} {}".format(*q2, *q1, *e))

    def trace(self, types, origin, forward, right, margin):
        # Note: type belongs to the piece on the right!
        if margin > 0:
            self.data.append("M {} {}".format(*(origin - forward * margin)))
            self.data.append("L {} {}".format(*origin))
        else:
            self.data.append("M {} {}".format(*origin))
        for i, t in enumerate(types):
            self.segment(t, origin + i * forward, forward, right)
        if margin > 0:
            self.data.append("L {} {}".format(*(origin + forward * (len(types) + margin))))

    def mirror_types(self, types):
        result = []
        for t in types:
            if t > 0:
                if t % 2 == 1:
                    t += 1
                else:
                    t -= 1
            result.append(t)
        return result

    def jigsaw(self, matrix, margin=0.0):
        ex = np.array([1.0, 0.0])
        ey = np.array([0.0, 1.0])
        height, width, _ = matrix.shape
        self.height = height
        self.width = width

        # Draw horizontal lines
        for y in range(height):
            self.trace(matrix[y, :, 1], (0, y), ex, ey, margin)
        self.trace(self.mirror_types(matrix[-1, :, 3]), (0, height), ex, ey, margin)

        # Draw vertical lines
        for x in range(width):
            self.trace(matrix[:, x, 2], (x, 0), ey, ex, margin)
        self.trace(self.mirror_types(matrix[:, -1, 0]), (width, 0), ey, ex, margin)

    def save(self, file):
        data = " ".join(self.data)
        path = f'<path d="{data}"/>'
        scale = 64
        transform = f"scale({scale}) translate(0.5 0.5)"
        group = f'<g fill="none" stroke="red" stroke-width="{1 / scale}" transform="{transform}">{path}</g>'
        height = (self.height + 1) * scale
        width = (self.width + 1) * scale
        content = f'<svg width="{width}" height="{height}" xmlns="http://www.w3.org/2000/svg">{group}</svg>'
        file.write(content)


matrix = np.array([[[0, 0, 0, 4], [3, 0, 0, 4], [0, 0, 4, 3]], [[3, 3, 0, 3], [3, 3, 4, 4], [0, 4, 4, 4]], [[4, 4, 0, 0], [4, 3, 3, 0], [0, 3, 3, 0]]])

buffer = Buffer()
buffer.jigsaw(matrix)

with open("foo.svg", "w") as file:
    buffer.save(file)
