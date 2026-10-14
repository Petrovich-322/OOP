#include "painter.h"

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

static std::string WideToUtf8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    if (size <= 0) return {};
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &result[0], size, nullptr, nullptr);
    return result;
}

bool PAINTER::SaveToCSV(const std::wstring& filePath) const
{
    std::ofstream file(std::filesystem::path(filePath), std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    file.write("\xEF\xBB\xBF", 3);

    file << "ID;Type;X1;Y1;X2;Y2\r\n";

    for (size_t i = 0; i < shapes.size(); ++i)
    {
        if (shapes[i] != nullptr)
        {
            file << (i + 1) << ';' << WideToUtf8(shapes[i]->ToCSV()) << "\r\n";
        }
    }

    return true;
}