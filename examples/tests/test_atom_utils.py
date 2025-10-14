"""
Test script for atom utility functions

This script demonstrates usage of the atom utility functions including
bulk conversion methods for AtomArray and text conversion.
Run this inside the pktpy Max external using 'execfile' or 'load'.
"""

import api


def test_atom_gettext():
    """Test api.atom_gettext() function"""
    api.post("=== Testing api.atom_gettext() ===\n")

    # Create test data
    arr = api.AtomArray([1, 2.5, "test"])
    text = api.atom_gettext(arr)
    api.post(f"Input: {arr}\n")
    api.post(f"Output: '{text}'\n")

    api.post("\n")


def test_to_ints():
    """Test AtomArray.to_ints() method"""

    # Test with integers
    api.post("Test 1: Integer atoms\n")
    arr = api.AtomArray([10, 20, 30, 40, 50])
    int_list = arr.to_ints()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {int_list}\n")
    api.post(f"  Type: {type(int_list)}\n")
    api.post("\n")

    # Test with floats (truncated)
    api.post("Test 2: Float atoms (truncated)\n")
    arr = api.AtomArray([1.9, 2.5, 3.1])
    int_list = arr.to_ints()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {int_list}\n")
    api.post(f"  Note: Floats truncated to integers\n")
    api.post("\n")

    # Test with mixed
    api.post("Test 3: Mixed types\n")
    arr = api.AtomArray([100, 200.7, 300])
    int_list = arr.to_ints()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {int_list}\n")

    api.post("\n")


def test_to_floats():
    """Test AtomArray.to_floats() method"""
    # Test with floats
    api.post("Test 1: Float atoms\n")
    arr = api.AtomArray([1.5, 2.7, 3.14159])
    float_list = arr.to_floats()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {float_list}\n")
    api.post(f"  Type: {type(float_list)}\n")
    api.post("\n")

    # Test with integers (converted)
    api.post("Test 2: Integer atoms (converted)\n")
    arr = api.AtomArray([10, 20, 30])
    float_list = arr.to_floats()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {float_list}\n")
    api.post(f"  Note: Integers converted to floats\n")
    api.post("\n")

    # Test with mixed
    api.post("Test 3: Mixed types\n")
    arr = api.AtomArray([100, 200.5, 300, 400.7])
    float_list = arr.to_floats()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {float_list}\n")

    api.post("\n")


def test_to_symbols():
    """Test AtomArray.to_symbols() method"""

    # Test with symbols
    api.post("Test 1: Symbol atoms\n")
    arr = api.parse("hello world test message")
    symbol_list = arr.to_symbols()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {symbol_list}\n")
    api.post(f"  Type: {type(symbol_list)}\n")
    api.post("\n")

    # Test with message parsing
    api.post("Test 2: Parsed message\n")
    arr = api.parse("bang float list anything")
    symbol_list = arr.to_symbols()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: {symbol_list}\n")

    api.post("\n")


def test_to_text():
    """Test AtomArray.to_text() method"""

    # Test with mixed types
    api.post("Test 1: Mixed types\n")
    arr = api.AtomArray([42, 3.14, "hello"])
    text = arr.to_text()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: '{text}'\n")
    api.post("\n")

    # Test with parsed input
    api.post("Test 2: Parsed input\n")
    arr = api.parse("note 60 100 500")
    text = arr.to_text()
    api.post(f"  Input: {arr}\n")
    api.post(f"  Output: '{text}'\n")

    api.post("\n")


def test_comparison():
    """Compare conversion methods"""
    api.post("=== Comparing Conversion Methods ===\n")

    arr = api.AtomArray([10, 20, 30, 40, 50])

    api.post("Original AtomArray:\n")
    api.post(f"  {arr}\n")
    api.post("\n")

    api.post("Conversion comparison:\n")

    # to_list (original method)
    py_list = arr.to_list()
    api.post(f"  to_list():    {py_list}\n")
    api.post(f"                Type: list of Atoms\n")
    api.post("\n")

    # to_ints (new method)
    int_list = arr.to_ints()
    api.post(f"  to_ints():    {int_list}\n")
    api.post(f"                Type: list of ints\n")
    api.post("\n")

    # to_floats (new method)
    float_list = arr.to_floats()
    api.post(f"  to_floats():  {float_list}\n")
    api.post(f"                Type: list of floats\n")
    api.post("\n")

    # to_text (new method)
    text = arr.to_text()
    api.post(f"  to_text():    '{text}'\n")
    api.post(f"                Type: string\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Atom Utility Functions\n")
    api.post("====================================\n\n")

    try:
        test_atom_gettext()
        test_to_ints()
        test_to_floats()
        test_to_symbols()
        test_to_text()
        test_comparison()

        api.post("====================================\n")
        api.post("All tests completed successfully!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
