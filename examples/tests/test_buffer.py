"""
Test script for Buffer access

This script demonstrates usage of the Buffer class for accessing MSP buffer~ objects.
Run this inside the pktpy Max external using 'execfile' or 'load'.

NOTE: These tests require a buffer~ object to exist in Max.
Create a buffer~ named "mybuffer" before running these tests.
"""

import api


def test_buffer_creation():
    """Test Buffer object creation and reference"""
    api.post("=== Testing Buffer Creation ===\n")

    api.post("Buffer creation workflow:\n")
    api.post("\n")

    api.post("1. Create Buffer wrapper\n")
    api.post("   buf = api.Buffer()\n")
    api.post("\n")

    api.post("2. Create reference to buffer~ object\n")
    api.post("   buf.ref_new(owner_ptr, 'mybuffer')\n")
    api.post("   - owner_ptr: Pointer to your Max object\n")
    api.post("   - 'mybuffer': Name of buffer~ in Max\n")
    api.post("\n")

    api.post("3. Check if buffer exists\n")
    api.post("   if buf.exists():\n")
    api.post("       api.post('Buffer found!\\n')\n")

    api.post("\n")


def test_buffer_info():
    """Test getting buffer information"""
    api.post("=== Testing Buffer Information ===\n")

    api.post("Query Methods:\n")
    api.post("\n")

    api.post("1. getinfo() - Get all info as list\n")
    api.post("   info = buf.getinfo()\n")
    api.post("   # Returns: [name, samples_ptr, frames, channels, size, sr, modtime, ...]\n")
    api.post("\n")

    api.post("2. getchannelcount() - Get number of channels\n")
    api.post("   channels = buf.getchannelcount()\n")
    api.post("\n")

    api.post("3. getframecount() - Get number of frames\n")
    api.post("   frames = buf.getframecount()\n")
    api.post("\n")

    api.post("4. getsamplerate() - Get sample rate\n")
    api.post("   sr = buf.getsamplerate()\n")
    api.post("\n")

    api.post("5. getfilename() - Get last loaded file\n")
    api.post("   filename = buf.getfilename()\n")

    api.post("\n")


def test_buffer_locking():
    """Test buffer locking mechanisms"""
    api.post("=== Testing Buffer Locking ===\n")

    api.post("Thread-Safe Access:\n")
    api.post("\n")

    api.post("Method 1: lock()/unlock()\n")
    api.post("---\n")
    api.post("buf.lock()\n")
    api.post("try:\n")
    api.post("    # Safe to access buffer metadata\n")
    api.post("    frames = buf.getframecount()\n")
    api.post("    channels = buf.getchannelcount()\n")
    api.post("finally:\n")
    api.post("    buf.unlock()\n")
    api.post("\n")

    api.post("Method 2: locksamples()/unlocksamples()\n")
    api.post("---\n")
    api.post("samples_ptr = buf.locksamples()\n")
    api.post("if samples_ptr:\n")
    api.post("    try:\n")
    api.post("        # Safe to read/write sample data\n")
    api.post("        # samples_ptr is integer pointer to float array\n")
    api.post("        pass\n")
    api.post("    finally:\n")
    api.post("        buf.unlocksamples()\n")
    api.post("\n")

    api.post("IMPORTANT: Always unlock after locking!\n")
    api.post("Use try/finally to ensure unlock happens.\n")

    api.post("\n")


def test_peek_poke():
    """Test single sample access"""
    api.post("=== Testing Peek/Poke ===\n")

    api.post("Single Sample Access:\n")
    api.post("\n")

    api.post("peek(frame, channel) - Read sample\n")
    api.post("---\n")
    api.post("value = buf.peek(100, 0)  # Frame 100, channel 0\n")
    api.post("api.post(f'Sample value: {value}\\n')\n")
    api.post("\n")

    api.post("Features:\n")
    api.post("- Automatically locks/unlocks\n")
    api.post("- Validates frame and channel indices\n")
    api.post("- Returns float value\n")
    api.post("\n")

    api.post("poke(frame, channel, value) - Write sample\n")
    api.post("---\n")
    api.post("buf.poke(100, 0, 0.5)  # Set frame 100, channel 0 to 0.5\n")
    api.post("\n")

    api.post("Features:\n")
    api.post("- Automatically locks/unlocks\n")
    api.post("- Validates indices\n")
    api.post("- Sets dirty flag\n")
    api.post("- Thread-safe\n")

    api.post("\n")


