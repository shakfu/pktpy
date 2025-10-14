"""
Test api.Dictionary
"""

import api


def test_dictionary_creation():
    """Test Dictionary creation"""
    d = api.Dictionary()
    assert len(d) == 0


def test_dictionary_basic_set_get():
    """Test storing and retrieving basic types"""
    d = api.Dictionary()

    d["int_val"] = 42
    d["float_val"] = 3.14
    d["string_val"] = "hello world"

    assert len(d) == 3
    assert d["int_val"] == 42
    assert abs(d["float_val"] - 3.14) < 0.01
    assert d["string_val"] == "hello world"


def test_dictionary_getlong():
    """Test getlong method"""
    d = api.Dictionary()
    d["num"] = 100

    assert d.getlong("num") == 100
    assert d.getlong("missing", 999) == 999


def test_dictionary_getfloat():
    """Test getfloat method"""
    d = api.Dictionary()
    d["pi"] = 3.14159

    assert abs(d.getfloat("pi") - 3.14159) < 0.0001
    assert abs(d.getfloat("missing", 1.23) - 1.23) < 0.01


def test_dictionary_getstring():
    """Test getstring method"""
    d = api.Dictionary()
    d["name"] = "test"

    assert d.getstring("name") == "test"
    assert d.getstring("missing", "default") == "default"


def test_dictionary_lists():
    """Test storing lists"""
    d = api.Dictionary()
    d["numbers"] = [1, 2, 3, 4, 5]
    d["mixed"] = [10, 3.14, "text"]

    assert len(d) == 2


def test_dictionary_keys():
    """Test key operations"""
    d = api.Dictionary()
    d["a"] = 1
    d["b"] = 2
    d["c"] = 3

    keys = d.keys()
    assert len(keys) == 3
    assert "a" in keys
    assert "b" in keys
    assert "c" in keys


def test_dictionary_has_key():
    """Test has_key method"""
    d = api.Dictionary()
    d["a"] = 1

    assert d.has_key("a")
    assert not d.has_key("z")


def test_dictionary_in_operator():
    """Test 'in' operator"""
    d = api.Dictionary()
    d["b"] = 2

    assert "b" in d
    assert "x" not in d


def test_dictionary_get():
    """Test get method with defaults"""
    d = api.Dictionary()
    d["exists"] = 100

    assert d.get("exists") == 100
    assert d.get("missing") is None
    assert d.get("missing", 42) == 42
    assert d.get("missing", "default") == "default"


def test_dictionary_update():
    """Test updating existing values"""
    d = api.Dictionary()
    d["value"] = 10

    assert d["value"] == 10

    d["value"] = 20
    assert d["value"] == 20

    d["value"] = "now a string"
    assert d["value"] == "now a string"


def test_dictionary_delete():
    """Test deleting entries"""
    d = api.Dictionary()
    d["a"] = 1
    d["b"] = 2
    d["c"] = 3

    assert len(d) == 3

    d.delete("b")
    assert len(d) == 2
    assert "a" in d
    assert "b" not in d
    assert "c" in d


def test_dictionary_clear():
    """Test clearing dictionary"""
    d = api.Dictionary()
    d["a"] = 1
    d["b"] = 2
    d["c"] = 3

    assert len(d) == 3

    d.clear()
    assert len(d) == 0


def test_dictionary_nested():
    """Test nested dictionaries"""
    d = api.Dictionary()
    sub = api.Dictionary()

    sub["x"] = 10
    sub["y"] = 20

    d["point"] = sub
    d["name"] = "test"

    assert len(d) == 2

    point = d["point"]
    assert isinstance(point, api.Dictionary)
    assert point["x"] == 10
    assert point["y"] == 20


def test_dictionary_with_atoms():
    """Test using Atom objects"""
    d = api.Dictionary()

    a1 = api.Atom(42)
    a2 = api.Atom(3.14)
    a3 = api.Atom("symbol")

    d["atom_int"] = a1
    d["atom_float"] = a2
    d["atom_sym"] = a3

    assert d["atom_int"] == 42
    assert abs(d["atom_float"] - 3.14) < 0.01
    assert d["atom_sym"] == "symbol"


def test_dictionary_with_atomarray():
    """Test using AtomArray objects"""
    d = api.Dictionary()

    arr = api.AtomArray([1, 2, 3, 4, 5])
    arr_copy = arr.duplicate()
    d["array"] = arr_copy

    assert len(d) == 1
