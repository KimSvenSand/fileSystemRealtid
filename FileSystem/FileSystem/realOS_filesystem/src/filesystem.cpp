#include "filesystem.h"

FileSystem::FileSystem() {
	//Initiate file system with one home folder that is it's own parent.
	this->mFolder.setName("/");
	this->mFolder.setParent(&mFolder);
	this->currentFolder = &this->mFolder;

	//Pointer to the "home" folder.
	this->homeFolder = &this->mFolder;
}

FileSystem::~FileSystem() {

}

void FileSystem::createFolder(string name) {
	currentFolder->createFolder(name);
}

//ls
void FileSystem::listDir() {
	vector<Folder*> folders = currentFolder->getFolders();
	vector<Node*> nodes = currentFolder->getNodes();

	cout << "Folders: " << endl;
	cout << ".." << endl << "." << endl;
	for (size_t i = 0; i < folders.size(); i++) {
		cout << folders[i]->getName() << endl;
	}

	cout << "Files: " << endl;
	//Get fileNames.
}

//cd
string FileSystem::goToFolder(string path) {
	bool foundFolder = false;
	Folder* start = this->currentFolder;
	string tmp = "";

	if (path[path.size() - 1] != '/') {
		path += '/';
	}
	//Check all letters
	for (int i = 0; i < path.size(); i++) {
		//Save path in tmp variable
		if(path[i] != '/') {
			tmp += path[i];
		
		//If at '/'
		} else {
			foundFolder = false;
			//If ".." go to parent
			if (tmp == "..") {
				this->currentFolder = this->currentFolder->getParent();
			}
			//If "." -> current directory
			else if (tmp == ".") {
				//Already in current directory
			}
			else {
				//Search through folders and match with tmp
				for (int k = 0; k < this->currentFolder->getnrOfFolders(); k++) {
					if (this->currentFolder->getFolders()[k]->getName() == tmp) {
						this->currentFolder = this->currentFolder->getFolders()[k];
						foundFolder = true;
					}
				}

				//If no matching folders are found print error and return current folder
				if (!foundFolder) {
					this->currentFolder->getName();
					cout << "Error: Could not find folder" << endl;
					return this->currentFolder->getName();
				}
			}
		}
	}

	return this->currentFolder->getName();
}

