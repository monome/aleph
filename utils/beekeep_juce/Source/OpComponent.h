/*
  ==============================================================================

    OpComponent.h
    Created: 10 Feb 2016 3:19:25am
    Author:  ezra

  ==============================================================================
*/

#ifndef OPCOMPONENT_H_INCLUDED
#define OPCOMPONENT_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"
#include "op.h"
#include "OpGraph.h"
#include "GraphEditorComponent.h"

class OpComponent : public Component
{
public:
    OpComponent (OpGraph* graph_, op_t* op, u16 idx);
    ~OpComponent();
    
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    
    // TODO: override thist to include pin children
    //bool hitTest (int x, int y) override;
    
    void paint (Graphics& g) override;
    void update(void);
    
    // return number of vertical pixels needed for op
    static const int ROW_W = 200;
    static const int ROW_H = 20;
    static int getPixelHeight(op_t* op);
    
    const int getCanvasWidth(void) {
        //return GraphEditorComponent::CANVAS_W;
        return getParentWidth();
    }
    const int getCanvasHeight(void) {
        //return GraphEditorComponent::CANVAS_H;
        return getParentHeight();
    }
    
    GraphEditorComponent* getGraphEditor() const noexcept
    {
        return findParentComponentOfClass<GraphEditorComponent>();
    }
    
    OpGraph* graph_;
    op_t* op_;
    u16 op_idx_;
    
    u8 num_ins_;
    u8 num_outs_;
    
    int pin_size_ ;
    Font font_ ;
    
    Point<int> pos0_;
    
};


#endif  // OPCOMPONENT_H_INCLUDED
