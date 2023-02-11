#include "SDKLocator.h"

UObjectBase* FindUObject(size_t obj_full_name_hash);

void LocateSDKPointers()
{
	auto module = mem::module::main();
	{
		auto pattern = mem::pattern("48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8d 8d ? ? ? ? e8 ? ? ? ? 48 8d 8d ? ? ? ? e8");
		mem::default_scanner scanner(pattern);

		module.enum_segments([&](mem::region range, mem::prot_flags prot) {
			auto ptr = scanner.scan(range);
			if (ptr)
			{
				ptr += 3;
				ptr += *ptr.as<int*>();
				ptr += 4;

				Hogwarts::GUObjectArray = ptr.as<FUObjectArray*>();

				return true;
			}

			return false;
		});
	}

	{
		auto pattern = mem::pattern("48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20 8B 01 48 8B DA 8B F8 44 0F B7 C0 C1");
		mem::default_scanner scanner(pattern);

		module.enum_segments([&](mem::region range, mem::prot_flags prot) {
			auto ptr = scanner.scan(range);
			if (ptr)
			{
				Hogwarts::GToString = ptr.as<decltype(Hogwarts::GToString)>();
				return true;
			}

			return false;
		});

		if (Hogwarts::GToString == nullptr)
			spdlog::error("Failed to find FName::ToString!");
		else
			spdlog::info("Found FName::ToString!");
	}

	Hogwarts::GFindUObject = FindUObject;
}

std::string WideNarrow(std::wstring_view str)
{
	auto length = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.length(), nullptr, 0, nullptr, nullptr);
	std::string narrowStr{};

	narrowStr.resize(length);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.length(), (LPSTR)narrowStr.c_str(), length, nullptr, nullptr);

	return narrowStr;
}

std::string Narrow(const FString& fstr)
{
	auto& char_array = fstr.GetCharArray();
	return WideNarrow(char_array.Num() ? char_array.GetData() : L"");
}

std::string Narrow(const FName& fname)
{
	return Narrow(fname.ToString());
}

std::string GetFullObjectPath(UObjectBase* obj)
{
	auto c = obj->GetClass();

	if (c == nullptr) 
		return "null";

	auto obj_name = Narrow(obj->GetFName());

	for (auto outer = obj->GetOuter(); outer != nullptr; outer = outer->GetOuter()) 
	{
		obj_name = Narrow(outer->GetFName()) + '.' + obj_name;
	}

	return Narrow(c->GetFName()) + ' ' + obj_name;
}

UObjectBase* FindUObject(size_t HashToFind) 
{
	static std::unordered_map<size_t, UObjectBase*> CacheMap{};

	if (auto search = CacheMap.find(HashToFind); search != std::end(CacheMap))
		return search->second;

	auto ObjectArray = get_GUObjectArray();

	for (auto i = 0; i < ObjectArray->GetObjectArrayNum(); ++i)
	{
		if (auto Item = ObjectArray->IndexToObject(i))
		{
			if (auto Base = Item->Object)
			{
				if (fhash(GetFullObjectPath(Base).c_str()) == HashToFind)
				{
					CacheMap[HashToFind] = Base;
					return Base;
				}
			}
		}
	}

	return nullptr;
}