def test_buffer_practical():
    """Show practical usage examples"""
    api.post("=== Practical Examples ===\n")

    api.post("Example 1: Read buffer contents\n")
    api.post("---\n")
    api.post("def read_buffer(buf):\n")
    api.post("    if not buf.exists():\n")
    api.post("        api.post('Buffer does not exist\\n')\n")
    api.post("        return None\n")
    api.post("    \n")
    api.post("    frames = buf.getframecount()\n")
    api.post("    channels = buf.getchannelcount()\n")
    api.post("    \n")
    api.post("    api.post(f'Buffer: {frames} frames, {channels} channels\\n')\n")
    api.post("    \n")
    api.post("    # Read first 10 samples of channel 0\n")
    api.post("    for frame in range(min(10, frames)):\n")
    api.post("        value = buf.peek(frame, 0)\n")
    api.post("        api.post(f'  [{frame}] = {value:.4f}\\n')\n")
    api.post("\n")

    api.post("Example 2: Generate waveform\n")
    api.post("---\n")
    api.post("import math\n")
    api.post("\n")
    api.post("def generate_sine(buf, freq_hz):\n")
    api.post("    \"\"\"Generate sine wave in buffer\"\"\"\n")
    api.post("    if not buf.exists():\n")
    api.post("        return\n")
    api.post("    \n")
    api.post("    sr = buf.getsamplerate()\n")
    api.post("    frames = buf.getframecount()\n")
    api.post("    channels = buf.getchannelcount()\n")
    api.post("    \n")
    api.post("    for frame in range(frames):\n")
    api.post("        # Calculate sine value\n")
    api.post("        t = frame / sr\n")
    api.post("        value = math.sin(2.0 * math.pi * freq_hz * t)\n")
    api.post("        \n")
    api.post("        # Write to all channels\n")
    api.post("        for channel in range(channels):\n")
    api.post("            buf.poke(frame, channel, value)\n")
    api.post("    \n")
    api.post("    api.post(f'Generated {freq_hz}Hz sine wave\\n')\n")
    api.post("\n")

    api.post("Example 3: Analyze peaks\n")
    api.post("---\n")
    api.post("def find_peak(buf, channel=0):\n")
    api.post("    \"\"\"Find peak sample value\"\"\"\n")
    api.post("    frames = buf.getframecount()\n")
    api.post("    \n")
    api.post("    max_value = 0.0\n")
    api.post("    max_frame = 0\n")
    api.post("    \n")
    api.post("    for frame in range(frames):\n")
    api.post("        value = abs(buf.peek(frame, channel))\n")
    api.post("        if value > max_value:\n")
    api.post("            max_value = value\n")
    api.post("            max_frame = frame\n")
    api.post("    \n")
    api.post("    api.post(f'Peak: {max_value:.4f} at frame {max_frame}\\n')\n")
    api.post("    return max_frame, max_value\n")
    api.post("\n")

    api.post("Example 4: Apply gain\n")
    api.post("---\n")
    api.post("def apply_gain(buf, gain_db):\n")
    api.post("    \"\"\"Apply gain to buffer in dB\"\"\"\n")
    api.post("    import math\n")
    api.post("    \n")
    api.post("    # Convert dB to linear\n")
    api.post("    gain_linear = math.pow(10.0, gain_db / 20.0)\n")
    api.post("    \n")
    api.post("    frames = buf.getframecount()\n")
    api.post("    channels = buf.getchannelcount()\n")
    api.post("    \n")
    api.post("    for frame in range(frames):\n")
    api.post("        for channel in range(channels):\n")
    api.post("            value = buf.peek(frame, channel)\n")
    api.post("            buf.poke(frame, channel, value * gain_linear)\n")
    api.post("    \n")
    api.post("    api.post(f'Applied {gain_db}dB gain\\n')\n")

    api.post("\n")


def test_buffer_switching():
    """Test switching between buffers"""
    api.post("=== Buffer Switching ===\n")

    api.post("ref_set(name) - Switch to different buffer\n")
    api.post("---\n")
    api.post("buf = api.Buffer()\n")
    api.post("buf.ref_new(owner_ptr, 'buffer1')\n")
    api.post("\n")
    api.post("# Do work with buffer1\n")
    api.post("frames1 = buf.getframecount()\n")
    api.post("\n")
    api.post("# Switch to buffer2\n")
    api.post("buf.ref_set('buffer2')\n")
    api.post("\n")
    api.post("# Now working with buffer2\n")
    api.post("frames2 = buf.getframecount()\n")
    api.post("\n")

    api.post("Use case: Process multiple buffers\n")
    api.post("with same Buffer wrapper object.\n")

    api.post("\n")


