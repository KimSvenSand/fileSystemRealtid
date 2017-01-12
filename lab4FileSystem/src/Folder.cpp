#include "Folder.h"

Folder::Folder() {
	this->name = "";
	this->parent = nullptr;
	this->nrOfFolders = 0;
	this->nrOfNodes = 0;
}

Folder::Folder(string name, Folder * parent){
	this->parent = parent;
	this->nrOfFolders = 0;
	this->nrOfNodes = 0;
	this->name = name;
}

Folder::~Folder(){
	for (int i = 0; i < this->getnrOfFolders(); i++) {
		delete this->folders[i];
	}

	for (int i = 0; i < this->getnrOfNodes(); i++) {
		delete this->nodes[i];
	}
}

void Folder::createFolder(string name){
	this->folders.push_back(new Folder(name, this));
	this->nrOfFolders++;
}

void Folder::createNode(string name, int size, int blockNr) {
	this->nodes.push_back(new Node(name, size, blockNr));
	this->nrOfNodes++;
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

//Get a folder by name. Returns nullptr if fails.
Folder* Folder::getFolder(string name) {
	for (size_t i = 0; i < folders.size(); i++) {
		if (folders[i]->getName() == name)
		{
			return folders[i];
		}
	}

	return nullptr;
}

Node* Folder::getNode(string name) {
	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->getName() == name) {
			return nodes[i];
		}
	}

	return nullptr;
}

//Deletes node from nodes. Returns -1 if it fails.
int Folder::deleteNode(Node* node) {
	for (size_t i = 0; i < this->nodes.size(); i++) {
		if (this->nodes[i] == node) {
			this->nodes.erase(this->nodes.begin() + i);
			return 0;
		}
	}

	return -1;
};