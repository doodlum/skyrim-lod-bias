#include "MipLODBias.h"

#include <SimpleIni.h>

void MipLODBias::Update()
{
	bool shouldEnable = UnkOuterStruct::GetSingleton()->GetTAA();
	if (_forceDisable)
	{
		shouldEnable = false;
	}

	auto originalCollection = SamplerStatesCollection::GetSingleton();

	if (!_setup)
	{
		for (int i = 0; i < AddressMode; i++) {
			for (int k = 0; k < FilterMode; k++) {
				_backupCollection.states[i][k] = originalCollection->states[i][k];
			}
		}

		_setup = true;
	}
	
	static float mipLODBias = 0;

	if (shouldEnable != _enabled || mipLODBias != _mipLODBias) {
		auto manager = RE::BSRenderManager::GetSingleton();
		auto device = manager->GetRuntimeData().forwarder;

		for (int i = 0; i < AddressMode; i++) {
			for (int k = 0; k < FilterMode; k++) {
				if (auto state = _modifiedCollection.states[i][k]) {
					state->Release();
				}
				_modifiedCollection.states[i][k] = nullptr;
			}
		}


		for (int i = 0; i < AddressMode; i++) {
			{
			D3D11_SAMPLER_DESC samplerDesc;
			_backupCollection.states[i][3]->GetDesc(&samplerDesc);
			if (shouldEnable) {
				samplerDesc.MipLODBias = _mipLODBias;
			}

			device->CreateSamplerState(&samplerDesc, &_modifiedCollection.states[i][3]);
			}
		}

		for (int i = 0; i < AddressMode; i++) {
			for (int k = 0; k < FilterMode; k++) {
				if (auto state = _modifiedCollection.states[i][k]) {
					originalCollection->states[i][k] = state;
				} else {
					originalCollection->states[i][k] = _backupCollection.states[i][k];

				}
			}
		}

		_enabled = shouldEnable;
		mipLODBias = _mipLODBias;
	}
}

void MipLODBias::Menu()
{
	if (ImGui::Button("Load")) {
		Load();
	}

	ImGui::SameLine();

	if (ImGui::Button("Save"))
	{
		Save();
	}

	ImGui::Checkbox("Grass Sampler Fix", &_fixGrass);

	ImGui::Checkbox("Force Disable", &_forceDisable);
	ImGui::SameLine();
	ImGui::InputFloat("Mip LOD Bias", &_mipLODBias);
}

void MipLODBias::Load()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\TAADeblur.ini");

	_fixGrass = ini.GetBoolValue("Fixes", "GrassSamplerFix", true);

	_forceDisable = ini.GetBoolValue("TAA", "ForceDisable", false);
	_mipLODBias = (float)ini.GetDoubleValue("TAA", "MipLODBias", false);
}

void MipLODBias::Save()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.SetBoolValue("Fixes", "GrassSamplerFix", _fixGrass);
	
	ini.SetBoolValue("TAA", "ForceDisable", _forceDisable);
	ini.SetDoubleValue("TAA", "MipLODBias", _mipLODBias);

	ini.SaveFile(L"Data\\SKSE\\Plugins\\TAADeblur.ini");
}