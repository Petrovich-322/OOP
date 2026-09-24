#pragma once

#include "framework.h"
#include "shapes.h"
#include "resource.h"

class PAINTER
{
private:
    int startX = 0;
    int startY = 0;
    int shapesCount = 0;

    LINE protoLine;
    RECTANGLE protoRectangle;
    DOT protoDot;
    ELLIPSE protoEllipse;

    SHAPE* shape = nullptr;
    SHAPE* protoShape = &protoDot;

    bool isDrawing = false;

    SHAPE* shapes[MAX_SHAPES]{};

public:
    PAINTER();
    ~PAINTER();

    void SetProtoShape(int type);
    int GetShapesCount() const;

    void startDrawing(HWND hWnd, int startX, int startY);
    void tempDrawing(HWND hWnd, int endX, int endY);
    void endDrawing(HWND hWnd, int endX, int endY);

    void DrawAll(HDC hdc, HPEN hPen) const;
};