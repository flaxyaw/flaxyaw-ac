//win includes
#include "iostream"
#include "thread"
#include <Windows.h>

//custom includes
#include "../ext/logger/logger.h"


struct Vector3
{
    float x, y, z;
};
struct Vector2
{
    float x, y;
};


class playerent
{
public:
    char pad_0000[4]; //0x0000
    Vector3 vLocationHead; //0x0004
    char pad_0010[24]; //0x0010
    Vector3 vLocationFeet; //0x0028
    Vector2 vViewangle; //0x0034
    char pad_003C[56]; //0x003C
    int32_t movedirection; //0x0074
    char pad_0078[116]; //0x0078
    int32_t iHealth; //0x00EC
    int32_t iArmor; //0x00F0
    char pad_00F4[273]; //0x00F4
    char szName[16]; //0x0205
    char pad_0215[1525]; //0x0215
}; //Size: 0x080A


void Setup(HMODULE hInstance) {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    //setting up most important stuff.
    uintptr_t module_base = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    uintptr_t entitylist = *reinterpret_cast<uintptr_t*>(module_base + 0x0018AC00 + 0x4);

    playerent* localplayer = *reinterpret_cast<playerent**>(module_base + 0x17E0A8);

    logger::log(logger::success, "Injected successfully.");
    logger::log(logger::info, "base addr: %p", module_base);

    while (!GetAsyncKeyState(VK_END) & 1) {

        uintptr_t max_players = *(int*)(module_base + 0x0018AC00 + 0xC);

        if (!localplayer)
            continue;

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            localplayer->vLocationFeet = { 0, 0, 0 };

        }

        for (int i = 0; i < max_players; i++) {
            if (!entitylist)
                continue;
            playerent* entity = *reinterpret_cast<playerent**>(entitylist + (0x4 * i));
            if (!entity)
                continue;
            printf("entity health %i \n", entity->iHealth);
            printf("entity name: %s \n", entity->szName);
            printf("entity pos: %f %f %f \n", entity->vLocationFeet.x, entity->vLocationFeet.y, entity->vLocationFeet.z);
            printf("lp location: %f %f %f \n", localplayer->vLocationFeet.x, localplayer->vLocationFeet.y, localplayer->vLocationFeet.z);
            entity->vLocationFeet = localplayer->vLocationFeet;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

    fclose(stdout);
    FreeConsole();
    FreeLibraryAndExitThread(hInstance, 0);
}


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID Reserved) {
    if (dwReason != DLL_PROCESS_ATTACH)
        return FALSE;

    const HANDLE hThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup), hInstance, NULL, nullptr);

    if (hThread)
        CloseHandle(hThread);

    return TRUE;
}