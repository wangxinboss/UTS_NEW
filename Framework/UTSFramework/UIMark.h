#pragma once

//------------------------------------------------------------------------------
// UI mark struct for operator & device
typedef struct _ui_point_
{
    CPoint ptPos;
    COLORREF color;
} UI_POINT;

typedef struct _ui_line_
{
    CPoint ptBegin;
    CPoint ptEnd;
    COLORREF color;
} UI_LINE;

typedef struct _ui_rect_
{
    CRect rcPos;
    COLORREF color;
} UI_RECT;

typedef struct _ui_circle_
{
    CPoint ptCenter;
    double dRadius;
    COLORREF color;
} UI_CIRCLE;

typedef struct _ui_text_
{
    CString strText;
    COLORREF color;
    CPoint ptPos;
} UI_TEXT;

typedef struct _ui_text_rel_
{
    CString strText;
    COLORREF color;
    double dWidthPersent;
    double dHeightPersent;
} UI_TEXT_REL;

typedef struct _ui_mark_
{
    struct _ui_mark_()
    {
        vecUiCircle.clear();
        vecUiLine.clear();
        vecUiPoint.clear();
        vecUiRect.clear();
        vecUIText.clear();
        vecUITextRel.clear();
    }

    void clear()
    {
        vecUiCircle.clear();
        vecUiLine.clear();
        vecUiPoint.clear();
        vecUiRect.clear();
        vecUIText.clear();
        vecUITextRel.clear();
    }

    vector<UI_POINT> vecUiPoint;
    vector<UI_LINE> vecUiLine;
    vector<UI_RECT> vecUiRect;
    vector<UI_CIRCLE> vecUiCircle;
    vector<UI_TEXT> vecUIText;          // 文字，绝对坐标
    vector<UI_TEXT_REL> vecUITextRel;   // 文字，百分比坐标
} UI_MARK;
