
#include <sdk.gen/APlayerController.hpp>
#include <sdk.gen/AGameMode.hpp>

#include "Polyjuice.h"
#include "sdk/SDKLocator.h"

#include "Engine.h"

enum ELevelTick
{
	/** Update the level time only. */
	LEVELTICK_TimeOnly = 0,
	/** Update time and viewports. */
	LEVELTICK_ViewportsOnly = 1,
	/** Update all. */
	LEVELTICK_All = 2,
	/** Delta time is zero, we are paused. Components don't tick. */
	LEVELTICK_PauseTick = 3,
};

using TStaticLoadObject = UObject*(*)(UClass* uclass, UObject* InOuter, const wchar_t* InName, const wchar_t* Filename, unsigned int LoadFlags, void* Sandbox, bool bAllowObjectReconciliation);
using TAPlayerController_BeginPlay = void(*)(APlayerController*);
using TAPlayerController_TickPlayer = void(*)(APlayerController*, float DeltaSeconds, ELevelTick TickType, struct FActorTickFunction* ThisTickFunction);
using TAGameMode_InitGameState = void(*)(AGameMode*);

static TAPlayerController_BeginPlay RealAPlayerController_BeginPlay = nullptr;
static TAPlayerController_TickPlayer RealAPlayerController_TickPlayer = nullptr;
static TAGameMode_InitGameState RealAGameMode_InitGameState = nullptr;
static TStaticLoadObject RealStaticLoadObject = nullptr;


void APlayerController_BeginPlay(APlayerController* apThis)
{
	static std::once_flag s_init;
	std::call_once(s_init, [] {
		spdlog::info("Game initialized, running post-initialization...");
		Polyjuice::Get().DispatchPostInitialization();
		spdlog::info("Post-Initialization complete!");
	});

	RealAPlayerController_BeginPlay(apThis);

	Polyjuice::Get().DispatchBeginPlay(apThis);
}

void APlayerController_TickPlayer(APlayerController* apThis, float DeltaSeconds, ELevelTick TickType, struct FActorTickFunction* ThisTickFunction)
{
	RealAPlayerController_TickPlayer(apThis, DeltaSeconds, TickType, ThisTickFunction);

	Polyjuice::Get().DispatchTick(apThis, DeltaSeconds);
}

void AGameMode_InitGameState(AGameMode* apThis)
{
	static std::once_flag s_initGuard;
	std::call_once(s_initGuard, []()
	{
		TiltedPhoques::Initializer::RunAll();
	});

	Polyjuice::Get().ProcessInitGameState();

	RealAGameMode_InitGameState(apThis);
}

using T_get_narrow_winmain_command_line = char*(*)();
static T_get_narrow_winmain_command_line Real_get_narrow_winmain_command_line = nullptr;

static void HookAPlayerController_TickPlayer();
static void HookAGameMode_InitGameState();
static void HookPlayerController_BeginPlay();
static void FindStaticLoad();

char* Hook_get_narrow_winmain_command_line()
{
	spdlog::info("Denuvo unpack completed, running pre-initialization...");

	LocateSDKPointers();
	FindStaticLoad();
	HookAPlayerController_TickPlayer();
	HookPlayerController_BeginPlay();
	HookAGameMode_InitGameState();

	Polyjuice::Get().LoadPlugins();
	Polyjuice::Get().LoadMods();
	Polyjuice::Get().DispatchPreInitialization();

	spdlog::info("Pre-Initialization complete!");

	return Real_get_narrow_winmain_command_line();
}

bool InstallEngineHook()
{
	{
		auto handle = LoadLibrary(TEXT("api-ms-win-crt-runtime-l1-1-0.dll"));
		auto procAddr = GetProcAddress((HMODULE)handle, "_get_narrow_winmain_command_line");

		auto res = MH_CreateHook(procAddr, Hook_get_narrow_winmain_command_line, (void**)&Real_get_narrow_winmain_command_line);
		if (res != MH_OK)
		{
			spdlog::error("Failed to hook _get_narrow_winmain_command_line");
			return false;
		}
		res = MH_EnableHook(procAddr);
		if (res != MH_OK)
		{
			spdlog::error("Failed to enable hook _get_narrow_winmain_command_line");
			return false;
		}

		spdlog::info("Hooked _get_narrow_winmain_command_line");
	}

	return true;
}

