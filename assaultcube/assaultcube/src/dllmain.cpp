//win includes
#include "iostream"
#include "thread"
#include <Windows.h>

//custom includes
#include "../ext/logger/logger.h"


struct vector3_t
{
    float x, y, z;
};


void Setup(HMODULE hInstance) {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    //setting up most important stuff.
    uintptr_t module_base = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    uintptr_t playerent = *reinterpret_cast<uintptr_t*>(module_base + 0x0017E0A8);

    //logging setup
    logger::log(logger::success, "Injected successfully.");
    logger::log(logger::info, "base addr: %p", module_base);
    logger::log(logger::info, "player: %p", playerent);

    while (!GetAsyncKeyState(VK_END) & 1) {
        

        int health = *reinterpret_cast<int*>(playerent + 0xEC);
        int ar_mag = *reinterpret_cast<int*>(playerent + 0x14);
        vector3_t lp_pos = *reinterpret_cast<vector3_t*>(playerent + 0x28);
        vector3_t lp_viewangle = *reinterpret_cast<vector3_t*>(playerent + 0x34);

        //Entity list. First valid entity is 0x4. Always incremented by 0x4.
        uintptr_t entity_list_ptr = *reinterpret_cast<uintptr_t*>(module_base + 0x18AC04);


        for (int i = 0; i < 64; i++) {
            if (!entity_list_ptr)
                continue;
            uintptr_t entity = *reinterpret_cast<uintptr_t*>(entity_list_ptr + (0x4 * i));
            if (!entity)
                continue;

            printf("ent: %p \n", entity);
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