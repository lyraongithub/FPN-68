#include "pch.h"
#include "CFPNRadarScreen.h"
#include "CFPNRadarTarget.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <sstream>
#include <iomanip>

CFPNRadarScreen::CFPNRadarScreen() {

}

CFPNRadarScreen::~CFPNRadarScreen() = default;  // this is weird!

void CFPNRadarScreen::OnAsrContentLoaded(bool loaded) {
	// TODO: load settings
}

void CFPNRadarScreen::OnAsrContentToBeSaved() {
	// TODO: save settings
}

void CFPNRadarScreen::OnRefresh(HDC hDC, int Phase) {
	if (Phase != EuroScopePlugIn::REFRESH_PHASE_BEFORE_TAGS) return;

	CDC dc;
	dc.Attach(hDC);

	CRect radarArea = GetRadarArea();

	dc.FillRect(radarArea, &CBrush(BACKGROUND_COLOUR));

	//CRect chatArea = GetChatArea();
	radarArea.DeflateRect(70, 50);  // margins

	radarArea.right -= radarArea.right / 4;
	
	// Split screen in 2 across the middle
	CRect glideslopeArea = CRect(radarArea.left, radarArea.top, radarArea.right, radarArea.CenterPoint().y);
	CRect trackArea = CRect(radarArea.left, radarArea.CenterPoint().y, radarArea.right, radarArea.bottom);
	glideslopeArea.bottom -= 30;
	trackArea.top += 30;

	int range = ((CFPNPlugin*)GetPlugIn())->range;

	drawVerticalScale(&dc, glideslopeArea, trackArea, range);
	drawGlidepathAndHorizontalTicks(&dc, glideslopeArea, trackArea, range, 3.0f);
	drawGSAxes(&dc, glideslopeArea);
	drawTrackAxes(&dc, trackArea);

	drawInfoText(&dc, glideslopeArea.left + 80, glideslopeArea.bottom - 10);

	EuroScopePlugIn::CPosition runwayThreshold = ((CFPNPlugin*)GetPlugIn())->runwayThreshold;
	//runwayThreshold.LoadFromStrings("W000.10.19.000", "N051.09.02.420");

	EuroScopePlugIn::CPosition otherThreshold = ((CFPNPlugin*)GetPlugIn())->otherThreshold;
	//otherThreshold.LoadFromStrings("W000.12.24.520", "N051.08.45.120");

	std::vector<std::string> foundCallsigns = std::vector<std::string>();

	for (EuroScopePlugIn::CRadarTarget target = GetPlugIn()->RadarTargetSelectFirst(); target.IsValid(); target = GetPlugIn()->RadarTargetSelectNext(target)) {
		EuroScopePlugIn::CPosition pos = target.GetPosition().GetPosition();
		if (pos.DistanceTo(runwayThreshold) > range * 1.5) continue;

		int altitude = target.GetPosition().GetPressureAltitude();
		float hdgToRunway = pos.DirectionTo(runwayThreshold);
		float trackDeviationAngle = runwayThreshold.DirectionTo(otherThreshold) - hdgToRunway;

		if (abs(trackDeviationAngle) > 8) continue;

		std::string callsign = target.GetCallsign();
		foundCallsigns.push_back(callsign);

		bool found = false;
		std::vector<CFPNRadarTarget> *prevTargets = ((CFPNPlugin*)GetPlugIn())->getPreviousTargets();
		for (int i = 0; i < prevTargets->size(); i++) {
			if ((prevTargets->at(i)).callsign == callsign) {
				(prevTargets->at(i)).updatePosition(pos, altitude, range, runwayThreshold, otherThreshold);
				(prevTargets->at(i)).draw(&dc);

				found = true;
				break;
			}
		}
		if (!found) {
			CFPNRadarTarget targetPlot = CFPNRadarTarget(callsign, pos, altitude, runwayThreshold, runwayThreshold.DirectionTo(otherThreshold), range, 3.0f, glideslopeArea, trackArea);
			targetPlot.draw(&dc);
			prevTargets->push_back(targetPlot);
		}
	}

	/*EuroScopePlugIn::CPosition pos;
	pos.LoadFromStrings("E000.01.00.319", "N051.10.17.164");
	int altitude = 3000;
	CFPNRadarTarget targetPlot = CFPNRadarTarget(pos, altitude, runwayThreshold, runwayThreshold.DirectionTo(otherThreshold), range, 3.0f, glideslopeArea, trackArea);
	targetPlot.draw(&dc, (CFPNPlugin*)GetPlugIn());*/

	dc.Detach();
}

