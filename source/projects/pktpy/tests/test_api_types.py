"""
Test script for api.Atom and api.Symbol types

This script demonstrates usage of the Max types wrapped in Python.
Run this inside the pktpy Max external using 'execfile' or 'load'.
"""

import api

def test_symbol():
    """Test Symbol wrapper functionality"""
    api.post("=== Testing Symbol ===\n")

    # Create symbols
    s1 = api.Symbol("hello")
    s2 = api.Symbol("world")
    s3 = api.Symbol("hello")

    # Test string representation
    api.post(f"s1: {s1}\n")
    api.post(f"s2: {s2}\n")
    api.post(f"repr(s1): {repr(s1)}\n")

    # Test name property
    api.post(f"s1.name: {s1.name}\n")

    # Test equality
    api.post(f"s1 == s3: {s1 == s3}\n")
    api.post(f"s1 == s2: {s1 == s2}\n")
    api.post(f"s1 == 'hello': {s1 == 'hello'}\n")

    # Test gensym
    s4 = api.gensym("test")
    api.post(f"gensym('test'): {s4}\n")

    api.post("\n")


def test_atom():
    """Test Atom wrapper functionality"""
    api.post("=== Testing Atom ===\n")

    # Create atoms of different types
    a_long = api.Atom(42)
    a_float = api.Atom(3.14159)
    a_sym = api.Atom("symbol")
    a_empty = api.Atom()

    # Test repr
    api.post(f"a_long: {a_long}\n")
    api.post(f"a_float: {a_float}\n")
    api.post(f"a_sym: {a_sym}\n")
    api.post(f"a_empty: {a_empty}\n")

    # Test type property
    api.post(f"a_long.type: {a_long.type}\n")
    api.post(f"a_float.type: {a_float.type}\n")
    api.post(f"a_sym.type: {a_sym.type}\n")

    # Test value property
    api.post(f"a_long.value: {a_long.value}\n")
    api.post(f"a_float.value: {a_float.value}\n")
    api.post(f"a_sym.value: {a_sym.value}\n")

    # Test type checking methods
    api.post(f"a_long.is_long(): {a_long.is_long()}\n")
    api.post(f"a_long.is_float(): {a_long.is_float()}\n")
    api.post(f"a_float.is_float(): {a_float.is_float()}\n")
    api.post(f"a_sym.is_symbol(): {a_sym.is_symbol()}\n")

    # Test setting values
    a_empty.value = 100
    api.post(f"After setting value to 100: {a_empty}\n")
    api.post(f"a_empty.type: {a_empty.type}\n")

    a_empty.value = 2.718
    api.post(f"After setting value to 2.718: {a_empty}\n")
    api.post(f"a_empty.type: {a_empty.type}\n")

    a_empty.value = "changed"
    api.post(f"After setting value to 'changed': {a_empty}\n")
    api.post(f"a_empty.type: {a_empty.type}\n")

    api.post("\n")


def test_atom_conversion():
    """Test Max atom conversion methods"""
    api.post("=== Testing Atom Conversion Methods ===\n")

    # Create mixed-type atom
    a = api.Atom(3.7)

    # Test explicit methods
    api.post(f"Atom with float 3.7\n")
    api.post(f"a.getlong(): {a.getlong()} (truncated)\n")
    api.post(f"a.getfloat(): {a.getfloat()}\n")

    # Change to long
    a.value = 42
    api.post(f"\nAtom with long 42\n")
    api.post(f"a.getlong(): {a.getlong()}\n")
    api.post(f"a.getfloat(): {a.getfloat()} (converted to float)\n")

    # Change to symbol
    a.value = "test"
    api.post(f"\nAtom with symbol 'test'\n")
    sym = a.getsym()
    api.post(f"a.getsym(): {sym}\n")
    api.post(f"a.getsym().name: {sym.name}\n")

    # Test module-level functions
    api.post(f"\nModule-level conversion functions:\n")
    a_int = api.Atom(100)
    api.post(f"api.atom_getlong(Atom(100)): {api.atom_getlong(a_int)}\n")

    a_flt = api.Atom(2.5)
    api.post(f"api.atom_getfloat(Atom(2.5)): {api.atom_getfloat(a_flt)}\n")

    a_str = api.Atom("hello")
    sym2 = api.atom_getsym(a_str)
    api.post(f"api.atom_getsym(Atom('hello')): {sym2}\n")

    api.post("\n")


def test_magic_methods():
    """Test Pythonic magic methods (__int__, __float__, __str__)"""
    api.post("=== Testing Pythonic Magic Methods ===\n")

    # Test int() conversion
    a = api.Atom(3.7)
    api.post(f"Atom(3.7):\n")
    api.post(f"  int(a): {int(a)} (truncated)\n")
    api.post(f"  float(a): {float(a)}\n")
    api.post(f"  str(a): '{str(a)}'\n")

    # Test with integer atom
    a = api.Atom(42)
    api.post(f"\nAtom(42):\n")
    api.post(f"  int(a): {int(a)}\n")
    api.post(f"  float(a): {float(a)} (converted)\n")
    api.post(f"  str(a): '{str(a)}'\n")

    # Test with symbol atom
    a = api.Atom("hello")
    api.post(f"\nAtom('hello'):\n")
    api.post(f"  int(a): {int(a)} (symbol->0)\n")
    api.post(f"  float(a): {float(a)} (symbol->0.0)\n")
    api.post(f"  str(a): '{str(a)}'\n")

    # Use in Python expressions
    api.post(f"\nPython expressions:\n")
    a1 = api.Atom(10)
    a2 = api.Atom(3.5)
    api.post(f"  int(Atom(10)) * 2 = {int(a1) * 2}\n")
    api.post(f"  float(Atom(3.5)) + 1.5 = {float(a2) + 1.5}\n")

    # String concatenation
    a3 = api.Atom("world")
    api.post(f"  'hello ' + str(Atom('world')) = '{\"hello \" + str(a3)}'\n")

    api.post("\n")


def test_combined():
    """Test using Symbol with Atom"""
    api.post("=== Testing Symbol with Atom ===\n")

    # Create a symbol
    sym = api.Symbol("test")

    # Create an atom from the symbol
    atom = api.Atom(sym)
    api.post(f"Atom from Symbol: {atom}\n")
    api.post(f"Type: {atom.type}\n")
    api.post(f"Value: {atom.value}\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Max API Type Wrappers\n")
    api.post("====================================\n\n")

    try:
        test_symbol()
        test_atom()
        test_atom_conversion()
        test_magic_methods()
        test_combined()

        api.post("====================================\n")
        api.post("All tests completed successfully!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
