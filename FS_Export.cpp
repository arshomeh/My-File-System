#include <stdio.h>
#include <string.h>
#include "Commands.hpp"

void commandExport( FSInfo* fs, size_t wordsCount, char** words )
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
    char** filesToExport = new char*[filesCount];

    for ( int i = 0, j = 1; i < filesCount; i += 1, j += 1 ) {
        filesToExport[i] = words[j];
    }

    for ( ssize_t i = 0; i < filesCount; i += 1 ) {

        Inode* dirInode = NULL;
        uint32_t fileInodeId;
        uint32_t tokensCount = 0;
        char** pathTokens = NULL;

        tokensCount = ( uint32_t ) tokenize( filesToExport[i], &pathTokens, "/", PATH );

        if ( filesToExport[i][0] == '/' ) {
            dirInode = readInode( fs, 0 );
        } else {
            dirInode = readInode( fs, fs->currentDirInode->mInodeId );
        }

        fileInodeId = getInodeOfFile( fs, dirInode, pathTokens, 0, tokensCount-1 );

        if ( fileInodeId == ( uint32_t ) -1 ) {
            fprintf( stderr, "Error: File %s not found\n", filesToExport[i] );
        } else {
            Inode* fileInode = readInode( fs, fileInodeId );
            size_t outFileNameLength = strlen( directory ) + 1 +
                    strlen( pathTokens[tokensCount-1] ) + 1;
            char* outFile = new char[outFileNameLength];
            memset( outFile, 0, outFileNameLength );

            strcat( outFile, directory );
            strcat( outFile, "/" );
            strcat( outFile, pathTokens[tokensCount-1] );
            exportFromFileSystem( fs, outFile, fileInode );
            delete[] outFile;
            delete fileInode;
        }

        delete dirInode;
        delete[] pathTokens;
    }

    delete[] filesToExport;
    return;
}

void exportFromFileSystem( FSInfo* fs, char* toFile, Inode* inodeFromFile )
{
    uint32_t blockSize = fs->super->mBlockSize;
    IO_Manager* ioManager = new IO_Manager( blockSize );

    if ( ioManager->fileExists( toFile ) == 0 ) {
        fprintf( stderr, "Error: file %s already exists!\n", toFile );
        ioManager->cleanup();
        delete ioManager;
        return;
    }

    ioManager->openFile( toFile );

    if ( inodeFromFile->mSize > 0 ) {
        DataAndPointersBlocks blocksInfo = getDataAndPointrerBlocks( fs, inodeFromFile );
        uint32_t dataBlocksCount = blocksInfo.dataBlocksCount;
        uint32_t *dataBlocks = blocksInfo.dataBlocks;
        uint32_t *outputBlocks = new uint32_t[dataBlocksCount];

        for (uint32_t i = 0; i < dataBlocksCount; i += 1) {
            outputBlocks[i] = i;
        }
        copyBlocks( fs->ioManager, ioManager, dataBlocks, outputBlocks, inodeFromFile->mSize );

        delete[] outputBlocks;
        delete[] blocksInfo.dataBlocks;
        delete[] blocksInfo.pointerBlocks;
    }

    ioManager->cleanup();
    delete ioManager;
}
