"""
Test script for api.Dictionary

This script demonstrates usage of the Dictionary wrapper.
Run this inside the pktpy Max external using 'execfile' or 'load'.
"""

import api


def test_dictionary_creation():
    """Test Dictionary creation"""
    api.post("=== Testing Dictionary Creation ===\n")

    d = api.Dictionary()
    api.post(f"Empty dictionary: {d}\n")
    api.post(f"Length: {len(d)}\n")

    api.post("\n")


def test_dictionary_basic_types():
    """Test storing and retrieving basic types"""
    api.post("=== Testing Basic Types ===\n")

    d = api.Dictionary()

    # Store basic types
    d["int_val"] = 42
    d["float_val"] = 3.14
    d["string_val"] = "hello world"

    api.post(f"Length after adding 3 items: {len(d)}\n")

    # Retrieve values
    api.post(f"int_val: {d['int_val']}\n")
    api.post(f"float_val: {d['float_val']}\n")
    api.post(f"string_val: {d['string_val']}\n")

    api.post("\n")


def test_dictionary_get_methods():
    """Test typed get methods"""
    api.post("=== Testing Typed Get Methods ===\n")

    d = api.Dictionary()
    d["num"] = 100
    d["pi"] = 3.14159
    d["name"] = "test"

    # Typed getters
    api.post(f"getlong('num'): {d.getlong('num')}\n")
    api.post(f"getfloat('pi'): {d.getfloat('pi')}\n")
    api.post(f"getstring('name'): {d.getstring('name')}\n")

    # With defaults
    api.post(f"getlong('missing', 999): {d.getlong('missing', 999)}\n")
    api.post(f"getfloat('missing', 1.23): {d.getfloat('missing', 1.23)}\n")
    api.post(f"getstring('missing', 'default'): {d.getstring('missing', 'default')}\n")

    api.post("\n")


def test_dictionary_lists():
    """Test storing lists"""
    api.post("=== Testing Lists ===\n")

    d = api.Dictionary()

    # Store list
    d["numbers"] = [1, 2, 3, 4, 5]
    d["mixed"] = [10, 3.14, "text"]

    api.post(f"Stored 'numbers' and 'mixed' lists\n")
    api.post(f"Dictionary length: {len(d)}\n")

    # Note: Lists are stored as atomarrays in Max
    api.post("Lists are stored as AtomArray objects internally\n")

    api.post("\n")


def test_dictionary_keys():
    """Test key operations"""
    api.post("=== Testing Key Operations ===\n")

    d = api.Dictionary()
    d["a"] = 1
    d["b"] = 2
    d["c"] = 3

    # Get keys
    keys = d.keys()
    api.post(f"Keys: {keys}\n")

    # Check if key exists
    api.post(f"has_key('a'): {d.has_key('a')}\n")
    api.post(f"has_key('z'): {d.has_key('z')}\n")

    # Using 'in' operator
    api.post(f"'b' in d: {'b' in d}\n")
    api.post(f"'x' in d: {'x' in d}\n")

    api.post("\n")


def test_dictionary_get():
    """Test get method with defaults"""
    api.post("=== Testing get() Method ===\n")

    d = api.Dictionary()
    d["exists"] = 100

    # Get existing key
    api.post(f"get('exists'): {d.get('exists')}\n")

    # Get missing key (returns None)
    val = d.get("missing")
    api.post(f"get('missing'): {val}\n")

    # Get with default
    api.post(f"get('missing', 42): {d.get('missing', 42)}\n")
    api.post(f"get('missing', 'default'): {d.get('missing', 'default')}\n")

    api.post("\n")


def test_dictionary_update():
    """Test updating existing values"""
    api.post("=== Testing Value Updates ===\n")

    d = api.Dictionary()
    d["value"] = 10
    api.post(f"Initial value: {d['value']}\n")

    # Update value
    d["value"] = 20
    api.post(f"After update: {d['value']}\n")

    # Change type
    d["value"] = "now a string"
    api.post(f"After type change: {d['value']}\n")

    api.post("\n")


