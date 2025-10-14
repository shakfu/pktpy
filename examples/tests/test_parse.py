"""
Test script for api.parse() and AtomArray.from_parse()

This script demonstrates usage of the atom parsing functions.
Run this inside the pktpy Max external using 'execfile' or 'load'.
"""

import api


def test_parse_basic():
    """Test basic parsing with api.parse()"""
    api.post("=== Testing api.parse() ===\n")

    # Parse simple values
    arr = api.parse("hello world 42 3.14")
    api.post(f"Parsed: {arr}\n")
    api.post(f"Length: {len(arr)}\n")

    # Check individual atoms
    api.post(f"arr[0]: {arr[0]} (type: {arr[0].type})\n")
    api.post(f"arr[1]: {arr[1]} (type: {arr[1].type})\n")
    api.post(f"arr[2]: {arr[2]} (type: {arr[2].type})\n")
    api.post(f"arr[3]: {arr[3]} (type: {arr[3].type})\n")

    api.post("\n")


def test_parse_numbers():
    """Test parsing numbers"""
    api.post("=== Testing Number Parsing ===\n")

    # Parse integers
    arr = api.parse("1 2 3 4 5")
    api.post(f"Integers: {arr}\n")
    for i in range(len(arr)):
        api.post(f"  {i}: {int(arr[i])}\n")

    # Parse floats
    arr = api.parse("1.5 2.7 3.14159 -0.5")
    api.post(f"Floats: {arr}\n")
    for i in range(len(arr)):
        api.post(f"  {i}: {float(arr[i])}\n")

    # Parse mixed
    arr = api.parse("10 3.14 -5 0.001")
    api.post(f"Mixed: {arr}\n")

    api.post("\n")


def test_parse_symbols():
    """Test parsing symbols"""
    api.post("=== Testing Symbol Parsing ===\n")

    # Parse symbols
    arr = api.parse("bang float list anything")
    api.post(f"Symbols: {arr}\n")
    for i in range(len(arr)):
        api.post(f"  {i}: {str(arr[i])}\n")

    api.post("\n")


def test_parse_empty():
    """Test parsing empty string"""
    api.post("=== Testing Empty String ===\n")

    arr = api.parse("")
    api.post(f"Empty parse: {arr}\n")
    api.post(f"Length: {len(arr)}\n")

    api.post("\n")


def test_from_parse_classmethod():
    """Test AtomArray.from_parse() class method"""
    api.post("=== Testing AtomArray.from_parse() ===\n")

    # Create array using class method
    arr = api.AtomArray.from_parse("test 100 2.5 foo")
    api.post(f"Parsed: {arr}\n")
    api.post(f"Length: {len(arr)}\n")

    # Check values
    for i in range(len(arr)):
        atom = arr[i]
        api.post(f"  [{i}] {atom} - type: {atom.type}, value: {atom.value}\n")

    api.post("\n")


def test_parse_to_list():
    """Test converting parsed atoms to Python list"""
    api.post("=== Testing Parse to List ===\n")

    arr = api.parse("a b c 1 2 3")
    py_list = arr.to_list()

    api.post(f"AtomArray: {arr}\n")
    api.post(f"Python list: {py_list}\n")
    api.post(f"List length: {len(py_list)}\n")

    api.post("\n")


def test_parse_and_modify():
    """Test parsing and then modifying"""
    api.post("=== Testing Parse and Modify ===\n")

    # Parse initial values
    arr = api.parse("10 20 30")
    api.post(f"Original: {arr}\n")

    # Modify values
    arr[0] = 100
    arr[1] = 200
    arr[2] = 300

    api.post(f"Modified: {arr}\n")

    # Append to parsed array
    arr.append(400)
    api.post(f"After append: {arr}\n")
    api.post(f"Length: {len(arr)}\n")

    api.post("\n")


def test_parse_practical():
    """Practical example: parsing user input"""
    api.post("=== Practical Example ===\n")

    # Simulate parsing user input
    user_inputs = [
        "metro 100",
        "sine 440 0.5",
        "note 60 100 500",
        "bang",
        "clear all"
    ]

    for input_str in user_inputs:
        arr = api.parse(input_str)
        api.post(f"Input: '{input_str}'\n")
        api.post(f"  Parsed to {len(arr)} atoms\n")

        if len(arr) > 0:
            api.post(f"  Command: {str(arr[0])}\n")
            if len(arr) > 1:
                api.post(f"  Args: ")
                for i in range(1, len(arr)):
                    api.post(f"{arr[i]} ")
                api.post("\n")

    api.post("\n")


def test_parse_comparison():
    """Compare parse() vs manual construction"""
    api.post("=== Comparing parse() vs manual construction ===\n")

    # Using parse
    arr1 = api.parse("test 42 3.14")
    api.post(f"Parsed: {arr1}\n")

    # Manual construction
    arr2 = api.AtomArray()
    arr2.append("test")
    arr2.append(42)
    arr2.append(3.14)
    api.post(f"Manual: {arr2}\n")

    # Both should have same values
    api.post("Comparing values:\n")
    for i in range(len(arr1)):
        api.post(f"  [{i}] parsed: {arr1[i]}, manual: {arr2[i]}\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Atom Parsing Functions\n")
    api.post("====================================\n\n")

    try:
        test_parse_basic()
        test_parse_numbers()
        test_parse_symbols()
        test_parse_empty()
        test_from_parse_classmethod()
        test_parse_to_list()
        test_parse_and_modify()
        test_parse_practical()
        test_parse_comparison()

        api.post("====================================\n")
        api.post("All tests completed successfully!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
