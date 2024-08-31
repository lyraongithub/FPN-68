#pragma once
#include "EuroScopePlugIn.h"
#include <string>
#include "pch.h"

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

    virtual void sendMessage(std::string message);
};

