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

//Get folder pointer from path. Returns nullptr if invalid path, returns current folder if path is empty.
Folder* FileSystem::getFolderFromPath(vector<string> path) {
	Folder* pathFolder = this->currentFolder;
	
	if (path.size() > 0) {
		for (int i = 0; i < path.size(); i++) {
			if (path[i] == "..") {
				pathFolder = pathFolder->getParent();
			}
			else if (path[i] == ".") {

			}
			else if (path[i] == "") {
				pathFolder = this->homeFolder;
			}
			else {
				pathFolder = pathFolder->getFolder(path[i]);

				if (pathFolder == nullptr) {
					return nullptr;
				}
			}
		}

		return pathFolder;
	}

	return this->currentFolder;
}

Node* FileSystem::getNodeFromPath(vector<string> path) {
	vector<string> folderPath = path;
	Folder* folder = this->currentFolder;
	Node* node = nullptr;

	folderPath.pop_back();
	folder = this->getFolderFromPath(folderPath);

	if (folder != nullptr) {
		node = folder->getNode(path[path.size() - 1]);
		if (node == nullptr) {
			return nullptr;
		}
		//If node is found get file content and print.
		else {
			return node;
		}
	}
	else {
		return nullptr;
	}
}

string FileSystem::readFileContent(string path) {
	bool pathFound = true;
	string fileContent = "";
	Folder* pathFolder = this->currentFolder;
	Node* fileNode = nullptr;
	vector<string> parsedPath = this->parsePath(path);

	//Go to correct folder
	if (parsedPath.size() - 1  > 0) {
		for (int i = 0; i < parsedPath.size() - 1; i++) {
			if (parsedPath[i] == "..") {
				pathFolder = pathFolder->getParent();
			}
			else if (parsedPath[i] == ".") {

			}
			else if (parsedPath[i] == "") {
				pathFolder = this->homeFolder;
			}
			else {
				pathFolder = pathFolder->getFolder(parsedPath[i]);

				if (pathFolder == nullptr) {
					return "ERROR_1";
				}
			}
		}
	}

	//Get node
	fileNode = pathFolder->getNode(parsedPath[parsedPath.size() - 1]);
	if (fileNode == nullptr) {
		return "ERROR_1";
	}
	//If node is found get file content and print.
	else {
		for (size_t i = 0; i < fileNode->getSize(); i++) {
			fileContent += mMemblockDevice.readBlock(fileNode->getBlockNr() + i).toString();
		}
		return this->getFileFromBlock(fileContent);
	}
}

//Write file to memBlocks. Returns -1 if failed to write and returns -2 if no space was found.
int FileSystem::writeFile(string fileContent, int size) {
	vector<string> blockContent;
	bool foundSpace = false;
	int counter = 0;

	for (int i = 0; i < size; i++) {
		if (i < size - 1) {
			blockContent.push_back(fileContent.substr(512 * i, 512));
		}
		else {
			blockContent.push_back(fileContent.substr(512 * i));

			while (blockContent[i].size() < 512) {
				blockContent[i] += " ";
			}
		}
	}

	while (!foundSpace && counter < 511) {
		while (this->blockMap[counter] == 1 && counter < 511) {
			counter++;
		}

		foundSpace = this->hasSpace(counter, size, blockMap);
	}

	if (foundSpace) {
		for (size_t i = 0; i < size; i++) {
			bool wrote = this->mMemblockDevice.writeBlock(counter + i, blockContent[i]);

			if (wrote) {
				this->blockMap[counter + i] = 1;
			}
			else {
				return -1;
			}
		}
		return counter;
	}
	else {
		return -2;
	}
}

//cat <path>
void FileSystem::readFile(string path) {
	string fileContent = this->readFileContent(path);
	if (fileContent != "ERROR_1") {
		cout << fileContent << endl;
	}
	else {
		cout << "Error: invalid path " + path << endl;
	}
}

//Append <source> <destination>
void FileSystem::appendToFile(string source, string destination) {
	string sourceContent = this->readFileContent(source);
	string destContent = this->readFileContent(destination);
	Node* destNode = this->getNodeFromPath(this->parsePath(destination));

	if (sourceContent != "ERROR_!" && destNode != nullptr) {
		string newContent = "";
		int blockNr = 0;
		int size = 0;

		for (size_t i = 0; i < destNode->getSize(); i++) {
			blockMap[destNode->getBlockNr() + i] = 0;
		}

		newContent = destContent + sourceContent + "\n:q";
		size = newContent.size() / 512 + 1;
		blockNr = this->writeFile(newContent, size);
		destNode->setBlockNr(blockNr);
	}
	else {
		if (sourceContent == "ERROR_1") {
			cout << "Error: invalid path " + source;
		}

		if (destNode == nullptr) {
			cout << "Error: invalid path " + destination;
		}
	}
}

