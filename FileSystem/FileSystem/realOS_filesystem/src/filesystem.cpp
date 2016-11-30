#include "filesystem.h"

FileSystem::FileSystem() {
	//Initiate file system with one home folder that is it's own parent.
	this->mFolder.setName("home");
	this->mFolder.setParent(&mFolder);
	this->currentFolder = &this->mFolder;
}

FileSystem::~FileSystem() {

}

void FileSystem::createFolder(string name) {
	currentFolder->createFolder(name);
}

void FileSystem::listDir() {
	vector<Folder*> folders = currentFolder->getFolders();
	vector<Node*> nodes = currentFolder->getNodes();

	cout << "Folders: " << endl;
	for (size_t i = 0; i < folders.size(); i++) {
		cout << folders[i]->getName() << endl;
	}

	cout << "Files: " << endl;
	//Get fileNames.
}