#include <stdio.h>
#include <string.h>
#include "Commands.hpp"

void commandMkdir( FSInfo* fs, size_t wordsCount, char** words )
{
    if ( fs == NULL ) {
        fprintf( stderr, "Error: No open file system found!\n" );
        return;
    }

    char** usefulWords;
    size_t usefulWordsCount = getUsefulWords( STARTPOINT, words, wordsCount, &usefulWords );

    if ( usefulWordsCount == 0 ) {
        fprintf( stderr, "Error: Missing directories names!\n" );
        return;
    }

    uint32_t currentDirInodeId = fs->currentDirInode->mInodeId;
    uint32_t maxFileName = fs->super->mMaxFileName;
    uint32_t maxEntriesPerDir = fs->super->mMaxEntriesPerDir;
    delete fs->currentDirInode;
    fs->currentDirInode = readInode( fs, currentDirInodeId );

    for ( size_t i = 0; i < usefulWordsCount; i += 1 ) {
        uint32_t dirNameLength = ( uint32_t ) strlen( usefulWords[i] );

        if ( fs->currentDirInode->mLinks == maxEntriesPerDir ) {
            fprintf( stderr, "Error: Folder cannot hold more entries!\n" );
            break;
        }

        if ( dirNameLength > maxFileName ) {
            fprintf( stderr, "Error: Max file name length is: %d\n", maxFileName );
            continue;
        }

        uint32_t newDirInodeId = createNewDir( fs, currentDirInodeId );
        DirEntry dirEntry = { newDirInodeId, 1, DIR_TYPE, NULL };
        dirEntry.mName = new char[maxFileName+1];
        strcpy( dirEntry.mName, usefulWords[i] );
        addEntryToDir( fs, fs->currentDirInode, &dirEntry );

        delete fs->currentDirInode;
        fs->currentDirInode = readInode( fs, currentDirInodeId );
        delete[] dirEntry.mName;
    }

    for ( size_t i = 0; i < usefulWordsCount; i += 1 ) {
        delete[] usefulWords[i];
    }

    delete[]  usefulWords;
    return;
}