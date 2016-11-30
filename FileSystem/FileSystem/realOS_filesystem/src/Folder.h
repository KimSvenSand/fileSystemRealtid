#ifndef FOLDER_H
#define FOLDER_H
#include <string>
#include <iostream>
#include <vector>
#include "node.h"

using namespace std;

class Folder {
public:
	Folder();
	Folder(string name, Folder * parent);
	~Folder();
	void createFolder(string name);
	void createNode(int sizeInBlocks, int blockNrs[]);
	string getName();
	void setName(string newName);
	void setParent(Folder * parent);
	int getnrOfChildren();
	int getnrOfNodes();

	vector<Folder*> getFolders();
	vector<Node*> getNodes();
private:
	Folder *parent;
	vector<Folder *> folders;
	vector<Node *> nodes;
	int nrOfNodes;
	int nrOfChildren;
	string name;
};

#endif
