#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

void printMenu(bool bHealth, bool bAmmo, bool bRecoil) {
    system("CLS");
    std::string healthState = bHealth ? "x" : " ";
    std::cout << "[" << healthState << ']' << " Health Hack - NUM1" << std::endl;
    std::string ammoState = bAmmo ? "x" : " ";
    std::cout << "[" << ammoState << ']' << " Ammo Hack - NUM2" << std::endl;
    std::string recoilState = bRecoil ? "x" : " ";
    std::cout << "[" << recoilState << ']' << " Recoil Hack - NUM3" << std::endl;
    std::cout << std::endl << std::endl << "To exit please press INSERT" << std::endl;
}

int main()
{
    HANDLE hProcess = 0;
    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false;

    const int newValue = 1337;

    DWORD procId = GetProcId(L"ac_client.exe");

    if (procId) {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

        moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

        localPlayerPtr = moduleBase + 0x10f4f4;

        healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xf8 });
    } else {
        std::cout << "Process not found, press enter to exit" << std::endl;
        getchar();
        return 0;
    }

    DWORD dwExit = 0;

    printMenu(false, false, false);

    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {
        if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
            bHealth = !bHealth;
            printMenu(bHealth, bAmmo, bRecoil);
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
            bAmmo = !bAmmo;
            printMenu(bHealth, bAmmo, bRecoil);

            if (bAmmo) {
                // FF 06 = inc [esi]
                mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);
            } else {
                // FF 0E = dec [esi]
                mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E", 2, hProcess);
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
            bRecoil = !bRecoil;
            printMenu(bHealth, bAmmo, bRecoil);

            if (bRecoil) {
                mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
            }else {
                mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
            }
        }
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            return 0;
        }

        if (bHealth) {
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }

        Sleep(10);
    }

    std::cout << "Process not found, press enter to exit" << std::endl;
    getchar();
    return 0;
}