import numpy as np


(
    FLAT,
    CENTERED_BIG_MALE,
    CENTERED_BIG_FEMALE,
    CENTERED_MEDIUM_MALE,
    CENTERED_MEDIUM_FEMALE,
    CENTERED_SMALL_MALE,
    CENTERED_SMALL_FEMALE,
    RIGHT_BIG_MALE,
    RIGHT_BIG_FEMALE,
    RIGHT_MEDIUM_MALE,
    RIGHT_MEDIUM_FEMALE,
    RIGHT_SMALL_MALE,
    RIGHT_SMALL_FEMALE,
    LEFT_BIG_MALE,
    LEFT_BIG_FEMALE,
    LEFT_MEDIUM_MALE,
    LEFT_MEDIUM_FEMALE,
    LEFT_SMALL_MALE,
    LEFT_SMALL_FEMALE,
    DOUBLE_BIG_MALE,
    DOUBLE_BIG_FEMALE,
    DOUBLE_MEDIUM_MALE,
    DOUBLE_MEDIUM_FEMALE,
    DOUBLE_SMALL_MALE,
    DOUBLE_SMALL_FEMALE,
    TWISTED_BIG_MALE,
    TWISTED_BIG_FEMALE,
    TWISTED_MEDIUM_MALE,
    TWISTED_MEDIUM_FEMALE,
    TWISTED_SMALL_MALE,
    TWISTED_SMALL_FEMALE,
    FLAT_ALTERNATIVE,
) = range(32)


edge_names = np.array([
    "FLAT",
    "CENTERED_BIG_MALE",
    "CENTERED_BIG_FEMALE",
    "CENTERED_MEDIUM_MALE",
    "CENTERED_MEDIUM_FEMALE",
    "CENTERED_SMALL_MALE",
    "CENTERED_SMALL_FEMALE",
    "RIGHT_BIG_MALE",
    "RIGHT_BIG_FEMALE",
    "RIGHT_MEDIUM_MALE",
    "RIGHT_MEDIUM_FEMALE",
    "RIGHT_SMALL_MALE",
    "RIGHT_SMALL_FEMALE",
    "LEFT_BIG_MALE",
    "LEFT_BIG_FEMALE",
    "LEFT_MEDIUM_MALE",
    "LEFT_MEDIUM_FEMALE",
    "LEFT_SMALL_MALE",
    "LEFT_SMALL_FEMALE",
    "DOUBLE_BIG_MALE",
    "DOUBLE_BIG_FEMALE",
    "DOUBLE_MEDIUM_MALE",
    "DOUBLE_MEDIUM_FEMALE",
    "DOUBLE_SMALL_MALE",
    "DOUBLE_SMALL_FEMALE",
    "TWISTED_BIG_MALE",
    "TWISTED_BIG_FEMALE",
    "TWISTED_MEDIUM_MALE",
    "TWISTED_MEDIUM_FEMALE",
    "TWISTED_SMALL_MALE",
    "TWISTED_SMALL_FEMALE",
    "FLAT_ALTERNATIVE",
])


opposite = np.array([
    0,  # FLAT -> FLAT
    2,  # CENTERED_BIG_MALE -> CENTERED_BIG_FEMALE
    1,  # CENTERED_BIG_FEMALE -> CENTERED_BIG_MALE
    4,  # CENTERED_MEDIUM_MALE -> CENTERED_MEDIUM_FEMALE
    3,  # CENTERED_MEDIUM_FEMALE -> CENTERED_MEDIUM_MALE
    6,  # CENTERED_SMALL_MALE -> CENTERED_SMALL_FEMALE
    5,  # CENTERED_SMALL_FEMALE -> CENTERED_SMALL_MALE
    14, # RIGHT_BIG_MALE -> LEFT_BIG_FEMALE
    13, # RIGHT_BIG_FEMALE -> LEFT_BIG_MALE
    16, # RIGHT_MEDIUM_MALE -> LEFT_MEDIUM_FEMALE
    15, # RIGHT_MEDIUM_FEMALE -> LEFT_MEDIUM_MALE
    18, # RIGHT_SMALL_MALE -> LEFT_SMALL_FEMALE
    17, # RIGHT_SMALL_FEMALE -> LEFT_SMALL_MALE
    8,  # LEFT_BIG_MALE -> RIGHT_BIG_FEMALE
    7,  # LEFT_BIG_FEMALE -> RIGHT_BIG_MALE
    10, # LEFT_MEDIUM_MALE -> RIGHT_MEDIUM_FEMALE
    9,  # LEFT_MEDIUM_FEMALE -> RIGHT_MEDIUM_MALE
    12, # LEFT_SMALL_MALE -> RIGHT_SMALL_FEMALE
    11, # LEFT_SMALL_FEMALE -> RIGHT_SMALL_MALE
    20, # DOUBLE_BIG_MALE -> DOUBLE_BIG_FEMALE
    19, # DOUBLE_BIG_FEMALE -> DOUBLE_BIG_MALE
    22, # DOUBLE_MEDIUM_MALE -> DOUBLE_MEDIUM_FEMALE
    21, # DOUBLE_MEDIUM_FEMALE -> DOUBLE_MEDIUM_MALE
    24, # DOUBLE_SMALL_MALE -> DOUBLE_SMALL_FEMALE
    23, # DOUBLE_SMALL_FEMALE -> DOUBLE_SMALL_MALE
    25, # TWISTED_BIG_MALE -> TWISTED_BIG_MALE
    26, # TWISTED_BIG_FEMALE -> TWISTED_BIG_FEMALE
    27, # TWISTED_MEDIUM_MALE -> TWISTED_MEDIUM_MALE
    28, # TWISTED_MEDIUM_FEMALE -> TWISTED_MEDIUM_FEMALE
    29, # TWISTED_SMALL_MALE -> TWISTED_SMALL_MALE
    30, # TWISTED_SMALL_FEMALE -> TWISTED_SMALL_FEMALE
    31, # FLAT_ALTERNATIVE -> FLAT_ALTERNATIVE
], dtype=np.uint8)


