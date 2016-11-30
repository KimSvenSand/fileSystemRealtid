#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Folder.h"
#include "memblockdevice.h"

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
	Folder mFolder;
	Folder * currentFolder;
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

    /* Creates a folder in the filesystem */
	void createFolder(string name);

	/* Removes a file in the filesystem */
	// removeFile(...);

	/* Removes a folder in the filesystem */
	// removeFolder(...);

	/* Function will move the current location to a specified location in the filesystem */
	// goToFolder(...);

	/* This function will get all the files and folders in the specified folder */
	// listDir(...);
	void listDir();

    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
