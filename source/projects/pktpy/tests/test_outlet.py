"""
Test script for Outlet wrapper

This script demonstrates usage of the Outlet class for sending data
from Python to Max patches.
Run this inside the pktpy Max external using 'execfile' or 'load'.

NOTE: To use outlets, you must create them during object initialization
(typically in __init__ or during new method). This test file documents
the API rather than creating actual outlets.
"""

import api


def test_outlet_creation():
    """Test Outlet object creation"""
    api.post("=== Testing Outlet Creation ===\n")

    api.post("Outlet constructor signature:\n")
    api.post("  outlet = api.Outlet()\n")
    api.post("  outlet.wrap(outlet_ptr)\n")
    api.post("\n")

    api.post("Or create in Max C code:\n")
    api.post("  outlet_ptr = outlet_new(owner, NULL)\n")
    api.post("  # Pass pointer to Python\n")
    api.post("\n")

    api.post("Typically outlets are created during object init:\n")
    api.post("  self.outlet = api.Outlet()\n")
    api.post("  self.outlet.wrap(outlet_ptr)\n")

    api.post("\n")


def test_outlet_methods():
    """Document Outlet methods"""
    api.post("=== Outlet Methods ===\n")

    api.post("1. bang()\n")
    api.post("   - Send bang message\n")
    api.post("   - Example: outlet.bang()\n")
    api.post("\n")

    api.post("2. int(value)\n")
    api.post("   - Send integer value\n")
    api.post("   - Example: outlet.int(42)\n")
    api.post("\n")

    api.post("3. float(value)\n")
    api.post("   - Send float value\n")
    api.post("   - Example: outlet.float(3.14)\n")
    api.post("\n")

    api.post("4. list(atomarray)\n")
    api.post("   - Send list of atoms\n")
    api.post("   - Takes AtomArray argument\n")
    api.post("   - Example:\n")
    api.post("     arr = api.AtomArray([1, 2, 3])\n")
    api.post("     outlet.list(arr)\n")
    api.post("\n")

    api.post("5. anything(symbol, atomarray)\n")
    api.post("   - Send arbitrary message\n")
    api.post("   - Takes Symbol and AtomArray\n")
    api.post("   - Example:\n")
    api.post("     sym = api.Symbol('note')\n")
    api.post("     arr = api.AtomArray([60, 100, 500])\n")
    api.post("     outlet.anything(sym, arr)\n")
    api.post("\n")

    api.post("6. pointer()\n")
    api.post("   - Get raw C outlet pointer\n")
    api.post("   - Returns integer pointer value\n")
    api.post("   - Example: ptr = outlet.pointer()\n")

    api.post("\n")


def test_outlet_patterns():
    """Show common Outlet usage patterns"""
    api.post("=== Common Outlet Patterns ===\n")

    api.post("Pattern 1: Simple Value Output\n")
    api.post("---\n")
    api.post("def process(value):\n")
    api.post("    result = value * 2\n")
    api.post("    outlet.float(result)\n")
    api.post("\n")

    api.post("Pattern 2: Multiple Outlets (Right to Left)\n")
    api.post("---\n")
    api.post("# Max outlets are created right to left\n")
    api.post("# outlet_right = outlet_new(owner, NULL)  # Created first\n")
    api.post("# outlet_left = outlet_new(owner, NULL)   # Created second\n")
    api.post("\n")
    api.post("def process(value):\n")
    api.post("    # Output to multiple outlets\n")
    api.post("    outlet_left.float(value)\n")
    api.post("    outlet_right.bang()  # Signal completion\n")
    api.post("\n")

    api.post("Pattern 3: List Output\n")
    api.post("---\n")
    api.post("def generate_sequence(length):\n")
    api.post("    # Generate list of values\n")
    api.post("    arr = api.AtomArray()\n")
    api.post("    for i in range(length):\n")
    api.post("        arr.append(i * 10)\n")
    api.post("    outlet.list(arr)\n")
    api.post("\n")

    api.post("Pattern 4: Message Routing\n")
    api.post("---\n")
    api.post("def route_message(msg_type, *args):\n")
    api.post("    if msg_type == 'note':\n")
    api.post("        sym = api.Symbol('note')\n")
    api.post("        arr = api.AtomArray(args)\n")
    api.post("        outlet.anything(sym, arr)\n")
    api.post("    elif msg_type == 'control':\n")
    api.post("        sym = api.Symbol('control')\n")
    api.post("        arr = api.AtomArray(args)\n")
    api.post("        outlet.anything(sym, arr)\n")
    api.post("\n")

    api.post("Pattern 5: Batch Output\n")
    api.post("---\n")
    api.post("def output_batch(values):\n")
    api.post("    for v in values:\n")
    api.post("        outlet.float(v)\n")
    api.post("        # Each value sent separately\n")

    api.post("\n")


