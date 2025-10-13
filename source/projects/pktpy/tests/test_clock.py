"""
Test script for Clock wrapper and scheduling functions

This script demonstrates usage of the Clock class for timing and scheduling.
Run this inside the pktpy Max external using 'execfile' or 'load'.

NOTE: These are demonstration/documentation tests. Actual timing tests
would require the Max scheduler to be running and are better done
interactively in Max patches.
"""

import api


def test_clock_creation():
    """Test Clock object creation"""
    api.post("=== Testing Clock Creation ===\n")

    api.post("Clock constructor signature:\n")
    api.post("  clock = api.Clock(owner_ptr, callback_func)\n")
    api.post("\n")

    api.post("Parameters:\n")
    api.post("  owner_ptr: Pointer to the Max object (owner)\n")
    api.post("  callback_func: Python function to call when clock fires\n")
    api.post("\n")

    api.post("Example:\n")
    api.post("  def my_callback():\n")
    api.post("      api.post('Clock fired!\\n')\n")
    api.post("  \n")
    api.post("  clock = api.Clock(owner_ptr, my_callback)\n")

    api.post("\n")


def test_clock_methods():
    """Document Clock methods"""
    api.post("=== Clock Methods ===\n")

    api.post("1. delay(milliseconds)\n")
    api.post("   - Schedule callback to fire after N milliseconds\n")
    api.post("   - Uses integer time\n")
    api.post("   - Example: clock.delay(1000)  # Fire in 1 second\n")
    api.post("\n")

    api.post("2. fdelay(milliseconds)\n")
    api.post("   - Schedule callback with floating point time\n")
    api.post("   - More precise timing\n")
    api.post("   - Example: clock.fdelay(500.5)  # Fire in 500.5ms\n")
    api.post("\n")

    api.post("3. unset()\n")
    api.post("   - Cancel scheduled callback\n")
    api.post("   - Safe to call even if not scheduled\n")
    api.post("   - Example: clock.unset()\n")
    api.post("\n")

    api.post("4. pointer()\n")
    api.post("   - Get raw C clock pointer\n")
    api.post("   - Returns integer pointer value\n")
    api.post("   - Example: ptr = clock.pointer()\n")

    api.post("\n")


def test_defer_functions():
    """Document defer functions"""
    api.post("=== Defer Functions ===\n")

    api.post("Max requires many operations to run in the main thread.\n")
    api.post("Defer functions schedule operations to run safely.\n")
    api.post("\n")

    api.post("1. api.defer(owner_ptr, callback, data_ptr=0)\n")
    api.post("   - Schedule callback for normal priority\n")
    api.post("   - Runs at next safe time in main thread\n")
    api.post("   - Example:\n")
    api.post("     def safe_operation():\n")
    api.post("         # Do UI updates, etc.\n")
    api.post("     api.defer(owner_ptr, safe_operation)\n")
    api.post("\n")

    api.post("2. api.defer_low(owner_ptr, callback, data_ptr=0)\n")
    api.post("   - Schedule callback for low priority\n")
    api.post("   - Runs when system is idle\n")
    api.post("   - Use for non-urgent background tasks\n")
    api.post("   - Example:\n")
    api.post("     def background_task():\n")
    api.post("         # Non-urgent processing\n")
    api.post("     api.defer_low(owner_ptr, background_task)\n")

    api.post("\n")


def test_clock_patterns():
    """Show common Clock usage patterns"""
    api.post("=== Common Clock Patterns ===\n")

    api.post("Pattern 1: Simple Timer\n")
    api.post("---\n")
    api.post("counter = [0]\n")
    api.post("\n")
    api.post("def tick():\n")
    api.post("    counter[0] += 1\n")
    api.post("    api.post(f'Tick {counter[0]}\\n')\n")
    api.post("    if counter[0] < 10:\n")
    api.post("        timer.delay(1000)  # Reschedule\n")
    api.post("\n")
    api.post("timer = api.Clock(owner_ptr, tick)\n")
    api.post("timer.delay(1000)  # Start\n")
    api.post("\n")

    api.post("Pattern 2: Metronome\n")
    api.post("---\n")
    api.post("tempo = [120]  # BPM\n")
    api.post("\n")
    api.post("def metro_tick():\n")
    api.post("    api.post('Bang!\\n')\n")
    api.post("    # Calculate interval from BPM\n")
    api.post("    interval = 60000.0 / tempo[0]\n")
    api.post("    metro.fdelay(interval)\n")
    api.post("\n")
    api.post("metro = api.Clock(owner_ptr, metro_tick)\n")
    api.post("metro.fdelay(60000.0 / tempo[0])\n")
    api.post("\n")

    api.post("Pattern 3: One-Shot Delay\n")
    api.post("---\n")
    api.post("def delayed_action():\n")
    api.post("    api.post('Action executed!\\n')\n")
    api.post("    # Do something after delay\n")
    api.post("\n")
    api.post("delay = api.Clock(owner_ptr, delayed_action)\n")
    api.post("delay.delay(5000)  # Execute once after 5 seconds\n")
    api.post("\n")

    api.post("Pattern 4: Cancellable Timer\n")
    api.post("---\n")
    api.post("def timeout():\n")
    api.post("    api.post('Timeout!\\n')\n")
    api.post("\n")
    api.post("watchdog = api.Clock(owner_ptr, timeout)\n")
    api.post("watchdog.delay(10000)  # 10 second timeout\n")
    api.post("\n")
    api.post("# Cancel if needed:\n")
    api.post("watchdog.unset()\n")

    api.post("\n")


