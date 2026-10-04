# th06-gamecube-port AGENTS.md

## Quick start

```bash
make            # Builds th06-gc.elf and th06-gc.dol
make clean      # Removes build artifacts
```

**Required toolchain:** devkitPPC with `libogc2` for GameCube (not stock libogc).
**Environment variable:** `export DEVKITPPC=/opt/devkitpro/devkitPPC`

## Reference implementations

### th06-wii (reference only)

A reference implementation exists at `reference/th06-wii/` - this is a Wii port
by @niko1official that uses SDL2 + libogc2. It's useful for seeing how libogc2
functions like `VIDEO_Init()`, `GX_Init()`, `PAD_ScanPads()` are used in practice.

**Important:** This GameCube port uses a **pure libogc2 approach without SDL2**,
unlike th06-wii which uses SDL2 for windowing/input on non-Wii platforms.

## Architecture

- **Base:** GensokyoClub/th06 portable branch (SDL2 + OpenGL) as a git submodule at `source/game/original/`
- **Port target:** GameCube with libogc2/GX replacing SDL2/OpenGL
- **Build system:** devkitPPC makefile (`$(DEVKITPPC)/gamecube_rules`)

## Key directories

```
source/
├── game/original/          # Git submodule (untouched reference)
├── game/modified/          # SDL-dependent code being ported
└── platform/
    ├── platform.h/.c       # C stub layer (display, input, audio, timing)
    └── gx/
        └── GXBackend.hpp/.cpp   # GfxInterface impl for GX (stubs today)
```

## Build status

**⚠️ BUILD INCOMPLETE** - This is a partial SDL→libogc2 port. The code compiles but won't link because:
1. Many SDL2-dependent functions still need stub implementations
2. Image loading (SDL_image) not yet implemented
3. Font rendering (SDL_ttf) deferred to bitmap font atlas

### What compiles
- Platform stubs (`platform.h/.c`)
- Controller input stubs (using PAD)
- Some AnmManager stubs
- Basic GameWindow and Supervisor structures

### What needs work
- AnmManager.cpp image loading (needs PNG/JPG decoder or pre-converted assets)
- TextHelper.cpp font rendering (requires bitmap font system or FreeType port)
- Full SDL→libogc2 migration of modified/ files

## Platform stub API (`platform.h`)

```c
// Display
void platform_initDisplay(void);
void platform_swapBuffers(void);
void platform_shutdownDisplay(void);
void platform_initRenderer(void);

// Input
void platform_pollInput(void);
int  platform_keyDown(int key);
int  platform_mouseX(void);
int  platform_mouseY(void);

// Audio
void platform_playSound(int sound_id);
void platform_stopSound(int sound_id);
void platform_playMusic(int track_id);

// Timing
unsigned int platform_getTicks(void);
void platform_delay(int ms);
void platform_fontInit(void);
```

All bodies are currently empty/no-op stubs in `platform.c`.

## GCButton enum (`GCButtons.hpp`)

```cpp
enum class GCButton : i16 {
    A, B, X, Y, Start, Z, L, R,
    DPadUp, DPadDown, DPadLeft, DPadRight
};
```

Maps to `PAD_BUTTON_*` / `PAD_TRIGGER_*` macros.

## Progress tracking

- `progress.md` — dated devlog with open TODOs per session
- `th06-gc-project-context.md` — detailed design decisions, toolchain notes

## Completed SDL→libogc2 migrations

| File | Changes |
|------|---------|
| `main.cpp` | Removed SDL2 includes, replaced `SDL_ShowCursor`, `SDL_PollEvent`, `SDL_Delay`, `SDL_Quit` with platform stubs |
| `GameWindow.cpp` | Removed SDL2 includes, replaced with `platform_swapBuffers` |
| `Controller.cpp` | Replaced SDL_gamecontroller with PAD-based input, `SetButtonFromControllerInputs` takes `int chan` |
| `Controller.hpp` | Updated signature, removed SDL dependencies |
| `SoundPlayer.cpp` | Stubbed all SDL audio API calls |
| `SoundPlayer.hpp` | Replaced `SDL_AudioDeviceID` with `u32`, removed SDL headers |
| `GameErrorContext.cpp` | Removed SDL_messagebox, replaced with no-op |
| `ScreenEffect.cpp` | Removed SDL2 include |
| `MidiOutput.cpp` | Replaced `SDL_GetTicks`, `SDL_SwapBE*` with platform stubs |
| `MidiOutput.hpp` | Removed SDL2 headers |
| `TextHelper.cpp` | Stubbed font rendering (requires SDL_ttf replacement) |
| `GameWindow.hpp` | Removed SDL2 include |
| `Config/Widgets.hpp` | Stubbed SDL2_ttf types |
| `Config/config.cpp` | Stubbed SDL2_ttf types |
| `graphics/*.hpp/.cpp` | Removed SDL2/SDL_opengl.h includes, stubbed GL function pointers |

