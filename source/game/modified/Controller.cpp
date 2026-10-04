#include "Controller.hpp"

#include <gccore.h>

#include "GameErrorContext.hpp"
#include "Supervisor.hpp"
#include "i18n.hpp"
#include "utils.hpp"

// DIFFABLE_STATIC(JOYCAPSA, g_JoystickCaps)
static u16 g_FocusButtonConflictState;
static u8 *keyboardState;

u16 Controller::GetJoystickCaps(void)
{
    //    JOYINFOEX pji;

    //    pji.dwSize = sizeof(JOYINFOEX);
    //    pji.dwFlags = JOY_RETURNALL;

    //    if (joyGetPosEx(0, &pji) != MMSYSERR_NOERROR)
    //    {
    //        GameErrorContext::Log(&g_GameErrorContext, TH_ERR_NO_PAD_FOUND);
    //        return 1;
    //    }
    //
    //    joyGetDevCapsA(0, &g_JoystickCaps, sizeof(g_JoystickCaps));
    return 0;
}

#define JOYSTICK_MIDPOINT(min, max) ((min + max) / 2)
#define JOYSTICK_BUTTON_PRESSED(button, x, y) (x > y ? button : 0)
#define JOYSTICK_BUTTON_PRESSED_INVERT(button, x, y) (x < y ? button : 0)
#define KEYBOARD_KEY_PRESSED(button, x) keyboardState[x] ? button : 0

u16 Controller::GetControllerInput(u16 buttons)
{
    // GameCube uses direct PAD input in GetInput()
    // This function is kept for compatibility but doesn't need to do anything
    // The actual input reading is done in GetInput()
    (void)buttons;
    return buttons;
}

/* u32 Controller::SetButtonFromDirectInputJoystate(u16 *outButtons, i16 controllerButtonToTest,
                                                 enum TouhouButton touhouButton, const u8 *inputButtons)
{
    if (controllerButtonToTest < 0)
    {
        return 0;
    }

    *outButtons |= (inputButtons[controllerButtonToTest] & 0x80 ? touhouButton & 0xFFFF : 0);

    return inputButtons[controllerButtonToTest] & 0x80 ? touhouButton & 0xFFFF : 0;
} */

u32 Controller::SetButtonFromControllerInputs(u16 *outButtons, i16 controllerButtonToTest,
                                              enum TouhouButton touhouButton, int chan)
{
    u8 pressed = 0;
    const u8 *controllerState;

    if (controllerButtonToTest < 0)
    {
        return 0;
    }

    controllerState = Controller::GetControllerState();

    if (controllerState != NULL && controllerButtonToTest >= 0 && controllerButtonToTest < 12)
    {
        pressed = controllerState[controllerButtonToTest];
    }

    *outButtons |= pressed ? touhouButton & 0xFFFF : 0;

    return pressed ? touhouButton & 0xFFFF : 0;
}

static u8 g_ControllerData[12]; // 12 buttons in GCButton enum

// This is for rebinding keys
const u8 *Controller::GetControllerState()
{
    u32 connectedPads;
    int i;

    connectedPads = PAD_ScanPads();
    
    // Get the first connected controller
    if (g_Supervisor.gameControllerChannel >= 0 && g_Supervisor.gameControllerChannel < 4)
    {
        if (connectedPads & (1 << (31 - g_Supervisor.gameControllerChannel)))
        {
            u32 buttons = PAD_ButtonsDown(g_Supervisor.gameControllerChannel);
            
            // Map GC buttons to our enum
            for (i = 0; i < 12; i++)
            {
                switch (i)
                {
                case 0: // A
                    g_ControllerData[i] = (buttons & PAD_BUTTON_A) ? 0x80 : 0;
                    break;
                case 1: // B
                    g_ControllerData[i] = (buttons & PAD_BUTTON_B) ? 0x80 : 0;
                    break;
                case 2: // X
                    g_ControllerData[i] = (buttons & PAD_BUTTON_X) ? 0x80 : 0;
                    break;
                case 3: // Y
                    g_ControllerData[i] = (buttons & PAD_BUTTON_Y) ? 0x80 : 0;
                    break;
                case 4: // Start
                    g_ControllerData[i] = (buttons & PAD_BUTTON_START) ? 0x80 : 0;
                    break;
                case 5: // Z
                    g_ControllerData[i] = (buttons & PAD_TRIGGER_Z) ? 0x80 : 0;
                    break;
                case 6: // L
                    g_ControllerData[i] = (buttons & PAD_TRIGGER_L) ? 0x80 : 0;
                    break;
                case 7: // R
                    g_ControllerData[i] = (buttons & PAD_TRIGGER_R) ? 0x80 : 0;
                    break;
                case 8: // DPadUp
                    g_ControllerData[i] = (buttons & PAD_BUTTON_UP) ? 0x80 : 0;
                    break;
                case 9: // DPadDown
                    g_ControllerData[i] = (buttons & PAD_BUTTON_DOWN) ? 0x80 : 0;
                    break;
                case 10: // DPadLeft
                    g_ControllerData[i] = (buttons & PAD_BUTTON_LEFT) ? 0x80 : 0;
                    break;
                case 11: // DPadRight
                    g_ControllerData[i] = (buttons & PAD_BUTTON_RIGHT) ? 0x80 : 0;
                    break;
                }
            }
            
            // Get analog stick values for direction
            s16 stickX = PAD_StickX(g_Supervisor.gameControllerChannel);
            s16 stickY = PAD_StickY(g_Supervisor.gameControllerChannel);
            
            // Add stick directions to button state
            // Note: We're just adding these to the controller state for compatibility
            // The actual stick handling is in GetControllerInput
        }
    }

    return g_ControllerData;
}

