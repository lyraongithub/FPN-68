#ifndef SETTINGSBOX_H
#define SETTINGSBOX_H

#include <afxwin.h>

class SettingsBox {
public:
    SettingsBox(CPoint start , int width, int maxWidth, int height, bool shortBox = false);
    CPoint Draw(CDC* pDC);

private:
    CPoint m_start;
    int m_width;
    int m_maxWidth;
    int m_height;
    bool m_shortBox;
};

#endif // SETTINGSBOX_H