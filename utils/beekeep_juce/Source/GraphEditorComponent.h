
#ifndef GRAPHEDITORCOMPONENT_H_INCLUDED
#define GRAPHEDITORCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "op.h"
#include "OpGraph.h"

class OpComponent;

class GraphEditorComponent   :
    public Component,
    public ChangeListener

{
public:
    GraphEditorComponent();
    ~GraphEditorComponent();

    void paint (Graphics&);
    void resized();

    virtual void mouseDown (const MouseEvent& e) override;
    virtual void changeListenerCallback(ChangeBroadcaster* caster);
    
    OpComponent* getComponentForOp (const int op_idx) const;
    
    void addOp(op_id_t id);
    
    void updateComponents();
    
//    
//    const int getCanvasWidth(void) {
//        // FIXME
//        // return CANVAS_W;
//        return getWidth();
//    }
//    const int getCanvasHeight(void) {
//        // return CANVAS_H;
//        return getHeight();
//    }

    
private:
    OpGraph graph;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphEditorComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
