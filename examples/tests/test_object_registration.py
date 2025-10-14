"""
Test script for object registration and notification functions

This script demonstrates usage of the object_* functions for
registering, finding, attaching, and notifying objects.
Run this inside the pktpy Max external using 'execfile' or 'load'.
"""

import api


def test_register_and_find():
    """Test object registration and finding"""
    api.post("=== Testing Registration and Finding ===\n")

    # Note: In practice, you would register actual Max objects
    # This is a demonstration of the API

    api.post("Object registration functions:\n")
    api.post("- object_register(namespace, name, ptr)\n")
    api.post("- object_unregister(ptr)\n")
    api.post("- object_findregistered(namespace, name)\n")
    api.post("- object_findregisteredbyptr(ptr)\n")

    api.post("\n")


def test_attach_detach():
    """Test object attachment and detachment"""
    api.post("=== Testing Attach/Detach ===\n")

    api.post("Object attachment functions:\n")
    api.post("- object_attach(namespace, name, client_ptr)\n")
    api.post("- object_detach(namespace, name, client_ptr)\n")
    api.post("- object_attach_byptr(client_ptr, registered_ptr)\n")
    api.post("- object_detach_byptr(client_ptr, registered_ptr)\n")

    api.post("\n")


def test_notify():
    """Test object notification"""
    api.post("=== Testing Notification ===\n")

    api.post("Object notification function:\n")
    api.post("- object_notify(obj_ptr, message, data_ptr)\n")
    api.post("\n")

    api.post("Notification allows registered objects to send\n")
    api.post("messages to all attached clients.\n")

    api.post("\n")


def test_workflow_explanation():
    """Explain typical workflow"""
    api.post("=== Typical Workflow ===\n")

    api.post("1. Registration:\n")
    api.post("   obj_ptr = api.object_register('myspace', 'myobj', ptr)\n")
    api.post("\n")

    api.post("2. Finding:\n")
    api.post("   found = api.object_findregistered('myspace', 'myobj')\n")
    api.post("   Or get namespace/name from pointer:\n")
    api.post("   info = api.object_findregisteredbyptr(ptr)\n")
    api.post("   # Returns [namespace, name] or None\n")
    api.post("\n")

    api.post("3. Attaching clients:\n")
    api.post("   api.object_attach('myspace', 'myobj', client_ptr)\n")
    api.post("   Or by pointer:\n")
    api.post("   api.object_attach_byptr(client_ptr, obj_ptr)\n")
    api.post("\n")

    api.post("4. Sending notifications:\n")
    api.post("   api.object_notify(obj_ptr, 'modified', 0)\n")
    api.post("   # All attached clients receive notification\n")
    api.post("\n")

    api.post("5. Cleanup:\n")
    api.post("   api.object_detach('myspace', 'myobj', client_ptr)\n")
    api.post("   api.object_unregister(obj_ptr)\n")

    api.post("\n")


def test_use_cases():
    """Explain use cases"""
    api.post("=== Use Cases ===\n")

    api.post("Object Registration & Notification is used for:\n")
    api.post("\n")

    api.post("1. Inter-object Communication\n")
    api.post("   - Objects can notify each other of changes\n")
    api.post("   - Clients can listen for updates\n")
    api.post("\n")

    api.post("2. Resource Sharing\n")
    api.post("   - Register shared resources (buffers, tables, etc.)\n")
    api.post("   - Multiple objects can access same resource\n")
    api.post("\n")

    api.post("3. Global State\n")
    api.post("   - Register state objects in global namespace\n")
    api.post("   - Any object can find and attach to them\n")
    api.post("\n")

    api.post("4. Event Broadcasting\n")
    api.post("   - Notify all attached clients of events\n")
    api.post("   - Implements observer pattern\n")
    api.post("\n")

    api.post("Common namespaces in Max:\n")
    api.post("- 'global' - Global namespace\n")
    api.post("- 'box' - Box namespace (UI objects)\n")
    api.post("- 'nobox' - Non-UI object namespace\n")
    api.post("- Custom namespaces for your objects\n")

    api.post("\n")


def test_integration_with_object():
    """Show integration with Object wrapper"""
    api.post("=== Integration with api.Object ===\n")

    api.post("You can use these functions with api.Object:\n")
    api.post("\n")

    api.post("# Create object wrapper\n")
    api.post("obj = api.Object()\n")
    api.post("obj.wrap(some_pointer)\n")
    api.post("\n")

    api.post("# Get pointer for registration\n")
    api.post("ptr = obj.pointer()\n")
    api.post("\n")

    api.post("# Register it\n")
    api.post("reg_ptr = api.object_register('myspace', 'myobj', ptr)\n")
    api.post("\n")

    api.post("# Later, find and wrap it\n")
    api.post("found_ptr = api.object_findregistered('myspace', 'myobj')\n")
    api.post("if found_ptr:\n")
    api.post("    obj2 = api.Object()\n")
    api.post("    obj2.wrap(found_ptr)\n")
    api.post("    # Now can use obj2 methods\n")

    api.post("\n")


def test_safety_notes():
    """Important safety notes"""
    api.post("=== Safety Notes ===\n")

    api.post("IMPORTANT:\n")
    api.post("\n")

    api.post("1. Pointer Lifetime\n")
    api.post("   - Ensure objects exist before registering\n")
    api.post("   - Unregister before freeing objects\n")
    api.post("   - Don't use invalid pointers\n")
    api.post("\n")

    api.post("2. Registration Returns Different Pointer\n")
    api.post("   - object_register() may duplicate the object\n")
    api.post("   - Always use the returned pointer\n")
    api.post("   - Store it for later unregistration\n")
    api.post("\n")

    api.post("3. UI Objects\n")
    api.post("   - Don't manually register UI objects\n")
    api.post("   - They auto-register in jbox_new()\n")
    api.post("   - Don't attach UI objects to themselves\n")
    api.post("\n")

    api.post("4. Namespaces\n")
    api.post("   - Choose meaningful namespace names\n")
    api.post("   - Avoid conflicts with Max's namespaces\n")
    api.post("   - Document your namespace usage\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Object Registration Functions\n")
    api.post("====================================\n\n")

    try:
        test_register_and_find()
        test_attach_detach()
        test_notify()
        test_workflow_explanation()
        test_use_cases()
        test_integration_with_object()
        test_safety_notes()

        api.post("====================================\n")
        api.post("Documentation completed!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
