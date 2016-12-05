#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Folder.h"
#include "memblockdevice.h"

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
	int blockMap[250];
	Folder mFolder;
	Folder * currentFolder;
	Folder * homeFolder;

	vector<string> parsePath(string path);
	string getFileContent();
	string getFileFromBlock(string stringFromBlock);
	bool hasSpace(size_t start, size_t size, int blockMap[]);
	string FileSystem::getPathFromRoot(Folder* currentFolder);
    // Here you can add your own data structures
public:
    FileSystem();
    ~FileSystem();

    /* These commands needs to implemented
     *
     * However, you are free to change the parameterlist and specify your own returntype for each function below.
     */

    /* This function creates a file in the filesystem */
    // createFile(...)
	int createFile(string path);

	int readFile(string path);

    /* Creates a folder in the filesystem */
	int createFolder(string path);

	/* Removes a file in the filesystem */
	// removeFile(...);

	/* Removes a folder in the filesystem */
	// removeFolder(...);

	/* Function will move the current location to a specified location in the filesystem */
	// goToFolder(...);
	string goToFolder(string path);

	/* This function will get all the files and folders in the specified folder */
	// listDir(...);
	void listDir();

    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
