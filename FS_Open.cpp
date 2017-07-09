#include <stdio.h>
#include <string.h>
#include "Commands.hpp"

FSInfo* openFS( char* fsName );

FSInfo* commandWorkWith( FSInfo* fs, size_t wordsCounter, char** words )
{
    if ( wordsCounter != 2 ) {
        fprintf( stderr, "Error: Run as \"mfs_workwith <FS_NAME>\"\n" );
        return NULL;
    }

    if ( fs != NULL ) {
        fprintf( stderr, "Error: Another file system is already open!\n" );
        return fs;
    }
    return openFS( words[1] );
}

FSInfo* openFS( char* fsName )
{
    IO_Manager* ioManager = new IO_Manager( 0 );

    if ( ioManager->fileExists( fsName ) != 0 ) {
        fprintf( stderr, "File system %s was not found!\n", fsName );
        delete ioManager;
        return NULL;
    }

    FSInfo* fs = new FSInfo;
    fs->ioManager = ioManager;

    ioManager->openFile( fsName );                      // Open Fs
    readSuperBlock( fs );                               // Read Super Block
    ioManager->setBlockSize( fs->super->mBlockSize );   // Set correct block size
    readBlockGroupsStrucs( fs );                        // Read group Structures
    fs->currentDirInode = readInode( fs, 0 );           // Read Inode 0 ( "/" )
    fs->currentDirName = new char[2];
    strcpy( fs->currentDirName, "/" );

    //printSuperBlock(*(fs->super));
    return fs;
}