void CFPNRadarScreen::drawGSAxes(CDC *pDC, CRect area) {
	CPen axesPen(0, 2, AXES_COLOUR);
	pDC->SelectObject(&axesPen);

	// Y Axis
	pDC->MoveTo(area.left, area.top);
	pDC->LineTo(area.left, area.bottom);
	
	// X Axis
	int xAxisHeight = area.bottom + (area.top - area.bottom) / 9;
	int xAxisLeft = area.left + X_AXIS_OFFSET;
	pDC->MoveTo(xAxisLeft, xAxisHeight);
	pDC->LineTo(area.right, xAxisHeight);
}

void CFPNRadarScreen::drawTrackAxes(CDC* pDC, CRect area) {
	CPen axesPen(0, 2, AXES_COLOUR);
	pDC->SelectObject(&axesPen);

	// Y Axis
	pDC->MoveTo(area.left, area.top);
	pDC->LineTo(area.left, area.bottom);

	// X Axis
	int xAxisHeight = area.CenterPoint().y;
	int xAxisLeft = area.left + X_AXIS_OFFSET;
	pDC->MoveTo(xAxisLeft, xAxisHeight);
	pDC->LineTo(area.right, xAxisHeight);
}

void CFPNRadarScreen::drawVerticalScale(CDC* pDC, CRect glideslopeArea, CRect trackArea, int range) {
	CPen axesPen(0, 2, AXES_COLOUR);
	pDC->SelectObject(&axesPen);

	CFont font;
	font.CreatePointFont(100, L"Courier New Bold", pDC);
	pDC->SetTextColor(AXES_COLOUR);
	pDC->SetTextAlign(TA_RIGHT);
	auto* defFont = pDC->SelectObject(&font);

	// Glideslope
	pDC->MoveTo(glideslopeArea.left - 8, glideslopeArea.bottom);
	pDC->LineTo(glideslopeArea.left + 8, glideslopeArea.bottom);
	pDC->TextOutW(glideslopeArea.left - 13, glideslopeArea.bottom - 9, ("-" + std::to_string(range * 100)).c_str());

	for (int i = 1; i <= 4; i++) {
		int yPos = glideslopeArea.bottom + (glideslopeArea.top - glideslopeArea.bottom) * (2 * i + 1) / 9;
		pDC->MoveTo(glideslopeArea.left - 8, yPos);
		pDC->LineTo(glideslopeArea.left + 8, yPos);
		pDC->TextOutW(glideslopeArea.left - 13, yPos - 7, std::to_string(i * 200 * range).c_str());
	}

	// Track
	for (int i = 0; i <= 8; i++) {
		if (i == 4) continue;

		int yPos = trackArea.bottom + (trackArea.top - trackArea.bottom) * i / 8;
		pDC->MoveTo(trackArea.left - 8, yPos);
		pDC->LineTo(trackArea.left + 8, yPos);
		pDC->TextOutW(trackArea.left - 13, yPos - 7, std::to_string((i - 4) * 400 * range).c_str());
	}

	pDC->SelectObject(defFont);
	font.DeleteObject();
}

void CFPNRadarScreen::drawHorizontalScale(CDC* pDC, CRect glideslopeArea, CRect trackArea, int range) {
	CPen axesPen(0, 2, AXES_COLOUR);
	pDC->SelectObject(&axesPen);

	CFont font;
	font.CreatePointFont(100, L"Courier New Bold", pDC);
	pDC->SetTextColor(AXES_COLOUR);
	pDC->SetTextAlign(TA_RIGHT);
	auto* defFont = pDC->SelectObject(&font);

	pDC->SelectObject(defFont);
	font.DeleteObject();
}

