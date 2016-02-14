/*
  ==============================================================================

    ConnectorComponent.h
    Created: 11 Feb 2016 3:11:32am
    Author:  ezra

  ==============================================================================
*/

#ifndef CONNECTORCOMPONENT_H_INCLUDED
#define CONNECTORCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class ConnectorComponent    : public Component
{
public:
    ConnectorComponent();
    ~ConnectorComponent();

    void paint (Graphics&);
    void resized();

    // positive slope
    bool slope_;
    
private:
    // index of source operator
    int src_op_;
    // index of source output in operator
    int src_out_;
    // idnex of destination operator
    int dst_op_;
    // index of destination input in operator
    int dst_in_;
    bool dragging_;
public:
    int getSrcOp() { return src_op_; }
    int getSrcOut() { return src_out_; }
    int getDstOp() { return dst_op_; }
    int getDstIn() { return dst_in_; }
    
    bool isDragging() { return dragging_; }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorComponent)
};


#endif  // CONNECTORCOMPONENT_H_INCLUDED
