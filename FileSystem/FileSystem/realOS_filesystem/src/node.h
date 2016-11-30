#ifndef NODE_H
#define NODE_H

#include <vector>
#include <iostream>

class Node {
  private:
    int size = 0;
    int * blockNrs;
  public:
    Node();
    Node(int size, int blockNrs[]);
    ~Node();

    int getSize();
    int * getBlockNrs();
};

#endif //NODE_H
