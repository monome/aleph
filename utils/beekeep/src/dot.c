#include <stdio.h>

#include "net_protected.h"
#include "dot.h"

// write graphviz format to open filepointer
void net_write_dot(void* fp) {
  int i, j;
  int nId = 0; // node index
  int eId = 0; // edge index

  fprintf(fp,
	  "digraph g { 				\r\n"
	  "	graph[rankdir=\"LR\"]; 		\r\n"
	  "	node[ 			\r\n"
	  "	fontsize=\"10\" 	\r\n"
	  "	shape= \"ellipse\" ];	\r\n"
	  "	edge[]; 		\r\n" 
	  );

  for(i=0; i<net->numOps; ++i) {
    op_t* op = net->ops[i];
    // add node and name label
    fprintf(fp, "\r\n"
	    " \"node%d\" [ \r\n"
	    " label = \"<f0> %d.%s ",
	    i, i, net_op_name(i));

    if(op->numInputs > 0) { 
      fprintf(fp, " | ");
    }


    // add inputs labels
    for(j=0; j< (op->numInputs); ++j) {
      fprintf(fp, 
	      "<f%d> -- %d.%s ",
	      j+1, j, net_in_name(net_op_in_idx(i, j)) );
      if(j < ((op->numInputs) - 1)) { 
	fprintf(fp, " | ");
      }

    }
    if(op->numOutputs > 0) { 
      fprintf(fp, " | ");
    }

    // add output labels
    for(j=0; j< (op->numOutputs); ++j) {
      fprintf(fp, 
	      "<f%d> %d.%s -- ",
	      j+1 + op->numInputs, j, net_out_name(net_op_out_idx(i, j)));
      if(j < ((op->numOutputs) - 1)) { 
	fprintf(fp, " | ");
      }

    }

    // close the node
    fprintf(fp, 
	    "\" \r\n"
	    "shape = \"record\" \r\n"
	    "]; \r\n" );
    nId++;
  } // end op loop

  fprintf(fp, "\r\n");

  // make a node for each DSP param?
  // or one giant node with tons of records??
  /// or a subgraph?
  for(i=0; i<net_num_params(); i++ ) {
    
    //    nId++;
  }

  fprintf(fp, "\r\n");
  fprintf(fp, "\r\n");
  fprintf(fp, "\r\n");


    /// loop over outputs and draw connections...
  for(i=0; i<net->numOuts; i++) {
    if(net_get_target(i) > -1) {
      //      if(
    }
      eId++;
  }


  
    /// presets... ??


    // close the graph
  fprintf(fp, "\r\n}");
  }
