#include "GameWindow.hpp"
#include "AnmManager.hpp"
#include "GameErrorContext.hpp"
#include "ScreenEffect.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"
#include "ZunMath.hpp"
#include "graphics/FixedFunctionGL.hpp"
#include "graphics/Software.hpp"
#include "graphics/WebGL.hpp"
#include "i18n.hpp"
#include "utils.hpp"

#include "../../platform/platform.h"

GameWindow g_GameWindow;
GfxInterface *g_GfxBackend;
i32 g_TickCountToEffectiveFramerate;
f64 g_LastFrameTime;

#define FRAME_TIME (1000. / 60.)

static const struct
{
    const char *name;
    GfxInterface *(*TryInit)();
} s_RenderBackends[] = {{"GL 2.1 / GL ES 2.0 / WebGL", WebGL::Create},
                        {"Fixed function GL(ES)", FixedFunctionGL::Init},
                        {"Software fallback (VERY SLOW)", Software::Init}};

RenderResult GameWindow::Render()
{
    // Platform-specific rendering
    // TODO: Implement proper rendering
    return RENDER_RESULT_KEEP_RUNNING;
}

void GameWindow::Present()
{
    // Platform swap buffers
    platform_swapBuffers();
    // In D3D, this was done after the present call, but SDL makes no guarantees
    // about the color buffer state immediately after a swap, so it has to be moved to be before it
    g_AnmManager->TakeScreenshotIfRequested();
    if (g_Supervisor.unk198 != 0)
    {
        g_Supervisor.unk198--;
    }

    // g_GfxBackend->SwapBuffers();
}

void GameWindow::CreateGameWindow()
{
    platform_initDisplay();
}

ZunResult GameWindow::InitD3dRendering()
{
    platform_initRenderer();
    return ZUN_SUCCESS;
}

void GameWindow::InitD3dDevice(void)
{
    platform_initDisplay();
}