u16 Controller::GetInput(void)
{
    u16 buttons = 0;
    u32 connectedPads;
    int chan;

    // Get active controller channel
    connectedPads = PAD_ScanPads();
    for (chan = 0; chan < 4; chan++)
    {
        if (connectedPads & (1 << (31 - chan)))
        {
            g_Supervisor.gameControllerChannel = chan;
            break;
        }
    }
    
    if (chan >= 4)
    {
        g_Supervisor.gameControllerChannel = -1;
        return 0;
    }

    // Get button state from PAD
    u32 downButtons = PAD_ButtonsDown(g_Supervisor.gameControllerChannel);
    u32 heldButtons = PAD_ButtonsHeld(g_Supervisor.gameControllerChannel);

    // Map controller buttons
    if (downButtons & PAD_BUTTON_A) buttons |= TH_BUTTON_SHOOT;
    if (downButtons & PAD_BUTTON_B) buttons |= TH_BUTTON_BOMB;
    if (heldButtons & PAD_TRIGGER_Z) buttons |= TH_BUTTON_FOCUS;
    if (downButtons & PAD_BUTTON_START) buttons |= TH_BUTTON_MENU;
    if (heldButtons & PAD_BUTTON_UP) buttons |= TH_BUTTON_UP;
    if (heldButtons & PAD_BUTTON_DOWN) buttons |= TH_BUTTON_DOWN;
    if (heldButtons & PAD_BUTTON_LEFT) buttons |= TH_BUTTON_LEFT;
    if (heldButtons & PAD_BUTTON_RIGHT) buttons |= TH_BUTTON_RIGHT;
    if (downButtons & PAD_BUTTON_Y) buttons |= TH_BUTTON_SKIP;

    // Get analog stick values
    s16 stickX = PAD_StickX(g_Supervisor.gameControllerChannel);
    s16 stickY = PAD_StickY(g_Supervisor.gameControllerChannel);

    // Deadzone for stick
    const s16 deadzone = 40;
    if (stickX > deadzone) buttons |= TH_BUTTON_RIGHT;
    if (stickX < -deadzone) buttons |= TH_BUTTON_LEFT;
    if (stickY < -deadzone) buttons |= TH_BUTTON_UP;
    if (stickY > deadzone) buttons |= TH_BUTTON_DOWN;

    // Directional combinations
    if ((buttons & TH_BUTTON_UP) && (buttons & TH_BUTTON_LEFT)) buttons |= TH_BUTTON_UP_LEFT;
    if ((buttons & TH_BUTTON_UP) && (buttons & TH_BUTTON_RIGHT)) buttons |= TH_BUTTON_UP_RIGHT;
    if ((buttons & TH_BUTTON_DOWN) && (buttons & TH_BUTTON_LEFT)) buttons |= TH_BUTTON_DOWN_LEFT;
    if ((buttons & TH_BUTTON_DOWN) && (buttons & TH_BUTTON_RIGHT)) buttons |= TH_BUTTON_DOWN_RIGHT;

    // Handle focus button conflict (Z + A/B)
    if ((buttons & TH_BUTTON_FOCUS) && (buttons & (TH_BUTTON_SHOOT | TH_BUTTON_BOMB)))
    {
        // Focus conflict handling
    }

    return buttons;
}

void Controller::ResetKeyboard(void)
{
    // GameCube has no physical keyboard - this is a no-op
}
