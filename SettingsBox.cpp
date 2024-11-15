#pragma once
#include "pch.h"
#include "SettingsBox.h"
#include <vector>
#include <string>
#include <sstream>

SettingsBox::SettingsBox(CPoint start,const std::string& title, const std::vector<std::vector<std::string>>& text, int maxWidth, bool shortBox, bool closeBox)
    : m_start(start),m_title(title), m_text(text), m_maxWidth(maxWidth), m_shortBox(shortBox), m_closeBox(closeBox) {}

CPoint SettingsBox::Draw(CDC* pDC) {
    int boxHeight;
    int titleBoxHeight = 30;
    if (m_shortBox) {
        boxHeight = 30;
    }
    else {
        boxHeight = 40;
    }

    int padding = 5;
    int m_height = m_text.size();
    int m_width;
    if (m_height > 0) {
        m_width = m_text[0].size();
    }
    else {
        m_width = 0;
    }

    m_start.y -= ((m_height * boxHeight) + titleBoxHeight); // create space for text boxes + title box

    // Define the main rectangle
    CRect boxRect(m_start.x, m_start.y, m_start.x + m_maxWidth, m_start.y + (m_height * boxHeight) + titleBoxHeight);
    boxRect.DeflateRect(10, 10);
    boxRect.OffsetRect(0, 2);

    // Create and select the white pen
    CPen whitePen(PS_SOLID, padding, RGB(255, 255, 255));
    CPen* oldPen = pDC->SelectObject(&whitePen);

    // Select the null brush
    CBrush* oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    // Draw the main rectangle
    pDC->Rectangle(boxRect);

    // Restore the old pen and brush
    pDC->SelectObject(oldPen);
    pDC->SelectObject(oldBrush);

    // Define the light blue rectangle across the top, padded from the white border
    CPoint topRectPoint(boxRect.left , boxRect.top);
    CSize topRectSize(boxRect.Width(), titleBoxHeight);
    CRect topRect(topRectPoint,topRectSize);
    topRect.DeflateRect(padding, padding);

    // Create and select the light blue pen and brush
    CPen lightBluePen(PS_SOLID, 1, RGB(0, 187, 253));
    oldPen = pDC->SelectObject(&lightBluePen);
    CBrush lightBlueBrush(RGB(0, 187, 253));
    oldBrush = pDC->SelectObject(&lightBlueBrush);

    // Draw the title rectangle
    pDC->Rectangle(topRect);
    CString text(m_title.c_str());
    pDC->DrawText(text, topRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // define settings box widths
    auto boxWidth = (boxRect.Width() - 4 - padding) / m_width;
    boxHeight = (boxRect.Height() - titleBoxHeight) / m_height;

    CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
    CBrush blackBrush(RGB(0, 0, 0));

    pDC->SelectObject(&blackPen);
    pDC->SelectObject(&blackBrush);
    // Draw boxes
for (int row = 0; row < m_height; ++row) {
    for (int col = 0; col < m_width; ++col) {
        std::string text = m_text[row][col];
        if (text.empty()) {
            continue;
        }

        bool flipColors = false;
        if (text[0] == '.') { // is the box selected
            flipColors = true;
            text = text.substr(1); 
        }

        int left = topRect.left + (col * boxWidth);
        int top = topRect.bottom + (row * boxHeight);
        int right = left + boxWidth;
        int bottom = top + boxHeight;

        CRect subBox(left, top, right, bottom);
        subBox.DeflateRect(2, 2);

        // Set colors based on flipColors
        COLORREF boxColor = flipColors ? RGB(253, 254, 222) : RGB(87, 86, 104);
        COLORREF textColor = flipColors ? RGB(87, 86, 104) : RGB(253, 254, 222);

        CBrush brush(boxColor);
        CPen pen(PS_SOLID, 1, boxColor);

        pDC->SelectObject(&pen);
        pDC->SelectObject(&brush);
        pDC->Rectangle(subBox);

        // Split the text on '\n'
        std::istringstream stream(text);
        std::string line;
        int lineCount = std::count(text.begin(), text.end(), '\n') + 1;
        int lineHeight = subBox.Height() / lineCount;
        int currentTop = subBox.top;

        // Set text color
        pDC->SetTextColor(textColor);

        while (std::getline(stream, line)) {
            CString cstrText(line.c_str());
            CRect lineRect(subBox.left, currentTop, subBox.right, currentTop + lineHeight);
            pDC->DrawText(cstrText, lineRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            currentTop += lineHeight;
        }
    }
}

    // Restore the old pen and brush
    pDC->SelectObject(oldPen);
    pDC->SelectObject(oldBrush);


    // Return the new bottom-left point of the main rectangle
    return CPoint(m_start.x, boxRect.top + padding);
}