def test_dictionary_delete():
    """Test deleting entries"""
    api.post("=== Testing Entry Deletion ===\n")

    d = api.Dictionary()
    d["a"] = 1
    d["b"] = 2
    d["c"] = 3
    api.post(f"Initial length: {len(d)}\n")

    # Delete entry
    d.delete("b")
    api.post(f"After deleting 'b': {len(d)}\n")
    api.post(f"Keys: {d.keys()}\n")

    api.post("\n")


def test_dictionary_clear():
    """Test clearing dictionary"""
    api.post("=== Testing Clear ===\n")

    d = api.Dictionary()
    d["a"] = 1
    d["b"] = 2
    d["c"] = 3
    api.post(f"Before clear: {len(d)} entries\n")

    d.clear()
    api.post(f"After clear: {len(d)} entries\n")

    api.post("\n")


def test_dictionary_nested():
    """Test nested dictionaries"""
    api.post("=== Testing Nested Dictionaries ===\n")

    # Create nested structure
    d = api.Dictionary()
    sub = api.Dictionary()

    sub["x"] = 10
    sub["y"] = 20

    d["point"] = sub
    d["name"] = "test"

    api.post(f"Created nested dictionary\n")
    api.post(f"Outer length: {len(d)}\n")

    # Access nested dict
    point = d["point"]
    api.post(f"Nested dict type: {type(point)}\n")
    api.post(f"point['x']: {point['x']}\n")
    api.post(f"point['y']: {point['y']}\n")

    api.post("\n")


def test_dictionary_with_atoms():
    """Test using Atom objects"""
    api.post("=== Testing with Atom Objects ===\n")

    d = api.Dictionary()

    # Create Atom objects
    a1 = api.Atom(42)
    a2 = api.Atom(3.14)
    a3 = api.Atom("symbol")

    # Store Atom objects
    d["atom_int"] = a1
    d["atom_float"] = a2
    d["atom_sym"] = a3

    api.post(f"Stored 3 Atom objects\n")
    api.post(f"Retrieved values: {d['atom_int']}, {d['atom_float']}, {d['atom_sym']}\n")

    api.post("\n")


def test_dictionary_with_atomarray():
    """Test using AtomArray objects"""
    api.post("=== Testing with AtomArray Objects ===\n")

    d = api.Dictionary()

    # Create AtomArray
    arr = api.AtomArray([1, 2, 3, 4, 5])

    # Note: When storing AtomArray, dictionary takes ownership
    # So create a duplicate if you need to keep the original
    arr_copy = arr.duplicate()
    d["array"] = arr_copy

    api.post(f"Stored AtomArray\n")
    api.post(f"Dictionary length: {len(d)}\n")

    api.post("\n")


def test_dictionary_dump():
    """Test dump functionality"""
    api.post("=== Testing Dump ===\n")

    d = api.Dictionary()
    d["int"] = 42
    d["float"] = 3.14
    d["string"] = "hello"
    d["list"] = [1, 2, 3]

    api.post("Dumping dictionary to Max console:\n")
    d.dump()

    api.post("\n")


def test_dictionary_practical():
    """Practical example: configuration storage"""
    api.post("=== Practical Example: Configuration ===\n")

    config = api.Dictionary()

    # Store configuration
    config["sample_rate"] = 44100
    config["buffer_size"] = 512
    config["num_channels"] = 2
    config["device_name"] = "Built-in Audio"
    config["enabled"] = 1

    api.post("Configuration stored:\n")
    for key in config.keys():
        api.post(f"  {key}: {config[key]}\n")

    # Access configuration
    sr = config.getlong("sample_rate")
    buf = config.getlong("buffer_size")
    device = config.getstring("device_name")

    api.post(f"\nProcessing with:\n")
    api.post(f"  Sample rate: {sr} Hz\n")
    api.post(f"  Buffer size: {buf} samples\n")
    api.post(f"  Device: {device}\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Dictionary Type\n")
    api.post("====================================\n\n")

    try:
        test_dictionary_creation()
        test_dictionary_basic_types()
        test_dictionary_get_methods()
        test_dictionary_lists()
        test_dictionary_keys()
        test_dictionary_get()
        test_dictionary_update()
        test_dictionary_delete()
        test_dictionary_clear()
        test_dictionary_nested()
        test_dictionary_with_atoms()
        test_dictionary_with_atomarray()
        test_dictionary_dump()
        test_dictionary_practical()

        api.post("====================================\n")
        api.post("All tests completed successfully!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
