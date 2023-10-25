#include "pch.h"
#include "EGKKHoldingPointsPlugin.h"
#include "Constant.h"


#define FORCAP(G) for (const char* cap : G) {if (strcmp(dest, cap) == 0) {return true;}} break;


CEGKKHoldingPointsPlugin::CEGKKHoldingPointsPlugin(void) : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, "EGKK Holding Points", "1.0.0", "Alice Ford","GPL v3") {
	sendMessage("Hello!");

	RegisterTagItemType("EGKK Plugin HP", TAG_ITEM_EGKK_HP);
	RegisterTagItemType("EGKK Plugin LVL", TAG_ITEM_EGKK_LVL);
}

CEGKKHoldingPointsPlugin::~CEGKKHoldingPointsPlugin() {
	sendMessage("Bye!");
}

void CEGKKHoldingPointsPlugin::OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan,
	EuroScopePlugIn::CRadarTarget RadarTarget,
	int ItemCode,
	int TagData,
	char sItemString[16],
	int* pColorCode,
	COLORREF* pRGB,
	double* pFontSize
) {
	if (ItemCode == TAG_ITEM_EGKK_HP) {
		*pColorCode = EuroScopePlugIn::TAG_COLOR_DEFAULT;
		*pRGB = TAG_WHITE;

		const char* sid = FlightPlan.GetFlightPlanData().GetSidName();
		const char* rwy = FlightPlan.GetFlightPlanData().GetDepartureRwy();
		if (strcmp(sid, "")) {  // if not equal
			if (!strcmp(rwy, "26L")) {
				if (strstr(sid, "BOGNA") || strstr(sid, "HARDY") || strstr(sid, "NOVMA") || strstr(sid, "IMVUR") || strstr(sid, "SAM") || strstr(sid, "KENET") || strstr(sid, "SFD") || strstr(sid, "DAGGA") || strstr(sid, "TIGER") || strstr(sid, "WIZAD")) {
					strcpy_s(sItemString, 16, "A2");
				}
				else if (strstr(sid, "MIMFO") || strstr(sid, "DVR") || strstr(sid, "FRANE") || strstr(sid, "CLN") || strstr(sid, "LAM")) {
					strcpy_s(sItemString, 16, "A3");
				}
				else {
					strcpy_s(sItemString, 16, "??");
				}
			}
			else if (!strcmp(rwy, "08R")) {
				if (strstr(sid, "ODVIK") || strstr(sid, "DVR") || strstr(sid, "FRANE") || strstr(sid, "CLN") || strstr(sid, "LAM")) {
					strcpy_s(sItemString, 16, "J4");
				}
				else if (strstr(sid, "SFD") || strstr(sid, "NOVMA") || strstr(sid, "IMVUR") || strstr(sid, "SAM") || strstr(sid, "KENET") || strstr(sid, "DAGGA") || strstr(sid, "TIGER") || strstr(sid, "WIZAD")) {
					strcpy_s(sItemString, 16, "J7");
				}
				else {
					strcpy_s(sItemString, 16, "??");
				}
			}
		}
		else {
			strcpy_s(sItemString, 16, "??");
		}
	}
	else if (ItemCode == TAG_ITEM_EGKK_LVL) {
		*pColorCode = EuroScopePlugIn::TAG_COLOR_DEFAULT;
		*pRGB = TAG_WHITE;

		const char* dest = FlightPlan.GetFlightPlanData().GetDestination();
		const char* fpRte = FlightPlan.GetFlightPlanData().GetRoute();

		if (InGroup(dest, fpRte, 1)) strcpy_s(sItemString, 16, "180");
		else if (InGroup(dest, fpRte, 2)) strcpy_s(sItemString, 16, "260");
		else if (InGroup(dest, fpRte, 3)) strcpy_s(sItemString, 16, "240");
		else if (InGroup(dest, fpRte, 4)) strcpy_s(sItemString, 16, "340");
		else if (InGroup(dest, fpRte, 5)) strcpy_s(sItemString, 16, "320");
		else if (InGroup(dest, fpRte, 6)) strcpy_s(sItemString, 16, "340");
		else if (InGroup(dest, fpRte, 71)) strcpy_s(sItemString, 16, "230");
		else if (InGroup(dest, fpRte, 72)) strcpy_s(sItemString, 16, "210");
		else if (InGroup(dest, fpRte, 73)) strcpy_s(sItemString, 16, "290");
		else if (InGroup(dest, fpRte, 8)) strcpy_s(sItemString, 16, "230");
		else if (InGroup(dest, fpRte, 9)) strcpy_s(sItemString, 16, "290");
		else if (InGroup(dest, fpRte, 10)) strcpy_s(sItemString, 16, "250");
		else if (InGroup(dest, fpRte, 11)) strcpy_s(sItemString, 16, "170");
		else if (InGroup(dest, fpRte, 11)) strcpy_s(sItemString, 16, "290");
		else if (InGroup(dest, fpRte, 11)) strcpy_s(sItemString, 16, "350");
		else if (InGroup(dest, fpRte, 11)) strcpy_s(sItemString, 16, "270");
		else if (InGroup(dest, fpRte, 99)) strcpy_s(sItemString, 16, "SPC");
		else if (strcmp(dest, "EGJJ") == 0) strcpy_s(sItemString, 16, "JJ!");
		else strcpy_s(sItemString, 16, "NLC");

		/*if (strcmp(dest, "EGBB") == 0 || strcmp(dest, "EGBE") == 0 || strcmp(dest, "EGNX") == 0) {
			strcpy_s(sItemString, 16, "180");
		} 
		else if (strcmp(dest, "EGCC") == 0 || strcmp(dest, "EGGP") == 0 || strcmp(dest, "EGNR") == 0) {
			strcpy_s(sItemString, 16, "260");
		}
		else if (strcmp(dest, "EGNH") == 0 || strcmp(dest, "EGNJ") == 0 || strcmp(dest, "EGNM") == 0 || strcmp(dest, "EGNO") == 0) {
			strcpy_s(sItemString, 16, "240");
		}
		else if (strcmp(dest, "EGNT") == 0 || strcmp(dest, "EGNV") == 0) {
			strcpy_s(sItemString, 16, "340");
		}
		else {
			strcpy_s(sItemString, 16, "NLC");
		}*/
		//sendMessage(dest);
	}
}

