// EGKKHoldingPoints.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
//#include "framework.h"
#include "EGKKHoldingPointsPlugin.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

CFPNPlugin* pMyPlugIn = NULL;

// added stuff

void __declspec (dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance) {
	// allocate
	*ppPlugInInstance = pMyPlugIn = new CFPNPlugin();
}

void __declspec (dllexport) EuroScopePlugInExit(void) {
	delete pMyPlugIn;
}

