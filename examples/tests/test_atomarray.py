"""
Test api.AtomArray
"""

import api


def test_atomarray_empty():
    """Test empty AtomArray creation"""
    arr = api.AtomArray()
    assert len(arr) == 0


def test_atomarray_from_list():
    """Test AtomArray from list"""
    arr = api.AtomArray([1, 2, 3])
    assert len(arr) == 3


def test_atomarray_mixed_types():
    """Test AtomArray with mixed types"""
    arr = api.AtomArray([42, 3.14, "hello"])
    assert len(arr) == 3
    assert int(arr[0]) == 42
    assert abs(float(arr[1]) - 3.14) < 0.01
    assert str(arr[2]) == "hello"


def test_atomarray_index_access():
    """Test accessing AtomArray elements by index"""
    arr = api.AtomArray([10, 20, 30, 40, 50])

    assert int(arr[0]) == 10
    assert int(arr[2]) == 30
    assert int(arr[-1]) == 50
    assert int(arr[-2]) == 40


def test_atomarray_append():
    """Test appending to AtomArray"""
    arr = api.AtomArray([1, 2, 3])
    assert len(arr) == 3

    arr.append(4)
    arr.append(5)
    assert len(arr) == 5


def test_atomarray_setitem():
    """Test setting AtomArray items"""
    arr = api.AtomArray([1, 2, 3])

    arr[0] = 100
    assert int(arr[0]) == 100

    arr[1] = 3.14
    assert abs(float(arr[1]) - 3.14) < 0.01

    arr[2] = "test"
    assert str(arr[2]) == "test"


def test_atomarray_clear():
    """Test clearing AtomArray"""
    arr = api.AtomArray([1, 2, 3, 4, 5])
    assert len(arr) == 5

    arr.clear()
    assert len(arr) == 0


def test_atomarray_to_list():
    """Test converting AtomArray to Python list"""
    arr = api.AtomArray([1, 2.5, "hello", 42])
    py_list = arr.to_list()

    assert len(py_list) == 4
    assert isinstance(py_list, list)


def test_atomarray_duplicate():
    """Test duplicating AtomArray"""
    arr1 = api.AtomArray([1, 2, 3])
    arr2 = arr1.duplicate()

    assert len(arr1) == 3
    assert len(arr2) == 3

    # Modify duplicate
    arr2.append(4)
    arr2[0] = 100

    # Original should be unchanged
    assert len(arr1) == 3
    assert int(arr1[0]) == 1

    # Duplicate should be modified
    assert len(arr2) == 4
    assert int(arr2[0]) == 100


def test_atomarray_iteration():
    """Test iterating over AtomArray"""
    arr = api.AtomArray([10, 20, 30])
    values = []

    for i in range(len(arr)):
        values.append(int(arr[i]))

    assert values == [10, 20, 30]


def test_atomarray_with_atoms():
    """Test AtomArray with Atom objects"""
    a1 = api.Atom(42)
    a2 = api.Atom(3.14)
    a3 = api.Atom("test")

    arr = api.AtomArray()
    arr.append(a1)
    arr.append(a2)
    arr.append(a3)

    assert len(arr) == 3
    assert int(arr[0]) == 42
    assert abs(float(arr[1]) - 3.14) < 0.01
    assert str(arr[2]) == "test"


def test_atomarray_processing():
    """Test processing AtomArray data"""
    input_data = [1, 2, 3, 4, 5]
    arr = api.AtomArray(input_data)

    # Multiply by 2
    for i in range(len(arr)):
        val = int(arr[i])
        arr[i] = val * 2

    # Verify results
    result = arr.to_list()
    total = sum(result)
    assert total == 30  # (1+2+3+4+5) * 2 = 30
