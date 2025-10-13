"""
Test script for api.AtomArray

This script demonstrates usage of the AtomArray wrapper.
Run this inside the pktpy Max external using 'execfile' or 'load'.
"""

import api

def test_atomarray_creation():
    """Test AtomArray creation"""
    api.post("=== Testing AtomArray Creation ===\n")

    # Empty array
    arr = api.AtomArray()
    api.post(f"Empty: {arr}, len={len(arr)}\n")

    # From list
    arr = api.AtomArray([1, 2, 3])
    api.post(f"From list [1, 2, 3]: {arr}, len={len(arr)}\n")

    # Mixed types
    arr = api.AtomArray([42, 3.14, "hello"])
    api.post(f"Mixed types: {arr}, len={len(arr)}\n")

    api.post("\n")


def test_atomarray_access():
    """Test accessing AtomArray elements"""
    api.post("=== Testing AtomArray Access ===\n")

    arr = api.AtomArray([10, 20, 30, 40, 50])

    # Index access
    api.post(f"arr[0]: {arr[0]}\n")
    api.post(f"arr[2]: {arr[2]}\n")
    api.post(f"arr[-1]: {arr[-1]}\n")
    api.post(f"arr[-2]: {arr[-2]}\n")

    # Convert atoms to values
    api.post(f"int(arr[0]): {int(arr[0])}\n")
    api.post(f"float(arr[1]): {float(arr[1])}\n")

    api.post("\n")


def test_atomarray_modification():
    """Test modifying AtomArray"""
    api.post("=== Testing AtomArray Modification ===\n")

    arr = api.AtomArray([1, 2, 3])
    api.post(f"Initial: len={len(arr)}\n")

    # Append
    arr.append(4)
    arr.append(5)
    api.post(f"After append(4), append(5): len={len(arr)}\n")

    # Set item
    arr[0] = 100
    api.post(f"After arr[0] = 100: arr[0] = {arr[0]}\n")

    # Set with different types
    arr[1] = 3.14
    arr[2] = "test"
    api.post(f"After mixed types: arr[1]={arr[1]}, arr[2]={arr[2]}\n")

    # Clear
    arr.clear()
    api.post(f"After clear(): len={len(arr)}\n")

    api.post("\n")


def test_atomarray_to_list():
    """Test converting AtomArray to Python list"""
    api.post("=== Testing to_list() ===\n")

    arr = api.AtomArray([1, 2.5, "hello", 42])
    py_list = arr.to_list()

    api.post(f"AtomArray: {arr}\n")
    api.post(f"Python list: {py_list}\n")
    api.post(f"List types: {[type(x).__name__ for x in py_list]}\n")

    api.post("\n")


def test_atomarray_duplicate():
    """Test duplicating AtomArray"""
    api.post("=== Testing duplicate() ===\n")

    arr1 = api.AtomArray([1, 2, 3])
    arr2 = arr1.duplicate()

    api.post(f"Original: {arr1}, len={len(arr1)}\n")
    api.post(f"Duplicate: {arr2}, len={len(arr2)}\n")

    # Modify duplicate
    arr2.append(4)
    arr2[0] = 100

    api.post(f"\nAfter modifying duplicate:\n")
    api.post(f"Original: len={len(arr1)}, arr1[0]={arr1[0]}\n")
    api.post(f"Duplicate: len={len(arr2)}, arr2[0]={arr2[0]}\n")

    api.post("\n")


def test_atomarray_iteration():
    """Test iterating over AtomArray"""
    api.post("=== Testing Iteration ===\n")

    arr = api.AtomArray([10, 20, 30])

    # Manual iteration
    api.post("Manual iteration:\n")
    for i in range(len(arr)):
        api.post(f"  arr[{i}] = {int(arr[i])}\n")

    api.post("\n")


def test_atomarray_with_atoms():
    """Test AtomArray with Atom objects"""
    api.post("=== Testing with Atom Objects ===\n")

    # Create atoms
    a1 = api.Atom(42)
    a2 = api.Atom(3.14)
    a3 = api.Atom("test")

    # Create array from Atom objects
    arr = api.AtomArray()
    arr.append(a1)
    arr.append(a2)
    arr.append(a3)

    api.post(f"Array length: {len(arr)}\n")
    api.post(f"arr[0]: {arr[0]}\n")
    api.post(f"arr[1]: {arr[1]}\n")
    api.post(f"arr[2]: {arr[2]}\n")

    api.post("\n")


def test_atomarray_practical():
    """Practical example: processing data"""
    api.post("=== Practical Example ===\n")

    # Simulate receiving data
    input_data = [1, 2, 3, 4, 5]
    arr = api.AtomArray(input_data)

    # Process: multiply by 2
    api.post("Original: ")
    for i in range(len(arr)):
        api.post(f"{int(arr[i])} ")
    api.post("\n")

    # Modify in place
    for i in range(len(arr)):
        val = int(arr[i])
        arr[i] = val * 2

    api.post("After *2: ")
    for i in range(len(arr)):
        api.post(f"{int(arr[i])} ")
    api.post("\n")

    # Convert to list for further processing
    result = arr.to_list()
    total = sum(result)
    api.post(f"Sum: {total}\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing AtomArray Type\n")
    api.post("====================================\n\n")

    try:
        test_atomarray_creation()
        test_atomarray_access()
        test_atomarray_modification()
        test_atomarray_to_list()
        test_atomarray_duplicate()
        test_atomarray_iteration()
        test_atomarray_with_atoms()
        test_atomarray_practical()

        api.post("====================================\n")
        api.post("All tests completed successfully!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
