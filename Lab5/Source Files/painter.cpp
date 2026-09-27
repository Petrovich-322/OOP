#include "framework.h"
#include "painter.h"
#include "fstream"
#include "string"
#include "vector"

PAINTER& PAINTER::GetInstance()
{
    static PAINTER instance;
    return instance;
}

PAINTER::~PAINTER()
{
    if (shape != nullptr)
    {
        delete shape;
        shape = nullptr;
    }

    for (SHAPE* s : shapes)
    {
        delete s;
    }
    shapes.clear();
}

void PAINTER::RemoveShapeByIndex(size_t index)
{
    if (index < shapes.size())
    {
        delete shapes[index];
        shapes.erase(shapes.begin() + index);
    }
}

const std::vector<SHAPE*>& PAINTER::GetShapes() const
{
    return shapes;
}

void PAINTER::SetProtoShape(std::string type)
{
    if(type == "dot") protoShape = &protoDot;
    if(type == "line") protoShape = &protoLine;
    if(type == "rectangle") protoShape = &protoRectangle;
    if(type == "ellipse") protoShape = &protoEllipse;
    if(type == "line_oo") protoShape = &protoLine_OO;
    if(type == "cube") protoShape = &protoCube;   
    if(type == "curve") protoShape = &protoCurve;
}

void PAINTER::StartDrawing(HWND hWnd, int startX, int startY)
{
    this->startX = startX;
    this->startY = startY;

    if (shape != nullptr)
    {
        delete shape;
    }

    shape = protoShape->Clone();
    shape->Set(startX, startY, startX, startY);

    CURVE* curve = dynamic_cast<CURVE*>(shape);
    if (curve != nullptr)
    {
        curve->AddPoint(startX, startY);
    }

    isDrawing = true;
}

void PAINTER::TempDrawing(HWND hWnd, int endX, int endY)
{
    if (isDrawing && shape != nullptr)
    {
        CURVE* curve = dynamic_cast<CURVE*>(shape);
        if (curve != nullptr)
        {
            curve->AddPoint(endX, endY);
        }
        else
        {
            shape->Set(startX, startY, endX, endY);
        }

        InvalidateRect(hWnd, NULL, FALSE);
    }
}

void PAINTER::EndDrawing(HWND hWnd, int endX, int endY)
{
    if (isDrawing && shape != nullptr)
    {
        if (shapes.size() < MAX_SHAPES)
        {
            CURVE* curve = dynamic_cast<CURVE*>(shape);
            
            if (curve != nullptr)
            {
                curve->AddPoint(endX, endY);
            }
            else {
                shape->Set(startX, startY, endX, endY);
            }
            
            shapes.push_back(shape);
            shape = nullptr;
        }

        else 
        {
            delete shape;
            shape = nullptr;
        }

        isDrawing = false;
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void PAINTER::DrawAll(HDC hdc) const
{
    HPEN defPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN dashPen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
    HPEN bluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
    
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        if (shapes[i] != nullptr)
        {
            bool isSelected = shapes[i]->IsSelected();
            if(isSelected) 
            {
                shapes[i]->Show(hdc, bluePen);
            }
            else 
            {
                shapes[i]->Show(hdc, defPen);
            }
        }
    }

    if (isDrawing && shape != nullptr)
    {
        shape->Show(hdc, dashPen);
    }

    DeleteObject(dashPen);  
    DeleteObject(defPen);
}

bool PAINTER::SaveToCSV(const std::wstring& filePath) const
{
    int len = WideCharToMultiByte(CP_ACP, 0, filePath.c_str(), -1, NULL, 0, NULL, NULL);
    std::string narrowPath(len, 0);
    WideCharToMultiByte(CP_ACP, 0, filePath.c_str(), -1, &narrowPath[0], len, NULL, NULL);

    std::ofstream file(narrowPath.c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
    file.write((const char*)bom, 3);

    std::string header = "ID;Type;X1;Y1;X2;Y2\r\n";
    file.write(header.c_str(), header.length());

    for (size_t i = 0; i < shapes.size(); ++i)
    {
        if (shapes[i] != nullptr)
        {
            std::wstring line = std::to_wstring(i + 1) + L";" + shapes[i]->ToCSV() + L"\r\n";

            int bytesNeeded = WideCharToMultiByte(CP_UTF8, 0, line.c_str(), (int)line.length(), NULL, 0, NULL, NULL);
            if (bytesNeeded > 0)
            {
                std::string utf8Line(bytesNeeded, 0);
                WideCharToMultiByte(CP_UTF8, 0, line.c_str(), (int)line.length(), &utf8Line[0], bytesNeeded, NULL, NULL);
                file.write(utf8Line.c_str(), utf8Line.length());
            }
        }
    }

    file.close();
    return true;
}