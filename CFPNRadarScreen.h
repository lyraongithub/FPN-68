#pragma once
#include "EuroScopePlugIn.h"
#include "EGKKHoldingPointsPlugin.h"
#include <afxpriv.h>

class CFPNRadarScreen :
    public EuroScopePlugIn::CRadarScreen
{
public:
    CFPNRadarScreen();
    virtual ~CFPNRadarScreen();

    void OnAsrContentLoaded(bool loaded) override;
    void OnAsrContentToBeSaved() override;
    void OnRefresh(HDC hDC, int Phase) override;

    inline void OnAsrContentToBeClosed(void) override {
        delete this;
    }

private:
    void drawGSAxes(CDC *pDC, CRect area);
    void drawTrackAxes(CDC* pDC, CRect area);
    void drawInfoText(CDC *pDC, int x, int y);
};

