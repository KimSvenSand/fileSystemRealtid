#include "Folder.h"

Folder::Folder() {
	this->name = "";
	this->parent = nullptr;
}

Folder::Folder(string name, Folder * parent){
	this->parent = parent;
	this->nrOfFolders = 0;
	this->name = name;
}

Folder::~Folder(){
	//No need to destroy vector objects
}

void Folder::createFolder(string name){
	this->folders.push_back(new Folder(name, this));
	this->nrOfFolders++;
}

void Folder::createNode(int sizeInBlocks, int blockNrs[]) {
	this->nodes.push_back(new Node(sizeInBlocks, blockNrs));
}

string Folder::getName(){
	return this->name;
}

void Folder::setParent(Folder * parent) {
	this->parent = parent;
}

void Folder::setName(string name){
	this->name = name;
}
int Folder::getnrOfFolders(){
	return this->nrOfFolders;
}
int Folder::getnrOfNodes(){
	return this->nrOfNodes;
}

Folder* Folder::getParent() {
	return this->parent;
}

vector<Folder*> Folder::getFolders() {
	return folders;
}

vector<Node*> Folder::getNodes() {
	return nodes;
}