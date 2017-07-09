#include <string.h>
#include <time.h>
#include <stdio.h>
#include "Inode.hpp"

Inode* createInode( uint32_t inodeId, uint32_t links, uint32_t size,
        uint32_t type, uint32_t parentId, uint32_t time )
{
    Inode* inode = new Inode;

    inode->mInodeId = inodeId;
    inode->mLinks = links;
    inode->mSize = size;
    inode->mType = type;
    inode->mParentId  = parentId;
    inode->mCreationTime = time;
    inode->mAccessTime = time;
    inode-> mModificationTime = time;
    memset( inode->mDataBlocks, 0, DATA_BLOCKS * sizeof( uint32_t) );

    return inode;
}

void changeInodeSize( Inode* inode, uint32_t size )
{
    inode->mSize = size;
}

void changeInodeAccessTime( Inode* inode, uint32_t time )
{
    inode->mAccessTime = time;
}

void changeInodeModificationTime( Inode* inode, uint32_t time )
{
    inode-> mModificationTime = time;
}

struct tm* getTime( uint32_t now )
{
    time_t now1;
    struct tm* tm;

    now1 = now;
    if ( ( tm = localtime ( &now1 ) ) == NULL ) {
        printf ( "Error extracting time stuff\n" );
        return NULL;
    }

    return tm;
}

void printfInode( Inode* inode ) 
{
    struct tm* creatTime = NULL;
    struct tm* modificationTime = NULL;
    struct tm* accessTime = NULL;

    if ( inode->mType == DIR_TYPE ) {
        printf( "d | ");
    } else if ( inode->mType == FILE_TYPE ) {
        printf( "f | " );
    } else if ( inode->mType == LINK_TYPE ){
        printf( "l | " );
    }

    if ( inode->mType == DIR_TYPE ) {
        printf( "%d | ", DIR_SIZE );
    } else {
        printf( "%d | ", inode->mSize );
    }

    creatTime = getTime ( inode->mCreationTime );
    printf( "%04d-%02d-%02d %02d:%02d:%02d | ",
        creatTime->tm_year+1900, creatTime->tm_mon+1, creatTime->tm_mday,
        creatTime->tm_hour, creatTime->tm_min, creatTime->tm_sec );

    modificationTime = getTime ( inode->mModificationTime );
    printf( "%04d-%02d-%02d %02d:%02d:%02d | ",
        modificationTime->tm_year+1900, modificationTime->tm_mon+1, modificationTime->tm_mday,
        modificationTime->tm_hour, modificationTime->tm_min, modificationTime->tm_sec );

    accessTime = getTime ( inode->mAccessTime );
    printf( "%04d-%02d-%02d %02d:%02d:%02d | ",
        accessTime->tm_year+1900, accessTime->tm_mon+1, accessTime->tm_mday,
        accessTime->tm_hour, accessTime->tm_min, accessTime->tm_sec );
}