def test_outlet_use_cases():
    """Document Outlet use cases"""
    api.post("=== Outlet Use Cases ===\n")

    api.post("1. Data Processing\n")
    api.post("   - Transform input values\n")
    api.post("   - Filter and route data\n")
    api.post("   - Mathematical operations\n")
    api.post("\n")

    api.post("2. Sequence Generation\n")
    api.post("   - Generate MIDI note sequences\n")
    api.post("   - Create control sequences\n")
    api.post("   - Algorithmic composition\n")
    api.post("\n")

    api.post("3. State Reporting\n")
    api.post("   - Report internal state\n")
    api.post("   - Send status updates\n")
    api.post("   - Notify completion\n")
    api.post("\n")

    api.post("4. Event Broadcasting\n")
    api.post("   - Trigger multiple receivers\n")
    api.post("   - Coordinate multiple objects\n")
    api.post("   - Synchronization signals\n")
    api.post("\n")

    api.post("5. Message Translation\n")
    api.post("   - Convert between protocols\n")
    api.post("   - Format data for receivers\n")
    api.post("   - Adapt interfaces\n")

    api.post("\n")


def test_outlet_integration():
    """Show integration with other features"""
    api.post("=== Integration Examples ===\n")

    api.post("Example 1: Outlet + Clock (Sequencer)\n")
    api.post("---\n")
    api.post("class Sequencer:\n")
    api.post("    def __init__(self, owner_ptr, outlet_ptr):\n")
    api.post("        self.outlet = api.Outlet()\n")
    api.post("        self.outlet.wrap(outlet_ptr)\n")
    api.post("        self.clock = api.Clock(owner_ptr, self.tick)\n")
    api.post("        self.notes = [60, 64, 67, 72]\n")
    api.post("        self.pos = 0\n")
    api.post("    \n")
    api.post("    def tick(self):\n")
    api.post("        # Output current note\n")
    api.post("        self.outlet.int(self.notes[self.pos])\n")
    api.post("        self.pos = (self.pos + 1) % len(self.notes)\n")
    api.post("        self.clock.delay(500)\n")
    api.post("\n")

    api.post("Example 2: Outlet + AtomArray (Chord Generator)\n")
    api.post("---\n")
    api.post("def generate_chord(root, chord_type):\n")
    api.post("    # Generate chord notes\n")
    api.post("    if chord_type == 'major':\n")
    api.post("        intervals = [0, 4, 7]\n")
    api.post("    elif chord_type == 'minor':\n")
    api.post("        intervals = [0, 3, 7]\n")
    api.post("    else:\n")
    api.post("        intervals = [0, 4, 7, 10]  # maj7\n")
    api.post("    \n")
    api.post("    arr = api.AtomArray()\n")
    api.post("    for interval in intervals:\n")
    api.post("        arr.append(root + interval)\n")
    api.post("    \n")
    api.post("    outlet.list(arr)\n")
    api.post("\n")

    api.post("Example 3: Outlet + Dictionary (Data Export)\n")
    api.post("---\n")
    api.post("def export_config(config_dict):\n")
    api.post("    # Send configuration as messages\n")
    api.post("    for key in config_dict.keys():\n")
    api.post("        value = config_dict[key]\n")
    api.post("        \n")
    api.post("        sym = api.Symbol(key)\n")
    api.post("        arr = api.AtomArray([value])\n")
    api.post("        outlet.anything(sym, arr)\n")

    api.post("\n")


