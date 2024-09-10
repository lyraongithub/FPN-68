#include "pch.h"
#include "SettingsBox.h"

SettingsBox::SettingsBox(CPoint start, int width, int maxWidth, int height, bool shortBox)
    : m_start(start), m_width(width), m_maxWidth(maxWidth), m_height(height), m_shortBox(shortBox) {}

CPoint SettingsBox::Draw(CDC* pDC) {
    int boxHeight = 50;
    int padding = 5;
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

    auto boxWidth = (boxRect.Width() - padding * 3) / m_width;
    boxHeight = (boxRect.Height() - topRect.Height() - padding * 4) / m_height;

    CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
    CBrush blackBrush(RGB(0, 0, 0));

    pDC->SelectObject(&blackPen);
    pDC->SelectObject(&blackBrush);
    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            // Calculate the position of the current box relative to boxRect
            int left = topRect.left + (col * boxWidth);
            int top = topRect.bottom + padding + (row * boxHeight);
            int right = left + boxWidth;
            int bottom = top + boxHeight;

            CRect subBox(left, top, right, bottom);
            subBox.DeflateRect(2, 2);
            pDC->Rectangle(subBox);
        }
    }

    // Restore the old pen and brush
    pDC->SelectObject(oldPen);
    pDC->SelectObject(oldBrush);


    // Return the new bottom-left point of the main rectangle
    return CPoint(boxRect.left, boxRect.bottom);
}