static void FindStaticLoad() 
{
	static auto module = mem::module::main();
	static auto pattern = mem::pattern("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 28 FA FF FF 48 81 EC");
	mem::default_scanner scanner(pattern);

	module.enum_segments([&](mem::region range, mem::prot_flags prot) {
		auto ptr = scanner.scan(range);
		if (ptr)
		{
			RealStaticLoadObject = ptr.as<decltype(RealStaticLoadObject)>();

			return true;
		}

		return false;
	});

	if (RealStaticLoadObject)
		spdlog::info("Found UObject::StaticLoadObject");
	else
		spdlog::error("Could not find UObject::StaticLoadObject");

}

static void HookPlayerController_BeginPlay() 
{
	static auto module = mem::module::main();
	static auto pattern = mem::pattern("40 56 48 83 EC 40 48 89 7C 24 58 48 8B F1 E8");
	mem::default_scanner scanner(pattern);

	void* func = nullptr;

	module.enum_segments([&](mem::region range, mem::prot_flags prot) {
		auto ptr = scanner.scan(range);
		if (ptr)
		{
			func = ptr.as<void*>();

			return true;
		}

		return false;
		});

	auto res = MH_CreateHook(func, APlayerController_BeginPlay, (void**)&RealAPlayerController_BeginPlay);
	if (res != MH_OK)
	{
		spdlog::error("Failed to hook APlayerController::BeginPlay");
		return;
	}
	res = MH_EnableHook(func);
	if (res != MH_OK)
	{
		spdlog::error("Failed to enable hook APlayerController::BeginPlay");
		return;
	}

	spdlog::info("Hooked APlayerController::BeginPlay");
}

static void HookAGameMode_InitGameState() 
{
	static auto module = mem::module::main();
	static auto pattern = mem::pattern("40 53 48 83 EC 20 48 8B 41 10 48 8B D9 48 8B 91");
	mem::default_scanner scanner(pattern);

	void* func = nullptr;

	module.enum_segments([&](mem::region range, mem::prot_flags prot) {
		auto ptr = scanner.scan(range);
	if (ptr)
	{
		func = ptr.as<void*>();

		return true;
	}

	return false;
		});

	auto res = MH_CreateHook(func, AGameMode_InitGameState, (void**)&RealAGameMode_InitGameState);
	if (res != MH_OK)
	{
		spdlog::error("Failed to hook AGameMode::InitGameState");
		return;
	}
	res = MH_EnableHook(func);
	if (res != MH_OK)
	{
		spdlog::error("Failed to enable hook AGameMode::InitGameState");
		return;
	}

	spdlog::info("Hooked AGameMode::InitGameState");

}

static void HookAPlayerController_TickPlayer() 
{
	static auto module = mem::module::main();
	static auto pattern = mem::pattern("40 53 55 57 48 81 EC A0 00 00 00 48 8B F9 44 0F");
	mem::default_scanner scanner(pattern);

	void* func = nullptr;

	module.enum_segments([&](mem::region range, mem::prot_flags prot) {
		auto ptr = scanner.scan(range);
	if (ptr)
	{
		func = ptr.as<void*>();

		return true;
	}

	return false;
		});

	auto res = MH_CreateHook(func, APlayerController_TickPlayer, (void**)&RealAPlayerController_TickPlayer);
	if (res != MH_OK)
	{
		spdlog::error("Failed to hook APlayerController::TickPlayer");
		return;
	}
	res = MH_EnableHook(func);
	if (res != MH_OK)
	{
		spdlog::error("Failed to enable hook APlayerController::TickPlayer");
		return;
	}

	spdlog::info("Hooked APlayerController::TickPlayer");
}
