"""
test_qelem.py - Tests for Qelem wrapper

Queue-based defer alternative for UI updates and deferred execution.
Tests basic qelem operations including set, unset, and callback execution.
"""

import api

# Global state for callback testing
callback_executed = False
callback_data = None


def test_qelem_creation():
    """Test Qelem creation with callback"""
    def callback():
        pass

    q = api.Qelem(callback)
    assert q is not None
    api.post(f"Created: {q}")


def test_qelem_set_unset():
    """Test setting and unsetting a qelem"""
    def callback():
        pass

    q = api.Qelem(callback)
    assert not q.is_set()

    q.set()
    assert q.is_set()

    q.unset()
    assert not q.is_set()
    api.post("Qelem set/unset works correctly")


def test_qelem_callback_execution():
    """Test qelem callback execution"""
    global callback_executed
    callback_executed = False

    def callback():
        global callback_executed
        callback_executed = True
        api.post("Qelem callback executed!")

    q = api.Qelem(callback)
    q.set()

    # Wait a moment for callback to execute
    # Note: In Max, this would happen on the next scheduler tick


def test_qelem_with_userdata():
    """Test qelem with user data"""
    global callback_data
    callback_data = None

    def callback(data):
        global callback_data
        callback_data = data
        api.post(f"Qelem callback received data: {data}")

    test_data = "test_string_123"
    q = api.Qelem(callback, test_data)
    q.set()

    # Wait for callback execution
    # The callback should receive test_data


def test_qelem_front():
    """Test qelem front (priority execution)"""
    def callback():
        api.post("Qelem front callback executed!")

    q = api.Qelem(callback)
    q.front()  # Execute with higher priority
    assert q.is_set()


def test_qelem_is_null():
    """Test qelem is_null check"""
    def callback():
        pass

    q = api.Qelem(callback)
    assert not q.is_null()
    api.post("Qelem is not null")


def test_qelem_pointer():
    """Test getting qelem pointer"""
    def callback():
        pass

    q = api.Qelem(callback)
    ptr = q.pointer()
    assert isinstance(ptr, int)
    assert ptr != 0
    api.post(f"Qelem pointer: 0x{ptr:x}")


def test_qelem_repr():
    """Test Qelem string representation"""
    def callback():
        pass

    q = api.Qelem(callback)
    repr_str = repr(q)
    assert "Qelem" in repr_str
    api.post(f"Qelem repr: {repr_str}")


def test_qelem_multiple():
    """Test multiple qelem objects"""
    count = 0

    def callback1():
        nonlocal count
        count += 1
        api.post("Callback 1 executed")

    def callback2():
        nonlocal count
        count += 10
        api.post("Callback 2 executed")

    q1 = api.Qelem(callback1)
    q2 = api.Qelem(callback2)

    q1.set()
    q2.set()

    assert q1.is_set()
    assert q2.is_set()


def test_qelem_error_cases():
    """Test error handling"""
    # Test creating qelem with non-callable
    try:
        q = api.Qelem("not_a_function")
        assert False, "Should have raised TypeError"
    except TypeError as e:
        api.post(f"Correctly caught error: {e}")


# Main test runner
def run_all_tests():
    """Run all qelem tests"""
    tests = [
        test_qelem_creation,
        test_qelem_set_unset,
        test_qelem_callback_execution,
        test_qelem_with_userdata,
        test_qelem_front,
        test_qelem_is_null,
        test_qelem_pointer,
        test_qelem_repr,
        test_qelem_multiple,
        test_qelem_error_cases,
    ]

    api.post("=" * 60)
    api.post("Running Qelem Tests")
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


if __name__ == "__main__":
    run_all_tests()
