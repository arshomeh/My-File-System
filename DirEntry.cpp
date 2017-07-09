#include <stdio.h>
#include <string.h>
#include "DirEntry.hpp"
#include "FS.hpp"

void writeEntryToBlock( DirEntry* pEntry, char* addr )
{
    memcpy( addr, &( pEntry->mInode ), UINT32_SIZE );
    addr += UINT32_SIZE;
    memcpy( addr, &( pEntry->mOccupied ), CHAR_SIZE );
    addr += CHAR_SIZE;
    memcpy( addr, &( pEntry->mType ), CHAR_SIZE );
    addr += CHAR_SIZE;
    memcpy( addr, pEntry->mName, strlen( pEntry->mName ) + 1 );
}

void readEntryFromBlock( DirEntry* pEntry, char* addr, size_t maxNameLength )
{
    memcpy( &( pEntry->mInode ), addr, UINT32_SIZE );
    addr += UINT32_SIZE;
    memcpy( &( pEntry->mOccupied ), addr, CHAR_SIZE );
    addr += CHAR_SIZE;
    memcpy( &( pEntry->mType ), addr, CHAR_SIZE );
    addr += CHAR_SIZE;
    memcpy( pEntry->mName, addr, maxNameLength + 1 );
}

EntriesPosition findPosition ( DirEntry* entries, char* name, uint32_t entriesCount )
{
    EntriesPosition entriesPosition = { -1, -1, -1 };

    for ( uint32_t i = 0; i < entriesCount; i += 1 ) {
        if ( entries[i].mOccupied == 1 ) {
            if ( strcmp( entries[i].mName, name ) == 0 ) {
                if ( entries[i].mType == FILE_TYPE ) {
                    entriesPosition.file = i;
                } else if ( entries[i].mType == LINK_TYPE ) {
                    entriesPosition.link = i;
                } else {
                    entriesPosition.directory = i;
                }
            }
        }
    }

    return entriesPosition;
}

void removeDirEntry( DirEntry* entries, char* name, uint32_t entriesCount, uint32_t maxFilename )
{
    EntriesPosition entriesPosition;
    int position = -1;

    entriesPosition = findPosition ( entries, name, entriesCount );

    if ( entriesPosition.file != -1 ) {
        position = entriesPosition.file;
        resetEntry( &( entries[position] ), maxFilename );
    }

    if ( entriesPosition.link != -1 ) {
        position = entriesPosition.link;
        resetEntry( &( entries[position] ), maxFilename );
    }

    if ( entriesPosition.directory != -1 ) {
        position = entriesPosition.directory;
        resetEntry( &( entries[position] ), maxFilename );
    }
}

void resetEntry( DirEntry* entries, uint32_t maxFilename )
{
    memset( &( entries->mInode ),    0, sizeof( uint32_t ) );
    memset( &( entries->mOccupied ), 0, sizeof( char ) );
    memset( &( entries->mType ),     0, sizeof( char ) );
    memset( entries->mName,          0, maxFilename + 1 );
}

void copyEntry( DirEntry* destination, DirEntry* source, uint32_t maxFilename )
{
    memcpy( &( destination->mInode ),    &( source->mInode ),    sizeof( uint32_t ) );
    memcpy( &( destination->mOccupied ), &( source->mOccupied ), sizeof( char ) );
    memcpy( &( destination->mType ),     &( source->mType ),     sizeof( char ) );
    memcpy( destination->mName,          source->mName,          maxFilename + 1 );
}

void copyDirEntries( DirEntry* entriesTo, DirEntry* entriesFrom, uint32_t entriesToCount, uint32_t entriesFromCount, uint32_t maxFilename )
{
    size_t to = 0;

    for ( size_t from = 0; from < entriesFromCount; from += 1 ) {
        if ( entriesFrom[from].mOccupied == 1 ) {
            for ( ; to < entriesToCount; to += 1 ) {
                if ( entriesTo[to].mOccupied == 0 ) {
                    copyEntry( &( entriesTo[to] ), &( entriesFrom[from] ), maxFilename );
                    break;
                }
            }
        }
    }

}
DirEntry* createBlankEntries( uint32_t entriesCount, uint32_t maxFileName )
{
    DirEntry* entries = new DirEntry[entriesCount];

    for ( size_t i = 0; i < entriesCount; i += 1 ) {
        entries[i].mInode = 0;
        entries[i].mOccupied = 0;
        entries[i].mType = 0;
        entries[i].mName = new char[maxFileName+1];
        memset( entries[i].mName, 0, maxFileName+1 );
    }

    return entries;
}

void freeDirEntries( DirEntry* entries, size_t entriesCount )
{
    for ( size_t i = 0; i < entriesCount; i += 1 ) {
        delete[] entries[i].mName;
    }

    delete[] entries;
}