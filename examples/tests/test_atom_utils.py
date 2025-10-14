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

    api.post("Function signature:\n")
    api.post("  text = api.atom_gettext(atomarray)\n")
    api.post("\n")

    api.post("Converts an AtomArray to a text string.\n")
    api.post("Each atom is converted to its string representation\n")
    api.post("and concatenated with spaces.\n")
    api.post("\n")

    # Create test data
    arr = api.AtomArray([1, 2.5, "test"])
    text = api.atom_gettext(arr)
    api.post(f"Input: {arr}\n")
    api.post(f"Output: '{text}'\n")

    api.post("\n")


def test_to_ints():
    """Test AtomArray.to_ints() method"""
    api.post("=== Testing AtomArray.to_ints() ===\n")

    api.post("Method signature:\n")
    api.post("  int_list = atomarray.to_ints()\n")
    api.post("\n")

    api.post("Converts all atoms to integers and returns Python list.\n")
    api.post("Uses Max's atom_getlong_array() for efficient bulk conversion.\n")
    api.post("Floats are truncated to integers.\n")
    api.post("\n")

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
    api.post("=== Testing AtomArray.to_floats() ===\n")

    api.post("Method signature:\n")
    api.post("  float_list = atomarray.to_floats()\n")
    api.post("\n")

    api.post("Converts all atoms to floats and returns Python list.\n")
    api.post("Uses Max's atom_getdouble_array() for efficient bulk conversion.\n")
    api.post("Integers are converted to floats.\n")
    api.post("\n")

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
    api.post("=== Testing AtomArray.to_symbols() ===\n")

    api.post("Method signature:\n")
    api.post("  symbol_list = atomarray.to_symbols()\n")
    api.post("\n")

    api.post("Extracts symbol atoms and returns Python list of strings.\n")
    api.post("Uses Max's atom_getsym_array() for efficient bulk extraction.\n")
    api.post("Only works with symbol atoms.\n")
    api.post("\n")

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
    api.post("=== Testing AtomArray.to_text() ===\n")

    api.post("Method signature:\n")
    api.post("  text = atomarray.to_text()\n")
    api.post("\n")

    api.post("Converts AtomArray to text string.\n")
    api.post("Method version of api.atom_gettext().\n")
    api.post("More Pythonic to use as a method.\n")
    api.post("\n")

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

    api.post("\n")


def test_use_cases():
    """Show practical use cases"""
    api.post("=== Practical Use Cases ===\n")

    api.post("Use Case 1: MIDI Note Processing\n")
    api.post("---\n")
    api.post("# Parse MIDI note message\n")
    api.post("msg = api.parse('note 60 100 500')\n")
    api.post("\n")
    api.post("# Extract values as integers\n")
    api.post("values = msg.to_ints()\n")
    api.post("note = values[1]    # 60\n")
    api.post("velocity = values[2]  # 100\n")
    api.post("duration = values[3]  # 500\n")
    api.post("\n")

    api.post("Use Case 2: DSP Parameter Arrays\n")
    api.post("---\n")
    api.post("# Get array of frequencies\n")
    api.post("freqs = api.AtomArray([440.0, 880.0, 1320.0])\n")
    api.post("\n")
    api.post("# Convert to Python floats for processing\n")
    api.post("freq_list = freqs.to_floats()\n")
    api.post("# Now can use with numpy, math, etc.\n")
    api.post("processed = [f * 2.0 for f in freq_list]\n")
    api.post("\n")

    api.post("Use Case 3: Message Logging\n")
    api.post("---\n")
    api.post("# Convert message to text for logging\n")
    api.post("msg = api.parse('load preset 42')\n")
    api.post("log_text = msg.to_text()\n")
    api.post("# Save to file or print\n")
    api.post("api.post(f'Received: {log_text}\\n')\n")
    api.post("\n")

    api.post("Use Case 4: Command Parsing\n")
    api.post("---\n")
    api.post("# Parse user command\n")
    api.post("cmd = api.parse('set tempo 120 swing 50')\n")
    api.post("words = cmd.to_symbols()\n")
    api.post("\n")
    api.post("# Process command tokens\n")
    api.post("if words[0] == 'set':\n")
    api.post("    param = words[1]  # 'tempo'\n")
    api.post("    # Get numeric value\n")
    api.post("    values = cmd.to_ints()\n")
    api.post("    value = values[2]  # 120\n")
    api.post("\n")

    api.post("Use Case 5: Efficient Bulk Conversion\n")
    api.post("---\n")
    api.post("# Large array of values\n")
    api.post("large = api.AtomArray()\n")
    api.post("for i in range(1000):\n")
    api.post("    large.append(i)\n")
    api.post("\n")
    api.post("# Efficient conversion using Max's C array functions\n")
    api.post("int_array = large.to_ints()\n")
    api.post("# Much faster than looping and converting individually\n")

    api.post("\n")


