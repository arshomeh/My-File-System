#include <stdint.h>
#include <time.h>
#include <string.h>
#include "Inode.hpp"
#include "FS.hpp"

uint32_t createNewDir( FSInfo* fs, uint32_t parentInodeId )
{
    Inode* inodeData;
    uint32_t inodeId = 0;
    uint32_t dataBlocks[1];
    uint32_t blockSize = fs->super->mBlockSize;
    uint32_t currentTime = ( uint32_t ) time( NULL );

    inodeId = getInode( fs );
    getBlocks( fs, dataBlocks, 1 );

    // Fill Inode with information
    inodeData = createInode( inodeId, 0, 1, DIR_TYPE, parentInodeId, currentTime );
    inodeData->mDataBlocks[0] = dataBlocks[0];

    writeInode( fs, inodeData );  // Write inode to file system

    char* buffer = new char[blockSize];
    memset( buffer, 0, blockSize );
    fs->ioManager->writeBlock( dataBlocks[0], buffer );

    delete inodeData;
    delete[] buffer;
    return inodeId;
}

void addEntryToDir( FSInfo* fs, Inode* inode, DirEntry* entry )
{
    uint32_t maxFileName = fs->super->mMaxFileName;
    uint32_t blockSize = fs->super->mBlockSize;
    uint32_t dataBlocksCount = inode->mSize;
    uint32_t currentEntriesCount = inode->mLinks;
    uint32_t entriesPerBlock = fs->super->mEntriesPerBlock;
    uint32_t maxEntriesCount = entriesPerBlock * dataBlocksCount;
    uint32_t totalEntriesCount = ( currentEntriesCount+1 > maxEntriesCount ) ?
            maxEntriesCount + entriesPerBlock : maxEntriesCount;
    DataAndPointersBlocks blocksInfo = getDataAndPointrerBlocks( fs, inode );
    uint32_t pointerBlocksCount = blocksInfo.pointerBlocksCount;
    uint32_t* dataBlocks = blocksInfo.dataBlocks;
    uint32_t* pointerBlocks = blocksInfo.pointerBlocks;

    DirEntry* oldEntries = readDirEntries( fs, inode, dataBlocks );
    DirEntry* newEntries = createBlankEntries( totalEntriesCount, maxFileName );
    copyDirEntries( newEntries, oldEntries, currentEntriesCount, maxEntriesCount, maxFileName );
    copyEntry( &( newEntries[currentEntriesCount] ) , entry, maxFileName );
    freeDirEntries( oldEntries, maxEntriesCount );

    if ( totalEntriesCount > maxEntriesCount ) {
        uint32_t newDataBlock[1];
        uint32_t allDataBlocksCount = dataBlocksCount + 1;
        uint32_t* allDataBlocks = new uint32_t[allDataBlocksCount];

        for ( uint32_t i = 0; i < dataBlocksCount; i += 1 ) {
            allDataBlocks[i] = dataBlocks[i];
        }
        getBlocks( fs, newDataBlock, 1 );
        allDataBlocks[dataBlocksCount] = newDataBlock[0];
        delete[] dataBlocks;
        dataBlocks = allDataBlocks;
        dataBlocksCount = allDataBlocksCount;

        uint32_t allPointersBlocksCount =
                neededPointerBlocksCount( allDataBlocksCount, blockSize );
        if ( allPointersBlocksCount > pointerBlocksCount ) {
            uint32_t newPointerBlock[1];
            uint32_t* allPointerBlocks = new uint32_t[allPointersBlocksCount];

            for( uint32_t i = 0; i < allPointersBlocksCount-1; i += 1 ) {
                allPointerBlocks[i] = pointerBlocks[i];
            }
            getBlocks( fs, newPointerBlock, 1 );
            allPointerBlocks[allPointersBlocksCount-1] = newPointerBlock[0];
            delete[] pointerBlocks;
            pointerBlocks = allPointerBlocks;
            pointerBlocksCount = allPointersBlocksCount;
        }
    }

    writeDirEntries( fs, newEntries, inode, dataBlocks );
    freeDirEntries( newEntries, totalEntriesCount );

    inode->mLinks += 1;
    inode->mSize = dataBlocksCount;
    storeDataBlocksIds( fs, inode, dataBlocks, pointerBlocks, dataBlocksCount, pointerBlocksCount );

    delete[] dataBlocks;
    delete[] pointerBlocks;
}

void writeDirEntries( FSInfo* fs, DirEntry* entries, Inode* inode, uint32_t* dataBlocks )
{
    IO_Manager* ioManager = fs->ioManager;
    uint32_t blockSize = fs->super->mBlockSize;
    uint32_t entrySize = fs->super->mEntrySize;
    uint32_t entriesPerBlock = fs->super->mEntriesPerBlock;
    uint32_t dataBlocksCount = inode->mSize;
    uint32_t currentEntry = 0;
    char* buffer = new char[blockSize];


    for ( size_t iBlock = 0; iBlock < dataBlocksCount; iBlock += 1 ) {
        memset( buffer, 0, blockSize );

        size_t bufferOffset = 0;
        for ( size_t i = 0; i < entriesPerBlock; i += 1 ) {
            bufferOffset = i * entrySize;
            char* pBuffer = buffer + bufferOffset;

            writeEntryToBlock( &( entries[currentEntry] ), pBuffer );
            currentEntry += 1;
        }

        ioManager->writeBlock( dataBlocks[iBlock], buffer );
    }
    delete[] buffer;
}

DirEntry* readDirEntries( FSInfo* fs, Inode* inode, uint32_t* dataBlocks )
{
    IO_Manager* ioManager = fs->ioManager;
    uint32_t blockSize = fs->super->mBlockSize;
    uint32_t entrySize = fs->super->mEntrySize;
    uint32_t maxFileName = fs->super->mMaxFileName;
    uint32_t entriesPerBlock = fs->super->mEntriesPerBlock;
    uint32_t dataBlocksCount = inode->mSize;
    uint32_t entriesCount = dataBlocksCount * entriesPerBlock;
    uint32_t unReadEntriesCount = entriesCount;
    DirEntry* entries = new DirEntry[entriesCount];
    char* buffer = new char[blockSize];

    for ( size_t iBlock = 0; iBlock < dataBlocksCount; iBlock += 1 ) {
        memset( buffer, 0, blockSize );
        ioManager->readBlock( dataBlocks[iBlock], buffer );

        size_t bufferOffset = 0;
        for ( size_t i = 0; i < entriesPerBlock; i += 1 ) {
            bufferOffset = i * entrySize;
            char* pBuffer = buffer + bufferOffset;
            uint32_t currentEntry = entriesCount-unReadEntriesCount;
            entries[currentEntry].mName = new char[maxFileName+1];

            readEntryFromBlock( &( entries[currentEntry] ), pBuffer, maxFileName );
            unReadEntriesCount -= 1;
        }
    }

    delete[] buffer;
    return entries;
}