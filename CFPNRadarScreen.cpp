#include "pch.h"
#include "CFPNRadarScreen.h"
#define _USE_MATH_DEFINES
#include <math.h>

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

	int range = 10;

	drawGSAxes(&dc, glideslopeArea);
	drawTrackAxes(&dc, trackArea);
	drawVerticalScale(&dc, glideslopeArea, trackArea, range);
	drawGlidepathAndHorizontalTicks(&dc, glideslopeArea, trackArea, range, 3.0f);

	drawInfoText(&dc, glideslopeArea.left + 80, glideslopeArea.bottom - 10);

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
	int xAxisLeft = area.left + 20;
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
	int xAxisLeft = area.left + 20;
	pDC->MoveTo(xAxisLeft, xAxisHeight);
	pDC->LineTo(area.right, xAxisHeight);
}

void CFPNRadarScreen::drawVerticalScale(CDC* pDC, CRect glideslopeArea, CRect trackArea, int range) {
	CPen axesPen(0, 2, AXES_COLOUR);
	pDC->SelectObject(&axesPen);

	CFont font;
	font.CreatePointFont(100, L"Courier New", pDC);
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
	font.CreatePointFont(100, L"Courier New", pDC);
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
	int xAxisLeft = glideslopeArea.left + 20;

	int topOfGS = xAxisHeight + (tan(angle * (M_PI / 180)) * 6076.0f * (range / 3000.0f) * (double)((glideslopeArea.top - glideslopeArea.bottom) * 2 / 9));

	pDC->MoveTo(xAxisLeft, xAxisHeight);
	pDC->LineTo(glideslopeArea.right, topOfGS);

	// Both axis ticks and text
	CPen axesPen(0, 2, AXES_COLOUR);
	pDC->SelectObject(&axesPen);

	CFont font;
	font.CreatePointFont(120, L"Courier New", pDC);
	pDC->SetTextColor(AXES_COLOUR);
	pDC->SetTextAlign(TA_CENTER);
	auto* defFont = pDC->SelectObject(&font);

	int trackXAxisHeight = trackArea.CenterPoint().y;
	int trackXAxisLeft = trackArea.left + 20;

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
		int yPos = xAxisHeight + (tan(angle * (M_PI / 180)) * 6076.0f * (((float)i/2.0f) / 3000.0f) * (double)((glideslopeArea.top - glideslopeArea.bottom) * 2 / 9));
		pDC->MoveTo(xPos, yPos - displacement);  // GS
		pDC->LineTo(xPos, yPos + displacement);
	}

	pDC->SelectObject(defFont);
	font.DeleteObject();
}

void CFPNRadarScreen::drawInfoText(CDC* pDC, int x, int y) {
	CFont font;
	font.CreatePointFont(130, L"Courier New", pDC);
	pDC->SetTextColor(RGB(200, 255, 211));
	pDC->SetTextAlign(TA_LEFT);
	auto* defFont = pDC->SelectObject(&font);

	std::string text = "GS: 3.0  RWY: 204.7  DH: 200FT     TIME: ";

	SYSTEMTIME st;
	GetSystemTime(&st);

	text += (st.wHour < 10 ? "0" : "") + std::to_string(st.wHour) + ":"
		 + (st.wMinute < 10 ? "0" : "") + std::to_string(st.wMinute) + ":"
		 + (st.wSecond < 10 ? "0" : "") + std::to_string(st.wSecond);

	pDC->TextOutW(x, y, text.c_str());

	pDC->SelectObject(defFont);
	font.DeleteObject();
}
