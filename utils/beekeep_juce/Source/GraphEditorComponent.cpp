
#include "GraphEditorComponent.h"
#include "OpComponent.h"

#include "GfxUtil.h"

#include "net.h"
#include "net_protected.h"
#include "op.h"

GraphEditorComponent::GraphEditorComponent()

{
    setSize (GfxUtil::CANVAS_W, GfxUtil::CANVAS_H);
    graph.addChangeListener (this);
    
    // build initial graph from system operators
    graph.repopulate();
    
    // set initial node positions
    //// FIXME: looks like crap
    double y = 0;
    double x = OpComponent::getScreenWidth() * 0.5;
    for(int i=0; i<graph.getNumOps(); i++){
        OpGraph::Node* node = graph.getNode(i);
        double h = OpComponent::getScreenHeight(node->op_) + 0.001;
        if(i==0) {
            y = h * 0.5;
        } else {
            y += h;
        }
        while(y > 1.0) {
            y -= 1.0;
            x += OpComponent::getScreenWidth() * 1.1;
        }
        if(y < (h*0.5)) { y = h*0.5; }
        node->x_ = x;
        node->y_ = y;
    }
    
    updateComponents();
    
    changeListenerCallback(&graph);
}

GraphEditorComponent::~GraphEditorComponent()
{
    graph.removeChangeListener (this);
    deleteAllChildren();
}

void GraphEditorComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void GraphEditorComponent::resized()
{
    updateComponents();
}

void GraphEditorComponent::changeListenerCallback(ChangeBroadcaster* caster) {
    updateComponents();
}

void GraphEditorComponent::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;
        
        // add operator types
        for (int i=0; i<NUM_USER_OP_TYPES; i++) {
            m.addItem(i + 1, op_registry[userOpTypes[i]].name);
        }
        
        const int r = m.show();
        if(r > 0) {
            // add selected operator
            graph.addOpNode((op_id_t)(userOpTypes[r - 1]),
                            GfxUtil::pixToScreen(e.x),
                            GfxUtil::pixToScreen(e.y) );
            
        }
    }
}

OpComponent* GraphEditorComponent::getComponentForOp (const int op_idx) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (OpComponent* const c = dynamic_cast<OpComponent*> (getChildComponent (i)))
            if (c->getIdx() == op_idx)
                return c;
    }
    
    return nullptr;
}

void GraphEditorComponent::updateComponents()
{
    
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (OpComponent* const oc = dynamic_cast<OpComponent*> (getChildComponent (i))) {
            oc->update();
        }
    }
    
    for (int i = graph.getNumOps(); --i >= 0;)
    {
        const OpGraph::Node* node (graph.getNode (i));
        
        if (getComponentForOp (node->idx_) == nullptr)
        {
            OpComponent* const comp = new OpComponent (&graph, node->op_, node->idx_);
            addAndMakeVisible (comp);
            comp->update();
        }
    }
}

ConnectorComponent* GraphEditorComponent::getConnectorForInput(int op_idx, int in_idx) {
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (ConnectorComponent* const c = dynamic_cast<ConnectorComponent*> (getChildComponent (i))) {
            if(c->getDstOp() == op_idx && c->getDstIn() == in_idx) {
                return c;
            }
        }
    }
    return nullptr;
}

ConnectorComponent* GraphEditorComponent::getConnectorForOutput(int op_idx, int out_idx) {
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (ConnectorComponent* const c = dynamic_cast<ConnectorComponent*> (getChildComponent (i))) {
            if(c->getSrcOp() == op_idx && c->getSrcOut() == out_idx) {
                return c;
            }
        }
    }
    return nullptr;
}