### Color constant renaming
All `COLOR_*` macros renamed to `TH06_COLOR_*` to avoid conflicts with libogc2's color.h:
- `COLOR_BLACK` → `TH06_COLOR_BLACK`
- `COLOR_WHITE` → `TH06_COLOR_WHITE`
- etc.

## Current build state (as of Oct 04, 2026)

**⚠️ PARTIALLY COMPLETE** - Many files compile but Supervisor.cpp fails due to SDL2 references.

### Files that compile
- Platform stubs (`platform.h/.c`)
- Controller input (using PAD)
- Most game logic files (`GameManager`, `Stage`, `EnemyManager`, etc.)
- Graphics stubs (`GLFunc`, `FixedFunctionGL`, `Software`, `WebGL`)

### Files with build errors
- `Supervisor.cpp` - Uses SDL2 functions not yet stubbed:
  - `SDL_GameControllerOpen()` → `PAD_ScanPads()` 
  - `SDL_GameControllerClose()` → no-op
  - `SDL_IsGameController()` → `PAD_ScanPads() & (1 << (31 - i))`
  - `SDL_GetTicks()` → `platform_getTicks()`
  - `midiOutput` member now declared in Supervisor.hpp
- `MainMenu.cpp` - Uses:
  - `SDL_GetTicks()` (3 occurrences) → `platform_getTicks()`
  - `SDL_CONTROLLER_BUTTON_MAX` → `12` (number of buttons in GCButton enum)
- `GCButton` enum defined in `GCButtons.hpp` maps to `PAD_BUTTON_*` / `PAD_TRIGGER_*`

## Remaining issues

1. **Supervisor.cpp** - Uses SDL2 functions (`SDL_GameControllerOpen`, `SDL_GetTicks`, etc.) - needs PAD equivalents
2. **MainMenu.cpp** - Uses `SDL_GetTicks()` and `SDL_CONTROLLER_BUTTON_*` macros
3. **Color constants** - Renamed to `TH06_COLOR_*` to avoid libogc2 conflicts

## Build status

**⚠️ PARTIALLY COMPLETE** - Partial compilation but not linking due to:
1. SDL2 references in Supervisor.cpp (needs PAD equivalents)
2. SDL2 references in MainMenu.cpp (needs timing and button enum updates)

### To complete the port:
1. Replace SDL_GameControllerOpen/Close with `PAD_ScanPads()` / `PAD_ButtonsDown()`
2. Replace `SDL_GetTicks()` with `platform_getTicks()`
3. Replace `SDL_CONTROLLER_BUTTON_*` with `GCButton` enum values
4. Implement real GX calls in `GXBackend.cpp` for actual rendering
5. Link with libogc2 libraries (`-logc`, `-lm`)

## Quick reference: SDL→GameCube equivalents

| SDL Function | GameCube equivalent |
|--------------|---------------------|
| `SDL_GameControllerOpen(i)` | Use `PAD_ScanPads()` to check controller connection |
| `SDL_GameControllerClose(c)` | No-op (no explicit close needed) |
| `SDL_IsGameController(i)` | `PAD_ScanPads() & (1 << (31 - i))` |
| `SDL_GetTicks()` | `platform_getTicks()` |
| `SDL_CONTROLLER_BUTTON_A` | `GCButton::A` |
| `SDL_CONTROLLER_BUTTON_B` | `GCButton::B` |
| `SDL_CONTROLLER_BUTTON_X` | `GCButton::X` |
| `SDL_CONTROLLER_BUTTON_Y` | `GCButton::Y` |
| `SDL_CONTROLLER_BUTTON_START` | `GCButton::Start` |
| `SDL_CONTROLLER_BUTTON_LEFTSHOULDER` | `GCButton::L` |
| `SDL_CONTROLLER_BUTTON_RIGHTSHOULDER` | `GCButton::R` |
| `SDL_CONTROLLER_BUTTON_DPAD_UP` | `GCButton::DPadUp` |
| `SDL_CONTROLLER_BUTTON_DPAD_DOWN` | `GCButton::DPadDown` |
| `SDL_CONTROLLER_BUTTON_DPAD_LEFT` | `GCButton::DPadLeft` |
| `SDL_CONTROLLER_BUTTON_DPAD_RIGHT` | `GCButton::DPadRight` |