def test_outlet_practical():
    """Practical complete example"""
    api.post("=== Practical Example: MIDI Arpeggiator ===\n")

    api.post("class Arpeggiator:\n")
    api.post("    \"\"\"MIDI arpeggiator with tempo and pattern control\"\"\"\n")
    api.post("    \n")
    api.post("    def __init__(self, owner_ptr, outlet_ptr):\n")
    api.post("        # Setup outlet\n")
    api.post("        self.outlet = api.Outlet()\n")
    api.post("        self.outlet.wrap(outlet_ptr)\n")
    api.post("        \n")
    api.post("        # Setup clock\n")
    api.post("        self.clock = api.Clock(owner_ptr, self.tick)\n")
    api.post("        \n")
    api.post("        # State\n")
    api.post("        self.notes = []  # Held notes\n")
    api.post("        self.position = 0\n")
    api.post("        self.tempo = 120  # BPM\n")
    api.post("        self.pattern = 'up'  # up, down, updown, random\n")
    api.post("        self.running = False\n")
    api.post("    \n")
    api.post("    def add_note(self, note):\n")
    api.post("        \"\"\"Add note to arpeggio\"\"\"\n")
    api.post("        if note not in self.notes:\n")
    api.post("            self.notes.append(note)\n")
    api.post("            self.notes.sort()\n")
    api.post("    \n")
    api.post("    def remove_note(self, note):\n")
    api.post("        \"\"\"Remove note from arpeggio\"\"\"\n")
    api.post("        if note in self.notes:\n")
    api.post("            self.notes.remove(note)\n")
    api.post("    \n")
    api.post("    def tick(self):\n")
    api.post("        \"\"\"Clock callback - output next note\"\"\"\n")
    api.post("        if not self.notes:\n")
    api.post("            return\n")
    api.post("        \n")
    api.post("        # Get next note based on pattern\n")
    api.post("        if self.pattern == 'up':\n")
    api.post("            note = self.notes[self.position]\n")
    api.post("            self.position = (self.position + 1) % len(self.notes)\n")
    api.post("        elif self.pattern == 'down':\n")
    api.post("            note = self.notes[-(self.position + 1)]\n")
    api.post("            self.position = (self.position + 1) % len(self.notes)\n")
    api.post("        \n")
    api.post("        # Output note\n")
    api.post("        self.outlet.int(note)\n")
    api.post("        \n")
    api.post("        # Reschedule\n")
    api.post("        if self.running:\n")
    api.post("            interval = 60000.0 / self.tempo\n")
    api.post("            self.clock.fdelay(interval)\n")
    api.post("    \n")
    api.post("    def start(self):\n")
    api.post("        \"\"\"Start arpeggiator\"\"\"\n")
    api.post("        self.running = True\n")
    api.post("        self.position = 0\n")
    api.post("        interval = 60000.0 / self.tempo\n")
    api.post("        self.clock.fdelay(interval)\n")
    api.post("    \n")
    api.post("    def stop(self):\n")
    api.post("        \"\"\"Stop arpeggiator\"\"\"\n")
    api.post("        self.running = False\n")
    api.post("        self.clock.unset()\n")
    api.post("    \n")
    api.post("    def set_tempo(self, bpm):\n")
    api.post("        \"\"\"Change tempo\"\"\"\n")
    api.post("        self.tempo = bpm\n")
    api.post("    \n")
    api.post("    def set_pattern(self, pattern):\n")
    api.post("        \"\"\"Change pattern\"\"\"\n")
    api.post("        if pattern in ['up', 'down', 'updown', 'random']:\n")
    api.post("            self.pattern = pattern\n")
    api.post("            self.position = 0\n")
    api.post("\n")
    api.post("# Usage:\n")
    api.post("# arp = Arpeggiator(owner_ptr, outlet_ptr)\n")
    api.post("# arp.add_note(60)\n")
    api.post("# arp.add_note(64)\n")
    api.post("# arp.add_note(67)\n")
    api.post("# arp.start()\n")

    api.post("\n")


def test_outlet_notes():
    """Important notes about Outlets"""
    api.post("=== Important Notes ===\n")

    api.post("1. Outlet Creation Order\n")
    api.post("   - Outlets are created right to left in Max\n")
    api.post("   - First outlet_new() creates rightmost outlet\n")
    api.post("   - Plan outlet order carefully\n")
    api.post("\n")

    api.post("2. Thread Safety\n")
    api.post("   - Outlet methods must run in main thread\n")
    api.post("   - Use defer() for thread-safe calls\n")
    api.post("   - Never call from audio thread\n")
    api.post("\n")

    api.post("3. Pointer Lifetime\n")
    api.post("   - Outlet pointer must remain valid\n")
    api.post("   - Don't free outlets while wrapped\n")
    api.post("   - Wrapper doesn't own the outlet\n")
    api.post("\n")

    api.post("4. Message Routing\n")
    api.post("   - Use anything() for custom messages\n")
    api.post("   - Symbol determines message type\n")
    api.post("   - Receivers use method handlers\n")
    api.post("\n")

    api.post("5. Performance\n")
    api.post("   - Outlet calls trigger immediate routing\n")
    api.post("   - Avoid excessive output in loops\n")
    api.post("   - Consider batching when possible\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Outlet Functions\n")
    api.post("====================================\n\n")

    try:
        test_outlet_creation()
        test_outlet_methods()
        test_outlet_patterns()
        test_outlet_use_cases()
        test_outlet_integration()
        test_outlet_practical()
        test_outlet_notes()

        api.post("====================================\n")
        api.post("Documentation completed!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