def test_advanced_processing():
    """Show advanced processing examples"""
    api.post("=== Advanced Processing ===\n")

    api.post("Example: Normalize buffer\n")
    api.post("---\n")
    api.post("def normalize(buf):\n")
    api.post("    \"\"\"Normalize buffer to -1.0 to 1.0 range\"\"\"\n")
    api.post("    frames = buf.getframecount()\n")
    api.post("    channels = buf.getchannelcount()\n")
    api.post("    \n")
    api.post("    # Find peak\n")
    api.post("    peak = 0.0\n")
    api.post("    for frame in range(frames):\n")
    api.post("        for channel in range(channels):\n")
    api.post("            value = abs(buf.peek(frame, channel))\n")
    api.post("            if value > peak:\n")
    api.post("                peak = value\n")
    api.post("    \n")
    api.post("    if peak == 0.0:\n")
    api.post("        return  # Avoid division by zero\n")
    api.post("    \n")
    api.post("    # Normalize\n")
    api.post("    scale = 1.0 / peak\n")
    api.post("    for frame in range(frames):\n")
    api.post("        for channel in range(channels):\n")
    api.post("            value = buf.peek(frame, channel)\n")
    api.post("            buf.poke(frame, channel, value * scale)\n")
    api.post("    \n")
    api.post("    api.post(f'Normalized (peak was {peak:.4f})\\n')\n")
    api.post("\n")

    api.post("Example: Reverse buffer\n")
    api.post("---\n")
    api.post("def reverse(buf):\n")
    api.post("    \"\"\"Reverse buffer contents\"\"\"\n")
    api.post("    frames = buf.getframecount()\n")
    api.post("    channels = buf.getchannelcount()\n")
    api.post("    \n")
    api.post("    # Swap samples from ends toward center\n")
    api.post("    for frame in range(frames // 2):\n")
    api.post("        reverse_frame = frames - 1 - frame\n")
    api.post("        for channel in range(channels):\n")
    api.post("            # Swap\n")
    api.post("            a = buf.peek(frame, channel)\n")
    api.post("            b = buf.peek(reverse_frame, channel)\n")
    api.post("            buf.poke(frame, channel, b)\n")
    api.post("            buf.poke(reverse_frame, channel, a)\n")
    api.post("    \n")
    api.post("    api.post('Buffer reversed\\n')\n")
    api.post("\n")

    api.post("Example: Mix buffers\n")
    api.post("---\n")
    api.post("def mix_buffers(buf1, buf2, mix_ratio=0.5):\n")
    api.post("    \"\"\"Mix two buffers\"\"\"\n")
    api.post("    frames = min(buf1.getframecount(), buf2.getframecount())\n")
    api.post("    channels = min(buf1.getchannelcount(), buf2.getchannelcount())\n")
    api.post("    \n")
    api.post("    for frame in range(frames):\n")
    api.post("        for channel in range(channels):\n")
    api.post("            a = buf1.peek(frame, channel)\n")
    api.post("            b = buf2.peek(frame, channel)\n")
    api.post("            mixed = a * (1.0 - mix_ratio) + b * mix_ratio\n")
    api.post("            buf1.poke(frame, channel, mixed)\n")
    api.post("    \n")
    api.post("    api.post(f'Mixed buffers ({mix_ratio*100}% buf2)\\n')\n")

    api.post("\n")


def test_use_cases():
    """Document use cases"""
    api.post("=== Buffer~ Use Cases ===\n")

    api.post("1. Waveform Generation\n")
    api.post("   - Generate sine, square, saw, noise\n")
    api.post("   - Create lookup tables\n")
    api.post("   - Build wavetables\n")
    api.post("\n")

    api.post("2. Audio Processing\n")
    api.post("   - Apply gain, normalize\n")
    api.post("   - Reverse, fade in/out\n")
    api.post("   - Time stretching (simple)\n")
    api.post("\n")

    api.post("3. Analysis\n")
    api.post("   - Find peaks, RMS\n")
    api.post("   - Detect zero crossings\n")
    api.post("   - Calculate statistics\n")
    api.post("\n")

    api.post("4. Sample Manipulation\n")
    api.post("   - Copy, mix buffers\n")
    api.post("   - Extract regions\n")
    api.post("   - Splice samples\n")
    api.post("\n")

    api.post("5. Data Visualization\n")
    api.post("   - Read samples for display\n")
    api.post("   - Calculate waveform overview\n")
    api.post("   - Export data for analysis\n")

    api.post("\n")


