#include <stdio.h>
#include <time.h>
#include <string.h>
#include "FS.hpp"
#include "Commands.hpp"


void importToFS( FSInfo* fs, uint32_t dirInodeId, char** filesToImport, int fileCount );

void commandImport( FSInfo* fs, size_t wordsCount, char** words )
{
    if ( fs == NULL ) {
        fprintf( stderr, "Error: No open file system found!\n" );
        return;
    }

    int filesCount = ( int ) wordsCount - 2;

    if ( filesCount < 1 ) {
        fprintf( stderr, "Error: Missing arguments!\n" );
        return;
    }

    char* directory = words[wordsCount-1];
    char** filesToImport = new char*[filesCount];

    for ( int i = 0, j = 1; i < filesCount; i += 1, j += 1 ) {
        filesToImport[i] = words[j];
    }


    Inode* dirInode = NULL;
    uint32_t dirInodeId;
    uint32_t tokensCount = 0;
    char** pathTokens = NULL;

    tokensCount = ( uint32_t ) tokenize( directory, &pathTokens, "/", PATH );

    if ( !strcmp( directory, "/" ) ) {
        dirInodeId = 0;
    } else {
        if ( directory[0] == '/' ) {
            dirInode = readInode( fs, 0 );
        } else {
            dirInode = readInode( fs, fs->currentDirInode->mInodeId );
        }
        dirInodeId = getInodeOfDir( fs, dirInode, pathTokens, 0, tokensCount-1 );
        delete dirInode;
    }

    if ( dirInodeId == ( uint32_t ) -1 ) {
        fprintf( stderr, "Error: Invalid path %s\n", directory );
    } else {
        importToFS( fs, dirInodeId, filesToImport, filesCount );
    }

    delete[] pathTokens;
    delete[] filesToImport;
    return;
}

void importToFS( FSInfo* fs, uint32_t dirInodeId, char** filesToImport, int fileCount )
{
    Inode* dirInode = readInode( fs, dirInodeId );
    uint32_t maxFileName = fs->super->mMaxFileName;
    uint32_t maxEntriesPerDir = fs->super->mMaxEntriesPerDir;

    for ( int i = 0; i < fileCount; i += 1 ) {
        uint32_t fileNameLength = ( uint32_t ) strlen( filesToImport[i] );

        if ( dirInode->mLinks == maxEntriesPerDir ) {
            fprintf( stderr, "Error: Folder cannot hold more entries!\n" );
            break;
        }

        if ( fileNameLength > maxFileName ) {
            fprintf( stderr, "Error: Max file name length is: %d\n", maxFileName );
            continue;
        }

        uint32_t fileInodeId = importFile( fs, filesToImport[i], dirInodeId );
        DirEntry dirEntry = { fileInodeId, 1, FILE_TYPE, NULL };
        dirEntry.mName = new char[maxFileName+1];
        strcpy( dirEntry.mName, filesToImport[i] );
        if ( fileInodeId != 0 ) {
            addEntryToDir( fs, dirInode, &dirEntry );
        }

        delete dirInode;
        dirInode = readInode( fs, dirInodeId );
        delete[] dirEntry.mName;
    }

    delete dirInode;
}

/*
 * Import file to file system and return the inode id.
 * On error returned inode id is 0.
 */
uint32_t importFile( FSInfo* fs, char* fileName, uint32_t parentId )
{
    uint32_t fileSize = 0;
    uint32_t blockSize = fs->super->mBlockSize;
    uint32_t maxFileSize = fs->super->mMaxFileSize;
    IO_Manager* fIOManager = new IO_Manager( blockSize );

    if ( fIOManager->fileExists( fileName ) != 0 ) {
        fprintf( stderr, "Error: file \"%s\" not found!\n", fileName );
        delete fIOManager;
        return 0;
    }

    fIOManager->openFile( fileName );
    fileSize = ( uint32_t ) fIOManager->getFileSize();
    if ( fileSize > maxFileSize ) {
        fprintf( stderr, "Error: file size is too big: %d bytes\n", fileSize );
        fprintf( stderr, "Max file size: %d bytes\n", maxFileSize );
        fIOManager->cleanup();
        delete fIOManager;
        return 0;
    }

    uint32_t inodeId = getInode( fs );
    uint32_t ttime = ( uint32_t ) time( NULL );
    uint32_t* dataBlocks = NULL;
    uint32_t dataBlocksCount = neededDataBlocksCount( fileSize, blockSize );

    Inode* inode = createInode( inodeId, 1, fileSize, FILE_TYPE, parentId, ttime );

    if ( fileSize > 0 && dataBlocksCount > 0 ) {
        // If file is not empty
        dataBlocks = new uint32_t[dataBlocksCount];

        for ( uint32_t i = 0; i < dataBlocksCount; i += 1 ) {
            dataBlocks[i] = i;
        }
        writeFileBlocks( fs, fIOManager, inode, dataBlocks );
        delete[] dataBlocks;
    } else {
        writeInode( fs, inode );
    }


    fIOManager->cleanup();
    delete fIOManager;
    delete inode;
    return inodeId;
}