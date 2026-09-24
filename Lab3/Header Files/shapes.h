#pragma once
#include "framework.h"

class SHAPE
{
    protected: 
        int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    public:
        void Set(int x1, int y1, int x2, int y2);

        virtual ~SHAPE() = default;
        virtual SHAPE* Clone() const = 0;
        virtual void Show(HDC hdc, HPEN hPen) const = 0;
};

class LINE: public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;
};

class DOT: public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;
};

class RECTANGLE: public SHAPE
{    
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;
};

class ELLIPSE: public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;
};