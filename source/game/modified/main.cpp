#include "platform.h"
#include <cstdio>
#include <cstring>
#include <fat.h>
#include <unistd.h>

#include "AnmManager.hpp"
#include "Chain.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameWindow.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "i18n.hpp"
#include "utils.hpp"

// chdir into the directory holding the assets so relative paths (TH_*_DAT_FILE,
// TH_CONFIG_FILE, bgm/...) resolve. GameCube passes no argv, so fall back to
// the conventional app directory on the SD/SDGecko mount.
//    Right now it just returns with no asset dir found, but it'll be important later
//    for swiss support


static void ChangeToAssetDirectory(int argc, char *argv[])
{
    if (argc > 0 && argv != NULL && argv[0] != NULL)
    {
        const char *lastSlash = std::strrchr(argv[0], '/');
        const size_t length = lastSlash ? (size_t)(lastSlash - argv[0]) : 0;
        if (length > 0 && length < 1024)
        {
            char directory[1024];
            std::memcpy(directory, argv[0], length);
            directory[length] = '\0';
            if (chdir(directory) == 0)
                return;
        }
    }

    static const char *fallbackDirectories[] = {
        // Relative paths resolve against libfat's default device set by fatInitDefault()
        "apps/th06", "th06",
        // GameCube SD Gecko is mounted as carda:/cardb: (no sd: on GameCube)
        "carda:/apps/th06", "carda:", "cardb:/apps/th06", "cardb:"};
    for (size_t i = 0; i < sizeof(fallbackDirectories) / sizeof(fallbackDirectories[0]); i++)
    {
        if (chdir(fallbackDirectories[i]) == 0)
        {
            utils::DebugPrint("Assets dir: %s", fallbackDirectories[i]);
            return;
        }
    }
    utils::DebugPrint("Warning: no asset directory found");
}

int main(int argc, char *argv[])
{
    i32 renderResult = 0;

    utils::DebugPrint("=== TH06 GameCube Port Starting ===");
    utils::DebugPrint("argc=%d, argv=%p", argc, (void*)argv);

    // Mount the filesystem (SD Gecko / SD2SP2). Not fatal: the assets are
    // embedded, so the game runs without a filesystem; saves/logs just won't persist.
    if (!fatInitDefault())
    {
        utils::DebugPrint("WARNING: fatInitDefault() failed - no SD card mounted");
    }
    ChangeToAssetDirectory(argc, argv);

    // Early platform initialization check
    platform_initDisplay();
    utils::DebugPrint("Platform init done");

    //    MSG msg;
    //    i32 waste1, waste2, waste3, waste4, waste5, waste6;

    //    if (utils::CheckForRunningGameInstance())
    //    {
    //        g_GameErrorContext.Flush();
    //
    //        return 1;
    //    }

    //    g_Supervisor.hInstance = hInstance;

    if (g_Supervisor.LoadConfig(TH_CONFIG_FILE) != ZUN_SUCCESS)
    {
        utils::DebugPrint("Failed to load config");
        g_GameErrorContext.Flush();
        return -1;
    }
    utils::DebugPrint("Config loaded successfully");

    //    if (GameWindow::InitD3dInterface())
    //    {
    //        g_GameErrorContext.Flush();
    //        return 1;
    //    }

    //    SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &g_GameWindow.screenSaveActive, 0);
    //    SystemParametersInfo(SPI_GETLOWPOWERACTIVE, 0, &g_GameWindow.lowPowerActive, 0);
    //    SystemParametersInfo(SPI_GETPOWEROFFACTIVE, 0, &g_GameWindow.powerOffActive, 0);
    //    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, NULL, SPIF_SENDCHANGE);
    //    SystemParametersInfo(SPI_SETLOWPOWERACTIVE, 0, NULL, SPIF_SENDCHANGE);
    //    SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 0, NULL, SPIF_SENDCHANGE);

restart:
    GameWindow::CreateGameWindow();
    utils::DebugPrint("GameWindow created");

    g_AnmManager = new AnmManager();
    utils::DebugPrint("AnmManager created: %p", (void*)g_AnmManager);

    if (GameWindow::InitD3dRendering() != ZUN_SUCCESS)
    {
        utils::DebugPrint("Failed to init rendering");
        g_GameErrorContext.Flush();
        return 1;
    }
    utils::DebugPrint("Rendering initialized");

    g_SoundPlayer.InitializeDSound();
    utils::DebugPrint("SoundPlayer initialized");
    Controller::GetJoystickCaps();
    Controller::ResetKeyboard();

    if (Supervisor::RegisterChain() != ZUN_SUCCESS)
    {
        utils::DebugPrint("Failed to register supervisor chain");
        goto stop;
    }
    utils::DebugPrint("Supervisor chain registered");
    if (!g_Supervisor.cfg.windowed)
    {
        // Cursor handling - no-op for GameCube
    }

    g_GameWindow.curFrame = 0;

    while (true)
    {
        // Platform event polling
        platform_pollInput();
        
        // Check for quit condition through platform
        if (0) // TODO: Add platform-specific quit check
        {
            goto stop;
        }

        renderResult = g_GameWindow.Render();
        if (renderResult != 0)
        {
            break;
        }

        platform_delay(16); // ~60fps

        //        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        //        {
        //            TranslateMessage(&msg);
        //            DispatchMessage(&msg);
        //        }
        //        else
        //        {
        //            testCoopLevelRes = g_Supervisor.d3dDevice->TestCooperativeLevel();
        //            if (testCoopLevelRes == D3D_OK)
        //            {
        //                renderResult = g_GameWindow.Render();
        //                if (renderResult != 0)
        //                {
        //                    goto stop;
        //                }
        //            }
        //            else if (testCoopLevelRes == D3DERR_DEVICENOTRESET)
        //            {
        //                g_AnmManager->ReleaseSurfaces();
        //                testResetRes = g_Supervisor.d3dDevice->Reset(&g_Supervisor.presentParameters);
        //                if (testResetRes != 0)
        //                {
        //                    goto stop;
        //                }
        //                GameWindow::InitD3dDevice();
        //                g_Supervisor.unk198 = 3;
        //            }
        //        }
    }

stop:
    g_Chain.Release();
    g_SoundPlayer.Release();

    delete g_AnmManager;
    g_AnmManager = NULL;

    if (g_GfxBackend != NULL)
        delete g_GfxBackend;
    // SDL_Quit() - no-op, just cleanup

    if (renderResult == 2)
    {
        g_GameErrorContext.ResetContext();

        g_GameErrorContext.Log(TH_ERR_OPTION_CHANGED_RESTART);

        if (!g_Supervisor.cfg.windowed)
        {
            // Cursor handling - no-op for GameCube
        }

        goto restart;
    }

    FileSystem::WriteDataToFile(TH_CONFIG_FILE, &g_Supervisor.cfg, sizeof(g_Supervisor.cfg));

    // Cursor handling - no-op for GameCube
    g_GameErrorContext.Flush();
    return 0;
}
