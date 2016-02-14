/*
  ==============================================================================

    OpGraph.h
    Created: 10 Feb 2016 2:52:35am
    Author:  ezra

  ==============================================================================
*/

#ifndef OPGRAPH_H_INCLUDED
#define OPGRAPH_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "op.h"

class OpGraph : public ChangeBroadcaster {

public:
    
    class Node : public ReferenceCountedObject {
    public:
        op_id_t id_;
        int idx_;
        op_t* op_;
        // position in parent, relative to upper left
        // expressed in screen coordinates
        double x_;
        double y_;
    };
    
    void addOpNode (op_id_t op_id, double x, double y);
    int getNumOps(void);
    
    void setNodePosition (uint32 nodeId, double x, double y);
    void setNodePosition (uint32 nodeId, Point<double> pos);
    Point<double> getNodePosition (uint32 nodeId) const;
    
    void clear();
    void repopulate();
    
    OpGraph::Node* getNode(int idx) {
        return nodes_[idx];
    };
    
    class Edge : public ReferenceCountedObject {
        // TODO
    };

private:
    ReferenceCountedArray<OpGraph::Node> nodes_;
    
};

#endif  // OPGRAPH_H_INCLUDED
