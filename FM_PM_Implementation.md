# FM/PM Dual Implementation - Backend Summary

## Changes Made

### 1. Added ModulationType Enum (`Operator.h`)
```cpp
enum class ModulationType
{
    FM,  // Frequency modulation (modulate Hz)
    PM   // Phase modulation (DX7-style, modulate phase angle)
};
```

### 2. Added Operator Members (`Operator.h`)
- `ModulationType modulationType = ModulationType::PM` - Defaults to DX7-style PM
- `float modulationIndex = 1.0f` - Modulation depth control
- Public setters/getters:
  - `setModulationType(ModulationType type)`
  - `getModulationType()`
  - `setModulationIndex(float index)`
  - `getModulationIndex()`

### 3. Updated Oscillator (`Oscillator.h`)
Modified `nextSample()` to accept optional phase offset:
```cpp
float nextSample(float phaseOffsetRadians = 0.0f)
{
    // Compute modulated phase (normalized 0-1 + offset converted from radians)
    float modulatedPhase = phase + (phaseOffsetRadians / TWO_PI);
    
    // Wrap to [0, 1) range
    modulatedPhase = modulatedPhase - std::floor(modulatedPhase);
    
    // Advance carrier phase
    phase += inc;
    if (phase >= 1.0f) {
        phase -= 1.0f;
    }
    
    return std::sin(TWO_PI * modulatedPhase);
}
```

### 4. Updated getCachedSample() (`Operator.cpp`)
Split modulation logic based on mode:

#### FM Mode (Frequency Modulation)
- Modulates instantaneous frequency each sample
- `currentFreq = baseFrequency + (modulationIndex * modSample * baseFrequency)`
- Updates oscillator phase increment every sample
- Higher CPU cost, can drift with extreme modulation

#### PM Mode (Phase Modulation - DX7 Style)
- Modulates phase angle directly
- Keeps base frequency stable
- `phaseOffset = modulationIndex * modSample` (in radians)
- Lower CPU cost, more stable
- Produces same harmonic spectrum as FM when tuned correctly

## Key Differences

| Aspect | FM | PM (DX7) |
|--------|-----|----------|
| **What's modulated** | Instantaneous frequency (Hz) | Phase angle (radians) |
| **Freq updates** | Every sample | Only on note/param changes |
| **CPU cost** | Higher | Lower |
| **Stability** | Can drift | Always stable |
| **Aliasing risk** | Higher | Lower |
| **Sound character** | Experimental | Classic DX7 |

## Usage Examples

```cpp
// Set operator to FM mode
operator.setModulationType(ModulationType::FM);
operator.setModulationIndex(2.5f); // Higher depth

// Set operator to PM mode (DX7-style, default)
operator.setModulationType(ModulationType::PM);
operator.setModulationIndex(1.0f); // DX7-typical depth
```

## Performance Notes

- **PM mode** is recommended for most use cases (authentic DX7, more efficient)
- **FM mode** provides extended creative possibilities but with higher CPU overhead
- Phase wrapping in PM mode uses `std::floor()` - could be optimized with fast modulo if needed
- Per-sample frequency updates in FM mode bypass smoothing for instant response

## Next Steps for Integration

1. **Add APVTS parameters** for per-operator:
   - `MOD_TYPE_X` (FM/PM toggle)
   - `MOD_INDEX_X` (depth 0.0 - 10.0)

2. **UI Controls**:
   - Dropdown or toggle button for mode selection
   - Slider for modulation index

3. **Optional enhancements**:
   - Global vs per-operator mode setting
   - Preset mappings (e.g., "DX7 Classic" = all PM, index 1.0)
   - Oversampling for high modulation indexes (anti-aliasing)
   - Micro-iterations for more accurate feedback in PM mode

## Testing Recommendations

1. Compare PM vs FM with identical settings - should sound very similar
2. Test extreme modulation indexes (>5.0) for aliasing
3. Verify cycle protection still works in both modes
4. Test feedback operators in both modes
5. Profile CPU usage difference between modes

## Compatibility Notes

- Backward compatible: existing code defaults to PM mode
- No breaking changes to existing API
- Can switch modes per-operator at runtime without audio glitches
- Both modes respect the re-entrancy guard for cycle protection
