/*
  ==============================================================================

    OpComponent.cpp
    Created: 10 Feb 2016 3:19:25am
    Author:  ezra

  ==============================================================================
*/

#include "OpComponent.h"
#include "OpGraph.h"

#include "GfxUtil.h"

const double OpComponent::ROW_W = 0.05;
const double OpComponent::ROW_H = 0.005;


OpComponent::OpComponent (OpGraph* graph, op_t* op, u16 idx) :
    graph_ (graph),
    op_(op),
    op_idx_(idx),
    num_ins_(op->numInputs),
    num_outs_(op->numOutputs),
    pin_size_ (16),
    font_ (13.0f, Font::bold)
{
    setSize (80, 20);
    setName(op->opString);
}

OpComponent::~OpComponent()
{
    deleteAllChildren();
}

void OpComponent::mouseDown (const MouseEvent& e)
{
    pos0_ = localPointToGlobal (Point<int>());
    
    toFront (true);
    
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;
        m.addItem (1, "delete");
        m.addItem (2, "disconnect all inputs");
        m.addItem (3, "disconnect all outputs");
        
        const int r = m.show();
        
        if (r == 1)
        {
            // TODO
            // graph_->removeOp (op_idx_);
            return;
        }
        else if (r == 2)
        {
            // TODO
        }
        else
        {
            // TODO
        }
    }
}



void OpComponent::mouseDrag (const MouseEvent& e)
{
    
    if (getParentComponent() == nullptr) { return; }
    
    if (! e.mods.isPopupMenu())
    {
        
        printf("\r\n old pos: %d, %d", pos0_.x, pos0_.y);
        
        Point<int> delta(e.getDistanceFromDragStartX(),
                    e.getDistanceFromDragStartY());
        
        printf("\r\n drag delta: %d, %d", delta.x, delta.y);
        Point<int> pos = pos0_ + delta;
        
        
        printf("\r\n new pos: %d, %d", pos.x, pos.y);
        
        pos = getParentComponent()->getLocalPoint (nullptr, pos);

        printf("\r\n new pos relative to parent: %d, %d", pos.x, pos.y);

        double screen_x = GfxUtil::pixToScreen(pos.x);
        double screen_y = GfxUtil::pixToScreen(pos.y);
        
        graph_->setNodePosition (op_idx_, screen_x, screen_y);
        printf("\r\n drag to screen pos: %f, %f \r\n", screen_x, screen_y);
        
        getGraphEditor()->updateComponents();
    }
}

void OpComponent::mouseUp (const MouseEvent& e)
{
}

// TODO: might need to override to include pins
//bool OpComponent::hitTest (int x, int y)
//{
//    for (int i = getNumChildComponents(); --i >= 0;)
//        if (getChildComponent(i)->getBounds().contains (x, y))
//            return true;
//
//    return x >= 3 && x < getWidth() - 6 && y >= pin_size_ && y < getHeight() - pin_size_;
//}

void OpComponent::paint (Graphics& g)
{

    // TODO: pins and whatnot
    Rectangle<int> rect(0, 0, getWidth(), getHeight());
    
    g.setColour (Colours::lightgrey);
    g.fillRect(rect);
    g.setColour (Colours::black);
    g.setFont (font_);
    g.drawFittedText (getName(), getLocalBounds().reduced (4, 2), Justification::centred, 2);
    
    g.setColour (Colours::grey);
    g.drawRect(rect);
}

void OpComponent::update(void) {
    const OpGraph::Node* node = graph_->getNode(op_idx_);
    if(node == nullptr) {
        delete this;
        return;
    }
    num_ins_ = node->op_->numInputs;
    num_outs_ = node->op_->numOutputs;
    
    setSize(GfxUtil::screenToPix(ROW_W),
            GfxUtil::screenToPix(OpComponent::getScreenHeight(node->op_)));
    
    Point<double> p = graph_->getNodePosition (op_idx_);
    
    p.x = GfxUtil::screenToCanvas(p.x);
    p.y = GfxUtil::screenToCanvas(p.y);

    
    // printf("\r\n drawing op %d at (%f, %f) in parent", op_idx_, p.x, p.y);
    setCentreRelative(p.x, p.y);
}

double OpComponent::getScreenHeight(op_t* op) {
    int num = (op->numInputs > op->numOutputs) ? op->numInputs : op->numOutputs;
    return (num+1) * ROW_H;
}