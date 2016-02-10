/*
  ==============================================================================

    OpComponent.cpp
    Created: 10 Feb 2016 3:19:25am
    Author:  ezra

  ==============================================================================
*/

#include "OpComponent.h"
#include "OpGraph.h"

OpComponent::OpComponent (OpGraph* graph, op_t* op, u16 idx) :
    graph_ (graph),
    op_(op),
    op_idx_(idx),
    num_ins_(op->numInputs),
    num_outs_(op->numOutputs),
    pin_size_ (16),
    font_ (13.0f, Font::bold)
{
//        shadow.setShadowProperties (DropShadow (Colours::black.withAlpha (0.5f), 3, Point<int> (0, 1)));
//        setComponentEffect (&shadow);
    
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
    if (! e.mods.isPopupMenu())
    {
        Point<int> pos = pos0_ + Point<int> (e.getDistanceFromDragStartX(), e.getDistanceFromDragStartY());
        
        if (getParentComponent() == nullptr) { return; }
        pos = getParentComponent()->getLocalPoint (nullptr, pos);

        graph_->setNodePosition (op_idx_,
                                 (pos.getX() + getWidth() / 2) / (double) getCanvasWidth(),
                                 (pos.getY() + getHeight() / 2) / (double) getCanvasHeight() );

        
        getGraphEditor()->updateComponents();
    }
}

void OpComponent::mouseUp (const MouseEvent& e)
{
}

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
//    const int x = 4;
//    const int y = pin_size_;
//    const int w = getWidth() - x * 2;
//    const int h = getHeight() - pin_size_ * 2;
//    
//    g.fillRect (x, y, w, h);

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
    
    setSize(ROW_W, OpComponent::getPixelHeight(node->op_));
    
    Point<double> p = graph_->getNodePosition (op_idx_);
    //setBounds(p.x, p.y, w, h);
    printf("\r\n drawing op %d at (%f, %f) in parent", op_idx_, p.x, p.y);
    setCentreRelative(p.x, p.y);
}


int OpComponent::getPixelHeight(op_t* op) {
    // number of rows
    int num = (op->numInputs > op->numOutputs) ? op->numInputs : op->numOutputs;
    return (num+1) * ROW_H;
}