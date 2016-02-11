/*
  ==============================================================================

    GraphicsUtils.h
    Created: 10 Feb 2016 10:45:11pm
    Author:  ezra

  ==============================================================================
*/

#ifndef GFXUTIL_H_INCLUDED
#define GFXUTIL_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"


class GfxUtil {
public:
    
    static const int CANVAS_W;
    static const int CANVAS_H;
    
    // convert coordinate as screen proportion to pixel
    static int screenToPix(double x);
    static Point<int> screenToPix(Point<double>p);
    
    // convert pixel to screen coordinate
    static double pixToScreen(int x);
    static Point<double> pixToScreen(Point<int>p);
    
    // convert coordinate as screen proportion to canvas proportion
    static double screenToCanvas(double x);
    
    // biggest screen dimension of main display
    static int maxScreenDimension(void);
};



#endif  // GRAPHICSUTILS_H_INCLUDED
