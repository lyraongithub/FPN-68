#include "pch.h"
#include "CFPNRadarTarget.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>


CFPNRadarTarget::CFPNRadarTarget(std::string callsign, EuroScopePlugIn::CPosition pos,int groundSpeed, int altitude, EuroScopePlugIn::CPosition runwayThreshold, float runwayHeading, int radarRange, float glideslopeAngle, CRect glideslopeArea, CRect trackArea) {
	this->callsign = callsign;
	this->pos = pos;
	this->groundSpeed = groundSpeed;
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

void logMessage(const std::string& message) {
	std::ofstream logFile("FPN_plugin.log", std::ios_base::app);
	if (logFile.is_open()) {
		logFile << message << std::endl;
		logFile.close();
	}
}

void CFPNRadarTarget::updatePosition(EuroScopePlugIn::CPosition pos,int groundSpeed, int altitude, int radarRange, EuroScopePlugIn::CPosition runwayThreshold, EuroScopePlugIn::CPosition otherThreshold) {
	this->radarRange = radarRange;
	this->runwayThreshold = runwayThreshold;
	this->runwayHeading = runwayThreshold.DirectionTo(otherThreshold);
	
	if (this->pos.m_Latitude != pos.m_Latitude || this->pos.m_Longitude != pos.m_Longitude || this->altitude != altitude) {
		previousPos = this->pos;
		previousGroundSpeed = this->groundSpeed;
		previousAltitude = this->altitude;
		previousPosAltTime = posAltTime;

		this->pos = pos;
		this->altitude = altitude;
		this->groundSpeed = groundSpeed;

		SYSTEMTIME st;
		GetSystemTime(&st);

		posAltTime = st;

		pastPositions.push_back(std::tuple<EuroScopePlugIn::CPosition, int, SYSTEMTIME>(pos, altitude, st));
	} else {  // lerp
		if (previousAltitude == -1) return;

		double deltaT = (posAltTime.wSecond * 1000 + posAltTime.wMilliseconds) - (previousPosAltTime.wSecond * 1000 + previousPosAltTime.wMilliseconds);
		if (deltaT == 0) return; // Don't divide by 0 please


		EuroScopePlugIn::CPosition deltaPos = EuroScopePlugIn::CPosition();
		deltaPos.m_Latitude = pos.m_Latitude - previousPos.m_Latitude;
		deltaPos.m_Longitude = pos.m_Longitude - previousPos.m_Longitude;

		int deltaAlt = altitude - previousAltitude;

		SYSTEMTIME st;
		GetSystemTime(&st);

        double currentTotalMilliseconds = st.wSecond * 1000 + st.wMilliseconds;
        double previousTotalMilliseconds = posAltTime.wSecond * 1000 + posAltTime.wMilliseconds;

        double newDeltaT = currentTotalMilliseconds - previousTotalMilliseconds;
		if (newDeltaT < 0){
			newDeltaT += 60000; // account for minute change
		}

		double timeMultiplier = newDeltaT / deltaT;
		
		EuroScopePlugIn::CPosition newDeltaPos = EuroScopePlugIn::CPosition();
		newDeltaPos.m_Latitude = timeMultiplier * deltaPos.m_Latitude;
		newDeltaPos.m_Longitude = timeMultiplier * deltaPos.m_Longitude;
		int newDeltaAlt = timeMultiplier * deltaAlt;

		EuroScopePlugIn::CPosition newPos = EuroScopePlugIn::CPosition();
		newPos.m_Latitude = pos.m_Latitude + newDeltaPos.m_Latitude;
		newPos.m_Longitude = pos.m_Longitude + newDeltaPos.m_Longitude;
		// Check if the new position is far away from the last one
		double latDifferenceNM = (newPos.m_Latitude - pos.m_Latitude) * 60.0;
		double lonDifferenceNM = (newPos.m_Longitude - pos.m_Longitude) * 60.0;

		// Calculate the distance in nautical miles
		double distanceNM = sqrt(pow(latDifferenceNM, 2) + pow(lonDifferenceNM, 2));

		std::string logMsg = "Current Time: " + std::to_string(st.wHour) + ":" + std::to_string(st.wMinute) + ":" + std::to_string(st.wSecond) + "." + std::to_string(st.wMilliseconds) + ":";

		if (distanceNM > 0.5) { // Adjust the threshold as needed
			logMsg += "\n==================Large Jump Detected===========================\n";
		}
		logMsg += " / Previous Position: (" + std::to_string(pos.m_Latitude) + ", " + std::to_string(pos.m_Longitude) + ")";
		logMsg += " / New Position: (" + std::to_string(newPos.m_Latitude) + ", " + std::to_string(newPos.m_Longitude) + ")";
		logMsg += " / Distance: " + std::to_string(distanceNM);
		logMsg += " / DeltaT: " + std::to_string(deltaT) + " / NewDeltaT: " + std::to_string(newDeltaT);
		logMsg += " / CurrentMs: " + std::to_string(currentTotalMilliseconds) + " / PreviousMs: " + std::to_string(previousTotalMilliseconds) + "\n";
		logMessage(logMsg);

		pastPositions.push_back(std::tuple<EuroScopePlugIn::CPosition, int, SYSTEMTIME>(newPos, altitude + newDeltaAlt, posAltTime)); 
	}
	

	
	if (pastPositions.size() > 5) {
		pastPositions.erase(pastPositions.begin());
	}
}

void CFPNRadarTarget::draw(CDC *pDC) {
	int index = 0;
	for (auto& target : pastPositions) {
		EuroScopePlugIn::CPosition thisPos = std::get<0>(target);
		int thisAlt = std::get<1>(target);

		float distanceToRunway = thisPos.DistanceTo(runwayThreshold);
		float hdgToRunway = thisPos.DirectionTo(runwayThreshold);
		float trackDeviationAngle = runwayHeading - hdgToRunway;
		float range = distanceToRunway * cos(trackDeviationAngle * (M_PI / 180));

		int xAxisHeight = glideslopeArea.bottom + (glideslopeArea.top - glideslopeArea.bottom) / 9;
		int xAxisLeft = glideslopeArea.left + X_AXIS_OFFSET;
		int xPos = xAxisLeft + (glideslopeArea.right - xAxisLeft) * range / (radarRange);

		double angle = atan2(thisAlt, range);
		double apparentAlt = tan(angle) * range;
		int yPos = xAxisHeight + (apparentAlt / (radarRange * 200)) * (double)((glideslopeArea.top - glideslopeArea.bottom) * 2 / 9);

		CPen primaryPen(0, 1, PRIMARY_COLOUR);
		pDC->SelectObject(&primaryPen);

		//CBrush primaryBrush(PRIMARY_COLOUR);
		pDC->SetDCBrushColor(PRIMARY_COLOUR);

		pDC->MoveTo(xPos, yPos);
		pDC->Ellipse(xPos - 3, yPos - 3, xPos + 3, yPos + 3);

		if (index == 4) { // Vertical tag
			
			COLORREF originalTextColor = pDC->GetTextColor();
			pDC->SetTextColor(TRACK_DEVIATION_COLOUR);
			pDC->TextOutW(xPos - 3, yPos - 18, _T("A"));

			CPen thickPen(PS_SOLID, 3, TRACK_DEVIATION_COLOUR);
			CPen* pOldPen = pDC->SelectObject(&thickPen);
			pDC->MoveTo(xPos - 10, yPos - 10);
			pDC->LineTo(xPos - 27, yPos - 27);

			pDC->SelectObject(pOldPen);


			CSize textSize = pDC->GetTextExtent(_T("A"));
			int textHeight = textSize.cy;
			int totalTextHeight = textHeight * 3;
			int spacing = 10;

			pDC->TextOutW(xPos - 35, yPos - 30 - totalTextHeight, _T("S"));

			//GroundSpeed / distance
			std::wstringstream wss;
			wss << std::fixed << std::setprecision(1) << distanceToRunway;
			std::wstring dist = wss.str();

			std::wstring groundSpeedStr = std::to_wstring(groundSpeed) + L" " + dist;
			CString groundSpeedCStr(groundSpeedStr.c_str());

			pDC->TextOutW(xPos - 75, yPos - 30 - totalTextHeight + textHeight + 3, groundSpeedCStr);

			// Vertical Deviation
			int altDiff = static_cast<int>(apparentAlt - ((tan(3 * (M_PI) / 180) * (distanceToRunway*6076.0f))));

			if (altDiff > 0) { // High
				std::wstringstream ss;
				ss << L"+" << std::setw(3) << std::setfill(L'0') << altDiff << L"\u2193";
				std::wstring altOffset = ss.str();
				CString altOffsetCStr = altOffset.c_str();
				pDC->TextOutW(xPos - 75, yPos - 30 - totalTextHeight + textHeight * 2 + 3, altOffsetCStr);
			}
			else if (altDiff < 0) { // Low
				std::wstringstream ss;
				ss << L"-" << std::setw(3) << std::setfill(L'0') << -altDiff << L"\u2191";
				std::wstring altOffset = ss.str();
				CString altOffsetCStr = altOffset.c_str();
				pDC->TextOutW(xPos - 75, yPos - 30 - totalTextHeight + textHeight * 2 + 3, altOffsetCStr);
			}


			pDC->SetTextColor(originalTextColor);
		}

		int trackXAxisHeight = trackArea.CenterPoint().y;
		int trackXAxisLeft = trackArea.left + X_AXIS_OFFSET;

		xPos = trackXAxisLeft + (trackArea.right - trackXAxisLeft) * range / (radarRange);
		yPos = trackXAxisHeight + (tan(trackDeviationAngle * (M_PI / 180)) * 6076.0f * (range / 6000.0f) * (double)((trackArea.top - trackArea.bottom) / 8));

		pDC->MoveTo(xPos, yPos);
		pDC->Ellipse(xPos - 3, yPos - 3, xPos + 3, yPos + 3);
		if (index == 4) { // Horizontal tag

			COLORREF originalTextColor = pDC->GetTextColor();
			pDC->SetTextColor(TRACK_DEVIATION_COLOUR);
			pDC->TextOutW(xPos - 3, yPos - 18, _T("A"));

			CPen thickPen(PS_SOLID, 3, TRACK_DEVIATION_COLOUR);
			CPen* pOldPen = pDC->SelectObject(&thickPen);
			pDC->MoveTo(xPos - 10, yPos - 10);
			pDC->LineTo(xPos - 27, yPos - 27);

			pDC->SelectObject(pOldPen);


			CSize textSize = pDC->GetTextExtent(_T("A"));
			int textHeight = textSize.cy;
			int totalTextHeight = textHeight * 3;
			int spacing = 10;

			pDC->TextOutW(xPos - 35, yPos - 30 - totalTextHeight, _T("S"));
			// GroundSpeed Dist line
			std::wstringstream wss;
			wss << std::fixed << std::setprecision(1) << distanceToRunway;
			std::wstring dist = wss.str();

			std::wstring groundSpeedStr = std::to_wstring(groundSpeed) + L" " + dist;
			CString groundSpeedCStr(groundSpeedStr.c_str());

			pDC->TextOutW(xPos - 75, yPos - 30 - totalTextHeight + textHeight + 3, groundSpeedCStr);

			// Lateral Deviation
			int lateralOffset = static_cast<int>(tan(trackDeviationAngle * (M_PI / 180.0)) * (distanceToRunway*6076.0f));
			
			if (lateralOffset > 0) { // Right of centerline
				std::wstringstream ss;
				ss << L"+" << std::setw(3) << std::setfill(L'0') << lateralOffset << L"\u2190";
				std::wstring trackOffset = ss.str();
				CString trackOffsetCStr = trackOffset.c_str();
				pDC->TextOutW(xPos - 75, yPos - 30 - totalTextHeight + textHeight * 2 + 3, trackOffsetCStr);
			}
			else if (lateralOffset < 0) { // Left
				std::wstringstream ss;
				ss << L"-" << std::setw(3) << std::setfill(L'0') << -lateralOffset << L"\u2192";
				std::wstring trackOffset = ss.str();
				CString trackOffsetCStr = trackOffset.c_str();
				pDC->TextOutW(xPos - 75, yPos - 30 - totalTextHeight + textHeight * 2 + 3, trackOffsetCStr);
			}


			pDC->SetTextColor(originalTextColor);
		}

		index++;
	}
}
