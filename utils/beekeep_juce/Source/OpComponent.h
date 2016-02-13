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
    // row width and height in screen units
    static const double ROW_W;
    static const double ROW_H;
    
    // get required component height for given op, in screen units
    static double getScreenHeight(op_t* op);
    static double getScreenWidth(void) { return ROW_W; }
    
public:
    OpComponent (OpGraph* graph_, op_t* op, u16 idx);
    ~OpComponent();
    
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    
    void paint (Graphics& g) override;
    void update(void);
    
    // get position in screen coordinates for input connection point
    // relative to parent
    Point<double> getInputScreenPos(int idx);
    
    // get position in screen coordinates for outptu connection point
    // relative to parent
    Point<double> getOutputScreenPos(int idx);


private:
    void addInputComponent(int idx);
    void addOutputComponent(int idx);
    void addNameComponent(void);
    
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
    
public:
    int getIdx(void) { return op_idx_; }
    
};


#endif  // OPCOMPONENT_H_INCLUDED
