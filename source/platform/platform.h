#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

void platform_initDisplay(void);
void platform_swapBuffers(void);
void platform_shutdownDisplay(void);
void platform_initRenderer(void);
void platform_pollInput(void);
int platform_keyDown(int key);
int platform_mouseX(void);
int platform_mouseY(void);
void platform_playSound(int sound_id);
void platform_stopSound(int sound_id);
void platform_playMusic(int track_id);
void platform_fontInit(void);

unsigned int platform_getTicks(void);
void platform_delay(int ms);

#ifdef __cplusplus
}
#endif

#endif
