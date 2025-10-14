"""
Test script for api.Object

This script demonstrates usage of the Object wrapper for generic Max objects.
Run this inside the pktpy Max external using 'execfile' or 'load'.
"""

import api


def test_object_creation():
    """Test Object creation and basic operations"""
    api.post("=== Testing Object Creation ===\n")

    # Create empty object wrapper
    obj = api.Object()
    api.post(f"Created: {obj}\n")
    api.post(f"Is null: {obj.is_null()}\n")

    api.post("\n")


def test_object_wrap():
    """Test wrapping existing object pointers"""
    api.post("=== Testing Object Wrapping ===\n")

    # Note: In practice, you'd get pointers from Max's patcher API
    # or from other Max objects. This is just for demonstration.

    obj = api.Object()
    api.post(f"Empty object is_null: {obj.is_null()}\n")

    # You would typically wrap existing Max objects like this:
    # obj.wrap(some_pointer_from_max)

    api.post("\n")


def test_object_create():
    """Test creating Max objects"""
    api.post("=== Testing Object Creation with create() ===\n")

    # Note: object_new_typed requires objects to be registered in Max
    # This example may not work unless you have registered classes available

    obj = api.Object()

    # Example (may fail if class not available):
    # obj.create("buffer~", "mybuffer")

    api.post("Object creation requires registered Max classes\n")
    api.post("See Max SDK documentation for object_new_typed()\n")

    api.post("\n")


def test_object_pointer():
    """Test getting object pointer"""
    api.post("=== Testing Pointer Access ===\n")

    obj = api.Object()
    ptr = obj.pointer()
    api.post(f"Object pointer: {ptr} (0x{ptr:x})\n")

    api.post("\n")


def test_object_repr():
    """Test object representation"""
    api.post("=== Testing Object Representation ===\n")

    obj = api.Object()
    api.post(f"Empty object: {obj}\n")
    api.post(f"repr: {repr(obj)}\n")

    api.post("\n")


def test_object_attributes():
    """Test attribute access (when object exists)"""
    api.post("=== Testing Attribute Access ===\n")

    # Note: This requires a valid Max object with attributes
    # In practice, you would use this with objects like:
    # - buffer~ objects
    # - patcher objects
    # - jit.matrix objects
    # etc.

    api.post("Attribute access requires a valid Max object\n")
    api.post("Example usage:\n")
    api.post("  obj.setattr('name', 'value')\n")
    api.post("  value = obj.getattr('name')\n")
    api.post("  names = obj.attrnames()\n")

    api.post("\n")


def test_object_methods():
    """Test method calling (when object exists)"""
    api.post("=== Testing Method Calling ===\n")

    # Note: This requires a valid Max object with methods
    # In practice, you would use this with objects to call their methods

    api.post("Method calling requires a valid Max object\n")
    api.post("Example usage:\n")
    api.post("  result = obj.method('getsize')\n")
    api.post("  obj.method('clear')\n")
    api.post("  obj.method('set', 100, 200)\n")

    api.post("\n")


def test_practical_example():
    """Practical documentation example"""
    api.post("=== Practical Example ===\n")

    api.post("The Object wrapper is designed for:\n")
    api.post("1. Wrapping existing Max objects from the patcher\n")
    api.post("2. Accessing attributes of Max objects\n")
    api.post("3. Calling methods on Max objects\n")
    api.post("4. Creating new Max objects dynamically\n")
    api.post("\n")

    api.post("Common use cases:\n")
    api.post("- Manipulating buffer~ objects\n")
    api.post("- Controlling patcher objects\n")
    api.post("- Working with jit.matrix\n")
    api.post("- Accessing MSP objects\n")
    api.post("\n")

    api.post("Example workflow:\n")
    api.post("  # Get object pointer from Max context\n")
    api.post("  obj = api.Object()\n")
    api.post("  obj.wrap(max_object_pointer)\n")
    api.post("\n")
    api.post("  # Get class name\n")
    api.post("  name = obj.classname()\n")
    api.post("\n")
    api.post("  # Access attributes\n")
    api.post("  size = obj.getattr('size')\n")
    api.post("  obj.setattr('size', 1024)\n")
    api.post("\n")
    api.post("  # Call methods\n")
    api.post("  obj.method('clear')\n")
    api.post("  result = obj.method('getvalue', 100)\n")

    api.post("\n")


def test_object_ownership():
    """Test ownership semantics"""
    api.post("=== Testing Ownership ===\n")

    api.post("Object ownership rules:\n")
    api.post("1. Objects created with create() are owned (will be freed)\n")
    api.post("2. Objects wrapped with wrap() are NOT owned\n")
    api.post("3. Call free() to manually free owned objects\n")
    api.post("4. Destructor automatically frees owned objects\n")

    obj = api.Object()
    api.post(f"New object is_null: {obj.is_null()}\n")

    # obj.free() would free if owned
    api.post("Use obj.free() to manually free owned objects\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Object Type\n")
    api.post("====================================\n\n")

    try:
        test_object_creation()
        test_object_wrap()
        test_object_create()
        test_object_pointer()
        test_object_repr()
        test_object_attributes()
        test_object_methods()
        test_practical_example()
        test_object_ownership()

        api.post("====================================\n")
        api.post("All tests completed successfully!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
