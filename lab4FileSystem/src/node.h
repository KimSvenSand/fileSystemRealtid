#ifndef NODE_H
#define NODE_H

#include <vector>
#include <iostream>

class Node {
  private:
    int blockNr;
	int size;
	std::string name;
  public:
    Node();
    Node(std::string name, int size, int blockNr);
    ~Node();
	
	int getSize();
	std::string getName();
	int getBlockNr();
	void setBlockNr(int blockNr);
	void setSize(int size);
};

#endif //NODE_H
