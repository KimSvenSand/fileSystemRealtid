#include "node.h"
#include <iostream>

Node::Node() {
  this->blockNr = 0;  
}

Node::Node(std::string name, int blockNr) {
  this->blockNr = blockNr;
  this->name = name;
}

Node::~Node() {
}

int Node::getBlockNr() {
  return this->blockNr;
}

std::string Node::getName() {
	return this->name;
}