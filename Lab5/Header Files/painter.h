#pragma once

#include "framework.h"
#include "shapes.h"
#include "resource.h"

class PAINTER
{
    private:
        int startX = 0;
        int startY = 0;

        LINE protoLine;
        RECTANGLE protoRectangle;
        DOT protoDot;
        ELLIPSE protoEllipse;
        LINE_OO protoLine_OO;
        CUBE protoCube;
        CURVE protoCurve;

        SHAPE* shape = nullptr;
        SHAPE* protoShape = &protoCurve;

        bool isDrawing = false;

        std::vector<SHAPE*> shapes;
        
        PAINTER() = default;
    public:
        static PAINTER& GetInstance();
        ~PAINTER();

        void SetProtoShape(std::string type);
        void RemoveShapeByIndex(size_t index);
        
        const std::vector<SHAPE*>& GetShapes() const;

        void startDrawing(HWND hWnd, int startX, int startY);
        void tempDrawing(HWND hWnd, int endX, int endY);
        void endDrawing(HWND hWnd, int endX, int endY);

        void DrawAll(HDC hdc) const;
};