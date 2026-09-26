#include "framework.h"
#include "shapes.h"

void SHAPE::Set(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

int SHAPE::GetX1() const { return x1; }
int SHAPE::GetX2() const { return x2; }
int SHAPE::GetY1() const { return y1; }
int SHAPE::GetY2() const { return y2; }



///CURVE

void CURVE::Show(HDC hdc, HPEN hPen) const 
{
    if(points.size() < 2 && !points.empty()) 
    {
        SetPixel(hdc, points[0].x, points[0].y, RGB(0, 0, 0));
        return;
    }
    
    Polyline(hdc, points.data(), (int)points.size());
}

void CURVE::AddPoint(int x, int y)
{
    points.push_back({x, y});
    if (points.size() == 1)
    {
        x1 = x2 = x;
        y1 = y2 = y;
    }
    else
    {
        x2 = x;
        y2 = y;
    }
}

const wchar_t* LINE::GetName() const { return L"line"; }

SHAPE* CURVE::Clone() const
{
    CURVE* copy = new CURVE();
    copy->points = this->points;
    copy->x1 = this->x1; copy->y1 = this->y1;
    copy->x2 = this->x2; copy->y2 = this->y2;
    return copy;
}

// LINE

void LINE::Show(HDC hdc, HPEN hPen) const
{
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    SetBkMode(hdc, oldBkMode);
}

const wchar_t* CURVE::GetName() const { return L"curve"; }

SHAPE* LINE::Clone() const
{
    return new LINE(*this);
}

//DOT

void DOT::Show(HDC hdc, HPEN hPen) const
{
    SetPixel(hdc, x1, y1, RGB(0, 0, 0));
}

const wchar_t* DOT::GetName() const { return L"dot"; }

SHAPE* DOT::Clone() const
{
    return new DOT(*this);
}



//RECTANGLE

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

const wchar_t* RECTANGLE::GetName() const { return L"rectangle"; }

SHAPE* RECTANGLE::Clone() const
{
    return new RECTANGLE(*this);
}


//ELLIPSE

void ELLIPSE::Show(HDC hdc, HPEN hPen) const
{
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

    const int dx = abs(x1 - x2);
    const int dy = abs(y1 - y2);

    Ellipse(hdc, x1 - dx, y1 - dy, x1 + dx, y1 + dy);

    SelectObject(hdc, oldPen);
}

const wchar_t* ELLIPSE::GetName() const { return L"ellipse"; }

SHAPE* ELLIPSE::Clone() const
{
    return new ELLIPSE(*this);
}


//LINE_OO

SHAPE* LINE_OO::Clone() const
{
    return new LINE_OO(*this);
}

void LINE_OO::Show(HDC hdc, HPEN hPen) const
{
    LINE::Show(hdc, hPen);

    const int radius = 6;

    ELLIPSE circle;
    circle.Set(x1, y1, x1 + radius, y1 + radius);
    circle.Show(hdc, hPen);

    circle.Set(x2, y2, x2 + radius, y2 + radius);
    circle.Show(hdc, hPen);
}

const wchar_t* LINE_OO::GetName() const { return L"line_oo"; }



//CUBE

SHAPE* CUBE::Clone() const
{
    return new CUBE(*this);
}

const wchar_t* CUBE::GetName() const { return L"cube"; }

void CUBE::Show(HDC hdc, HPEN hPen) const
{
    const int dx = (x2 - x1) / 3;
    const int dy = (y2 - y1) / 3;

    //back rec
    RECTANGLE backRect;
    backRect.Set(x1 + dx, y1, x2, y2 - dy);
    backRect.Show(hdc, hPen);

    //front rec
    RECTANGLE frontRect;
    frontRect.Set(x1, y1 + dy, x2 - dx, y2);
    frontRect.Show(hdc, hPen);


    LINE edge;
    // lt
    edge.Set(x1, y1 + dy, x1 + dx, y1);
    edge.Show(hdc, hPen);

    // rt
    edge.Set(x2 - dx, y1 + dy, x2, y1);
    edge.Show(hdc, hPen);

    // //ld
    // edge.Set(x1, y2, x1 + dx, y2 - dy);
    // edge.Show(hdc, hPen);

    // rd
    edge.Set(x2 - dx, y2, x2, y2 - dy);
    edge.Show(hdc, hPen);
}