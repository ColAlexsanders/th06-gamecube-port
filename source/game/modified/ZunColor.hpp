#pragma once

#include "inttypes.hpp"

/* COLORS */
#define TH06_COLOR_BLACK 0xff000000
#define TH06_COLOR_GREY 0xff808080
#define TH06_COLOR_DARK_GREY 0xff505050
#define TH06_COLOR_ASHEN_GREY 0xff606060
#define TH06_COLOR_WHITE 0xffffffff
#define TH06_COLOR_TRANSPARENT_WHITE 0x80ffffff
#define TH06_COLOR_RED 0xffff0000
#define TH06_COLOR_YELLOW 0xffffff00
#define TH06_COLOR_LIGHT_RED 0xffff8080
#define TH06_COLOR_PASTEL_RED 0xffff6060
#define TH06_COLOR_SUNSHINEYELLOW 0xffffff40
#define TH06_COLOR_PINK 0xffffe0e0
#define TH06_COLOR_LIGHTCYAN 0xffe0ffff
#define TH06_COLOR_LAVENDER 0xffe0e0ff
#define TH06_COLOR_LIGHTBLUE 0xffd0d0ff
#define TH06_COLOR_LIGHTYELLOW 0xffffff80
#define TH06_COLOR_PALEBLUE 0xffc0b0ff
#define TH06_COLOR_NEONBLUE 0xff4040ff
#define TH06_COLOR_DEEPBLUE 0xff3030ff
#define TH06_COLOR_GUI_1 0xffe8f0ff
#define TH06_COLOR_GUI_2 0xffffe8f0

#define TH06_COLOR_RGB_MASK 0x00FFFFFF
#define TH06_COLOR_ALPHA_MASK 0xFF000000
#define TH06_COLOR_RGB(color) ((color) & TH06_COLOR_RGB_MASK)
#define TH06_COLOR_ALPHA(color) (((color) & TH06_COLOR_ALPHA_MASK) >> 24)
#define TH06_COLOR_SET_ALPHA(color, alpha) (((alpha) << 24) | TH06_COLOR_RGB(color))
#define TH06_COLOR_SET_ALPHA2(color, alpha) (TH06_COLOR_RGB(color) | (((alpha) & 0xff) << 24))
#define TH06_COLOR_SET_ALPHA3(color, alpha) (TH06_COLOR_RGB(color) | ((alpha) << 24))
#define TH06_COLOR_COMBINE_ALPHA(color, alpha) (((alpha) & TH06_COLOR_ALPHA_MASK) | TH06_COLOR_RGB(color))

#define TH06_COLOR_MENU_ACTIVE_BACKGROUND 0x40000000
// TODO: find a better name for this color
#define TH06_COLOR_START_MENU_ITEM_INACTIVE 0x80300000

#define TH06_COLOR_KEYBOARD_KEY_HIGHLIGHT 0xffffffc0
#define TH06_COLOR_KEYBOARD_KEY_NORMAL 0x60c0c0c0

#define TH06_COLOR_MENU_ITEM_HIGHLIGHT 0x800000ff
#define TH06_COLOR_MENU_ITEM_DEFAULT 0x80202050

// BGR
#define TH06_COLOR_END_TEXT_SHADOW 0xc0d0d0
#define TH06_COLOR_MUSIC_ROOM_SONG_TITLE_TEXT 0xc0e0ff
#define TH06_COLOR_MUSIC_ROOM_SONG_TITLE_SHADOW 0x302080
#define TH06_COLOR_MUSIC_ROOM_SONG_DESC_TEXT 0xffe0c0
#define TH06_COLOR_MUSIC_ROOM_SONG_DESC_SHADOW 0x300000

// TODO: The following assumes little endian
#define TH06_COLOR_RED_BYTE_IDX 0
#define TH06_COLOR_GREEN_BYTE_IDX 1
#define TH06_COLOR_BLUE_BYTE_IDX 2
#define TH06_COLOR_ALPHA_BYTE_IDX 3

#define TH06_COLOR_GET_COMPONENT(color, component) (((u8 *)&(color))[(component)])
#define TH06_COLOR_SET_COMPONENT(color, component, value) ((u8 *)&(color))[(component)] = (value);

typedef u32 ZunColor;