flip = np.array([
    0,  # FLAT -> FLAT
    1,  # CENTERED_BIG_MALE -> CENTERED_BIG_MALE
    2,  # CENTERED_BIG_FEMALE -> CENTERED_BIG_FEMALE
    3,  # CENTERED_MEDIUM_MALE -> CENTERED_MEDIUM_MALE
    4,  # CENTERED_MEDIUM_FEMALE -> CENTERED_MEDIUM_FEMALE
    5,  # CENTERED_SMALL_MALE -> CENTERED_SMALL_MALE
    6,  # CENTERED_SMALL_FEMALE -> CENTERED_SMALL_FEMALE
    13, # RIGHT_BIG_MALE -> LEFT_BIG_MALE
    14, # RIGHT_BIG_FEMALE -> LEFT_BIG_FEMALE
    15, # RIGHT_MEDIUM_MALE -> LEFT_MEDIUM_MALE
    16, # RIGHT_MEDIUM_FEMALE -> LEFT_MEDIUM_FEMALE
    17, # RIGHT_SMALL_MALE -> LEFT_SMALL_MALE
    18, # RIGHT_SMALL_FEMALE -> LEFT_SMALL_FEMALE
    7,  # LEFT_BIG_MALE -> RIGHT_BIG_MALE
    8,  # LEFT_BIG_FEMALE -> RIGHT_BIG_FEMALE
    9,  # LEFT_MEDIUM_MALE -> RIGHT_MEDIUM_MALE
    10, # LEFT_MEDIUM_FEMALE -> RIGHT_MEDIUM_FEMALE
    11, # LEFT_SMALL_MALE -> RIGHT_SMALL_MALE
    12, # LEFT_SMALL_FEMALE -> RIGHT_SMALL_FEMALE
    19, # DOUBLE_BIG_MALE -> DOUBLE_BIG_MALE
    20, # DOUBLE_BIG_FEMALE -> DOUBLE_BIG_FEMALE
    21, # DOUBLE_MEDIUM_MALE -> DOUBLE_MEDIUM_MALE
    22, # DOUBLE_MEDIUM_FEMALE -> DOUBLE_MEDIUM_FEMALE
    23, # DOUBLE_SMALL_MALE -> DOUBLE_SMALL_MALE
    24, # DOUBLE_SMALL_FEMALE -> DOUBLE_SMALL_FEMALE
    26, # TWISTED_BIG_MALE -> TWISTED_BIG_FEMALE
    25, # TWISTED_BIG_FEMALE -> TWISTED_BIG_MALE
    28, # TWISTED_MEDIUM_MALE -> TWISTED_MEDIUM_FEMALE
    27, # TWISTED_MEDIUM_FEMALE -> TWISTED_MEDIUM_MALE
    30, # TWISTED_SMALL_MALE -> TWISTED_SMALL_FEMALE
    29, # TWISTED_SMALL_FEMALE -> TWISTED_SMALL_MALE
    31, # FLAT_ALTERNATIVE -> FLAT_ALTERNATIVE
], dtype=np.uint8)
