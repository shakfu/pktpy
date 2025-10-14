## Overview

The ITM (Internal Time Manager) wrapper provides Python access to Max's transport and timing system. ITM objects provide tempo-aware time conversion, transport control, and synchronization with Max's global or named timing sources.

## Implementation

**File**: `api_time.h`
**Type**: `api.ITM`
**C Type**: `t_itm*`

## Usage

### Creating an ITM Object

```python
import api

# Use global transport
itm = api.ITM()

# Use named transport
itm = api.ITM("my_transport")

# Wrap existing ITM pointer
itm = api.ITM(ptr_value, None)
```

## ITM Methods

### Time Query Methods

#### `gettime() -> float`

Get current time in milliseconds.

**Returns:** Current time in ms

**Example:**
```python
time_ms = itm.gettime()
api.post(f"Current time: {time_ms} ms")
```

#### `getticks() -> float`

Get current time in ticks.

**Returns:** Current time in ticks (tempo-aware)

**Example:**
```python
ticks = itm.getticks()
api.post(f"Current position: {ticks} ticks")
```

#### `getstate() -> int`

Get transport state (playing, stopped, etc.).

**Returns:** Integer state value

**Example:**
```python
state = itm.getstate()
if state == 1:
    api.post("Transport is playing")
```

### Time Conversion Methods

#### `tickstoms(ticks: float) -> float`

Convert ticks to milliseconds (tempo-aware).

**Parameters:**
- `ticks` (float): Time in ticks

**Returns:** Time in milliseconds

**Example:**
```python
ms = itm.tickstoms(480.0)
api.post(f"480 ticks = {ms} ms at current tempo")
```

#### `mstoticks(ms: float) -> float`

Convert milliseconds to ticks (tempo-aware).

**Parameters:**
- `ms` (float): Time in milliseconds

**Returns:** Time in ticks

**Example:**
```python
ticks = itm.mstoticks(1000.0)
api.post(f"1 second = {ticks} ticks")
```

#### `mstosamps(ms: float) -> float`

Convert milliseconds to samples.

**Parameters:**
- `ms` (float): Time in milliseconds

**Returns:** Time in samples (based on sample rate)

**Example:**
```python
samps = itm.mstosamps(1000.0)
api.post(f"1 second = {samps} samples")
```

#### `sampstoms(samps: float) -> float`

Convert samples to milliseconds.

**Parameters:**
- `samps` (float): Time in samples

**Returns:** Time in milliseconds

**Example:**
```python
ms = itm.sampstoms(44100.0)
api.post(f"44100 samples = {ms} ms")
```

#### `bbutoticsk(bars: int, beats: int, units: float) -> float`

Convert bars/beats/units to ticks.

**Parameters:**
- `bars` (int): Bar/measure number
- `beats` (int): Beat number
- `units` (float): Tick units past the beat

**Returns:** Time in ticks

**Example:**
```python
# Convert bar 2, beat 1 to ticks
ticks = itm.bbutoticsk(2, 1, 0.0)
api.post(f"Bar 2, beat 1 = {ticks} ticks")
```

#### `tickstobbu(ticks: float) -> tuple`

Convert ticks to bars/beats/units.

**Parameters:**
- `ticks` (float): Time in ticks

**Returns:** Tuple of (bars: int, beats: int, units: float)

**Example:**
```python
bars, beats, units = itm.tickstobbu(960.0)
api.post(f"960 ticks = {bars}.{beats}.{units}")
```

### Transport Control Methods

#### `pause()`

Pause the transport.

**Example:**
```python
itm.pause()
api.post("Transport paused")
```

#### `resume()`

Resume the transport.

**Example:**
```python
itm.resume()
api.post("Transport resumed")
```

#### `seek(oldticks: float, newticks: float, chase: int = 1)`

Seek to a new position.

**Parameters:**
- `oldticks` (float): Current position in ticks
- `newticks` (float): New position in ticks
- `chase` (int): Whether to chase events (default: 1)

**Example:**
```python
# Jump to beginning
itm.seek(itm.getticks(), 0.0)
```

### Time Signature Methods

#### `gettimesignature() -> tuple`

Get current time signature.

**Returns:** Tuple of (numerator: int, denominator: int)

**Example:**
```python
num, denom = itm.gettimesignature()
api.post(f"Time signature: {num}/{denom}")
```

#### `settimesignature(num: int, denom: int, flags: int = 0)`

Set time signature.

**Parameters:**
- `num` (int): Numerator (beats per measure)
- `denom` (int): Denominator (note value)
- `flags` (int): Optional flags (default: 0)

**Example:**
```python
# Set to 3/4 time
itm.settimesignature(3, 4)
```

### Utility Methods

#### `dump()`

Print diagnostic information about the ITM to Max console.

**Example:**
```python
itm.dump()
```

#### `sync()`

Synchronize the ITM.

**Example:**
```python
itm.sync()
```

#### `pointer() -> int`

Get raw pointer to t_itm object.

**Returns:** Integer representation of pointer

**Example:**
```python
ptr = itm.pointer()
```

#### `is_valid() -> bool`

