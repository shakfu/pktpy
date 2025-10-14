"""
Test api.parse() and AtomArray.from_parse()
"""

import api


def test_parse_basic():
    """Test basic parsing with api.parse()"""
    arr = api.parse("hello world 42 3.14")

    assert len(arr) == 4
    assert str(arr[0]) == "hello"
    assert str(arr[1]) == "world"
    assert int(arr[2]) == 42
    assert abs(float(arr[3]) - 3.14) < 0.01


def test_parse_integers():
    """Test parsing integers"""
    arr = api.parse("1 2 3 4 5")

    assert len(arr) == 5
    for i in range(5):
        assert int(arr[i]) == i + 1


def test_parse_floats():
    """Test parsing floats"""
    arr = api.parse("1.5 2.7 3.14159 -0.5")

    assert len(arr) == 4
    assert abs(float(arr[0]) - 1.5) < 0.01
    assert abs(float(arr[1]) - 2.7) < 0.01
    assert abs(float(arr[2]) - 3.14159) < 0.001
    assert abs(float(arr[3]) + 0.5) < 0.01


def test_parse_mixed():
    """Test parsing mixed types"""
    arr = api.parse("10 3.14 -5 0.001")

    assert len(arr) == 4


def test_parse_symbols():
    """Test parsing symbols"""
    arr = api.parse("bang float list anything")

    assert len(arr) == 4
    assert str(arr[0]) == "bang"
    assert str(arr[1]) == "float"
    assert str(arr[2]) == "list"
    assert str(arr[3]) == "anything"


def test_parse_empty():
    """Test parsing empty string"""
    arr = api.parse("")
    assert len(arr) == 0


def test_from_parse_classmethod():
    """Test AtomArray.from_parse() class method"""
    arr = api.AtomArray.from_parse("test 100 2.5 foo")

    assert len(arr) == 4
    assert str(arr[0]) == "test"
    assert int(arr[1]) == 100
    assert abs(float(arr[2]) - 2.5) < 0.01
    assert str(arr[3]) == "foo"


def test_parse_to_list():
    """Test converting parsed atoms to Python list"""
    arr = api.parse("a b c 1 2 3")
    py_list = arr.to_list()

    assert len(py_list) == 6
    assert isinstance(py_list, list)


def test_parse_and_modify():
    """Test parsing and then modifying"""
    arr = api.parse("10 20 30")

    assert int(arr[0]) == 10
    assert int(arr[1]) == 20
    assert int(arr[2]) == 30

    # Modify values
    arr[0] = 100
    arr[1] = 200
    arr[2] = 300

    assert int(arr[0]) == 100
    assert int(arr[1]) == 200
    assert int(arr[2]) == 300

    # Append to parsed array
    arr.append(400)
    assert len(arr) == 4


def test_parse_command():
    """Test parsing command strings"""
    arr = api.parse("metro 100")

    assert len(arr) == 2
    assert str(arr[0]) == "metro"
    assert int(arr[1]) == 100


def test_parse_note():
    """Test parsing MIDI note message"""
    arr = api.parse("note 60 100 500")

    assert len(arr) == 4
    assert str(arr[0]) == "note"
    assert int(arr[1]) == 60
    assert int(arr[2]) == 100
    assert int(arr[3]) == 500


def test_parse_vs_manual():
    """Compare parse() vs manual construction"""
    arr1 = api.parse("test 42 3.14")

    arr2 = api.AtomArray()
    arr2.append("test")
    arr2.append(42)
    arr2.append(3.14)

    # Both should have same length
    assert len(arr1) == len(arr2)

    # Both should have same values
    for i in range(len(arr1)):
        assert str(arr1[i]) == str(arr2[i])
