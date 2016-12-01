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

int FileSystem::readFile(string path) {
	bool pathFound = true;
	string fileContent = "";
	Folder* pathFolder = this->currentFolder;
	Node* fileNode = nullptr;
	vector<string> parsedPath = this->parsePath(path);

	if (parsedPath.size() - 1  > 0) {
		for (int i = 0; i < parsedPath.size() - 1; i++) {
			if (parsedPath[i] == "..") {
				pathFolder = pathFolder->getParent();
			}
			else if (parsedPath[i] == ".") {

			}
			else {
				pathFolder = pathFolder->getFolder(parsedPath[i]);

				if (pathFolder == nullptr) {
					cout << "Error: invalid path" << endl;
					return -1;
				}
			}
		}
	}

	fileNode = pathFolder->getNode(parsedPath[parsedPath.size() - 1]);
	if (fileNode == nullptr) {
		cout << "Error: invalid path" << endl;
		return -1;
	}
	else {
		fileContent = mMemblockDevice.readBlock(fileNode->getBlockNr()).toString();
		cout << this->getFileFromBlock(fileContent) << endl;
	}
}

int FileSystem::createFile(string name, string path) {
	bool pathFound = true;
	int counter = 0;
	string fileContent = this->getFileContent();
	Folder * pathFolder = this->currentFolder;
	vector<string> parsedPath = this->parsePath(path);

	if (parsedPath.size() > 0) {
		for (int i = 0; i < parsedPath.size(); i++) {
			if (parsedPath[i] == "..") {
				pathFolder = pathFolder->getParent();
			}
			else if (parsedPath[i] == ".") {

			}
			else {
				for (int k = 0; k < pathFolder->getnrOfFolders(); k++) {
					pathFound = false;

					if (pathFolder->getFolders()[k]->getName() == parsedPath[i]) {
						pathFolder = pathFolder->getFolders()[k];
						pathFound = true;
					}
				}

				if (!pathFound) {
					cout << "Error: invalid path" << endl;
					return -1;
				}
			}
		}
	}
	
	while (fileContent.size() != 512) {
		fileContent += " ";
	}

	while (this->blockMap[counter] == 1) {
		counter++;
	}

	this->mMemblockDevice.writeBlock(counter, fileContent);
	this->blockMap[counter] = 1;
	pathFolder->createNode(name, counter);
}

string FileSystem::getFileContent() {
	string content = "";
	string row = "";

	cout << "Input :q to exit input content\nInput file content: " << endl;

	getline(cin, row);

	content += row;

	while (row != ":q") {
		row = "";
		getline(cin, row);

		content += "\n" + row;
	}

	return content;
}

//mkdir <folderName>
int FileSystem::createFolder(string name, string path) {
	bool pathFound = true;
	Folder* pathFolder = this->currentFolder;
	vector<string> parsedPath = this->parsePath(path);

	if (parsedPath.size() > 0) {
		for (int i = 0; i < parsedPath.size(); i++) {
			if (parsedPath[i] == "..") {
				pathFolder = pathFolder->getParent();
			} else if (parsedPath[i] == ".") {
				
			} else {
				for (int k = 0; k < pathFolder->getnrOfFolders(); k++) {
					pathFound = false;

					if (pathFolder->getFolders()[k]->getName() == parsedPath[i]) {
						pathFolder = pathFolder->getFolders()[k];
						pathFound = true;
					}
				}
				if (!pathFound) {
					cout << "Error: Invalid path" << endl;
					return -1;
				}
			}
		}
	}

	pathFolder->createFolder(name);
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
	for (int i = 0; i < nodes.size(); i++) {
		cout << nodes[i]->getName() << endl;
	}
	//Get fileNames.
}

//cd <path>
string FileSystem::goToFolder(string path) {
	bool foundFolder = false;
	Folder* start = this->currentFolder;

	//Parse all path parts to vector
	vector<string> parsedPath = this->parsePath(path);

	if (path.size() <= 0) {
		this->currentFolder = this->homeFolder;
	}
	//Check all letters
	for (int i = 0; i < parsedPath.size(); i++) {
		foundFolder = false;

		if (parsedPath[i] == "..") {
			this->currentFolder = this->currentFolder->getParent();
		}
		//If "." -> current directory
		else if (parsedPath[i] == ".") {
			//Already in current directory
		}
		else {
			//Search through folders and match with tmp
			for (int k = 0; k < this->currentFolder->getnrOfFolders(); k++) {
				if (this->currentFolder->getFolders()[k]->getName() == parsedPath[i]) {
					this->currentFolder = this->currentFolder->getFolders()[k];
					foundFolder = true;
				}
			}

			//If no matching folders are found print error and return current folder
			if (!foundFolder) {
				this->currentFolder = start;
				cout << "Error: Could not find folder" << endl;
				return this->currentFolder->getName();
			}
		}
	}

	return this->currentFolder->getName();
}

vector<string> FileSystem::parsePath(string path) {
	string tmp = "";
	vector<string> parsedPath;

	if (path != "") {
		if (path[path.size() - 1] != '/') {
			path += "/";
		}

		for (int i = 0; i < path.size(); i++) {
			//Save path in tmp variable
			if (path[i] != '/') {
				tmp += path[i];
			}
			else {
				parsedPath.push_back(tmp);
				tmp = "";
			}
		}
	}

	return parsedPath;
}

string FileSystem::getFileFromBlock(string stringFromBlock) {
	string mainContent = "";
	string tmp = "";

	for (size_t i = 0; i < stringFromBlock.size(); i++) {
		tmp = "";
		if (stringFromBlock[i] == ':') {
			tmp += stringFromBlock[i + 1];
			tmp += stringFromBlock[i + 2];
			if (tmp == "q ") {
				return mainContent;
			}
		}

		mainContent += stringFromBlock[i];
	}

	return mainContent;
}
