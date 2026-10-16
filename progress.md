# Progress

Dated devlog. Kept up to date every session; tracked in git (no longer ignored).

---

## 2026-10-15 — game boots again; libfat root-caused

**Symptom.** DOL (and ELF) booted in Dolphin to a black screen, but nothing reached `main` — a `__attribute__((constructor))` probe also never printed, so it died in the C runtime startup.

**Cause.** Linking **libfat** (`-lfat` + `fatInitDefault()`). libogc2/libfat objects are built with `-mcall-sysv -msdata=sysv`, so their code accesses small globals as 16-bit offsets from r13/r2. The project's `MACHDEP` (`-DGEKKO -mogc -mcpu=750 -meabi -mhard-float`) didn't set those, so the bases weren't set up and the first small-data access faulted inside the CRT before any C++ ran. libfat has no `.ctors`/`init_array` (checked), so it wasn't a crashing static initializer.

**Fix.** Added `MACHDEP += -mcall-sysv -msdata=sysv` to the Makefile (matching how libogc2 and libfat were built), re-enabled `-lfat` and `fatInitDefault()`. Game now reaches `main` and logs.

**Ruled out along the way.** Embedded asset size (1-byte stand-in assets also failed); `elf2dol` (booting the `.elf` failed identically); the deleted desktop graphics backends (excluded from the build).

**Also from this session.**
- Asset embedding via bin2s is working: `data/{in,md,cm,ed,st,tl,cfg}.dat` → `build/*.dat.o` → `.rodata`, with a `kEmbeddedAssets` table in `FileSystem.cpp` and a read-only `fmemopen` shim in `FopenUTF8`. `data/` is gitignored (game data, ~8 MB).
- Dolphin emulates no FAT-capable GameCube storage (no SD Gecko/SD2SP2), so embedding is the Dolphin path; real hardware reads from SD via Swiss.
- `fatInitDefault()` failure is now non-fatal (warn + continue), since assets are embedded.
- The temporary `BootProbe` constructor has been removed.

---

## SDL mentions remaining

Scan: `grep -rc "SDL_" source` (excluding the `original/` submodule).

| File | Count | Nature | In build? |
|---|---|---|---|
| `source/game/modified/Config/Widgets.cpp` | 45 | real SDL / SDL_ttf calls | no — `Config/` not in `SOURCES` |
| `source/game/modified/Config/config.cpp` | 35 | real SDL / SDL_ttf calls | no |
| `source/game/modified/Config/Widgets.hpp` | 27 | real SDL / SDL_ttf types | no |
| `source/game/modified/SoundPlayer.cpp` | 8 | comments only | yes |
| `source/game/modified/TextHelper.cpp` | 3 | comments only | yes |
| `source/game/modified/MidiOutput.cpp` | 3 | comments only | yes |
| `source/game/modified/Supervisor.cpp` | 3 | comments only | yes |
| `source/game/modified/SoundPlayer.hpp` | 2 | comments only | yes |
| `source/game/modified/AnmManager.hpp` | 1 | comment only | yes |
| `source/game/modified/main.cpp` | 1 | comment only | yes |
| `source/game/modified/Supervisor.hpp` | 1 | comment only | yes |

The only *code* left is the desktop config-editor tool (`Config/`), which isn't compiled. All SDL references in compiled files are comments.

---

## Implementation backlog

1. **Create the game window / renderer.** `g_GfxBackend` is never assigned and `GXBackend::Init()` is never called; `platform_initDisplay()` is empty and `GXBackend::SwapBuffers()` does nothing. Implement `VIDEO_Init` → framebuffer → `GX_Init`, a clear, and `GX_DrawDone`/`GX_CopyDisp`/`VIDEO_Flush`; then assign the backend.
2. **Real timing.** `platform_getTicks()` returns `0` and `platform_delay()` is a no-op; the game loop depends on both.
3. **Implement the `GXBackend` methods** (28 `/* TODO */` stubs): state (blend, depth, cull), viewport/scissor, textures (TMem), matrices, and `Draw` (`GX_Begin`/`GX_End`).
4. **Image decoding** — SDL_image replacement for `AnmManager` (`LoadToSurfaceWithFormat`, `ExtractSurfacePixels`, `FlipSurface`, `ApplySurfaceToColorBuffer`). PNG/JPG decoder or pre-converted assets.
5. **Font rendering** — SDL_ttf replacement for `TextHelper`. Options: pre-baked bitmap atlas (simpler) vs porting FreeType.
6. **Audio** — `SoundPlayer` (SDL audio → ASND/aesnd) and `MidiOutput` (software synth vs pre-rendered PCM).
7. **Verify input** — `Controller.cpp` PAD wiring end to end, including the `GCButtons.hpp` mapping used by the config.
8. **Config tool** — stub, delete, or leave the SDL config editor (`Config/`).
9. **Tidy** — strip leftover SDL mentions from comments (optional, after navigation is done).
10. **Persistence** — with libfat working, confirm config/score/replay writes; on hardware via Swiss + SD Gecko (libfat device `carda:`/`cardb:`).
11. **Hardware testing** — boot the DOL via Swiss from an SD Gecko, assets on the card (or embedded, in which case nothing is needed on the card).
