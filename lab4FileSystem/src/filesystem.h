#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include<iostream>
#include<fstream>
#include "Folder.h"
#include "memblockdevice.h"

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
	int blockMap[250] = {0}; //For keeping track of free space
	int memBlockSize = 250; 
	Folder * currentFolder;
	Folder * homeFolder;

	vector<string> parsePath(string path);
	string getFileContent(); //Get content from user
	string getFileFromBlock(string stringFromBlock); //Get file content as string from blockContent
	bool hasSpace(size_t start, size_t size, int blockMap[]); //Find free space
	string getPathFromRoot(Folder* currentFolder); //Get cwr path from root
	string readFileContent(string path); //Get filecontent from a file in a string
	Folder* getFolderFromPath(vector<string> path); 
	Node* getNodeFromPath(vector<string> path);
	int writeFile(string fileContent, int size); //Write filecontent to memblock. Returns blockNr.
	string getFolderString(Folder* folder); //Get string of folder paths. For writing to image.
	string getNodeString(Folder* folder);
	int restoreFile(string path, string fileContent); //Restores a file from path and content. 
public:
    FileSystem();
    ~FileSystem();

	//create <path>
	void createFile(string path);
	//cat <path>
	void readFile(string path);
	//pwd
	void printCurrentPath();
	//mkdir <path>
	void createFolder(string path);
	//Append content of source to end of content of destination
	//append <source> <destination>
	void appendToFile(string source, string destination);
	//mv <source> <destination>
	void moveFile(string oldPath, string newPath);
	//rm <file-path>
	void removeFile(string path);
	//cp <source> <destination>
	void copyFile(string source, string destination);
	//createImage <real-file>
	void createImage(string realPath);
	//restoreImage <real-file>
	void restoreImage(string realFile);
	//format
	void formatDisk();
	//cd <path>
	string goToFolder(string path);
	//ls
	void listDir();
};

#endif // FILESYSTEM_H
