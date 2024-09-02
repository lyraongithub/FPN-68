#include "pch.h"
#include "EGKKHoldingPointsPlugin.h"
#include "Constant.h"
#include "CFPNRadarScreen.h"

#include <sstream>


CFPNPlugin::CFPNPlugin(void) : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, "FPN-68 PAR", "1.0.0", "Alice Ford","GPL v3") {
	loadNewAerodrome("EGKK", "26L");
	
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

bool CFPNPlugin::OnCompileCommand(const char* sCommandLine) {
	if (strcmp(sCommandLine, ".fpn 5") == 0) {
		range = 5;
		return true;
	} else if (strcmp(sCommandLine, ".fpn 10") == 0) {
		range = 10;
		return true;
	} else if (strcmp(sCommandLine, ".fpn 15") == 0) {
		range = 15;
		return true;
	} else if (strncmp(sCommandLine, ".fpn", 4) == 0) {
		std::stringstream f(sCommandLine);
		std::string s;
		std::string icao;
		std::string runway;
		int n = 0;
		while (std::getline(f, s, ' ')) {
			if (n == 1) icao = s;
			else if (n == 2) runway = s;
			n++;
		}
		sendMessage(icao.c_str());
		sendMessage(runway.c_str());
		loadNewAerodrome(icao.c_str(), runway.c_str());
		return true;
	}
	return false;
}

void CFPNPlugin::loadNewAerodrome(const char* icao, const char* runway) {
	EuroScopePlugIn::CSectorElement elem = SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY);

	bool foundRunway = false;

	do {
		if (strncmp(elem.GetAirportName(), icao, 4) == 0) {
			if (strcmp(elem.GetRunwayName(0), runway) == 0) {
				elem.GetPosition(&runwayThreshold, 0);
				elem.GetPosition(&otherThreshold, 1);
				sendMessage(("Loaded: " + std::string(elem.GetAirportName()) + ", runway " + std::string(elem.GetRunwayName(0))).c_str());
				foundRunway = true;
				break;
			} else if (strcmp(elem.GetRunwayName(1), runway) == 0) {
				elem.GetPosition(&runwayThreshold, 1);
				elem.GetPosition(&otherThreshold, 0);
				sendMessage(("Loaded: " + std::string(elem.GetAirportName()) + ", runway " + std::string(elem.GetRunwayName(1))).c_str());
				foundRunway = true;
				break;
			}
		}

		elem = SectorFileElementSelectNext(elem, EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY);
	} while (elem.IsValid());

	if (!foundRunway) {
		sendMessage(("Runway " + std::string(runway) + " at " + std::string(icao) + " not found!").c_str());
	}
}