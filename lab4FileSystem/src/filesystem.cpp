#include "filesystem.h"

FileSystem::FileSystem() {
	//Initiate file system with one home folder that is it's own parent.
	this->homeFolder = new Folder();
	this->homeFolder->setName("/");
	this->homeFolder->setParent(this->homeFolder);
	this->currentFolder = this->homeFolder;
}

FileSystem::~FileSystem() {
	delete this->homeFolder;
}

//Get folder pointer from path. Returns nullptr if invalid path, returns current folder if path is empty.
Folder* FileSystem::getFolderFromPath(vector<string> path) {
	Folder* pathFolder = this->currentFolder;
	
	if (path.size() > 0) {
		for (size_t i = 0; i < path.size(); i++) {
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

//Get node pointer from path. Returns nullptr if node is not found.
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
		else {
			return node;
		}
	}
	else {
		return nullptr;
	}
}

//Get content of a file as a string.
string FileSystem::readFileContent(string path) {
	string fileContent = "";
	Node* fileNode = this->getNodeFromPath(this->parsePath(path));

	if (fileNode == nullptr) {
		return "ERROR_1";
	}
	//If node is found get file content and print.
	else {
		for (int i = 0; i < fileNode->getSize(); i++) {
			fileContent += mMemblockDevice.readBlock(fileNode->getBlockNr() + i).toString();
		}
		return this->getFileFromBlock(fileContent);
	}
}

//Write file to memBlocks. Returns -1 if failed to write and returns -2 if no space was found.
//Returns blockNr if succeeds
int FileSystem::writeFile(string fileContent, int size) {
	vector<string> blockContent;
	bool foundSpace = false;
	int counter = 0;

	//Split content into substrings and place into blocksized strings. 
	//Fill up non-full blocks with " "
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

	//Find free block space
	while (!foundSpace && counter < 511) {
		while (this->blockMap[counter] == 1 && counter < 511) {
			counter++;
		}

		foundSpace = this->hasSpace(counter, size, blockMap);
	}

	if (foundSpace) {
		for (int i = 0; i < size; i++) {
			int wrote = this->mMemblockDevice.writeBlock(counter + i, blockContent[i]);

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
	string sourceContent = this->readFileContent(source); //Get source content
	string destContent = this->readFileContent(destination); //Get dest content
	Node* destNode = this->getNodeFromPath(this->parsePath(destination)); //get dest node

	if (sourceContent != "ERROR_!" && destNode != nullptr) {
		string newContent = "";
		int blockNr = 0;
		int size = 0;

		for (int i = 0; i < destNode->getSize(); i++) {
			blockMap[destNode->getBlockNr() + i] = 0;
		}

		newContent = destContent + sourceContent + "\n:q";
		size = newContent.size() / 512 + 1;
		blockNr = this->writeFile(newContent, size); //Write new content to memblocks
		//Set new size and new blockNr to destNode
		destNode->setBlockNr(blockNr);
		destNode->setSize(size); 
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

//Create file from path and fileContent
int FileSystem::restoreFile(string path, string fileContent) {
	int size = 0;
	int blockNr = 0;
	Folder * pathFolder = this->currentFolder;
	vector<string> parsedPath = this->parsePath(path);
	vector<string> folderPath = parsedPath;

	//Get folder. Need to remove last element to get path to folder and not file.
	folderPath.pop_back();
	pathFolder = this->getFolderFromPath(folderPath);

	//Check so file doesn't already exist
	for (int k = 0; k < pathFolder->getnrOfNodes(); k++) {
		if (pathFolder->getNodes()[k]->getName() == parsedPath[parsedPath.size() - 1]) {
			cout << "Error: File already exists" << endl;
			return -1;
		}
	}

	size = fileContent.size() / 512 + 1;
	blockNr = this->writeFile(fileContent, size);

	if (blockNr >= 0) {
		pathFolder->createNode(parsedPath[parsedPath.size() - 1], size, blockNr);
		return 0;
	}
	else {
		if (blockNr == -1) {
			cout << "Error: Failed to write" << endl;
		}
		else if (blockNr == -2) {
			cout << "Error: No free space available" << endl;
		}

		return -1;
	}
}

//Create <filename>
void FileSystem::createFile(string path) {
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

//Move file
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

//Copy file
void FileSystem::copyFile(string source, string destination) {
	Node* sourceNode = this->getNodeFromPath(this->parsePath(source));
	vector<string> destPath = this->parsePath(destination);
	string destName = destPath[destPath.size() - 1];
	Folder* destFolder = nullptr;
	string sourceContent = this->readFileContent(source);
	int wrote = 0;

	destPath.pop_back();
	destFolder = this->getFolderFromPath(destPath);

	//Get old content, write content to new blocks and create a new node.
	if (sourceNode != nullptr && destFolder != nullptr) {
		sourceContent += "\n:q";
		wrote = this->writeFile(sourceContent, sourceNode->getSize());
		if (wrote > 0) {
			destFolder->createNode(destName, sourceNode->getSize(), wrote);
		}
		else if (wrote == -1) {
			cout << "Error: Failed to write" << endl;
		}
		else {
			cout << "Error: Not enough space" << endl;
		}
	}
	else {
		if (destFolder == nullptr) {
			cout << "Error: invalid path " + destination << endl;
		}

		if (sourceNode == nullptr) {
			cout << "Error: invalid path " + source << endl;
		}
	}
}

//Removes file. Deletes the node and sets the space as free in the blockMap.
void FileSystem::removeFile(string path) {
	vector<string> parsedPath = this->parsePath(path);
	Node* node = this->getNodeFromPath(parsedPath);
	Folder* folder = nullptr;
	
	parsedPath.pop_back();
	folder = this->getFolderFromPath(parsedPath);

	if (node != nullptr) {
		for (int i = 0; i < node->getSize(); i++) {
			this->blockMap[node->getBlockNr() + i] = 0;
		}

		folder->deleteNode(node);
	}
	else {
		cout << "Error: invalid path" << endl;
	}
}

//Check if there are available space.
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

//Get input from the user for content. 
string FileSystem::getFileContent() {
	string content = "";
	string row = "";

	cout << "Press enter and input :q to exit input content\nInput file content: " << endl;

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
void FileSystem::createFolder(string path) {
	bool pathFound = true;
	Folder* pathFolder = this->currentFolder;
	vector<string> parsedPath = this->parsePath(path);
	string newFolder = parsedPath[parsedPath.size() - 1];
	
	parsedPath.pop_back();
	pathFolder = this->getFolderFromPath(parsedPath);

	if (pathFolder != nullptr) {
		if (pathFolder->getFolder(newFolder) == nullptr) {
			pathFolder->createFolder(newFolder);
		}
		else {
			cout << "Error: Folder already exists" << endl;
		}
	}
	else {
		cout << "Error: invalid path" << endl;
	}
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
	for (size_t i = 0; i < nodes.size(); i++) {
		cout << nodes[i]->getName() << endl;
	}
	//Get fileNames.
}

//cd <path>
string FileSystem::goToFolder(string path) {
	Folder* destFolder = this->getFolderFromPath(this->parsePath(path));

	if (destFolder != nullptr) {
		this->currentFolder = destFolder;
	}
	else {
		cout << "Error: Invalid path" << endl;
	}

	return getPathFromRoot(this->currentFolder) + "/";
}

//Get path from root. Get parent until currentFolder is its own parent.
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

	if (path == "/") {
		return "";
	}

	return path;
}

vector<string> FileSystem::parsePath(string path) {
	string tmp = "";
	vector<string> parsedPath;

	//Checks path for / and place parts in parsedPath.
	if (path != "") {
		if (path[path.size() - 1] != '/') {
			path += "/";
		}

		for (size_t i = 0; i < path.size(); i++) {
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

//Gets filecontent from block. Removes :q (end of file) and everything after and returns result as string.
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
	cout << this->getPathFromRoot(this->currentFolder) + "/" << endl;
}

//Creates file with folder paths and nodes
void FileSystem::createImage(string realFile) {
	Folder* current = this->homeFolder;
	ofstream out;

	out.open(realFile);
	
	if (out.is_open()) {
		out << "folders:" << endl;
		out << this->getFolderString(this->homeFolder) << endl;
		out << "nodes:" << endl;
		out << this->getNodeString(this->homeFolder) << endl;

		out.close();
	}
	else {
		cout << "Error: Failed to open file" << endl;
	}
}

void FileSystem::restoreImage(string realFile) {
	string str, line, path;
	ifstream in;
	vector<string> parsedPath;

	//Open file
	in.open(realFile);

	if (in.is_open()) {
		cout << "Formatting disk" << endl;
		this->formatDisk();
		//Read first line
		in >> line;
		//Read in folders
		if (line == "folders:") {
			cout << "Creating folders" << endl;
			in >> line;
			while (line != "nodes:" || in.eof()) {
				cout << line << endl;
				this->createFolder(line);
				in >> line;
			}
			//Read in nodes
			in >> line;
			cout << endl << "Creating files" << endl;
			while (!in.eof()) {
				//If line is a path save path in path
				if (line[0] == '/') {
					path = line;
					in >> line;
				}
				//If not a path, it's content. Collect content in str and create file.
				else {
					str = "";

					while (line != ":q") {
						str += line + " ";
						in >> line;
					}

					str += "\n" + line;
					this->restoreFile(path, str);

					cout << path << endl;
					in >> line;
				}
			}
		}
		else {
			cout << "Error: Invalid image" << endl;
		}

		in.close();
	}
	else {
		cout << "Error: Could not open image" << endl;
	}
}

//Recursive function getting all folder paths and putting them in a string separated by newline.
string FileSystem::getFolderString(Folder* folder) {
	string folderString = "";

	for (size_t i = 0; i < folder->getFolders().size(); i++) {
		folderString += this->getPathFromRoot(folder->getFolders()[i]) + "\n";
		folderString += this->getFolderString(folder->getFolders()[i]);
	}

	return folderString;
}

//Recursive funktion getting all nodes: name and content.
string FileSystem::getNodeString(Folder* folder) {
	string nodeString = "";
	string tmpPath = "";

	for (size_t k = 0; k < folder->getNodes().size(); k++) {
		tmpPath = this->getPathFromRoot(folder) + "/" + folder->getNodes()[k]->getName();
		nodeString += tmpPath + "\n";
		nodeString += this->readFileContent(tmpPath) + "\n:q\n";
	}

	for (size_t i = 0; i < folder->getFolders().size(); i++) {
		nodeString += this->getNodeString(folder->getFolders()[i]);
	}

	return nodeString;
}

//Format disk
void FileSystem::formatDisk() {
	for (int i = 0; i < this->memBlockSize; i++) {
		this->blockMap[i] = 0;
	}

	delete this->homeFolder;
	this->homeFolder = new Folder("/", this->homeFolder);
	this->currentFolder = this->homeFolder;
}