def test_safety_notes():
    """Important safety notes"""
    api.post("=== Safety Notes ===\n")

    api.post("IMPORTANT:\n")
    api.post("\n")

    api.post("1. Always Lock Before Access\n")
    api.post("   - Use lock()/unlock() or locksamples()/unlocksamples()\n")
    api.post("   - Prevents race conditions\n")
    api.post("   - Required for thread safety\n")
    api.post("\n")

    api.post("2. Always Unlock\n")
    api.post("   - Use try/finally blocks\n")
    api.post("   - Failure to unlock can freeze Max\n")
    api.post("   - peek() and poke() handle this automatically\n")
    api.post("\n")

    api.post("3. Check Buffer Exists\n")
    api.post("   - Always call exists() first\n")
    api.post("   - Buffer might be deleted\n")
    api.post("   - Name might be wrong\n")
    api.post("\n")

    api.post("4. Validate Indices\n")
    api.post("   - peek() and poke() validate automatically\n")
    api.post("   - Raw pointer access needs manual checks\n")
    api.post("   - Out of bounds = crash\n")
    api.post("\n")

    api.post("5. Set Dirty Flag\n")
    api.post("   - Call setdirty() after modifications\n")
    api.post("   - poke() does this automatically\n")
    api.post("   - Tells Max buffer changed\n")
    api.post("\n")

    api.post("6. Performance\n")
    api.post("   - peek()/poke() lock each call\n")
    api.post("   - For bulk access, use locksamples()\n")
    api.post("   - Then access raw pointer (advanced)\n")

    api.post("\n")


def test_complete_example():
    """Complete working example"""
    api.post("=== Complete Example: Buffer Processor ===\n")

    api.post("class BufferProcessor:\n")
    api.post("    \"\"\"Process MSP buffer~ objects from Python\"\"\"\n")
    api.post("    \n")
    api.post("    def __init__(self, owner_ptr, buffer_name):\n")
    api.post("        self.buf = api.Buffer()\n")
    api.post("        self.buf.ref_new(owner_ptr, buffer_name)\n")
    api.post("    \n")
    api.post("    def info(self):\n")
    api.post("        \"\"\"Print buffer information\"\"\"\n")
    api.post("        if not self.buf.exists():\n")
    api.post("            api.post('Buffer does not exist\\n')\n")
    api.post("            return\n")
    api.post("        \n")
    api.post("        frames = self.buf.getframecount()\n")
    api.post("        channels = self.buf.getchannelcount()\n")
    api.post("        sr = self.buf.getsamplerate()\n")
    api.post("        filename = self.buf.getfilename()\n")
    api.post("        \n")
    api.post("        api.post(f'Frames: {frames}\\n')\n")
    api.post("        api.post(f'Channels: {channels}\\n')\n")
    api.post("        api.post(f'Sample Rate: {sr} Hz\\n')\n")
    api.post("        if filename:\n")
    api.post("            api.post(f'File: {filename}\\n')\n")
    api.post("    \n")
    api.post("    def fill_sine(self, freq_hz):\n")
    api.post("        \"\"\"Fill buffer with sine wave\"\"\"\n")
    api.post("        import math\n")
    api.post("        \n")
    api.post("        if not self.buf.exists():\n")
    api.post("            return\n")
    api.post("        \n")
    api.post("        frames = self.buf.getframecount()\n")
    api.post("        channels = self.buf.getchannelcount()\n")
    api.post("        sr = self.buf.getsamplerate()\n")
    api.post("        \n")
    api.post("        for frame in range(frames):\n")
    api.post("            t = frame / sr\n")
    api.post("            value = math.sin(2.0 * math.pi * freq_hz * t)\n")
    api.post("            for channel in range(channels):\n")
    api.post("                self.buf.poke(frame, channel, value)\n")
    api.post("        \n")
    api.post("        api.post(f'Filled with {freq_hz}Hz sine\\n')\n")
    api.post("\n")
    api.post("# Usage:\n")
    api.post("# processor = BufferProcessor(owner_ptr, 'mybuffer')\n")
    api.post("# processor.info()\n")
    api.post("# processor.fill_sine(440.0)\n")

    api.post("\n")


def run_all_tests():
    """Run all tests"""
    api.post("====================================\n")
    api.post("Testing Buffer Access\n")
    api.post("====================================\n\n")

    try:
        test_buffer_creation()
        test_buffer_info()
        test_buffer_locking()
        test_peek_poke()
        test_buffer_practical()
        test_buffer_switching()
        test_advanced_processing()
        test_use_cases()
        test_safety_notes()
        test_complete_example()

        api.post("====================================\n")
        api.post("Documentation completed!\n")
        api.post("====================================\n")
    except Exception as e:
        api.error(f"Test failed: {e}\n")
        raise


# Run tests when script is executed
if __name__ == "__main__":
    run_all_tests()
