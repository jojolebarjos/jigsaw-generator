from enum import IntEnum

import numpy as np


class Type(IntEnum):
    TYPE_FLAT = 0
    TYPE_CENTERED_BIG_MALE = 1
    TYPE_CENTERED_BIG_FEMALE = 2
    TYPE_CENTERED_MEDIUM_MALE = 3
    TYPE_CENTERED_MEDIUM_FEMALE = 4
    TYPE_CENTERED_SMALL_MALE = 5
    TYPE_CENTERED_SMALL_FEMALE = 6
    TYPE_RIGHT_BIG_MALE = 7
    TYPE_RIGHT_BIG_FEMALE = 8
    TYPE_RIGHT_MEDIUM_MALE = 9
    TYPE_RIGHT_MEDIUM_FEMALE = 10
    TYPE_RIGHT_SMALL_MALE = 11
    TYPE_RIGHT_SMALL_FEMALE = 12
    TYPE_LEFT_BIG_MALE = 13
    TYPE_LEFT_BIG_FEMALE = 14
    TYPE_LEFT_MEDIUM_MALE = 15
    TYPE_LEFT_MEDIUM_FEMALE = 16
    TYPE_LEFT_SMALL_MALE = 17
    TYPE_LEFT_SMALL_FEMALE = 18
    TYPE_DOUBLE_BIG_MALE = 19
    TYPE_DOUBLE_BIG_FEMALE = 20
    TYPE_DOUBLE_MEDIUM_MALE = 21
    TYPE_DOUBLE_MEDIUM_FEMALE = 22
    TYPE_DOUBLE_SMALL_MALE = 23
    TYPE_DOUBLE_SMALL_FEMALE = 24
    TYPE_TWISTED_BIG_MALE = 25
    TYPE_TWISTED_BIG_FEMALE = 26
    TYPE_TWISTED_MEDIUM_MALE = 27
    TYPE_TWISTED_MEDIUM_FEMALE = 28
    TYPE_TWISTED_SMALL_MALE = 29
    TYPE_TWISTED_SMALL_FEMALE = 30


MIRRORED_TYPES = {

    # Centered
    Type.TYPE_CENTERED_BIG_MALE: Type.TYPE_CENTERED_BIG_FEMALE,
    Type.TYPE_CENTERED_BIG_FEMALE: Type.TYPE_CENTERED_BIG_MALE,
    Type.TYPE_CENTERED_MEDIUM_MALE: Type.TYPE_CENTERED_MEDIUM_FEMALE,
    Type.TYPE_CENTERED_MEDIUM_FEMALE: Type.TYPE_CENTERED_MEDIUM_MALE,
    Type.TYPE_CENTERED_SMALL_MALE: Type.TYPE_CENTERED_SMALL_FEMALE,
    Type.TYPE_CENTERED_SMALL_FEMALE: Type.TYPE_CENTERED_SMALL_MALE,

    # Right-aligned
    Type.TYPE_RIGHT_BIG_MALE: Type.TYPE_LEFT_BIG_FEMALE,
    Type.TYPE_RIGHT_BIG_FEMALE: Type.TYPE_LEFT_BIG_MALE,
    Type.TYPE_RIGHT_MEDIUM_MALE: Type.TYPE_LEFT_MEDIUM_FEMALE,
    Type.TYPE_RIGHT_MEDIUM_FEMALE: Type.TYPE_LEFT_MEDIUM_MALE,
    Type.TYPE_RIGHT_SMALL_MALE: Type.TYPE_LEFT_SMALL_FEMALE,
    Type.TYPE_RIGHT_SMALL_FEMALE: Type.TYPE_LEFT_SMALL_MALE,

    # Left-aligned
    Type.TYPE_LEFT_BIG_MALE: Type.TYPE_RIGHT_BIG_FEMALE,
    Type.TYPE_LEFT_BIG_FEMALE: Type.TYPE_RIGHT_BIG_MALE,
    Type.TYPE_LEFT_MEDIUM_MALE: Type.TYPE_RIGHT_MEDIUM_FEMALE,
    Type.TYPE_LEFT_MEDIUM_FEMALE: Type.TYPE_RIGHT_MEDIUM_MALE,
    Type.TYPE_LEFT_SMALL_MALE: Type.TYPE_RIGHT_SMALL_FEMALE,
    Type.TYPE_LEFT_SMALL_FEMALE: Type.TYPE_RIGHT_SMALL_MALE,

    # Doubled (symmetric)
    Type.TYPE_DOUBLE_BIG_MALE: Type.TYPE_DOUBLE_BIG_FEMALE,
    Type.TYPE_DOUBLE_BIG_FEMALE: Type.TYPE_DOUBLE_BIG_MALE,
    Type.TYPE_DOUBLE_MEDIUM_MALE: Type.TYPE_DOUBLE_MEDIUM_FEMALE,
    Type.TYPE_DOUBLE_MEDIUM_FEMALE: Type.TYPE_DOUBLE_MEDIUM_MALE,
    Type.TYPE_DOUBLE_SMALL_MALE: Type.TYPE_DOUBLE_SMALL_FEMALE,
    Type.TYPE_DOUBLE_SMALL_FEMALE: Type.TYPE_DOUBLE_SMALL_MALE,

    # Twisted (i.e. doubled, but with gender-swap)
    Type.TYPE_TWISTED_BIG_MALE: Type.TYPE_TWISTED_BIG_MALE,
    Type.TYPE_TWISTED_BIG_FEMALE: Type.TYPE_TWISTED_BIG_FEMALE,
    Type.TYPE_TWISTED_MEDIUM_MALE: Type.TYPE_TWISTED_MEDIUM_MALE,
    Type.TYPE_TWISTED_MEDIUM_FEMALE: Type.TYPE_TWISTED_MEDIUM_FEMALE,
    Type.TYPE_TWISTED_SMALL_MALE: Type.TYPE_TWISTED_SMALL_MALE,
    Type.TYPE_TWISTED_SMALL_FEMALE: Type.TYPE_TWISTED_SMALL_FEMALE,
}


