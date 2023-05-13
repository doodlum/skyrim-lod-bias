#include "MipLODBias.h"

extern "C" DLLEXPORT const char* NAME = "TAA Deblur for Skyrim";
extern "C" DLLEXPORT const char* DESCRIPTION = "";

HMODULE m_hModule;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
		m_hModule = hModule;
	return TRUE;
}

static void DrawMenu(reshade::api::effect_runtime*)
{
	MipLODBias::GetSingleton()->Menu();
}

static void MessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		{
			if (reshade::register_addon(m_hModule)) {
				logger::info("Registered addon");
				reshade::register_overlay(nullptr, &DrawMenu);
			} else {
				logger::info("Failed to register addon");
			}
			break;
		}
	}
}

bool Load()
{
	MipLODBias::InstallHooks();
	
	SKSE::GetMessagingInterface()->RegisterListener("SKSE", MessageHandler);

	return true;
}