#include "Polyjuice.h"
#include "loader/VersionDll.h"

TiltedPhoques::UniquePtr<Polyjuice> s_pPolyjuice;

Polyjuice& Polyjuice::Get()
{
    return *s_pPolyjuice;
}

void Initialize()
{
    MH_Initialize();

    LoadOriginalDll();

    s_pPolyjuice = TiltedPhoques::MakeUnique<Polyjuice>();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        Initialize();
        break;

    case DLL_THREAD_ATTACH:
        s_pPolyjuice->OnThreadAttach();
        break;

    case DLL_THREAD_DETACH:
        s_pPolyjuice->OnThreadDetach();
        break;

    case DLL_PROCESS_DETACH:
        spdlog::default_logger()->flush();
        s_pPolyjuice = nullptr;
        MH_Uninitialize();
        break;
    }
    return TRUE;
}
