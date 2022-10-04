#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"

int mainxD()
{
    // Get Proc ID of the target process
    DWORD procId = GetProcId(L"ac_client.exe");

    //GetModuleBaseAddress
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

    //Get Handle to Process
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

    //Resolve base address of the pointer chain
    uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10f4f4;
    std::cout << "DYnamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;

    //Resolve our ammo pointer chain
    std::vector<unsigned int> AmmoOffsets = { 0x374, 0x14, 0x0 };
    uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, AmmoOffsets);
    std::cout << "ammoAddr = " << "0x" << std::hex << ammoAddr << std::endl;

    //Read Ammo Value
    int ammoValue = 0;
    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "Current ammo = " << std::dec << ammoValue << std::endl;

    //Write to it
    int newAmmo = 1337;
    WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);

    //Read out again
    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "New ammo = " << std::dec << ammoValue << std::endl;



    getchar();
    return 0;
}