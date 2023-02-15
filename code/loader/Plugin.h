#pragma once

struct Plugin
{
	struct PluginInfo
	{
		uint32_t MajorVersion;
		uint32_t MinorVersion;
		uint32_t PatchVersion;
		const char* Name;

		using TCallback = void(*)();
		using TBeginPlay = void(*)(class APlayerController*);
		using TEndPlay = void(*)(class APlayerController*);
		using TTick = void(*)(class APlayerController*, float);

		TCallback OnPreInitialize = nullptr;
		TCallback OnPostInitialize = nullptr;
		TBeginPlay OnBeginPlay = nullptr;
		TTick OnTick = nullptr;
		TEndPlay OnEndPlay = nullptr;
	};

	struct PolyjuiceInfo
	{
		uint32_t MajorVersion;
		uint32_t MinorVersion;
		uint32_t PatchVersion;
		uint32_t ApiVersion;

		using TGetExtension = void* (*)(uint32_t aId);
		TGetExtension GetExtension = nullptr;

		Hogwarts::TToString ToString;
		FUObjectArray* GUObjectArray;
		Hogwarts::TFindUObject FindUObject;
	};

	PluginInfo const* GetInfo() const { return &m_info; };

	void PreInit();
	void PostInit();
	void BeginPlay(class APlayerController* apController);
	void EndPlay(class APlayerController* apController);
	void Tick(class APlayerController* apController, float DeltaSeconds);

	static std::unique_ptr<Plugin> Create(std::filesystem::path aPath, PolyjuiceInfo* apInfo);

private:

	using TQuery = bool(*)(const PolyjuiceInfo* apInfo, PluginInfo* apPlugin);

	TQuery QueryCallback = nullptr;
	PluginInfo m_info;
};