def test_clock_use_cases():
    """Document Clock use cases"""
    api.post("=== Clock Use Cases ===\n")

    api.post("1. Sequencing\n")
    api.post("   - Step through musical sequences\n")
    api.post("   - Trigger events at specific times\n")
    api.post("   - Create rhythmic patterns\n")
    api.post("\n")

    api.post("2. Animation\n")
    api.post("   - Update parameter values over time\n")
    api.post("   - Create ramps and envelopes\n")
    api.post("   - Smooth transitions\n")
    api.post("\n")

    api.post("3. Timeouts\n")
    api.post("   - Detect lack of activity\n")
    api.post("   - Auto-reset after inactivity\n")
    api.post("   - Watchdog timers\n")
    api.post("\n")

    api.post("4. Polling\n")
    api.post("   - Check external state periodically\n")
    api.post("   - Monitor file changes\n")
    api.post("   - Update displays\n")
    api.post("\n")

    api.post("5. Rate Limiting\n")
    api.post("   - Throttle rapid events\n")
    api.post("   - Debounce user input\n")
    api.post("   - Control update frequency\n")

    api.post("\n")


def test_clock_notes():
    """Important notes about Clock"""
    api.post("=== Important Notes ===\n")

    api.post("1. Callback Lifetime\n")
    api.post("   - Clock keeps reference to callback\n")
    api.post("   - Callback must remain valid\n")
    api.post("   - Clock destructor unsets and frees\n")
    api.post("\n")

    api.post("2. Owner Pointer\n")
    api.post("   - Must be valid Max object pointer\n")
    api.post("   - Usually the pktpy external instance\n")
    api.post("   - Used for thread safety\n")
    api.post("\n")

    api.post("3. Timing Precision\n")
    api.post("   - delay() uses integer milliseconds\n")
    api.post("   - fdelay() uses float milliseconds\n")
    api.post("   - Actual precision depends on Max scheduler\n")
    api.post("   - Not suitable for audio-rate timing\n")
    api.post("\n")

    api.post("4. Rescheduling\n")
    api.post("   - Callback can reschedule itself\n")
    api.post("   - Creates repeating timer pattern\n")
    api.post("   - Be careful with recursion\n")
    api.post("\n")

    api.post("5. Cleanup\n")
    api.post("   - Always unset() before deletion if scheduled\n")
    api.post("   - Clock destructor handles cleanup\n")
    api.post("   - Safe to delete unscheduled clock\n")

    api.post("\n")


def test_integration_example():
    """Show complete integration example"""
    api.post("=== Complete Integration Example ===\n")

    api.post("# Step sequencer with tempo control\n")
    api.post("\n")
    api.post("class StepSequencer:\n")
    api.post("    def __init__(self, owner_ptr):\n")
    api.post("        self.steps = [60, 64, 67, 72]  # MIDI notes\n")
    api.post("        self.position = 0\n")
    api.post("        self.tempo = 120  # BPM\n")
    api.post("        self.clock = api.Clock(owner_ptr, self.tick)\n")
    api.post("    \n")
    api.post("    def tick(self):\n")
    api.post("        # Output current step\n")
    api.post("        note = self.steps[self.position]\n")
    api.post("        api.post(f'Step {self.position}: {note}\\n')\n")
    api.post("        \n")
    api.post("        # Advance position\n")
    api.post("        self.position = (self.position + 1) % len(self.steps)\n")
    api.post("        \n")
    api.post("        # Reschedule\n")
    api.post("        interval = 60000.0 / self.tempo\n")
    api.post("        self.clock.fdelay(interval)\n")
    api.post("    \n")
    api.post("    def start(self):\n")
    api.post("        interval = 60000.0 / self.tempo\n")
    api.post("        self.clock.fdelay(interval)\n")
    api.post("    \n")
    api.post("    def stop(self):\n")
    api.post("        self.clock.unset()\n")
    api.post("    \n")
    api.post("    def set_tempo(self, bpm):\n")
    api.post("        self.tempo = bpm\n")
    api.post("\n")
    api.post("# Usage:\n")
    api.post("# seq = StepSequencer(owner_ptr)\n")
    api.post("# seq.start()\n")
    api.post("# seq.set_tempo(140)\n")
    api.post("# seq.stop()\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Clock and Scheduling\n")
    api.post("====================================\n\n")

    try:
        test_clock_creation()
        test_clock_methods()
        test_defer_functions()
        test_clock_patterns()
        test_clock_use_cases()
        test_clock_notes()
        test_integration_example()

        api.post("====================================\n")
        api.post("Documentation completed!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
