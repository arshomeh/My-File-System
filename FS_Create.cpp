#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "Commands.hpp"

#define MIN_BLOCK_SIZE 512


void createFS( char* fsName, FSOptions options );

void commandCreate( size_t wordsCounter, char** words )
{
    int bsflag = -1, fnsflag = -1, mfsflag = -1, mdfnflag = -1;
    optind = 0;

    while ( true ){
        static struct option long_options[] = {
                { "bs", required_argument, 0, 'a' },
                { "fns", required_argument, 0, 'b' },
                { "mfs", required_argument, 0, 'c' },
                { "mdfn", required_argument, 0, 'd' },
                { 0,    0,                 0,  0  }
        };

        int option_index = 0;
        int c = getopt_long_only( ( int ) wordsCounter, words, "a:b:c:d:", long_options, &option_index );

        if ( c == -1 )
            break;

        switch ( c ) {
            case 'a':
                bsflag = atoi(optarg);
                break;
            case 'b':
                fnsflag = atoi(optarg);
                break;
            case 'c':
                mfsflag = atoi(optarg);
                break;
            case 'd':
                mdfnflag = atoi(optarg);
                break;
            default:
                break;
        }
    }

    if ( bsflag <= 0 || bsflag % MIN_BLOCK_SIZE != 0 ) {
        fprintf( stderr, "Error: Block size must be a multiple of 512 bytes!\n" );
        return;
    } else if ( fnsflag <= 0 ) {
        fprintf( stderr, "Error: Max file name size must be greater than 0!\n" );
        return;
    } else if ( mfsflag <= 0 ) {
        fprintf( stderr, "Error: Max file size must be greater than 0!\n" );
        return;
    } else if ( mdfnflag <= 0 ) {
        fprintf( stderr, "Error: Max files per directory must be greater than 0!\n" );
        return;
    }

    char** usefulWords;
    size_t usefulWordsCount = 0;
    usefulWordsCount = getUsefulWords( optind, words, wordsCounter, &usefulWords );
    FSOptions fsOptions = { ( uint32_t ) bsflag, ( uint32_t ) fnsflag,
            ( uint32_t ) mfsflag, ( uint32_t ) mdfnflag, };

    if ( usefulWordsCount == 1 ) {
        createFS( usefulWords[0], fsOptions );
    } else {
        fprintf( stderr, "Error: Provide only one file system name!\n" );
    }

    for ( size_t i = 0; i < usefulWordsCount; i += 1 ){
        delete[] usefulWords[i];
    }

    if ( usefulWordsCount > 0 ) {
        delete[] usefulWords;
    }
}

void createFS( char* fsName, FSOptions options )
{
    IO_Manager io( 0 );

    // FS filename has to end to ".mfs"
    size_t length = strlen( fsName ) + strlen( ".mfs" );
    char* newFsName = new char[length + 1];
    memset( newFsName, 0, length + 1 );
    strcat( newFsName, fsName );
    strcat( newFsName, ".mfs" );


    if ( io.fileExists( newFsName ) == 0 ) {
        fprintf( stderr, "Error: File %s already exists!\n", newFsName );
        delete[] newFsName;
        return;
    }

    FSInfo* fs = new FSInfo;
    fs->groupStructures = NULL;
    IO_Manager* ioManager = new IO_Manager( options.mBlockSize );

    fs->ioManager = ioManager;
    fs->ioManager->openFile( newFsName );

    fs->super = createSuperBlock( options );
    writeSuperBlock( fs );
    createBlockGroup( fs );
    createNewDir( fs, 0 );
    fs->currentDirInode = readInode( fs, 0 );
    fs->currentDirName = new char[2];
    strcpy( fs->currentDirName, "/" );

    //printSuperBlock( *(fs->super) );

    delete[] newFsName;
    closeFS( fs );
}
