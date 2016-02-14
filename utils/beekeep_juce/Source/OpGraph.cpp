/*
  ==============================================================================

    OpGraph.cpp
    Created: 10 Feb 2016 2:52:35am
    Author:  ezra

  ==============================================================================
*/

#include "OpGraph.h"

#include "net.h"
#include "net_protected.h"

void OpGraph::addOpNode (op_id_t op_id, double x, double y) {
    s16 idx = net_add_op(op_id);
    Node* const node = new Node();
    node->x_ = x;
    node->y_ = y;
    node->op_ = net->ops[idx];
    node->idx_ = idx;
    nodes_.add(node);
    sendChangeMessage();
}



int OpGraph::getNumOps(void) {
    return nodes_.size();
}

void OpGraph::setNodePosition (uint32 nodeId, Point<double> pos) {
    nodes_[nodeId]->x_ = pos.x;
    nodes_[nodeId]->y_ = pos.y;
}


void OpGraph::setNodePosition (uint32 nodeId, double x, double y) {
    nodes_[nodeId]->x_ = x;
    nodes_[nodeId]->y_ = y;
}

Point<double> OpGraph::getNodePosition (uint32 nodeId) const {
    return Point<double>(nodes_[nodeId]->x_, nodes_[nodeId]->y_);
}

void OpGraph::repopulate() {
    for(int i=0; i<net->numOps; i++) {
        Node* const node = new Node();
        node->op_ = net->ops[i];
        node->idx_ = i;
        nodes_.add(node);
    }
}