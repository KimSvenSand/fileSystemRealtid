#include "node.h"
#include <iostream>

Node::Node() {
  this->blockNr = 0;  
  this->size = 0;
}

Node::Node(std::string name, int size, int blockNr) {
  this->blockNr = blockNr;
  this->name = name;
  this->size = size;
}

Node::~Node() {
}

int Node::getBlockNr() {
  return this->blockNr;
}

int Node::getSize() {
	return this->size;
}

std::string Node::getName() {
	return this->name;
}