bool CEGKKHoldingPointsPlugin::InGroup(const char* dest, const char* rte, int group) {
	switch (group) {
	case 1: FORCAP(LEVEL_CAPS_1);
	case 2: FORCAP(LEVEL_CAPS_2);
	case 3: FORCAP(LEVEL_CAPS_3);
	case 4: FORCAP(LEVEL_CAPS_4);
	case 5:
		if (strstr(rte, "LIPGO")) {
			FORCAP(LEVEL_CAPS_5);
		}
		break;
	case 6: 
		if (strstr(rte, "LIPGO") || strstr(rte, "VATRY")) {
			FORCAP(LEVEL_CAPS_6);
		}
		break;
	case 71: 
		if ((strstr(rte, "VABIK") || strstr(rte, "REDFA")) && strstr(rte, "EH")) {
			bool in7b = false;
			for (const char* cap : LEVEL_CAPS_7B) {
				if (strcmp(dest, cap) == 0) {
					in7b = true;
				}
			}
			if (!in7b) {
				if (strcmp(dest, "EHAM")) {
					return true;
				}
			}
		}
		break;
	case 72:
		FORCAP(LEVEL_CAPS_7B);
	case 73:
		if (strstr(rte, "REDFA") && strcmp(dest, "EHAM") == 0) {
			return true;
		}
	case 8: FORCAP(LEVEL_CAPS_8);
	case 9: 
		if (!strstr(rte, "KONAN")) {
			FORCAP(LEVEL_CAPS_9);
		}
		break;
	case 10: FORCAP(LEVEL_CAPS_10);
	case 11: FORCAP(LEVEL_CAPS_11);
	case 12: FORCAP(LEVEL_CAPS_12);
	case 13: FORCAP(LEVEL_CAPS_13);
	case 14: FORCAP(LEVEL_CAPS_14);
	case 99: FORCAP(SPECIAL_AIRPORTS);
	default:
		break;
	}

	return false;
}

void CEGKKHoldingPointsPlugin::sendMessage(std::string message) {
	try {
		DisplayUserMessage("EGKK Plugin", "System", message.c_str(), true, true, true, false, false);
	}
	catch (...) {
		DisplayUserMessage("EGKK Plugin", "System", "ERROR", true, true, true, true, false);
	}
}