//Create <filename>
int FileSystem::createFile(string path) {
	int size = 0;
	int blockNr = 0;
	string fileContent = "";
	Folder * pathFolder = this->currentFolder;
	vector<string> parsedPath = this->parsePath(path);
	vector<string> folderPath = parsedPath;

	folderPath.pop_back();
	pathFolder = this->getFolderFromPath(folderPath);
	
	for (int k = 0; k < pathFolder->getnrOfNodes(); k++) {
		if (pathFolder->getNodes()[k]->getName() == parsedPath[parsedPath.size() - 1]) {
			cout << "Error: File already exists" << endl;
			return -1;
		}
	}

	fileContent = this->getFileContent();

	size = fileContent.size() / 512 + 1;
	blockNr = this->writeFile(fileContent, size);

	if (blockNr >= 0) {
		pathFolder->createNode(parsedPath[parsedPath.size() - 1], size, blockNr);
	}
	else {
		if (blockNr == -1) {
			cout << "Error: Failed to write" << endl;
		}
		else if (blockNr == -2) {
			cout << "Error: No free space available" << endl;
		}
	}
}

void FileSystem::moveFile(string oldPath, string newPath) {
	Node* oldNode = this->getNodeFromPath(this->parsePath(oldPath));
	vector<string> newFolderPath = this->parsePath(newPath);
	string newName = newFolderPath[newFolderPath.size() - 1];
	vector<string> oldFolderPath = this->parsePath(oldPath);
	Folder* newFolder = nullptr;

	newFolderPath.pop_back();
	oldFolderPath.pop_back();
	newFolder = this->getFolderFromPath(newFolderPath);

	if (oldNode != nullptr && newFolder != nullptr) {
		newFolder->createNode(newName, oldNode->getSize(), oldNode->getBlockNr());

		this->getFolderFromPath(oldFolderPath)->deleteNode(oldNode);
	}
	else {
		if (oldNode == nullptr) {
			cout << "Error: invalid path " + oldPath << endl;
		}
		if (newFolder == nullptr) {
			cout << "Error: invalid path " + newPath << endl;
		}
	}
}

bool FileSystem::hasSpace(size_t start, size_t size, int blockMap[]) {
	for (size_t i = 0; i < size; i++) {
		if (start + i < 512) {
			if (blockMap[start + i] != 0) {
				return false;
			}
		}
		else {
			return false;
		}
	}

	return true;
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
int FileSystem::createFolder(string path) {
	bool pathFound = true;
	Folder* pathFolder = this->currentFolder;
	vector<string> parsedPath = this->parsePath(path);

	if (parsedPath.size() - 1 > 0) {
		for (int i = 0; i < parsedPath.size() - 1; i++) {
			if (parsedPath[i] == "..") {
				pathFolder = pathFolder->getParent();
			} 
			else if (parsedPath[i] == ".") {
				
			} 
			else if (parsedPath[i] == "") {
				pathFolder = this->homeFolder;
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
					cout << "Error: Invalid path" << endl;
					return -1;
				}
			}
		}
	}

	for (int k = 0; k < pathFolder->getnrOfFolders(); k++) {
		pathFound = false;

		if (pathFolder->getFolders()[k]->getName() == parsedPath[parsedPath.size() - 1]) {
			cout << "Error: Folder already exists" << endl;
			return -1;
		}
	}

	pathFolder->createFolder(parsedPath[parsedPath.size() - 1]);
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
	string currentPath = "";

	//Parse all path parts to vector
	vector<string> parsedPath = this->parsePath(path);

	if (path.size() <= 0) {
		this->currentFolder = this->homeFolder;
	}
	//Check all letters
	for (size_t i = 0; i < parsedPath.size(); i++) {
		foundFolder = false;

		if (parsedPath[i] == "..") {
			this->currentFolder = this->currentFolder->getParent();
		}
		//If "." -> current directory
		else if (parsedPath[i] == ".") {
			//Already in current directory
		}
		else if (parsedPath[i] == "") {
			this->currentFolder = this->homeFolder;
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

	return getPathFromRoot(this->currentFolder);
}

string FileSystem::getPathFromRoot(Folder* currentFolder) {
	Folder* tmp = currentFolder->getParent();
	string path = currentFolder->getName();

	while (currentFolder != tmp) {
		if (tmp->getName() != "/") {
			path = tmp->getName() + "/" + path;
		}
		else {
			path = tmp->getName() + path;
		}
		currentFolder = tmp;
		tmp = currentFolder->getParent();
	}

	if (path[path.size() - 1] != '/') {
		path += "/";
	}

	return path;
}

vector<string> FileSystem::parsePath(string path) {
	string tmp = "";
	vector<string> parsedPath;

	if (path != "") {
		if (path[path.size() - 1] != '/') {
			path += "/";
		}

		for (size_t i = 0; i < path.size(); i++) {
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
	else {
		parsedPath.push_back("");
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
				mainContent.pop_back();
				return mainContent;
			}
		}

		mainContent += stringFromBlock[i];
	}

	return mainContent;
}

void FileSystem::printCurrentPath() {
	cout << this->getPathFromRoot(this->currentFolder) << endl;
}