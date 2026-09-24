#include "framework.h"
#include "shapes.h"

void SHAPE::Set(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

void LINE::Show(HDC hdc, HPEN hPen) const
{
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    SetBkMode(hdc, oldBkMode);
}

SHAPE* LINE::Clone() const
{
    return new LINE(*this);
}

void DOT::Show(HDC hdc, HPEN hPen) const
{
    SetPixel(hdc, x1, y1, RGB(0, 0, 0));
}

SHAPE* DOT::Clone() const
{
    return new DOT(*this);
}

void RECTANGLE::Show(HDC hdc, HPEN hPen) const
{
    HBRUSH orangeBrush = CreateSolidBrush(RGB(255, 128, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, orangeBrush); 
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    
    Rectangle(hdc, x1, y1, x2, y2);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(orangeBrush);
}

SHAPE* RECTANGLE::Clone() const
{
    return new RECTANGLE(*this);
}

void ELLIPSE::Show(HDC hdc, HPEN hPen) const
{
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

    const int dx = abs(x1 - x2);
    const int dy = abs(y1 - y2);

    Ellipse(hdc, x1 - dx, y1 - dy, x1 + dx, y1 + dy);

    SelectObject(hdc, oldPen);
}

SHAPE* ELLIPSE::Clone() const
{
    return new ELLIPSE(*this);
}