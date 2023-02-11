add_requires(
    "tiltedcore",
    "spdlog",
    "hopscotch-map",
    "cryptopp",
    "gamenetworkingsockets",
    "imgui",
    "minhook",
    "mem",
    "xbyak",
    "mimalloc")

target("polyjuice")
    set_kind("shared")
    set_basename("version")
    add_files("**.cpp", "main.def")
    add_headerfiles(
        "**.h", 
        "**.hpp",
        "**.inl",
        "../../HogwartsSDK/code/**.h"
        )

    set_pcxxheader("stdafx.h")

    add_includedirs(
        ".",
        "../../HogwartsSDK/code/",
        "../../HogwartsSDK/code/Source/Runtime/Core/Public",
        "../../HogwartsSDK/code/Source/Runtime/Core/Public/Misc",
        "../../HogwartsSDK/code/Source/Runtime/Core/Public/Math",
        "../../HogwartsSDK/code/Source/Runtime/CoreUObject/Public",
        "../../HogwartsSDK/code/Source/Runtime/TraceLog/Public")

    add_defines(
        "UBT_COMPILED_PLATFORM=Windows",
        "UE_BUILD_SHIPPING=1",
        "WITH_EDITOR=0",
        "WITH_EDITORONLY_DATA=0",
        "WITH_ENGINE=1",
        "WITH_UNREAL_DEVELOPER_TOOLS=0",
        "WITH_PLUGIN_SUPPORT=0",
        "WITH_SERVER_CODE=0",
        "IS_MONOLITHIC=1",
        "IS_PROGRAM=1",
        "PLATFORM_WINDOWS",
        "PLATFORM_64BITS",
        "CORE_API=",
        "COREUOBJECT_API=",
        "UNICODE",
        "_UNICODE"
    )

    add_linkdirs("../../HogwartsSDK/build/windows/x64/release/")
    add_links("HogwartsSDK")

    add_packages(
        "mimalloc",
        "tiltedcore",
        "spdlog",
        "hopscotch-map",
        "cryptopp",
        "gamenetworkingsockets",
        "imgui",
        "minhook",
        "mem",
        "xbyak")