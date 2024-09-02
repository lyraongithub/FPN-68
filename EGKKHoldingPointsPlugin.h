#pragma once
#include "EuroScopePlugIn.h"
#include "CFPNRadarTarget.h"
#include <string>
#include "pch.h"
#include <vector>

class CFPNPlugin : public EuroScopePlugIn::CPlugIn {

public:
    CFPNPlugin();
    virtual ~CFPNPlugin();

    virtual EuroScopePlugIn::CRadarScreen* OnRadarScreenCreated(const char* sDisplayName,
        bool NeedRadarContent,
        bool GeoReferenced,
        bool CanBeSaved,
        bool CanBeCreated
    );

    virtual bool OnCompileCommand(const char* sCommandLine);

    virtual void sendMessage(std::string message);
    inline std::vector<CFPNRadarTarget>* getPreviousTargets() {
        return &previousTargets;
    }

    std::vector<CFPNRadarTarget> previousTargets = std::vector<CFPNRadarTarget>();

    int range = 15;
    EuroScopePlugIn::CPosition runwayThreshold;
    EuroScopePlugIn::CPosition otherThreshold;

private:
    void loadNewAerodrome(const char* icao, const char* runway);
};

