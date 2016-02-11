
#include "GraphEditorComponent.h"
#include "OpComponent.h"

#include "net.h"
#include "net_protected.h"
#include "op.h"

GraphEditorComponent::GraphEditorComponent()

{
    setSize (CANVAS_W, CANVAS_H);
    graph.addChangeListener (this);
    
    // build initial graph from system operators
    graph.repopulate();
    // set initial node positions
    double y = 0;
    double x = OpComponent::ROW_W / (double)getCanvasWidth() * 0.5;
    for(int i=0; i<graph.getNumOps(); i++){
        OpGraph::Node* node = graph.getNode(i);
        int hpx = OpComponent::getPixelHeight(node->op_) + 10;
        double h = hpx / (double)getCanvasHeight();
        if(i==0) {
            y = h * 0.5;
        } else {
            y += h;
        }
        while(y > 1.0) {
            y -= 1.0;
            x += OpComponent::ROW_W / (double)getCanvasWidth() * 1.2;
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
            m.addItem(i + 1, op_registry[i].name);
        }
        
        const int r = m.show();
        if(r > 0) {
            // add selected operator
            graph.addOpNode((op_id_t)(r - 1),
                            e.x / (double)getCanvasWidth(),
                            e.y / (double)getCanvasHeight());
        }
    }
}



OpComponent* GraphEditorComponent::getComponentForOp (const int op_idx) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (OpComponent* const c = dynamic_cast<OpComponent*> (getChildComponent (i)))
            if (c->op_idx_ == op_idx)
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
        } else {
            
        }
    }
}