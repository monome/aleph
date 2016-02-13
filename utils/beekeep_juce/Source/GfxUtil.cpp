/*
  ==============================================================================

    GraphicsUtil.cpp
    Created: 10 Feb 2016 11:10:42pm
    Author:  ezra

  ==============================================================================
*/

#include "GfxUtil.h"


const int GfxUtil::CANVAS_W = 10000;
const int GfxUtil::CANVAS_H = 10000;

int GfxUtil::screenToPix(double x) {
    return (int)(x * maxScreenDimension());
}

Point<int> screenToPix(Point<double>p) {
    return Point<int>(GfxUtil::screenToPix(p.x), GfxUtil::screenToPix(p.y) );
}

double GfxUtil::pixToScreen(int x) {
    return (double)x / (double)maxScreenDimension();
}

Point<double> GfxUtil::pixToScreen(Point<int>p) {
    return Point<double>(GfxUtil::pixToScreen(p.x), GfxUtil::pixToScreen(p.y));
}

double GfxUtil::screenToCanvas(double x) {
    return x * GfxUtil::maxScreenDimension() / std::max(GfxUtil::CANVAS_W, GfxUtil::CANVAS_H);
}

int GfxUtil::maxScreenDimension(void) {
    Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().totalArea;
    return std::max(r.getWidth(), r.getHeight());
}