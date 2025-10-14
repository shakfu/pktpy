"""
test_systhread.py - Tests for SysThread and SysMutex wrappers

Thread management for Max/MSP.
WARNING: Use with caution - improper thread management can crash Max!
"""

import api

# Global state for testing
thread_executed = False
thread_data = None
mutex_shared_counter = 0


def test_systhread_creation():
    """Test SysThread creation"""
    def thread_func():
        api.post("Thread function called")
        return 0

    thread = api.SysThread(thread_func)
    assert thread is not None
    api.post(f"Created: {thread}")


def test_systhread_start_join():
    """Test starting and joining a thread"""
    global thread_executed
    thread_executed = False

    def thread_func():
        global thread_executed
        thread_executed = True
        api.post("Thread executing...")
        return 42

    thread = api.SysThread(thread_func)
    assert not thread.is_running()

    thread.start()
    assert thread.is_running()

    result = thread.join()
    assert not thread.is_running()
    assert result == 42
    api.post(f"Thread returned: {result}")


def test_systhread_with_userdata():
    """Test thread with user data"""
    global thread_data
    thread_data = None

    def thread_func(data):
        global thread_data
        thread_data = data
        api.post(f"Thread received data: {data}")
        return 0

    test_data = "thread_test_123"
    thread = api.SysThread(thread_func, test_data)
    thread.start()
    result = thread.join()
    api.post(f"Thread completed with result: {result}")


def test_systhread_get_result():
    """Test getting thread result"""
    def thread_func():
        return 999

    thread = api.SysThread(thread_func)
    thread.start()
    thread.join()

    result = thread.get_result()
    assert result == 999
    api.post(f"Thread result: {result}")


def test_systhread_sleep():
    """Test thread sleep function"""
    def thread_func():
        api.post("Thread sleeping for 100ms...")
        api.SysThread.sleep(100)
        api.post("Thread woke up!")
        return 0

    thread = api.SysThread(thread_func)
    thread.start()
    thread.join()


def test_systhread_repr():
    """Test SysThread string representation"""
    def thread_func():
        return 0

    thread = api.SysThread(thread_func)
    repr_str = repr(thread)
    assert "SysThread" in repr_str
    assert "running=False" in repr_str
    api.post(f"SysThread repr: {repr_str}")


def test_sysmutex_creation():
    """Test SysMutex creation"""
    mutex = api.SysMutex()
    assert mutex is not None
    assert not mutex.is_locked()
    api.post(f"Created: {mutex}")


def test_sysmutex_lock_unlock():
    """Test mutex lock and unlock"""
    mutex = api.SysMutex()
    assert not mutex.is_locked()

    mutex.lock()
    assert mutex.is_locked()

    mutex.unlock()
    assert not mutex.is_locked()
    api.post("Mutex lock/unlock works correctly")


def test_sysmutex_trylock():
    """Test mutex trylock (non-blocking)"""
    mutex = api.SysMutex()

    # First trylock should succeed
    result = mutex.trylock()
    assert result is True
    assert mutex.is_locked()

    # Second trylock should fail (already locked)
    result2 = mutex.trylock()
    assert result2 is False

    mutex.unlock()
    assert not mutex.is_locked()
    api.post("Mutex trylock works correctly")


def test_sysmutex_pointer():
    """Test getting mutex pointer"""
    mutex = api.SysMutex()
    ptr = mutex.pointer()
    assert isinstance(ptr, int)
    assert ptr != 0
    api.post(f"Mutex pointer: 0x{ptr:x}")


def test_sysmutex_repr():
    """Test SysMutex string representation"""
    mutex = api.SysMutex()
    repr_str = repr(mutex)
    assert "SysMutex" in repr_str
    assert "locked=False" in repr_str
    api.post(f"SysMutex repr: {repr_str}")


def test_sysmutex_with_thread():
    """Test using mutex with thread for synchronization"""
    global mutex_shared_counter
    mutex_shared_counter = 0

    mutex = api.SysMutex()

    def thread_func():
        global mutex_shared_counter
        mutex.lock()
        try:
            for i in range(10):
                mutex_shared_counter += 1
        finally:
            mutex.unlock()
        return 0

    # Start thread
    thread = api.SysThread(thread_func)
    thread.start()

    # Wait for thread to complete
    thread.join()

    api.post(f"Counter value: {mutex_shared_counter}")
    assert mutex_shared_counter == 10


def test_systhread_error_cases():
    """Test error handling"""
    # Test creating thread with non-callable
    try:
        thread = api.SysThread("not_a_function")
        assert False, "Should have raised TypeError"
    except TypeError as e:
        api.post(f"Correctly caught error: {e}")

    # Test starting thread twice
    def thread_func():
        api.SysThread.sleep(100)
        return 0

    thread = api.SysThread(thread_func)
    thread.start()

    try:
        thread.start()  # Should fail - already running
        assert False, "Should have raised ValueError"
    except ValueError as e:
        api.post(f"Correctly caught error: {e}")

    thread.join()


def test_sysmutex_error_cases():
    """Test mutex error handling"""
    mutex = api.SysMutex()

    # Try to unlock when not locked
    try:
        mutex.unlock()
        assert False, "Should have raised ValueError"
    except ValueError as e:
        api.post(f"Correctly caught error: {e}")


# Main test runner
def run_all_tests():
    """Run all systhread and sysmutex tests"""
    tests = [
        test_systhread_creation,
        test_systhread_start_join,
        test_systhread_with_userdata,
        test_systhread_get_result,
        test_systhread_sleep,
        test_systhread_repr,
        test_sysmutex_creation,
        test_sysmutex_lock_unlock,
        test_sysmutex_trylock,
        test_sysmutex_pointer,
        test_sysmutex_repr,
        test_sysmutex_with_thread,
        test_systhread_error_cases,
        test_sysmutex_error_cases,
    ]

    api.post("=" * 60)
    api.post("Running SysThread and SysMutex Tests")
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
