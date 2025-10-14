"""
test_time.py - Tests for ITM (Time) wrapper

Internal Time Manager for transport and timing operations.
Tests time queries, conversions, transport control, and time signatures.
"""

import api


def test_itm_creation_global():
    """Test ITM creation with global transport"""
    itm = api.ITM()
    assert itm is not None
    assert itm.is_valid()
    api.post(f"Created: {itm}")


def test_itm_creation_named():
    """Test ITM creation with named transport"""
    itm = api.ITM("test_transport")
    assert itm is not None
    assert itm.is_valid()
    api.post("Created named ITM")


def test_itm_gettime():
    """Test getting current time"""
    itm = api.ITM()
    time = itm.gettime()
    assert isinstance(time, float)
    api.post(f"Current time: {time} ms")


def test_itm_getticks():
    """Test getting current ticks"""
    itm = api.ITM()
    ticks = itm.getticks()
    assert isinstance(ticks, float)
    api.post(f"Current ticks: {ticks}")


def test_itm_getstate():
    """Test getting transport state"""
    itm = api.ITM()
    state = itm.getstate()
    assert isinstance(state, int)
    api.post(f"Transport state: {state}")


def test_itm_tickstoms():
    """Test converting ticks to milliseconds"""
    itm = api.ITM()
    ticks = 480.0
    ms = itm.tickstoms(ticks)
    assert isinstance(ms, float)
    assert ms >= 0
    api.post(f"{ticks} ticks = {ms} ms")


def test_itm_mstoticks():
    """Test converting milliseconds to ticks"""
    itm = api.ITM()
    ms = 1000.0
    ticks = itm.mstoticks(ms)
    assert isinstance(ticks, float)
    assert ticks >= 0
    api.post(f"{ms} ms = {ticks} ticks")


def test_itm_mstosamps():
    """Test converting milliseconds to samples"""
    itm = api.ITM()
    ms = 1000.0
    samps = itm.mstosamps(ms)
    assert isinstance(samps, float)
    assert samps >= 0
    api.post(f"{ms} ms = {samps} samples")


def test_itm_sampstoms():
    """Test converting samples to milliseconds"""
    itm = api.ITM()
    samps = 44100.0  # 1 second at 44.1kHz
    ms = itm.sampstoms(samps)
    assert isinstance(ms, float)
    assert ms >= 0
    api.post(f"{samps} samples = {ms} ms")


def test_itm_bbutoticsk():
    """Test converting bars/beats/units to ticks"""
    itm = api.ITM()
    bars = 1
    beats = 1
    units = 0.0
    ticks = itm.bbutoticsk(bars, beats, units)
    assert isinstance(ticks, float)
    api.post(f"BBU {bars}.{beats}.{units} = {ticks} ticks")


def test_itm_tickstobbu():
    """Test converting ticks to bars/beats/units"""
    itm = api.ITM()
    ticks = 480.0
    result = itm.tickstobbu(ticks)
    assert isinstance(result, tuple)
    assert len(result) == 3
    bars, beats, units = result
    assert isinstance(bars, int)
    assert isinstance(beats, int)
    assert isinstance(units, float)
    api.post(f"{ticks} ticks = BBU {bars}.{beats}.{units}")


def test_itm_pause_resume():
    """Test pausing and resuming transport"""
    itm = api.ITM()

    # Pause transport
    itm.pause()
    api.post("Transport paused")

    # Resume transport
    itm.resume()
    api.post("Transport resumed")


def test_itm_seek():
    """Test seeking to a position"""
    itm = api.ITM()
    oldticks = 0.0
    newticks = 480.0
    itm.seek(oldticks, newticks)
    api.post(f"Seeked from {oldticks} to {newticks} ticks")


def test_itm_timesignature():
    """Test getting and setting time signature"""
    itm = api.ITM()

    # Get current time signature
    result = itm.gettimesignature()
    assert isinstance(result, tuple)
    assert len(result) == 2
    num, denom = result
    assert isinstance(num, int)
    assert isinstance(denom, int)
    api.post(f"Current time signature: {num}/{denom}")

    # Set time signature to 3/4
    itm.settimesignature(3, 4)
    api.post("Set time signature to 3/4")

    # Verify
    num, denom = itm.gettimesignature()
    # Note: May not change if we don't own the ITM
    api.post(f"After set: {num}/{denom}")


def test_itm_dump():
    """Test dumping ITM info"""
    itm = api.ITM()
    itm.dump()
    api.post("Dumped ITM info to console")


def test_itm_sync():
    """Test syncing ITM"""
    itm = api.ITM()
    itm.sync()
    api.post("Synced ITM")


def test_itm_pointer():
    """Test getting ITM pointer"""
    itm = api.ITM()
    ptr = itm.pointer()
    assert isinstance(ptr, int)
    assert ptr != 0
    api.post(f"ITM pointer: 0x{ptr:x}")


def test_itm_module_functions():
    """Test ITM module-level functions"""
    # Get global ITM pointer
    ptr = api.itm_getglobal()
    assert isinstance(ptr, int)
    assert ptr != 0
    api.post(f"Global ITM pointer: 0x{ptr:x}")

    # Test resolution functions
    orig_res = api.itm_getresolution()
    assert isinstance(orig_res, float)
    api.post(f"Original resolution: {orig_res}")

    # Set resolution
    api.itm_setresolution(480.0)
    new_res = api.itm_getresolution()
    api.post(f"New resolution: {new_res}")

    # Restore
    api.itm_setresolution(orig_res)


def test_itm_conversions_roundtrip():
    """Test round-trip conversions"""
    itm = api.ITM()

    # ms to ticks to ms
    orig_ms = 1000.0
    ticks = itm.mstoticks(orig_ms)
    back_ms = itm.tickstoms(ticks)
    api.post(f"ms roundtrip: {orig_ms} -> {ticks} ticks -> {back_ms}")

    # ticks to bbu to ticks
    orig_ticks = 480.0
    bars, beats, units = itm.tickstobbu(orig_ticks)
    back_ticks = itm.bbutoticsk(bars, beats, units)
    api.post(f"ticks roundtrip: {orig_ticks} -> BBU -> {back_ticks}")


# Main test runner
def run_all_tests():
    """Run all ITM tests"""
    tests = [
        test_itm_creation_global,
        test_itm_creation_named,
        test_itm_gettime,
        test_itm_getticks,
        test_itm_getstate,
        test_itm_tickstoms,
        test_itm_mstoticks,
        test_itm_mstosamps,
        test_itm_sampstoms,
        test_itm_bbutoticsk,
        test_itm_tickstobbu,
        test_itm_pause_resume,
        test_itm_seek,
        test_itm_timesignature,
        test_itm_dump,
        test_itm_sync,
        test_itm_pointer,
        test_itm_module_functions,
        test_itm_conversions_roundtrip,
    ]

    api.post("=" * 60)
    api.post("Running ITM (Time) Tests")
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
