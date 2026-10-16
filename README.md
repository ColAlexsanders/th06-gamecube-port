# Touhou 6 EOSD Gamecube Port
Making this public because @niko1official got this ported out to the wii (https://github.com/niko1official/th06-wii). I was scared someone will decide to make a port of this game on the gamecube and claude code the crap out of it when I started this to learn how game porting works. 

I want to make this because 1. I want to play this game on actual hardware and 2. to actually make those retro aesthetic 'Touhou 6 on the gamecube!!11!!!' ads on the gamecube actually true (see https://youtu.be/KneEzQWOMOA?si=H5cHhoBsqL57gAqO for what I am talking about)

I am using the code from https://github.com/GensokyoClub/th06/tree/portable for the actual game, so most of the code in this is stolen from GensokyoClub and their contributors (ty :). (See progress.md implementation backlog for what I am doing exactly)

---

## Requirements

- a properly configured devkitPro
    - devkitPPC 
    - libogc2
- The game's own asset files. See the table below:

  | Your game file | Copy to |
  |---|---|
  | `紅魔郷IN.DAT` | `data/in.dat` |
  | `紅魔郷MD.DAT` | `data/md.dat` |
  | `紅魔郷CM.DAT` | `data/cm.dat` |
  | `紅魔郷ED.DAT` | `data/ed.dat` |
  | `紅魔郷ST.DAT` | `data/st.dat` |
  | `紅魔郷TL.DAT` | `data/tl.dat` |
  | `東方紅魔郷.cfg` | `data/cfg.dat` |

  Only the filenames change; the bytes are used as-is. English-patched installs (`th06e_*.DAT`) work too, but then `TH_LANG` in `source/game/modified/i18n.hpp` must be set to `TH_EN`. You're a big boy you can find them

## Build

```bash
cd /the/dir/u/want/dis/repo/in
git clone https://github.com/ColAlexsanders/th06-gamecube-port.git
cd th06-gamecube-port
mkdir -p data
cp <ur-dat-files> data/
make -j $(nproc)
```

## Status
Supports asset embedding into the binary via bin2s (in makefile dw), making the DOL a whopping 8.7 MB. This is for testing on Dolphin because 1. I don't want to copy the .dol to my Gamecube's sdcard 24/7 and 2. Dolphin currently doesn't support emulation for file loading with fat for Gamecube. The logic for it does exist when I do start testing using Swiss.

## Known limitations
- Nothing renders yet (video/GX layer is stubbed).
- Fonts and audio are stubbed; text and sound don't work.
- The desktop config editor under `source/game/modified/Config/` is not built and still contains SDL/SDL_ttf code.

## Repo-Specific Reference material
- `progress.md` — dated devlog, SDL leftovers, implementation backlog
