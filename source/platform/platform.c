#include "platform.h"

// --- Display ---
void platform_initDisplay(void)     {  }
void platform_swapBuffers(void)     {  }
void platform_shutdownDisplay(void) {  }
void platform_initRenderer(void)    {  }
void platform_fontInit(void)        {  }
// --- Input ---
void platform_pollInput(void)       {  }
int  platform_keyDown(int key)      { return 0; }
int  platform_mouseX(void)         { return 0; }
int  platform_mouseY(void)         { return 0; }

// --- Audio ---
void platform_playSound(int sound_id) {  }
void platform_stopSound(int sound_id) {  }
void platform_playMusic(int track_id) {  }

// --- Timing ---
unsigned int platform_getTicks(void)  { return 0; }
void platform_delay(int ms)    {  }
