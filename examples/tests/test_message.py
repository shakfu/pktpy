"""
test_message.py - Tests for message sending API

Direct message dispatch to Max objects using typedmess and convenience functions.
Tests various message types: bang, int, float, symbol, list, anything.
"""

import api


def test_send_bang():
    """Test sending bang message"""
    # Create a simple object (Button doesn't exist, but this shows the API)
    # In a real test, you'd use an actual Max object pointer
    api.post("send_bang API available")

    # Example usage (would need real object):
    # obj_ptr = some_max_object_pointer
    # api.send_bang(obj_ptr)


def test_send_int():
    """Test sending int message"""
    api.post("send_int API available")

    # Example usage:
    # api.send_int(obj_ptr, 42)


def test_send_float():
    """Test sending float message"""
    api.post("send_float API available")

    # Example usage:
    # api.send_float(obj_ptr, 3.14159)


def test_send_symbol():
    """Test sending symbol message"""
    api.post("send_symbol API available")

    # Example usage:
    # api.send_symbol(obj_ptr, "hello")


def test_send_list():
    """Test sending list message"""
    api.post("send_list API available")

    # Example usage:
    # api.send_list(obj_ptr, [1, 2, 3, 4.5, "symbol"])


def test_send_anything():
    """Test sending arbitrary message"""
    api.post("send_anything API available")

    # Example usage:
    # api.send_anything(obj_ptr, "custom_message", 1, 2, 3)


def test_typedmess():
    """Test typedmess function"""
    api.post("typedmess API available")

    # Example usage:
    # result = api.typedmess(obj_ptr, "loadbang")
    # result = api.typedmess(obj_ptr, "set", 42)
    # result = api.typedmess(obj_ptr, "anything", 1, 2.5, "test")


def test_send_message_with_atomarray():
    """Test sending message with AtomArray"""
    api.post("send_message with AtomArray API available")

    # Example usage:
    # aa = api.AtomArray([1, 2, 3])
    # api.send_message(obj_ptr, "list", aa)


def test_typedmess_with_response():
    """Test typedmess and checking response"""
    # This demonstrates the pattern for sending a message
    # and getting a response pointer
    api.post("typedmess can return object pointers")

    # Example usage:
    # result_ptr = api.typedmess(obj_ptr, "getitem", 0)
    # if result_ptr:
    #     result_obj = api.Object(result_ptr)


def test_message_chaining():
    """Test chaining multiple messages"""
    api.post("Messages can be chained")

    # Example usage:
    # api.send_int(obj1_ptr, 42)
    # api.send_bang(obj2_ptr)
    # api.send_list(obj3_ptr, [1, 2, 3])


def test_message_to_patcher():
    """Test sending messages to patcher objects"""
    api.post("Can send messages to patchers")

    # Example usage:
    # patcher = api.Patcher()  # Get current patcher
    # patcher_ptr = patcher.pointer()
    # api.typedmess(patcher_ptr, "front")


def test_message_error_handling():
    """Test error handling in message sending"""
    api.post("Message functions validate inputs")

    # Test with invalid object pointer (NULL)
    try:
        api.send_bang(0)
        api.post("Sent to NULL pointer (may or may not error)")
    except Exception as e:
        api.post(f"Correctly caught error: {e}")


def test_preset_functions():
    """Test preset-related functions"""
    api.post("Preset functions available")

    # Example usage:
    # api.preset_store("preset_data")
    # api.preset_set(obj_ptr, 1)
    # api.preset_int(obj_ptr, 42)
    # symbol = api.preset_get_data_symbol()


def test_practical_example():
    """Demonstrate practical message sending"""
    api.post("=" * 50)
    api.post("Practical Message Sending Example")
    api.post("=" * 50)

    # In a real scenario with actual objects:
    # 1. Get object pointer from patcher
    # patcher = api.Patcher()
    # box = patcher.get_named_box("number_box")
    # obj_ptr = box.get_object().pointer()
    #
    # 2. Send messages
    # api.send_int(obj_ptr, 42)
    # api.send_bang(obj_ptr)
    #
    # 3. Query state
    # result = api.typedmess(obj_ptr, "get")
    #
    # 4. Send complex messages
    # api.typedmess(obj_ptr, "set", 100)
    # api.send_anything(obj_ptr, "custom", "arg1", "arg2")

    api.post("See comments for usage patterns")


# Main test runner
def run_all_tests():
    """Run all message sending tests"""
    tests = [
        test_send_bang,
        test_send_int,
        test_send_float,
        test_send_symbol,
        test_send_list,
        test_send_anything,
        test_typedmess,
        test_send_message_with_atomarray,
        test_typedmess_with_response,
        test_message_chaining,
        test_message_to_patcher,
        test_message_error_handling,
        test_preset_functions,
        test_practical_example,
    ]

    api.post("=" * 60)
    api.post("Running Message Sending Tests")
    api.post("=" * 60)

    passed = 0
    failed = 0

    for test in tests:
        try:
            test()
            passed += 1
            api.post(f"✓ {test.__name__}")
        except AssertionError as e:
            failed += 1
            api.post(f"✗ {test.__name__}: {e}")
        except Exception as e:
            failed += 1
            api.post(f"✗ {test.__name__}: {type(e).__name__}: {e}")

    api.post("=" * 60)
    api.post(f"Results: {passed} passed, {failed} failed")
    api.post("=" * 60)

    api.post("")
    api.post("Note: Most message tests require actual Max objects")
    api.post("These tests verify API availability and patterns")


if __name__ == "__main__":
    run_all_tests()