Check if ITM is valid (not NULL).

**Returns:** Boolean indicating validity

**Example:**
```python
if itm.is_valid():
    api.post("ITM is valid")
```

## Module-Level Functions

### `itm_getglobal() -> int`

Get pointer to global ITM.

**Returns:** Integer pointer to global ITM

**Example:**
```python
ptr = api.itm_getglobal()
global_itm = api.ITM(ptr, None)
```

### `itm_setresolution(res: float)`

Set global tick resolution.

**Parameters:**
- `res` (float): Resolution in ticks per quarter note

**Example:**
```python
api.itm_setresolution(480.0)  # 480 PPQN
```

### `itm_getresolution() -> float`

Get global tick resolution.

**Returns:** Resolution in ticks per quarter note

**Example:**
```python
res = api.itm_getresolution()
api.post(f"Resolution: {res} PPQN")
```

## Common Use Cases

### Tempo-Synchronized Delays

```python
import api

itm = api.ITM()

# Calculate delay in ticks (e.g., 1 beat)
one_beat_ticks = 480.0  # Assuming 480 PPQN
delay_ms = itm.tickstoms(one_beat_ticks)

# Use with Clock
def callback():
    api.post("Beat!")

clock = api.Clock(owner_ptr, callback)
clock.fdelay(delay_ms)
```

### Musical Time Display

```python
import api

def format_musical_time(ticks):
    itm = api.ITM()
    bars, beats, units = itm.tickstobbu(ticks)
    return f"{bars}|{beats}|{int(units)}"

current = itm.getticks()
api.post(f"Position: {format_musical_time(current)}")
```

### Transport Control

```python
import api

itm = api.ITM()

def toggle_transport():
    state = itm.getstate()
    if state == 1:  # Playing
        itm.pause()
    else:
        itm.resume()

def goto_bar(bar_num):
    ticks = itm.bbutoticsk(bar_num, 1, 0.0)
    current = itm.getticks()
    itm.seek(current, ticks)
```

### Sample-Accurate Timing

```python
import api

itm = api.ITM()

# Convert musical time to samples
bars, beats, units = 2, 1, 0.0
ticks = itm.bbutoticsk(bars, beats, units)
ms = itm.tickstoms(ticks)
samples = itm.mstosamps(ms)

api.post(f"Bar 2, beat 1 = {samples} samples")
```

### Time Conversion Table

```python
import api

itm = api.ITM()

# Build conversion table
api.post("Ticks -> ms -> samples")
for ticks in [0, 240, 480, 960, 1920]:
    ms = itm.tickstoms(ticks)
    samps = itm.mstosamps(ms)
    api.post(f"{ticks:5} -> {ms:8.2f} -> {samps:10.0f}")
```

## Time Units

| Unit | Description | Fixed/Tempo |
|------|-------------|-------------|
| **ms** | Milliseconds | Fixed |
| **ticks** | Tempo-relative ticks | Tempo-aware |
| **samples** | Audio samples | Fixed (sample-rate dependent) |
| **BBU** | Bars/Beats/Units | Tempo-aware |

## Resolution and PPQN

Max uses PPQN (Pulses Per Quarter Note) for tick resolution:
- Common values: 480, 960, 1920
- Higher = more precision
- Set with `itm_setresolution()`

## Transport States

| State | Value | Description |
|-------|-------|-------------|
| Stopped | 0 | Transport stopped |
| Playing | 1 | Transport running |
| Paused | 2 | Transport paused |

## Important Notes

- **Global vs Named**: Global ITM is shared, named ITM can be independent
- **Tempo Dependency**: ticks and BBU depend on current tempo
- **Sample Rate**: Sample conversions depend on audio sample rate
- **Thread Safety**: ITM operations should be called from main thread
- **Ownership**: ITM created with name has reference counting

## Best Practices

1. **Cache ITM**: Create once and reuse
```python
# Good
itm = api.ITM()
for i in range(1000):
    ms = itm.tickstoms(i * 480)

# Bad
for i in range(1000):
    itm = api.ITM()  # Don't recreate!
    ms = itm.tickstoms(i * 480)
```

2. **Use appropriate time units**:
   - Musical events → ticks or BBU
   - Audio processing → samples
   - UI display → ms or BBU

3. **Handle transport state changes**:
```python
last_state = None

def check_transport():
    global last_state
    state = itm.getstate()
    if state != last_state:
        if state == 1:
            on_transport_start()
        elif state == 0:
            on_transport_stop()
        last_state = state
```

## See Also

- Clock API - For scheduling events
- Qelem API - For deferred execution
- Max ITM documentation

## Performance Considerations

- Conversions are fast (~100ns)
- BBU conversions slightly slower due to time signature lookups
- Cache conversion results when possible
- Use bulk conversions instead of per-sample conversions

## Debugging

```python
# Dump ITM state
itm.dump()

# Check validity
if not itm.is_valid():
    api.error("ITM is invalid!")

# Monitor time
api.post(f"Time: {itm.gettime():.2f} ms")
api.post(f"Ticks: {itm.getticks():.2f}")
api.post(f"State: {itm.getstate()}")
```
