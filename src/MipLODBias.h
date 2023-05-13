#pragma once

#include <shared_mutex>

#include "RE/BSGraphics.h"
#include "RE/BSGraphicsTypes.h"

struct UnkOuterStruct
{
	struct UnkInnerStruct
	{
		uint8_t unk00[0x18];  // 00
		bool bTAA;            // 18
	};

	// members
	uint8_t unk00[0x1F0];            // 00
	UnkInnerStruct* unkInnerStruct;  // 1F0

	static UnkOuterStruct* GetSingleton()
	{
		REL::Relocation<UnkOuterStruct*&> instance{ REL::VariantID(527731, 414660, 0x34234C0) };  // 31D11A0, 326B280, 34234C0
		return instance.get();
	}

	bool GetTAA() const
	{
		if (this == nullptr)
			return false;
		return unkInnerStruct->bTAA;
	}

	void SetTAA(bool a_enabled)
	{
		if (this == nullptr)
			return;
		unkInnerStruct->bTAA = a_enabled;
	}
};

struct SamplerStatesCollection
{
	#define AddressMode 6
	#define FilterMode 5
	ID3D11SamplerState* states[AddressMode][FilterMode];

	static SamplerStatesCollection* GetSingleton()
	{
		REL::Relocation<SamplerStatesCollection*> instance{ REL::RelocationID(524751, 411366) };
		return instance.get();
	}
};

class MipLODBias
{
public:
	static MipLODBias* GetSingleton()
	{
		static MipLODBias singleton;
		return &singleton;
	}

	static void InstallHooks()
	{
		Hooks::Install();
	}

	std::shared_mutex fileLock;

	SamplerStatesCollection _backupCollection{};
	SamplerStatesCollection _modifiedCollection{};

	bool _setup = false;
	bool _enabled = true;

	bool _fixGrass = true;

	bool _forceDisable = false;
	float _mipLODBias = -1.0f;

	void Update();

	void Menu();

	void Load();
	void Save();

protected:
	struct Hooks
	{
		struct Main_UpdateViewport
		{
			static void thunk(BSGraphics::State* a_state)
			{
				func(a_state);
				GetSingleton()->Update();
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install()
		{
			stl::write_thunk_call<Main_UpdateViewport>(REL::RelocationID(35556, 36555).address() + 0x2D);
		}
	};

private:
	MipLODBias()
	{
		Load();
	};

	MipLODBias(const MipLODBias&) = delete;
	MipLODBias(MipLODBias&&) = delete;

	~MipLODBias() = default;

	MipLODBias& operator=(const MipLODBias&) = delete;
	MipLODBias& operator=(MipLODBias&&) = delete;
};
