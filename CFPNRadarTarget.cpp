#include "pch.h"
#include "CFPNRadarTarget.h"
#define _USE_MATH_DEFINES
#include <math.h>

CFPNRadarTarget::CFPNRadarTarget(std::string callsign, EuroScopePlugIn::CPosition pos, int altitude, EuroScopePlugIn::CPosition runwayThreshold, float runwayHeading, int radarRange, float glideslopeAngle, CRect glideslopeArea, CRect trackArea) {
	this->callsign = callsign;
	this->pos = pos;
	this->altitude = altitude;
	this->runwayThreshold = runwayThreshold;
	this->runwayHeading = runwayHeading;
	this->radarRange = radarRange;
	this->glideslopeAngle = glideslopeAngle;
	this->glideslopeArea = glideslopeArea;
	this->trackArea = trackArea;

	pastPositions = std::vector<std::tuple<EuroScopePlugIn::CPosition, int, SYSTEMTIME>>();

	SYSTEMTIME st;
	GetSystemTime(&st);
	posAltTime = st;

	pastPositions.push_back(std::tuple<EuroScopePlugIn::CPosition, int, SYSTEMTIME>(pos, altitude, st));
}

CFPNRadarTarget::~CFPNRadarTarget() {

}

void CFPNRadarTarget::updatePosition(EuroScopePlugIn::CPosition pos, int altitude) {
	if (this->pos.m_Latitude != pos.m_Latitude || this->pos.m_Longitude != pos.m_Longitude || this->altitude != altitude) {
		previousPos = this->pos;
		previousAltitude = this->altitude;
		previousPosAltTime = posAltTime;

		this->pos = pos;
		this->altitude = altitude;

		SYSTEMTIME st;
		GetSystemTime(&st);

		posAltTime = st;

		pastPositions.push_back(std::tuple<EuroScopePlugIn::CPosition, int, SYSTEMTIME>(pos, altitude, st));
	} else {  // lerp
		if (previousAltitude == -1) return;
		double deltaT = posAltTime.wSecond + posAltTime.wMilliseconds / 1000.0f - previousPosAltTime.wSecond - previousPosAltTime.wMilliseconds / 1000.0f;

		EuroScopePlugIn::CPosition deltaPos = EuroScopePlugIn::CPosition();
		deltaPos.m_Latitude = pos.m_Latitude - previousPos.m_Latitude;
		deltaPos.m_Longitude = pos.m_Longitude - previousPos.m_Longitude;

		int deltaAlt = altitude - previousAltitude;

		SYSTEMTIME st;
		GetSystemTime(&st);

		double newDeltaT = st.wSecond + st.wMilliseconds / 1000.0f - posAltTime.wSecond - posAltTime.wMilliseconds / 1000.0f;

		double timeMultiplier = newDeltaT / deltaT;

		EuroScopePlugIn::CPosition newDeltaPos = EuroScopePlugIn::CPosition();
		newDeltaPos.m_Latitude = timeMultiplier * deltaPos.m_Latitude;
		newDeltaPos.m_Longitude = timeMultiplier * deltaPos.m_Longitude;
		int newDeltaAlt = timeMultiplier * deltaAlt;

		EuroScopePlugIn::CPosition newPos = EuroScopePlugIn::CPosition();
		newPos.m_Latitude = pos.m_Latitude + newDeltaPos.m_Latitude;
		newPos.m_Longitude = pos.m_Longitude + newDeltaPos.m_Longitude;

		pastPositions.push_back(std::tuple<EuroScopePlugIn::CPosition, int, SYSTEMTIME>(newPos, altitude + newDeltaAlt, posAltTime));  // time is wrong
	}
	

	
	if (pastPositions.size() > 5) {
		pastPositions.erase(pastPositions.begin());
	}
}

void CFPNRadarTarget::draw(CDC *pDC) {
	for (auto& target : pastPositions) {
		EuroScopePlugIn::CPosition thisPos = std::get<0>(target);
		int thisAlt = std::get<1>(target);

		float distanceToRunway = thisPos.DistanceTo(runwayThreshold);
		float hdgToRunway = thisPos.DirectionTo(runwayThreshold);
		float trackDeviationAngle = runwayHeading - hdgToRunway;
		float range = distanceToRunway * cos(trackDeviationAngle * (M_PI / 180));

		// Current method (unrealistic but easier): Use altitude scaling to plot as if altitude scale was good.
		// Better method (TODO): Calculate angle between receiver and plane using range and alt, then use tan formulae to calculate apparent altitude
		int xAxisHeight = glideslopeArea.bottom + (glideslopeArea.top - glideslopeArea.bottom) / 9;
		int xAxisLeft = glideslopeArea.left + X_AXIS_OFFSET;
		int xPos = xAxisLeft + (glideslopeArea.right - xAxisLeft) * range / (radarRange);
		int yPos = xAxisHeight + (thisAlt / 3000.0f) * (double)((glideslopeArea.top - glideslopeArea.bottom) * 2 / 9);

		CPen primaryPen(0, 1, PRIMARY_COLOUR);
		pDC->SelectObject(&primaryPen);

		//CBrush primaryBrush(PRIMARY_COLOUR);
		pDC->SetDCBrushColor(PRIMARY_COLOUR);

		pDC->MoveTo(xPos, yPos);
		pDC->Ellipse(xPos - 3, yPos - 3, xPos + 3, yPos + 3);

		int trackXAxisHeight = trackArea.CenterPoint().y;
		int trackXAxisLeft = trackArea.left + X_AXIS_OFFSET;

		xPos = trackXAxisLeft + (trackArea.right - trackXAxisLeft) * range / (radarRange);
		yPos = trackXAxisHeight + (tan(trackDeviationAngle * (M_PI / 180)) * 6076.0f * (range / 6000.0f) * (double)((trackArea.top - trackArea.bottom) / 8));

		pDC->MoveTo(xPos, yPos);
		pDC->Ellipse(xPos - 3, yPos - 3, xPos + 3, yPos + 3);
	}
}
