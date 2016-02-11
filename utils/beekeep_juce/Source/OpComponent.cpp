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

const double OpComponent::ROW_W = 0.06;
const double OpComponent::ROW_H = 0.008;


//----------------------------------------
class OpNameComponent : public Component  {
private:
    op_t* op_;
    int idx_;
public:
    OpNameComponent(op_t* op, int idx) {
        op_ = op;
        idx_ = idx;
    }
    
    const int getIdx() { return idx_; }
    const String getLabel() {
        return String(idx_) + "_" + String(op_->opString);
    }

    virtual void mouseDown (const MouseEvent& e) override
    {
        toFront (true);
        
        if (e.mods.isPopupMenu())
        {
            PopupMenu m;
            m.addItem (1, "delete");
            m.addItem (2, "duplicate");
            m.addItem (3, "disconnect all inputs");
            m.addItem (4, "disconnect all outputs");
            
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
                // ...TODO..
            }
        } else {
            getParentComponent()->mouseDown(e);
        }
    }

    virtual void mouseDrag (const MouseEvent& e) override
    {
        getParentComponent()->mouseDrag(e);
    }
    
    void paint (Graphics& g) {
        const Rectangle<int>r(0, 0, getWidth(), getHeight());
        g.setColour(Colours::lightblue);
        g.fillRect(r);
        g.setColour(Colours::white);
        g.drawRect(r);
        g.setColour(Colours::black);
        g.drawFittedText(getLabel(), r, Justification::left, 1);
    }
};


//----------------------------------------
class OpInputComponent : public Component {
private:
    op_t* op_;
    int idx_;
public:
    OpInputComponent(op_t* op, int idx) {
        op_ = op;
        idx_ = idx;
    }
    
    const int getIdx() { return idx_; }
    const String getLabel() {
        return String(idx_) + "_" + String(op_->inString + (idx_*8));
    }
    
    void paint (Graphics& g) {
        const Rectangle<int>r(0, 0, getWidth(), getHeight());
        g.setColour(Colours::oldlace);
        g.fillRect(r);
        g.setColour(Colours::lightgrey);
        g.drawRect(r);
        g.setColour(Colours::black);
        g.drawFittedText(getLabel(), r, Justification::left, 1);
    }
    
    
    // TODO: interaction
    virtual void mouseDown (const MouseEvent& e) override
    {
        getParentComponent()->mouseDown(e);
    }
    virtual void mouseDrag (const MouseEvent& e) override
    {
        getParentComponent()->mouseDrag(e);
    }
};

//----------------------------------------
class OpOutputComponent : public Component  {
private:
    op_t* op_;
    int idx_;
public:
    OpOutputComponent(op_t* op, int idx) {
        op_ = op;
        idx_ = idx;
    }
    
    const int getIdx() { return idx_; }
    const String getLabel() {
        return String(idx_) + "_" + String(op_->outString + (idx_*8));
    }
    
    void paint (Graphics& g) {
        const Rectangle<int>r(0, 0, getWidth(), getHeight());
        g.setColour(Colours::oldlace);
        g.fillRect(r);
        g.setColour(Colours::lightgrey);
        g.drawRect(r);
        g.setColour(Colours::black);
        g.drawFittedText(getLabel(), r, Justification::left, 1);
    }
    
    // TODO: interaction
    virtual void mouseDown (const MouseEvent& e) override
    {
        getParentComponent()->mouseDown(e);
    }
    virtual void mouseDrag (const MouseEvent& e) override
    {
        getParentComponent()->mouseDrag(e);
    }
};


//--------------------------------------------------------
OpComponent::OpComponent (OpGraph* graph, op_t* op, u16 idx) :
    graph_ (graph),
    op_(op),
    op_idx_(idx),
    num_ins_(op->numInputs),
    num_outs_(op->numOutputs),
    pin_size_ (16),
    font_ (GfxUtil::screenToPix(ROW_H * 0.9), Font::plain)
{
    const int w = GfxUtil::screenToPix(OpComponent::getScreenWidth());
    const int h = GfxUtil::screenToPix(OpComponent::getScreenHeight(op_));
    setSize (w, h);
    setName(String(idx) + "_" + String(op->opString));
    
    addNameComponent();
    
    for(int i=0; i<op_->numInputs; i++) {
        addInputComponent(i);
    }
    
    for(int i=0; i<op_->numOutputs; i++) {
        addOutputComponent(i);
    }
    
    // addMouseListener(this, false);
    
}