class Buffer:
    def __init__(self, style="original"):
        self.style = style
        self.data = []
        self.height = -1
        self.width = -1

    def flat_segment(self, origin, forward):
        a = origin + forward
        self.data.append("L {} {}".format(*a))

    def circle_hook_segment(self, origin, forward, right, unit, offset=0.0, shift=0.0):

        # Control points
        k = np.sqrt(2)
        a = origin + forward * (0.5 + shift - 0.4 * unit) + right * offset
        b = origin + forward * (0.5 + shift - 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        c = origin + forward * (0.5 + shift + 0.4 / k * unit) + right * (0.6 * unit - 0.4 / k * unit + offset)
        d = origin + forward * (0.5 + shift + 0.4 * unit) + right * offset
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
            q1 = origin + forward * (0.25 + shift * 0.5 - 0.2 * unit)
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
            q1 = origin + forward * (0.75 + shift * 0.5 + 0.2 * unit)
            q2 = q1 + right * offset
            self.data.append("C {} {}, {} {}, {} {}".format(*q2, *q1, *e))

    def double_circle_hook_segment(self, origin, forward, right, unit, bias, offset=0.0, twisted=False):

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
        r1 = 0.2 * unit
        r2 = 0.4 * unit

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
            self.data.append("L {} {}".format(*a1))
        else:
            q1 = origin + forward * (0.25 - bias * 0.5 - 0.2 * unit)
            q2 = q1 + twisted_right * offset
            self.data.append("C {} {}, {} {}, {} {}".format(*q1, *q2, *a1))

        # Draw first hook
        self.data.append("A {} {} 0 0 {} {} {}".format(r1, r1, twisted_sweep, *b1))
        self.data.append("A {} {} 0 1 {} {} {}".format(r2, r2, 1 - twisted_sweep, *c1))
        self.data.append("A {} {} 0 0 {} {} {}".format(r1, r1, twisted_sweep, *d1))

        # Slope to second hook
        if twisted and offset != 0:
            q1 = origin + forward * 0.5 + twisted_right * offset
            q2 = origin + forward * 0.5 + right * offset
            self.data.append("C {} {}, {} {}, {} {}".format(*q1, *q2, *a2))
        else:
            self.data.append("L {} {}".format(*a2))

        # Draw second hook
        self.data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *b2))
        self.data.append("A {} {} 0 1 {} {} {}".format(r2, r2, 1 - sweep, *c2))
        self.data.append("A {} {} 0 0 {} {} {}".format(r1, r1, sweep, *d2))

        # Slope back to corner
        if offset == 0:
            self.data.append("L {} {}".format(*e))
        else:
            q1 = origin + forward * (0.75 + bias * 0.5 + 0.2 * unit)
            q2 = q1 + right * offset
            self.data.append("C {} {}, {} {}, {} {}".format(*q2, *q1, *e))

    def capsule_hook_segment(self, origin, forward, right, inner_radius, outer_radius, length, shift=0.0):

        # Control points
        a = origin + forward * (0.5 - 0.5 * length + shift)
        b = a + right * (2.0 * inner_radius)
        c = a + right * (2.0 * (inner_radius + outer_radius))
        f = origin + forward * (0.5 + 0.5 * length + shift)
        e = f + right * (2.0 * inner_radius)
        d = f + right * (2.0 * (inner_radius + outer_radius))
        g = origin + forward

        # Which side to choose
        cross = forward[0] * right[1] - forward[1] * right[0]
        is_system_mirrored = cross < 0
        if is_system_mirrored:
            sweep = 0
        else:
            sweep = 1

        # Draw capsule
        self.data.append("L {} {}".format(*a))
        self.data.append("A {} {} 0 0 {} {} {}".format(inner_radius, inner_radius, sweep, *b))
        self.data.append("A {} {} 0 0 {} {} {}".format(outer_radius, outer_radius, 1 - sweep, *c))
        self.data.append("L {} {}".format(*d))
        self.data.append("A {} {} 0 0 {} {} {}".format(outer_radius, outer_radius, 1 - sweep, *e))
        self.data.append("A {} {} 0 0 {} {} {}".format(inner_radius, inner_radius, sweep, *f))
        self.data.append("L {} {}".format(*g))

    def double_capsule_hook_segment(self, origin, forward, right, inner_radius, outer_radius, length, bias, twisted=False):

        # Control points
        twisted_right = -right if twisted else right
        a = origin + forward * (0.5 - bias - 0.5 * length)
        b = a + twisted_right * (2.0 * inner_radius)
        c = a + twisted_right * (2.0 * (inner_radius + outer_radius))
        f = origin + forward * (0.5 - bias + 0.5 * length)
        e = f + twisted_right * (2.0 * inner_radius)
        d = f + twisted_right * (2.0 * (inner_radius + outer_radius))
        g = origin + forward * (0.5 + bias - 0.5 * length)
        h = g + right * (2.0 * inner_radius)
        i = g + right * (2.0 * (inner_radius + outer_radius))
        l = origin + forward * (0.5 + bias + 0.5 * length)
        k = l + right * (2.0 * inner_radius)
        j = l + right * (2.0 * (inner_radius + outer_radius))
        m = origin + forward

        # Which side to choose
        cross = forward[0] * right[1] - forward[1] * right[0]
        is_system_mirrored = cross < 0
        if is_system_mirrored:
            sweep = 0
        else:
            sweep = 1
        twisted_sweep = 1 - sweep if twisted else sweep

        # Draw capsule
        self.data.append("L {} {}".format(*a))
        self.data.append("A {} {} 0 0 {} {} {}".format(inner_radius, inner_radius, twisted_sweep, *b))
        self.data.append("A {} {} 0 0 {} {} {}".format(outer_radius, outer_radius, 1 - twisted_sweep, *c))
        self.data.append("L {} {}".format(*d))
        self.data.append("A {} {} 0 0 {} {} {}".format(outer_radius, outer_radius, 1 - twisted_sweep, *e))
        self.data.append("A {} {} 0 0 {} {} {}".format(inner_radius, inner_radius, twisted_sweep, *f))
        self.data.append("L {} {}".format(*g))
        self.data.append("A {} {} 0 0 {} {} {}".format(inner_radius, inner_radius, sweep, *h))
        self.data.append("A {} {} 0 0 {} {} {}".format(outer_radius, outer_radius, 1 - sweep, *i))
        self.data.append("L {} {}".format(*j))
        self.data.append("A {} {} 0 0 {} {} {}".format(outer_radius, outer_radius, 1 - sweep, *k))
        self.data.append("A {} {} 0 0 {} {} {}".format(inner_radius, inner_radius, sweep, *l))
        self.data.append("L {} {}".format(*m))

    def segment(self, type, origin, forward, right):

        # Type 0 is flat
        if type == 0:
            self.flat_segment(origin, forward)
            return

        # Circular-ish hooks
        if self.style == "circle":
            if type % 2 == 0:
                right = -right
            index = (type - 1) // 2
            unit = [0.45, 0.35, 0.25][index % 3]
            offset = -0.2 * unit

            # Centered
            if 0 <= index < 3:
                self.circle_hook_segment(origin, forward, right, unit, offset)
                return

            # Right
            if 3 <= index < 6:
                self.circle_hook_segment(origin, forward, right, unit, offset, 0.15)
                return

            # Left
            if 6 <= index < 9:
                self.circle_hook_segment(origin, forward, right, unit, offset, -0.15)
                return

            # Double
            # Note: medium and large are typically too big...
            if 9 <= index < 12:
                self.double_circle_hook_segment(origin, forward, right, unit, unit * 0.6, offset)
                return

            # Twisted
            # Note: medium and large are typically too big...
            if 12 <= index < 15:
                self.double_circle_hook_segment(origin, forward, right, unit, unit * 0.6, offset, twisted=True)
                return

        # Capsule-like hooks
        if self.style == "capsule":
            if type % 2 == 0:
                right = -right
            index = (type - 1) // 2
            inner_radius = 0.03
            outer_radius = 0.03

            # Centered
            # Note: medium size is discouraged, as it does not align nicely with the others
            if 0 <= index < 3:
                length = [0.6, 0.4, 0.2][index]
                self.capsule_hook_segment(origin, forward, right, inner_radius, outer_radius, length)
                return

            # Right
            # Note: only small one is supported, for space reasons
            if index == 5:
                self.capsule_hook_segment(origin, forward, right, inner_radius, outer_radius, 0.2, 0.2)
                return

            # Left
            # Note: only small one is supported, for space reasons
            if index == 8:
                self.capsule_hook_segment(origin, forward, right, inner_radius, outer_radius, 0.2, -0.2)
                return

            # Double
            # Note: only small one is supported, for space reasons
            if index == 11:
                self.double_capsule_hook_segment(origin, forward, right, inner_radius, outer_radius, 0.2, 0.2)
                return

            # Twisted
            # Note: only small one is supported, for space reasons
            if index == 14:
                self.double_capsule_hook_segment(origin, forward, right, inner_radius, outer_radius, 0.2, 0.2, twisted=True)
                return

        # Anything else is not supported
        raise KeyError(self.style, Type(type).name)

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
            m = MIRRORED_TYPES.get(t, t)
            result.append(m)
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
        #group = '<rect width="100%" height="100%" fill="#ffffff"/>' + group
        height = (self.height + 1) * scale
        width = (self.width + 1) * scale
        content = f'<svg width="{width}" height="{height}" xmlns="http://www.w3.org/2000/svg">{group}</svg>'
        file.write(content)


if __name__ == "__main__":

    matrix = [[[4, 0, 0, 5], [5, 0, 3, 3], [4, 0, 6, 6], [0, 0, 3, 3]], [[3, 6, 0, 4], [6, 4, 4, 5], [6, 5, 5, 5], [0, 4, 5, 3]], [[4, 3, 0, 6], [4, 6, 3, 6], [3, 6, 3, 5], [0, 4, 4, 5]], [[6, 5, 0, 0], [4, 5, 5, 0], [3, 6, 3, 0], [0, 6, 4, 0]]]
    matrix = np.array(matrix)

    style = "circle"
    #style = "capsule"
    buffer = Buffer(style=style)
    buffer.jigsaw(matrix)

    with open("foo.svg", "w") as file:
        buffer.save(file)
