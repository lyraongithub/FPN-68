#include "pch.h"
#include "CFPNRadarScreen.h"

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
	CDC dc;
	dc.Attach(hDC);

	CRect radarArea = GetRadarArea();
	//CRect chatArea = GetChatArea();
	radarArea.DeflateRect(70, 50);  // margins
	
	// Split screen in 2 down the middle
	CRect glideslopeArea = CRect(radarArea.left, radarArea.top, radarArea.right, radarArea.CenterPoint().y);
	CRect trackArea = CRect(radarArea.left, radarArea.CenterPoint().y, radarArea.right, radarArea.bottom);
	glideslopeArea.bottom -= 30;
	trackArea.top += 30;

	drawGSAxes(&dc, glideslopeArea);
	drawTrackAxes(&dc, trackArea);

	drawInfoText(&dc, glideslopeArea.left + 80, glideslopeArea.bottom - 15);

	dc.Detach();
}

void CFPNRadarScreen::drawGSAxes(CDC *pDC, CRect area) {
	//((CFPNPlugin*) GetPlugIn())->sendMessage("Drawing axes");
	CPen axesPen(0, 2, RGB(255, 202, 189));
	pDC->SelectObject(&axesPen);

	// Y Axis
	pDC->MoveTo(area.left, area.top);
	pDC->LineTo(area.left, area.bottom);

	// Y Ticks (fixed)
	pDC->MoveTo(area.left - 5, area.bottom);
	pDC->LineTo(area.left + 5, area.bottom);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 3 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 3 / 9);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 5 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 5 / 9);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 7 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 7 / 9);
	pDC->MoveTo(area.left - 5, area.top);
	pDC->LineTo(area.left + 5, area.top);
	
	// X Axis
	int xAxisHeight = area.bottom + (area.top - area.bottom) / 9;
	int xAxisLeft = area.left + 20;
	pDC->MoveTo(xAxisLeft, xAxisHeight);
	pDC->LineTo(area.right, xAxisHeight);
}

void CFPNRadarScreen::drawTrackAxes(CDC* pDC, CRect area) {
	CPen axesPen(0, 2, RGB(255, 202, 189));
	pDC->SelectObject(&axesPen);

	// Y Axis
	pDC->MoveTo(area.left, area.top);
	pDC->LineTo(area.left, area.bottom);

	// Y Ticks (fixed)
	pDC->MoveTo(area.left - 5, area.bottom);
	pDC->LineTo(area.left + 5, area.bottom);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 2 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 2 / 9);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 3 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 3 / 9);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 4 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 4 / 9);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 6 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 6 / 9);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 7 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 7 / 9);
	pDC->MoveTo(area.left - 5, area.bottom + (area.top - area.bottom) * 8 / 9);
	pDC->LineTo(area.left + 5, area.bottom + (area.top - area.bottom) * 8 / 9);
	pDC->MoveTo(area.left - 5, area.top);
	pDC->LineTo(area.left + 5, area.top);

	// X Axis
	int xAxisHeight = area.CenterPoint().y;
	int xAxisLeft = area.left + 20;
	pDC->MoveTo(xAxisLeft, xAxisHeight);
	pDC->LineTo(area.right, xAxisHeight);
}

void CFPNRadarScreen::drawInfoText(CDC* pDC, int x, int y) {
	CFont font;
	font.CreatePointFont(130, L"Courier New", pDC);
	pDC->SetTextColor(RGB(200, 255, 211));
	CFont *defFont = pDC->SelectObject(&font);

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
