#pragma once
#include "framework.h"
#include <vector>

class SHAPE
{
    protected: 
        int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        bool isSelected = false;
    public:
        void Set(int x1, int y1, int x2, int y2);

        int GetX1() const;
        int GetY1() const;
        int GetX2() const;
        int GetY2() const;

        SHAPE() {}
        virtual ~SHAPE() = default;
        bool IsSelected() const;
        void SetIsSelected(bool isSelected);
        virtual SHAPE* Clone() const = 0;
        virtual const wchar_t* GetName() const = 0;
        virtual void Show(HDC hdc, HPEN hPen) const = 0;

        virtual std::wstring ToCSV() const
        {
            return std::wstring(GetName()) + L";" +
                std::to_wstring(x1) + L";" +
                std::to_wstring(y1) + L";" +
                std::to_wstring(x2) + L";" +
                std::to_wstring(y2);
        }
};

class CURVE: virtual public SHAPE
{
    private:
        std::vector<POINT> points;
    public:
        CURVE() = default;

        void Show(HDC hdc, HPEN hPen) const override;
        
        SHAPE* Clone() const override;

        const wchar_t* GetName() const override;

        std::wstring ToCSV() const override;

        void AddPoint(int x, int y);
};

class LINE: virtual public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;

        const wchar_t* GetName() const override;
};

class DOT: public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;

        const wchar_t* GetName() const override;
};

class RECTANGLE: virtual public SHAPE
{    
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;

        const wchar_t* GetName() const override;
};

class ELLIPSE: virtual public SHAPE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;

        SHAPE* Clone() const override;

        const wchar_t* GetName() const override;
};

class LINE_OO: public LINE, public ELLIPSE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;
  
        SHAPE* Clone() const override;

        const wchar_t* GetName() const override;
};

class CUBE: public RECTANGLE, public LINE
{
    public:
        void Show(HDC hdc, HPEN hPen) const override;
  
        SHAPE* Clone() const override;

        const wchar_t* GetName() const override;
};