# FilterComp Draggable Implementation

## Overview
Successfully integrated the `DraggableGraph` base class into `FilterComp`, enabling interactive mouse control of filter parameters directly on the frequency response graph.

## Changes Made

### FilterComp.h
1. **Inheritance**: Changed from `juce::Component` to `DraggableGraph`
2. **Added includes**: `#include "DraggableGraph.h"`
3. **Virtual method declarations**: Added overrides for `onDragStart`, `onDragUpdate`, `onDragEnd`
4. **Drag state members**: Added `dragStartCutoff` and `dragStartResonance` to store initial values

### FilterComp.cpp

#### 1. Graph Bounds Setup
```cpp
graphBounds = bounds.withHeight(height); // Store for mouse interaction
```
Set in `paint()` method to define the interactive area (the frequency response graph region).

#### 2. Control Point Visualization
Added visual feedback showing the current cutoff frequency position on the graph:
- Circle at cutoff frequency position (logarithmic X scale)
- Y position calculated from actual filter magnitude at cutoff
- Semi-transparent fill with white outline for visibility

#### 3. Interactive Drag Implementation

**onDragStart():**
- Stores initial cutoff and resonance values
- Called when user clicks within graphBounds

**onDragUpdate():**
- **Horizontal (X) drag → Cutoff Frequency:**
  - Exponential/logarithmic scaling: `newCutoff = dragStartCutoff * pow(2, deltaX * sensitivity)`
  - Sensitivity: 0.003 (approximately 333 pixels per octave)
  - Range: 20 Hz to 20,000 Hz
  - Dragging right increases frequency, left decreases

- **Vertical (Y) drag → Q Factor (Resonance):**
  - Linear scaling: `newQ = dragStartResonance + (deltaY * sensitivity)`
  - Sensitivity: 0.01 (100 pixels per Q unit)
  - Range: 0.1 to 10.0
  - Dragging down increases Q (sharper peak), up decreases Q (broader)

**onDragEnd():**
- Currently empty (placeholder for future features like snap-to-grid, automation)

## User Experience

### Interaction Model
1. **Click and drag** anywhere on the frequency response graph
2. **Horizontal movement** adjusts cutoff frequency exponentially
3. **Vertical movement** adjusts resonance/Q factor linearly
4. Visual feedback through:
   - Real-time curve updates
   - Control point position
   - Knob values and text boxes update automatically

### Benefits
- **Direct manipulation**: Edit filter by dragging the response curve itself
- **Visual feedback**: See exactly how parameters affect the frequency response
- **Consistent with ADSR**: Same interaction pattern as envelope editor
- **Non-destructive**: Knobs and text boxes still work independently

## Technical Details

### Coordinate Mapping

**Frequency (X-axis):**
```cpp
// Position to frequency (logarithmic)
float normX = (pos.x - graphBounds.getX()) / graphBounds.getWidth();
float freq = minFreq * pow(maxFreq / minFreq, normX);

// Frequency to position
float normFreq = log(freq / minFreq) / log(maxFreq / minFreq);
float x = graphBounds.getX() + normFreq * graphBounds.getWidth();
```

**Q Factor (Y-axis):**
```cpp
// Linear mapping - drag delta directly affects Q
// Down (positive Y) = increase Q = sharper peak
// Up (negative Y) = decrease Q = broader peak
```

### Sensitivity Tuning
Current values provide good control feel:
- **Frequency sensitivity: 0.003** - Balanced between coarse and fine adjustment
- **Q sensitivity: 0.01** - Allows precise resonance control

These can be adjusted if users want:
- **More precision**: Decrease sensitivity (larger drag distance needed)
- **Faster changes**: Increase sensitivity (smaller drag distance needed)

## Future Enhancements

### Possible Additions:
1. **Modifier keys:**
   - Shift: Fine control (reduce sensitivity 10x)
   - Ctrl/Cmd: Snap to musical frequencies (octaves, semitones)
   - Alt: Adjust gain instead of Q for parametric EQ

2. **Multi-mode support:**
   - Click detection to select between LP/HP/BP/Notch modes
   - Different regions control different parameters per mode

3. **Snap features:**
   - Snap cutoff to musical notes (A=440, etc.)
   - Snap Q to common values (0.707 Butterworth, 1.0, etc.)

4. **Visual enhancements:**
   - Show frequency labels near control point
   - Draw Q bandwidth indicator (±3dB points)
   - Highlight draggable region on hover

5. **Automation:**
   - Record parameter gestures for DAW automation
   - Undo/redo support for drag operations

6. **Touch/Multi-touch:**
   - Two-finger drag for simultaneous freq+Q adjustment
   - Pinch gesture for Q control

## Testing Checklist

- [ ] Compile successfully
- [ ] Drag horizontally changes cutoff frequency
- [ ] Drag vertically changes Q/resonance
- [ ] Control point tracks cutoff position correctly
- [ ] Knobs update when dragging graph
- [ ] Text boxes update when dragging graph
- [ ] Graph updates when changing knobs
- [ ] No crashes or artifacts during interaction
- [ ] Delta-based dragging feels smooth (no jumping)
- [ ] Parameter ranges respected (20-20kHz, 0.1-10.0 Q)

## Code Reusability

This implementation demonstrates the `DraggableGraph` pattern:
1. Inherit from `DraggableGraph`
2. Set `graphBounds` in `paint()` or `resized()`
3. Implement three virtual methods with domain-specific logic
4. Base class handles all mouse events automatically

The pattern is now used by:
- **EnvComp**: Interactive ADSR envelope editing
- **FilterComp**: Interactive filter frequency/Q editing

Future candidates:
- Multi-band EQ with draggable bands
- LFO waveform shaping
- Waveshaper curve editing
- Compressor knee visualization