void CFPNRadarScreen::drawGlidepathAndHorizontalTicks(CDC* pDC, CRect glideslopeArea, CRect trackArea, int range, float angle) {
	// Glideslope
	CPen glideslopePen(0, 2, GLIDESLOPE_COLOUR);
	pDC->SelectObject(&glideslopePen);

	int xAxisHeight = glideslopeArea.bottom + (glideslopeArea.top - glideslopeArea.bottom) / 9;
	int xAxisLeft = glideslopeArea.left + X_AXIS_OFFSET;

	int topOfGS = xAxisHeight + (tan(angle * (M_PI / 180)) * 6076.0f * ((float)range / (range * 200)) * (double)((glideslopeArea.top - glideslopeArea.bottom) * 2 / 9));

	pDC->MoveTo(xAxisLeft, xAxisHeight);
	pDC->LineTo(glideslopeArea.right, topOfGS);

	// Track paths @ 1.25 and 3 degrees
	CPen trackDeviationPen(0, 1, TRACK_DEVIATION_COLOUR);
	pDC->SelectObject(&trackDeviationPen);

	int trackXAxisHeight = trackArea.CenterPoint().y;
	int trackXAxisLeft = trackArea.left + X_AXIS_OFFSET;

	for (int i = 0; i < 4; i++) {
		float angle;
		if (i == 0) angle = 3.0f;
		else if (i == 1) angle = 1.25f;
		else if (i == 2) angle = -1.25f;
		else if (i == 3) angle = -3.0f;

		int trackOffset = trackXAxisHeight + (tan(angle * (M_PI / 180)) * 6076.0f * ((float)range / (range * 400)) * (double)((trackArea.top - trackArea.bottom) / 8));

		pDC->MoveTo(trackXAxisLeft, trackXAxisHeight);
		pDC->LineTo(trackArea.right, trackOffset);
	}

	// Both axis ticks and text
	CPen axesPen(0, 2, AXES_COLOUR);
	pDC->SelectObject(&axesPen);

	CFont font;
	font.CreatePointFont(120, L"Courier New Bold", pDC);
	pDC->SetTextColor(AXES_COLOUR);
	pDC->SetTextAlign(TA_CENTER);
	auto* defFont = pDC->SelectObject(&font);

	for (int i = 0; i <= range * 2; i++) {
		int xPos = xAxisLeft + (glideslopeArea.right - xAxisLeft) * i / (range * 2);
		int displacement = i % 2 == 0 ? 8 : 4;
		pDC->MoveTo(xPos, xAxisHeight - displacement);  // GS
		pDC->LineTo(xPos, xAxisHeight + displacement);
		if (i % 2 == 0) pDC->TextOutW(xPos, xAxisHeight + 9, std::to_string(i / 2).c_str());

		pDC->MoveTo(xPos, trackXAxisHeight - displacement);  // track
		pDC->LineTo(xPos, trackXAxisHeight + displacement);
		if (i % 2 == 0) pDC->TextOutW(xPos, trackXAxisHeight + 9, std::to_string(i / 2).c_str());
	}

	// GS Ticks
	pDC->SelectObject(&glideslopePen);

	for (int i = 0; i <= range * 2; i++) {
		int xPos = xAxisLeft + (glideslopeArea.right - xAxisLeft) * i / (range * 2);
		int displacement = i % 2 == 0 ? 8 : 4;
		int yPos = xAxisHeight + (tan(angle * (M_PI / 180)) * 6076.0f * (((float)i/2.0f) / (range * 200)) * (double)((glideslopeArea.top - glideslopeArea.bottom) * 2 / 9));
		pDC->MoveTo(xPos, yPos - displacement);  // GS
		pDC->LineTo(xPos, yPos + displacement);
	}

	pDC->SelectObject(defFont);
	font.DeleteObject();
}

void CFPNRadarScreen::drawInfoText(CDC* pDC, int x, int y) {
	CFont font;
	font.CreatePointFont(130, L"Courier New Bold", pDC);
	pDC->SetTextColor(RGB(200, 255, 211));
	pDC->SetTextAlign(TA_LEFT);
	auto* defFont = pDC->SelectObject(&font);

	std::string text = "GS: 3.0  RWY: ";

	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << ((CFPNPlugin*)GetPlugIn())->runwayThreshold.DirectionTo(((CFPNPlugin*)GetPlugIn())->otherThreshold);

	text += stream.str();
	text += "  DH: 200FT     TIME : ";

	SYSTEMTIME st;
	GetSystemTime(&st);

	text += (st.wHour < 10 ? "0" : "") + std::to_string(st.wHour) + ":"
		 + (st.wMinute < 10 ? "0" : "") + std::to_string(st.wMinute) + ":"
		 + (st.wSecond < 10 ? "0" : "") + std::to_string(st.wSecond);

	pDC->TextOutW(x, y, text.c_str());

	pDC->SelectObject(defFont);
	font.DeleteObject();
}
