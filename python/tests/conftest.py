import pytest


@pytest.fixture
def path_4():
    """P4: 1-2-3-4"""
    return 4, [(1, 2), (2, 3), (3, 4)]


@pytest.fixture
def path_5():
    """P5: 1-2-3-4-5"""
    return 5, [(1, 2), (2, 3), (3, 4), (4, 5)]


@pytest.fixture
def cycle_4():
    """C4: 1-2-3-4-1"""
    return 4, [(1, 2), (2, 3), (3, 4), (4, 1)]


@pytest.fixture
def cycle_5():
    """C5: 1-2-3-4-5-1"""
    return 5, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 1)]


@pytest.fixture
def cycle_6():
    """C6: 1-2-3-4-5-6-1"""
    return 6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 1)]


@pytest.fixture
def complete_3():
    """K3"""
    return 3, [(1, 2), (1, 3), (2, 3)]


@pytest.fixture
def complete_4():
    """K4"""
    return 4, [(1, 2), (1, 3), (1, 4), (2, 3), (2, 4), (3, 4)]


@pytest.fixture
def complete_5():
    """K5"""
    return 5, [
        (1, 2), (1, 3), (1, 4), (1, 5),
        (2, 3), (2, 4), (2, 5),
        (3, 4), (3, 5),
        (4, 5),
    ]


@pytest.fixture
def empty_3():
    """3 isolated vertices"""
    return 3, []


@pytest.fixture
def star_4():
    """S4: center 1 connected to 2,3,4"""
    return 4, [(1, 2), (1, 3), (1, 4)]


@pytest.fixture
def complete_bipartite_2_3():
    """K_{2,3}: parts {1,2} and {3,4,5}"""
    return 5, [
        (1, 3), (1, 4), (1, 5),
        (2, 3), (2, 4), (2, 5),
    ]


@pytest.fixture
def complete_bipartite_3_3():
    """K_{3,3}: parts {1,2,3} and {4,5,6}"""
    return 6, [
        (1, 4), (1, 5), (1, 6),
        (2, 4), (2, 5), (2, 6),
        (3, 4), (3, 5), (3, 6),
    ]


@pytest.fixture
def claw():
    """K_{1,3}: center 1 connected to 2,3,4 (same as star_4)"""
    return 4, [(1, 2), (1, 3), (1, 4)]
