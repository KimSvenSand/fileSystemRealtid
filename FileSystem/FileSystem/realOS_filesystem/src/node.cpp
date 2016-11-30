#include "node.h"
#include <iostream>

Node::Node() {
  this->size = 0;
  this->blockNrs = NULL;  
}

Node::Node(int size, int blockNrs[]) {
  this->size = size;
  this->blockNrs = blockNrs;
}

Node::~Node() {
  this->size = 0;
}

int Node::getSize() {
  return this->size;
}

int * Node::getBlockNrs() {
  return this->blockNrs;
}
