#include "pch.h"
#include "EGKKHoldingPointsPlugin.h"
#include "Constant.h"
#include "CFPNRadarScreen.h"


CFPNPlugin::CFPNPlugin(void) : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, "FPN-68 PAR", "1.0.0", "Alice Ford","GPL v3") {
	sendMessage("FPN-68 PAR Plugin Loaded");

	RegisterDisplayType(FPN_NAME, false, false, true, true);
}

CFPNPlugin::~CFPNPlugin() {
	sendMessage("FPN-68 PAR Plugin Unloaded");
}

EuroScopePlugIn::CRadarScreen* CFPNPlugin::OnRadarScreenCreated(const char* sDisplayName,
	bool NeedRadarContent,
	bool GeoReferenced,
	bool CanBeSaved,
	bool CanBeCreated
) {
	if (strcmp(sDisplayName, FPN_NAME) != 0) {
		return nullptr;
	}
	else {
		return new CFPNRadarScreen;
	}
}

void CFPNPlugin::sendMessage(std::string message) {
	try {
		DisplayUserMessage("FPN-68", "System", message.c_str(), true, true, true, false, false);
	}
	catch (...) {
		DisplayUserMessage("FPN-68", "System", "ERROR", true, true, true, true, false);
	}
}