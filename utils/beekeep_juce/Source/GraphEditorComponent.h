
#ifndef GRAPHEDITORCOMPONENT_H_INCLUDED
#define GRAPHEDITORCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "op.h"
#include "OpGraph.h"
#include "ConnectorComponent.h"

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
    
    ConnectorComponent* getConnectorForInput(int op_idx, int in_idx);
    ConnectorComponent* getConnectorForOutput(int op_idx, int out_idx);

private:
    OpGraph graph;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphEditorComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