def test_integration():
    """Show integration with other features"""
    api.post("=== Integration Examples ===\n")

    api.post("Example 1: Parse + Convert + Process\n")
    api.post("---\n")
    api.post("def process_list(msg_str):\n")
    api.post("    # Parse string to atoms\n")
    api.post("    arr = api.parse(msg_str)\n")
    api.post("    \n")
    api.post("    # Convert to Python ints\n")
    api.post("    values = arr.to_ints()\n")
    api.post("    \n")
    api.post("    # Process with Python\n")
    api.post("    result = sum(values)\n")
    api.post("    \n")
    api.post("    return result\n")
    api.post("\n")
    api.post("# Usage:\n")
    api.post("# total = process_list('10 20 30 40')\n")
    api.post("# api.post(f'Sum: {total}\\n')  # Sum: 100\n")
    api.post("\n")

    api.post("Example 2: Outlet + Conversion\n")
    api.post("---\n")
    api.post("def send_processed(outlet, input_arr):\n")
    api.post("    # Convert to floats\n")
    api.post("    values = input_arr.to_floats()\n")
    api.post("    \n")
    api.post("    # Process\n")
    api.post("    processed = [v * 2.0 for v in values]\n")
    api.post("    \n")
    api.post("    # Create new array and send\n")
    api.post("    out_arr = api.AtomArray(processed)\n")
    api.post("    outlet.list(out_arr)\n")
    api.post("\n")

    api.post("Example 3: Dictionary + Text Conversion\n")
    api.post("---\n")
    api.post("def log_dictionary_entry(d, key):\n")
    api.post("    # Get value (might be array)\n")
    api.post("    value = d[key]\n")
    api.post("    \n")
    api.post("    # Convert to text if it's an AtomArray\n")
    api.post("    if isinstance(value, api.AtomArray):\n")
    api.post("        text = value.to_text()\n")
    api.post("    else:\n")
    api.post("        text = str(value)\n")
    api.post("    \n")
    api.post("    api.post(f'{key}: {text}\\n')\n")

    api.post("\n")


def test_performance_notes():
    """Performance considerations"""
    api.post("=== Performance Notes ===\n")

    api.post("1. Bulk Conversion Efficiency\n")
    api.post("   - to_ints(), to_floats(), to_symbols() use C array functions\n")
    api.post("   - Much faster than looping with individual atom conversion\n")
    api.post("   - Preferred for large arrays (100+ elements)\n")
    api.post("\n")

    api.post("2. When to Use Each Method\n")
    api.post("   - to_list(): Keep Atom objects, need type info\n")
    api.post("   - to_ints(): Need integer array for processing\n")
    api.post("   - to_floats(): Need float array for math/DSP\n")
    api.post("   - to_symbols(): Extract symbol strings\n")
    api.post("   - to_text(): Human-readable string output\n")
    api.post("\n")

    api.post("3. Memory Considerations\n")
    api.post("   - All conversion methods allocate new Python lists\n")
    api.post("   - Original AtomArray is not modified\n")
    api.post("   - Large arrays will consume more memory\n")
    api.post("\n")

    api.post("4. Type Handling\n")
    api.post("   - to_ints(): Floats truncated, symbols become 0\n")
    api.post("   - to_floats(): All numeric types work\n")
    api.post("   - to_symbols(): Only extracts symbols\n")
    api.post("   - to_text(): All types converted to strings\n")

    api.post("\n")


def test_comparison_chart():
    """Show comparison chart of methods"""
    api.post("=== Method Comparison Chart ===\n")
    api.post("\n")
    api.post("Method          | Returns       | Use When\n")
    api.post("----------------|---------------|----------------------------------\n")
    api.post("to_list()       | [Atom, ...]   | Need Atom objects with type info\n")
    api.post("to_ints()       | [int, ...]    | Need integer array for loops/math\n")
    api.post("to_floats()     | [float, ...]  | Need floating point for DSP/math\n")
    api.post("to_symbols()    | [str, ...]    | Extract symbol strings\n")
    api.post("to_text()       | str           | Human-readable text output\n")
    api.post("atom_gettext()  | str           | Same as to_text(), module-level\n")
    api.post("\n")

    api.post("Performance:\n")
    api.post("- to_ints(), to_floats(), to_symbols(): Fast (C array functions)\n")
    api.post("- to_list(): Medium (wraps each atom)\n")
    api.post("- to_text(), atom_gettext(): Medium (string concatenation)\n")

    api.post("\n")


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
        test_use_cases()
        test_integration()
        test_performance_notes()
        test_comparison_chart()

        api.post("====================================\n")
        api.post("All tests completed successfully!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
