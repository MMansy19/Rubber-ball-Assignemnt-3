# Bouncing Ball Simulation

A physics-based bouncing ball simulation built with **OpenGL (FreeGLUT/GLEW)** for the Computer Graphics course (Spring 2026, Assignment 3).

The ball follows projectile motion equations, bounces off the ground with energy loss via a coefficient of restitution, and eventually comes to rest. Features switchable gradient backgrounds, and adjustable physics parameters (mass, power, gravity, restitution) — all controlled via keyboard.

---

## Screenshots

<!-- Add screenshots here -->
<!-- ![Sky Background](Screenshots/sky.png) -->
<!-- ![Sunset Background](Screenshots/sunset.png) -->

---

## Physics Model

The simulation uses standard equations of motion:

| Equation | Formula |
|----------|---------|
| Horizontal position | `x = xOffset + h × power × t` |
| Vertical position | `y = vY × t - (1/2) × g × mass × t²` |
| Bounce (collision) | `vY_new = -vY_impact × restitution` |
| Stop condition | `|vY| < 0.1` → ball rests on ground |

- **Time `t`** resets to 0 after each bounce (new parabolic arc).
- **Horizontal offset `xOffset`** accumulates across bounces so X motion is continuous.
- **Effective gravity** = base gravity × mass (heavier ball falls faster).
- **Power** multiplies the initial velocity at launch.

---

## Background Themes

4 procedural gradient backgrounds generated as OpenGL textures at startup (no external image files needed):

| Key | Theme | Description |
|-----|-------|-------------|
| `1` | Sky | Light blue gradient, bright at bottom |
| `2` | Sunset | Orange-to-purple warm gradient |
| `3` | Space | Dark blue-black deep space |
| `4` | Nature | Green-to-blue natural gradient |
| `0` | None | Original white background |

---

## Adjustable Parameters

| Parameter | Keys | Default | Description |
|-----------|------|---------|-------------|
| Horizontal velocity | `←` / `→` | 0.5 | Initial horizontal speed |
| Vertical velocity | `↑` / `↓` | 4.0 | Initial vertical speed |
| Gravity | `PgUp` / `PgDn` | 0.2 | Base gravitational acceleration |
| Mass / Weight | `W` / `S` | 1.0 | Ball mass — heavier = falls faster |
| Power / Strength | `Q` / `A` | 1.0 | Launch power — multiplier on velocity |
| Restitution | `E` / `D` | 0.7 | Bounciness — energy retained per bounce |

All parameters are displayed in real-time on the HUD overlay.

---

## Controls

| Key | Action |
|-----|--------|
| `Space` | Play / Pause animation |
| `R` | Reset all parameters to defaults |
| `↑` / `↓` | Increase / decrease vertical velocity |
| `←` / `→` | Increase / decrease horizontal velocity |
| `PgUp` / `PgDn` | Increase / decrease gravity |
| `W` / `S` | Increase / decrease mass (weight) |
| `Q` / `A` | Increase / decrease power (launch strength) |
| `E` / `D` | Increase / decrease restitution (bounciness) |
| `1`, `2`, `3`, `4` | Switch background theme |
| `0` | No background (white) |
| `ESC` | Exit |

---

## Visual Design

### Ball
- **Solid sphere** rendered with `glutSolidSphere` (red rubber ball)
- **Wire overlay** with `glutWireSphere` for visual detail
- Clamped above ground — never visually goes below y = 0

### Ground
- Green horizontal line at y = 0 for visual reference
- Color adapts to match the active background theme

### HUD Overlay
- Top-left: all 6 physics parameter values with key hints
- Bottom: full controls legend
- Text color adapts to dark/light backgrounds (white text on dark, black on light)

---

## Project Structure

```
Assignemnt_3/
├── ThrowBall.cpp               # Single-file: all physics, rendering, input, backgrounds
├── Assignment_3.sln            # Visual Studio solution
├── Assignment_3.vcxproj        # Visual Studio project
├── Assignment_3.vcxproj.filters
├── Assignment_3.vcxproj.user
├── Instructions.md             # Assignment requirements
└── README.md                   # This file
```

---

## Build Instructions

### Requirements

- **Visual Studio 2022** (MSVC v143 toolset)
- **GLEW** — OpenGL Extension Wrangler
- **FreeGLUT** — OpenGL Utility Toolkit

### Setting Up GLEW & FreeGLUT

If you see `Cannot open include file: 'GL/glew.h'`:

1. Download GLEW from https://glew.sourceforge.net/ (Windows precompiled binaries)
2. In Visual Studio project properties (All Configurations | All Platforms):
   - **C/C++ → Additional Include Directories**: add `path\to\glew\include`
   - **Linker → Additional Library Directories**: add `path\to\glew\lib\Release\x64`
   - **Linker → Additional Dependencies**: add `glew32s.lib`
   - **C/C++ → Preprocessor Definitions**: add `GLEW_STATIC`
3. Repeat for freeglut: https://www.transmissionzero.co.uk/software/freeglut-devel/

### Build & Run

1. Open `Assignment_3.sln` in Visual Studio.
2. Select **Debug | x64** (or Release | x64).
3. Press `Ctrl+F5` to build and run.

---

## Team Members

| Name | Student ID |
|------|------------|
| Ali Mahmoud Ahmed | 1230338 |
| Ahmed Attay Kamal | 1230011 |
| Mahmoud Mohamed Abdelfattah | 4220142 |
| Seif Allah Alaa Mohamed | 1200324 |
