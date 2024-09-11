#pragma once
#include "pch.h"
#include "SettingsBox.h"
#include <vector>
#include <string>

SettingsBox::SettingsBox(CPoint start,const std::string& title, const std::vector<std::vector<std::string>>& text, int maxWidth, bool shortBox, bool closeBox)
    : m_start(start),m_title(title), m_text(text), m_maxWidth(maxWidth), m_shortBox(shortBox), m_closeBox(closeBox) {}

CPoint SettingsBox::Draw(CDC* pDC) {
    int boxHeight = 50;
    int padding = 5;
    m_height = m_text.size();
    if (m_height > 0) {
        m_width = m_text[0].size();
    }
    else {
        m_width = 0;
    }

    if (m_width > m_maxWidth) {
        m_width = m_maxWidth;
    }

    m_start.y -= (m_height * boxHeight + boxHeight / 2);

    // Define the main rectangle
    CRect boxRect(m_start.x, m_start.y, m_start.x + m_maxWidth, m_start.y + m_height * boxHeight + boxHeight / 2);
    boxRect.DeflateRect(10, 10);
    boxRect.OffsetRect(0, 6);

    // Create and select the white pen
    CPen whitePen(PS_SOLID, 5, RGB(255, 255, 255));
    CPen* oldPen = pDC->SelectObject(&whitePen);

    // Select the null brush
    CBrush* oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    // Draw the main rectangle
    pDC->Rectangle(boxRect);

    // Restore the old pen and brush
    pDC->SelectObject(oldPen);
    pDC->SelectObject(oldBrush);

    // Define the light blue rectangle across the top, padded 2px from the white border
    CPoint topRectPoint(boxRect.left + (2 + padding), boxRect.top + (2 + padding));
    CSize topRectSize(boxRect.Width() - padding*3, boxHeight / 2);
    CRect topRect(topRectPoint,topRectSize);

    // Create and select the light blue pen and brush
    CPen lightBluePen(PS_SOLID, 1, RGB(0, 187, 253));
    oldPen = pDC->SelectObject(&lightBluePen);
    CBrush lightBlueBrush(RGB(0, 187, 253));
    oldBrush = pDC->SelectObject(&lightBlueBrush);

    // Draw the light blue rectangle
    pDC->Rectangle(topRect);
    CString text(m_title.c_str());
    pDC->DrawText(text, topRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


    auto boxWidth = (boxRect.Width() - padding * 3) / m_width;
    boxHeight = (boxRect.Height() - topRect.Height() - padding * 4) / m_height;

    CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
    CBrush blackBrush(RGB(0, 0, 0));

    pDC->SelectObject(&blackPen);
    pDC->SelectObject(&blackBrush);
    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            const std::string& text = m_text[row][col];
            if (text.empty()){
                continue;
            }
            // Calculate the position of the current box relative to boxRect
            int left = topRect.left + (col * boxWidth);
            int top = topRect.bottom + padding + (row * boxHeight);
            int right = left + boxWidth;
            int bottom = top + boxHeight;

            CRect subBox(left, top, right, bottom);
            subBox.DeflateRect(2, 2);
            pDC->Rectangle(subBox);
            CString cstrText(text.c_str());
            pDC->DrawText(cstrText, subBox, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    // Restore the old pen and brush
    pDC->SelectObject(oldPen);
    pDC->SelectObject(oldBrush);


    // Return the new bottom-left point of the main rectangle
    return CPoint(m_start.x, boxRect.top + padding);
}