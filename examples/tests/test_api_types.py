"""
Test api.Atom and api.Symbol types
"""

import api


def test_symbol_creation():
    """Test Symbol creation"""
    s1 = api.Symbol("hello")
    s2 = api.Symbol("world")
    assert s1.name == "hello"
    assert s2.name == "world"


def test_symbol_equality():
    """Test Symbol equality"""
    s1 = api.Symbol("hello")
    s2 = api.Symbol("hello")
    s3 = api.Symbol("world")
    assert s1 == s2
    assert s1 != s3
    assert s1 == "hello"


def test_symbol_repr():
    """Test Symbol string representation"""
    s1 = api.Symbol("test")
    assert "test" in str(s1)
    assert "test" in repr(s1)


def test_gensym():
    """Test gensym function"""
    s = api.gensym("test")
    assert s is not None
    assert hasattr(s, 'name')


def test_atom_creation():
    """Test Atom creation with different types"""
    a_long = api.Atom(42)
    a_float = api.Atom(3.14159)
    a_sym = api.Atom("symbol")
    a_empty = api.Atom()

    assert a_long.value == 42
    assert abs(a_float.value - 3.14159) < 0.00001
    assert a_sym.value == "symbol"


def test_atom_type_checking():
    """Test Atom type checking methods"""
    a_long = api.Atom(42)
    a_float = api.Atom(3.14)
    a_sym = api.Atom("test")

    assert a_long.is_long()
    assert not a_long.is_float()
    assert not a_long.is_symbol()

    assert a_float.is_float()
    assert not a_float.is_long()

    assert a_sym.is_symbol()
    assert not a_sym.is_long()


def test_atom_value_setting():
    """Test setting Atom values"""
    a = api.Atom()

    a.value = 100
    assert a.value == 100
    assert a.is_long()

    a.value = 2.718
    assert abs(a.value - 2.718) < 0.001
    assert a.is_float()

    a.value = "changed"
    assert a.value == "changed"
    assert a.is_symbol()


def test_atom_getlong():
    """Test Atom getlong conversion"""
    a = api.Atom(3.7)
    assert a.getlong() == 3  # truncated

    a.value = 42
    assert a.getlong() == 42


def test_atom_getfloat():
    """Test Atom getfloat conversion"""
    a = api.Atom(3.7)
    assert abs(a.getfloat() - 3.7) < 0.1

    a.value = 42
    assert abs(a.getfloat() - 42.0) < 0.1


def test_atom_getsym():
    """Test Atom getsym conversion"""
    a = api.Atom("test")
    sym = a.getsym()
    assert sym.name == "test"


def test_atom_module_functions():
    """Test module-level atom conversion functions"""
    a_int = api.Atom(100)
    assert api.atom_getlong(a_int) == 100

    a_flt = api.Atom(2.5)
    assert abs(api.atom_getfloat(a_flt) - 2.5) < 0.1

    a_str = api.Atom("hello")
    sym = api.atom_getsym(a_str)
    assert sym.name == "hello"


def test_atom_int_conversion():
    """Test Atom __int__ magic method"""
    a = api.Atom(3.7)
    assert int(a) == 3  # truncated

    a = api.Atom(42)
    assert int(a) == 42


def test_atom_float_conversion():
    """Test Atom __float__ magic method"""
    a = api.Atom(3.7)
    assert abs(float(a) - 3.7) < 0.1

    a = api.Atom(42)
    assert abs(float(a) - 42.0) < 0.1


def test_atom_str_conversion():
    """Test Atom __str__ magic method"""
    a = api.Atom(42)
    assert "42" in str(a)

    a = api.Atom(3.14)
    assert "3.14" in str(a)

    a = api.Atom("hello")
    assert "hello" in str(a)


def test_atom_python_expressions():
    """Test using Atom in Python expressions"""
    a1 = api.Atom(10)
    assert int(a1) * 2 == 20

    a2 = api.Atom(3.5)
    assert abs(float(a2) + 1.5 - 5.0) < 0.1

    a3 = api.Atom("world")
    assert "hello " + str(a3) == "hello world"


def test_atom_from_symbol():
    """Test creating Atom from Symbol"""
    sym = api.Symbol("test")
    atom = api.Atom(sym)

    assert atom.is_symbol()
    assert atom.value == "test"
