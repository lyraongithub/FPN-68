#pragma once
#include "EuroScopePlugIn.h"
#include "Constant.h"

#include <string>
#include <vector>
#include "pch.h"
#include <afxpriv.h>
#include <tuple>

class CFPNRadarTarget
{
public:
	CFPNRadarTarget(std::string callsign, EuroScopePlugIn::CPosition pos, int altitude, EuroScopePlugIn::CPosition runwayThreshold, float runwayHeading, int radarRange, float glideslopeAngle, CRect glideslopeArea, CRect trackArea);  // Threshold alt matters!!!
	~CFPNRadarTarget();

	void updatePosition(EuroScopePlugIn::CPosition pos, int altitude);
	void draw(CDC *pDC);

	std::string callsign;
private:
	EuroScopePlugIn::CPosition pos;
	int altitude;
	SYSTEMTIME posAltTime;
	EuroScopePlugIn::CPosition runwayThreshold;
	float runwayHeading;
	int radarRange;
	float glideslopeAngle;
	CRect glideslopeArea;
	CRect trackArea;

	EuroScopePlugIn::CPosition previousPos;
	int previousAltitude = -1;
	SYSTEMTIME previousPosAltTime;

	std::vector<std::tuple<EuroScopePlugIn::CPosition, int, SYSTEMTIME>> pastPositions;
};

