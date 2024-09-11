#ifndef SETTINGSBOX_H
#define SETTINGSBOX_H

#pragma once
#include <string>
#include <vector>
#include <afxwin.h>

class SettingsBox {
public:
    SettingsBox(CPoint start, const std::string& title, const std::vector<std::vector<std::string>>& text, int maxWidth, bool shortBox = false, bool closeBox = false);
    CPoint Draw(CDC* pDC);

private:
    CPoint m_start;
    
    int m_width;
    int m_maxWidth;
    int m_height;
    bool m_shortBox;
    bool m_closeBox;
    std::string m_title;
    std::vector<std::vector<std::string>> m_text;

};

#endif // SETTINGSBOX_H