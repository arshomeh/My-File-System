#include <stdio.h>
#include <string.h>
#include "FS.hpp"
#include "Commands.hpp"

char* fixPath( char* path );
void cdFS( FSInfo* fs, char* path, char** pathTokens, uint32_t tokensCount );

void commandCd( FSInfo* fs, size_t wordsCounter, char** words )
{
    if ( fs == NULL ) {
        fprintf( stderr, "Error: No open file system found!\n" );
        return;
    }

    if ( wordsCounter < 2 ) {
        fprintf( stderr, "Error: missing directory!\n" );
        return;
    }

    char** pathTokens = NULL;
    uint32_t tokensCount = 0;
    uint32_t maxFileName = fs->super->mMaxFileName;
    tokensCount = ( uint32_t ) tokenize( words[1], &pathTokens, "/", PATH );

    for ( uint32_t i = 0; i < tokensCount; i += 1 ) {
        if ( strlen( pathTokens[i] ) > maxFileName ) {
            fprintf( stderr, "Error: Invalid filename size!\n" );
            fprintf( stderr, "Max supported filename size: %d\n", maxFileName );
            delete[] pathTokens;
            return;
        }
    }

    cdFS( fs, words[1], pathTokens, tokensCount );
    delete[] pathTokens;
}

void cdFS( FSInfo* fs, char* path, char** pathTokens, uint32_t tokensCount )
{
    uint32_t dirInodeId;
    char* newPath;

    if ( strcmp( path, "/" ) == 0 ) {
        dirInodeId = 0;
        newPath = new char[2];
        strcpy( newPath, "/" );
    } else if ( path[0] == '/' ) {
        Inode* rootInode = readInode( fs, 0 );
        dirInodeId = getInodeOfDir( fs, rootInode, pathTokens, 0, tokensCount-1 );
        newPath = fixPath( concatPathTokens( pathTokens, tokensCount ) );
        delete rootInode;
    } else {
        dirInodeId = getInodeOfDir( fs, fs->currentDirInode, pathTokens, 0, tokensCount-1 );
        char* halfPath = concatPathTokens( pathTokens, tokensCount );
        uint32_t length = ( uint32_t ) ( strlen(fs->currentDirName) + strlen(halfPath) + 1 );
        newPath = new char[length];
        memset( newPath, 0, length );
        strcat( newPath, fs->currentDirName );
        strcat( newPath, halfPath );
        char* fixedPath = fixPath( newPath );
        delete[] newPath;
        newPath = fixedPath;
        delete[] halfPath;
    }

    if ( dirInodeId == ( uint32_t ) -1 ) {
        delete[] newPath;
    } else {
        delete[] fs->currentDirName;
        fs->currentDirName = newPath;
        delete fs->currentDirInode;
        fs->currentDirInode = readInode( fs, dirInodeId );
    }
}

void commandPwd( FSInfo* fs )
{
    if ( fs == NULL ) {
        fprintf( stderr, "Error: No open file system found!\n" );
        return;
    }

    fprintf( stdout, "%s\n", fs->currentDirName );
}

char* concatPathTokens( char** tokens, uint32_t tokensCount )
{
    uint32_t length = 0;

    for ( uint32_t i = 0; i < tokensCount; i += 1 ) {
        length += 1 + strlen( tokens[i] );  // "/+token"
    }

    char* newPath = new char[length+1];     // plus 1 byte for '\0'
    memset( newPath, 0, length+1 );

    for ( uint32_t i = 0; i < tokensCount; i += 1 ) {
        strcat( newPath, "/" );
        strcat( newPath, tokens[i] );
    }

    return newPath;
}

char* fixPath( char* path )
{
    char** pathTokens = NULL;
    uint32_t tokensCount = 0;
    uint32_t length = ( uint32_t ) strlen( path );

    tokensCount = ( uint32_t ) tokenize( path, &pathTokens, "/", PATH );

    char* fixedPath = new char[length+1];  // plus 1 byte for '\0'
    memset( fixedPath, 0, length+1 );

    for ( uint32_t i = 0; i < tokensCount; i += 1 ) {
        if ( !strcmp( pathTokens[i], "." ) ) {
            continue;
        } else if ( !strcmp( pathTokens[i], "..") ) {
            int len = ( int ) strlen( fixedPath );
            for ( int j = len-1; j >= 0; j -= 1 ) {
                char c = fixedPath[j];
                memset( &( fixedPath[j] ), 0, 1 );
                if ( c == '/' ) {
                    break;
                }
            }
            continue;
        }

        strcat( fixedPath, "/" );
        strcat( fixedPath, pathTokens[i] );
    }

    if ( !strcmp( fixedPath, "" ) ) {
        strcpy( fixedPath, "/" );
    }

    delete[] pathTokens;
    return fixedPath;
}
