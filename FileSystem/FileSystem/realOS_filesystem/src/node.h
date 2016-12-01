#ifndef NODE_H
#define NODE_H

#include <vector>
#include <iostream>

class Node {
  private:
    int blockNr;
	std::string name;
  public:
    Node();
    Node(std::string name, int blockNr);
    ~Node();
	
	std::string getName();
	int getBlockNr();
};

#endif //NODE_H
