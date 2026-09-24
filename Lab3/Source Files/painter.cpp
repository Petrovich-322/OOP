#include "framework.h"
#include "painter.h"

PAINTER::PAINTER() {}
PAINTER::~PAINTER()
{
    if (shape != nullptr)
    {
        delete shape;
        shape = nullptr;
    }

    for (int i = 0; i < shapesCount; ++i)
    {
        if (shapes[i] != nullptr)
        {
            delete shapes[i];
            shapes[i] = nullptr;
        }
    }
}

void PAINTER::SetProtoShape(int type)
{
    switch (type)
    {
        case 1:
            protoShape = &protoDot;
            break;
        case 2:
            protoShape = &protoLine;
            break;
        case 3:
            protoShape = &protoRectangle;
            break;
        case 4:
            protoShape = &protoEllipse;
            break;
    }
}

int PAINTER::GetShapesCount() const
{
    return shapesCount;
}

void PAINTER::startDrawing(HWND hWnd, int startX, int startY)
{
    this->startX = startX;
    this->startY = startY;

    if (shape != nullptr)
    {
        delete shape;
    }

    shape = protoShape->Clone();
    shape->Set(startX, startY, startX, startY);

    isDrawing = true;
}

void PAINTER::tempDrawing(HWND hWnd, int endX, int endY)
{
    if (isDrawing && shape != nullptr)
    {
        shape->Set(startX, startY, endX, endY);
        InvalidateRect(hWnd, NULL, FALSE);
    }
}

void PAINTER::endDrawing(HWND hWnd, int endX, int endY)
{
    if (isDrawing)
    {
        if (shapesCount < MAX_SHAPES)
        {
            SHAPE* finalShape = protoShape->Clone();
            finalShape->Set(startX, startY, endX, endY);
            shapes[shapesCount++] = finalShape;
        }

        if (shape != nullptr)
        {
            delete shape;
            shape = nullptr;
        }

        isDrawing = false;
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void PAINTER::DrawAll(HDC hdc, HPEN hPen) const
{
    for (int i = 0; i < shapesCount; ++i)
    {
        if (shapes[i] != nullptr)
        {
            shapes[i]->Show(hdc, hPen);
        }
    }

    if (isDrawing && shape != nullptr)
    {
        shape->Show(hdc, hPen);
    }
}