OpComponent::~OpComponent()
{
    deleteAllChildren();
}

void OpComponent::mouseDown (const MouseEvent& e)
{
    pos0_ = localPointToGlobal (Point<int>());
}



void OpComponent::mouseDrag (const MouseEvent& e)
{
    
    if (getParentComponent() == nullptr) { return; }
    
    if (! e.mods.isPopupMenu())
    {
        Point<int> delta(e.getDistanceFromDragStartX(),
                    e.getDistanceFromDragStartY());
        
        Point<int> pos = pos0_ + delta;
        
        pos = getParentComponent()->getLocalPoint (nullptr, pos);
        pos = pos + Point<int>(getWidth() / 2, getHeight() / 2);

        double screen_x = GfxUtil::pixToScreen(pos.x);
        double screen_y = GfxUtil::pixToScreen(pos.y);
        
        graph_->setNodePosition (op_idx_, screen_x, screen_y);
        
        getGraphEditor()->updateComponents();
    }
}

void OpComponent::mouseUp (const MouseEvent& e)
{
}
 
void OpComponent::paint (Graphics& g)
{
    g.fillAll(Colours::whitesmoke);
#if 0
//    const int h = GfxUtil::screenToPix(ROW_H);
//    const int w = getWidth();
//    Rectangle<int> r(0, 0, w, h);
//    String s;
//    g.setFont (font_);
//    
//#define RECT(color) \
//    g.setColour((color)); \
//    g.fillRect(r); \
//    g.setColour(Colours::black); \
//    g.drawRect(r)
//    
//    RECT(Colours::lightblue);
//    // op name
//    s = String(op_idx_) + "." + getName();
//    g.drawFittedText(s, r, Justification::centred, 1);
//    r += (Point<int>(0, h));
//    // inputs
//    r.setWidth(w / 2);
//    for(int i=0; i<op_->numInputs; i++) {
//        RECT(Colours::lightgrey);
//        s = String(i) + "." + String(op_->inString + i*8);
//        g.drawFittedText(s, r, Justification::left, 1);
//        r += (Point<int>(0, h));
//    }
//    // outputs
//    r.setY(h);
//    r.setX(w/2);
//    for(int i=0; i<op_->numOutputs; i++) {
//        RECT(Colours::lightgrey);
//        s = String(i) + "." + String(op_->outString + i*8);
//        g.drawFittedText(s, r, Justification::right, 1);
//        r += (Point<int>(0, h));
//    }
//#undef RECT
#endif
}

void OpComponent::update(void) {
    const OpGraph::Node* node = graph_->getNode(op_idx_);
    if(node == nullptr) {
        delete this;
        return;
    }
    num_ins_ = node->op_->numInputs;
    num_outs_ = node->op_->numOutputs;
    
    Point<double> p = graph_->getNodePosition (op_idx_);
    
    p.x = GfxUtil::screenToCanvas(p.x);
    p.y = GfxUtil::screenToCanvas(p.y);
    
    setCentreRelative(p.x, p.y);
}

double OpComponent::getScreenHeight(op_t* op) {
    int num = (op->numInputs > op->numOutputs) ? op->numInputs : op->numOutputs;
    return (num+1) * ROW_H;
}

void OpComponent::addInputComponent(int idx) {
    OpInputComponent* c = new OpInputComponent(op_, idx);
    addAndMakeVisible(c);
    const int h =  GfxUtil::screenToPix(ROW_H);
    const int w =  GfxUtil::screenToPix(ROW_W)/2;
    c->setBounds(0, h * (idx+1), w, h);
}

void OpComponent::addOutputComponent(int idx) {
    OpOutputComponent* c = new OpOutputComponent(op_, idx);
    addAndMakeVisible(c);
    const int h =  GfxUtil::screenToPix(ROW_H);
    const int w =  GfxUtil::screenToPix(ROW_W)/2;
    c->setBounds(w, h * (idx+1), w, h);
}

void OpComponent::addNameComponent(void) {
    OpNameComponent* c = new OpNameComponent(op_, op_idx_);
    addAndMakeVisible(c);
    const int h =  GfxUtil::screenToPix(ROW_H);
    const int w =  GfxUtil::screenToPix(ROW_W);
    c->setBounds(0, 0, w, h);
}
