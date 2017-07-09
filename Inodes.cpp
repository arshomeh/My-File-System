#include <stdio.h>
#include <string.h>
#include "FS.hpp"

uint32_t getInodeOfFile( FSInfo* fs, Inode* currentInode, char** pathTokens, uint32_t pathIndex, uint32_t pathEnd )
{
    if ( pathIndex > pathEnd ) {
        return ( uint32_t ) -1;
    } else if ( strcmp( pathTokens[pathIndex], "." ) == 0 ) {
        return getInodeOfFile( fs, currentInode, pathTokens, pathIndex+1, pathEnd );
    } else if ( strcmp( pathTokens[pathIndex], "..") == 0 ) {
        Inode* parentInode = readInode( fs, currentInode->mParentId );
        uint32_t inodeId = getInodeOfFile( fs, parentInode, pathTokens, pathIndex+1, pathEnd );
        delete parentInode;
        return inodeId;
    }

    uint32_t inodeId;
    uint32_t entriesPerBlock = fs->super->mEntriesPerBlock;
    DataAndPointersBlocks blocksInfo = getDataAndPointrerBlocks( fs, currentInode );
    DirEntry* dirEntries = readDirEntries( fs, currentInode, blocksInfo.dataBlocks );
    EntriesPosition entriesPos = findPosition( dirEntries, pathTokens[pathIndex], entriesPerBlock );

    if ( pathIndex < pathEnd && entriesPos.directory != -1 ) {
        Inode* childInode = readInode( fs, dirEntries[entriesPos.directory].mInode );
        inodeId = getInodeOfFile( fs, childInode, pathTokens, pathIndex+1, pathEnd );
        delete childInode;
    } else if ( pathIndex == pathEnd && entriesPos.file != -1 ) {
        inodeId = dirEntries[entriesPos.file].mInode;
    } else {
        inodeId = ( uint32_t ) -1;
    }

    delete[] blocksInfo.dataBlocks;
    delete[] blocksInfo.pointerBlocks;
    freeDirEntries( dirEntries, blocksInfo.dataBlocksCount * fs->super->mEntriesPerBlock );
    return inodeId;
}

uint32_t getInodeOfDir( FSInfo* fs, Inode* currentInode, char** pathTokens, uint32_t pathIndex, uint32_t pathEnd )
{
    if ( strcmp( pathTokens[pathIndex], "." ) == 0 ) {
        if ( pathIndex == pathEnd ) {
            return currentInode->mInodeId;
        } else {
            return getInodeOfDir( fs, currentInode, pathTokens, pathIndex+1, pathEnd );
        }
    } else if ( strcmp( pathTokens[pathIndex], ".." ) == 0 ) {
        if ( pathIndex == pathEnd ) {
            return currentInode->mParentId;
        } else {
            Inode *parentInode = readInode( fs, currentInode->mParentId );
            uint32_t inodeId = getInodeOfDir( fs, parentInode, pathTokens, pathIndex + 1, pathEnd );
            delete parentInode;
            return inodeId;
        }
   }

    uint32_t inodeId;
    uint32_t entriesPerBlock = fs->super->mEntriesPerBlock;
    DataAndPointersBlocks blocksInfo = getDataAndPointrerBlocks( fs, currentInode );
    DirEntry* dirEntries = readDirEntries( fs, currentInode, blocksInfo.dataBlocks );
    EntriesPosition entriesPos = findPosition( dirEntries, pathTokens[pathIndex], entriesPerBlock );

    if ( pathIndex == pathEnd && entriesPos.directory != -1 ) {
        inodeId = dirEntries[entriesPos.directory].mInode;
    } else if ( entriesPos.directory != -1 ) {
        Inode* childInode = readInode( fs, dirEntries[entriesPos.directory].mInode );
        inodeId = getInodeOfDir( fs, childInode, pathTokens, pathIndex+1, pathEnd );
        delete childInode;
    } else {
        inodeId = ( uint32_t ) -1;
    }

    delete[] blocksInfo.dataBlocks;
    delete[] blocksInfo.pointerBlocks;
    freeDirEntries( dirEntries, blocksInfo.dataBlocksCount * fs->super->mEntriesPerBlock );
    return inodeId;
}

uint32_t findBlockOfInode( FSInfo* fs, uint32_t inode, uint32_t* offset )
{
    SuperBlock* super = fs->super;
    uint32_t inodeSize = super->mInodeSize;
    uint32_t blockGroup = inode / super->mInodesPerGroup;
    uint32_t inodesPerBlock = super->mInodesPerGroup / super->mInodeBlocksPerGroup;
    uint32_t inodeBlock = ( inode % super->mInodesPerGroup ) / inodesPerBlock;
    uint32_t block = STARTING_BLOCK + blockGroup * super->mBlocksPerGroup +
            MAINTENANCE_BLOCKS_PER_GROUP + inodeBlock;
    uint32_t blockOffset = ( ( inode % super->mInodesPerGroup ) % inodesPerBlock )
            * inodeSize;

    *offset = blockOffset;
    return block;
}

Inode* readInode( FSInfo* fs, uint32_t inodeId )
{
    Inode* inodeData = new Inode;
    SuperBlock* super = fs->super;
    uint32_t blockSize = super->mBlockSize;
    char* buffer = new char[blockSize];
    uint32_t blockOffset = 0;
    uint32_t block = findBlockOfInode( fs, inodeId, &blockOffset );

    memset( buffer, 0, blockSize );
    fs->ioManager->readBlock( block, buffer );
    memcpy( inodeData, buffer+blockOffset, super->mInodeSize );
    delete[] buffer;
    return inodeData;
}

void writeInode( FSInfo* fs, Inode* inodeData )
{
    SuperBlock* super = fs->super;
    uint32_t inodeId = inodeData->mInodeId;
    uint32_t blockSize = super->mBlockSize;
    char* buffer = new char[blockSize];
    uint32_t blockOffset = 0;
    uint32_t block = findBlockOfInode( fs, inodeId, &blockOffset );

    memset( buffer, 0, blockSize );
    fs->ioManager->readBlock( block, buffer );
    memcpy( buffer+blockOffset, inodeData, super->mInodeSize );
    fs->ioManager->writeBlock( block, buffer );

    delete[] buffer;
}

uint32_t getInode( FSInfo* fs )
{
    if ( fs->super->mFreeInodes == 0 ) {
        // If there are no free Inodes
        createBlockGroup( fs );
        uint32_t newBlockGroup = fs->super->mBlockGroups - 1;
        fs->groupStructures[newBlockGroup]->mInodeBitSet->setBit(0);
        fs->groupStructures[newBlockGroup]->mBlockGroupDesc->mFreeInodes -= 1;
        fs->super->mFreeInodes -= 1;
        return newBlockGroup * fs->super->mInodesPerGroup;
    }

    uint32_t blockGroups = fs->super->mBlockGroups;
    for ( uint32_t iGroup = 0; iGroup < blockGroups; iGroup += 1 ) {
        if ( fs->groupStructures[iGroup]->mBlockGroupDesc->mFreeInodes == 0 ) {
            // If there are no free Inodes in this group
            continue;
        }

        BitSet* inodeBitSet = fs->groupStructures[iGroup]->mInodeBitSet;
        uint32_t bitsCount = ( uint32_t ) inodeBitSet->mBitsCount;

        for ( uint32_t iBit = 0; iBit < bitsCount; iBit += 1 ) {
            if ( inodeBitSet->getBit( iBit ) == 0 ) {
                inodeBitSet->setBit( iBit );
                fs->super->mFreeInodes -= 1;
                fs->groupStructures[iGroup]->mBlockGroupDesc->mFreeInodes -= 1;
                return iGroup * fs->super->mInodesPerGroup + iBit;
            }
        }
    }

    return ( uint32_t ) -1;  // can't reach and never should
}

void getBlocks( FSInfo* fs, uint32_t* blocks, uint32_t blocksCount )
{
    uint32_t freeBlocks = fs->super->mFreeBlocks;

    while ( freeBlocks < blocksCount ) {
        // Create block groups to satisfy the need of blocks
        createBlockGroup( fs );
        freeBlocks = fs->super->mFreeBlocks;
    }

    uint32_t neededBlocks = blocksCount;
    uint32_t blockGroups = fs->super->mBlockGroups;
    uint32_t iGroup = blockGroups-1;

    // Start from the newest blockgroup and move backwards
    for ( ; ( iGroup >= 0 ) && neededBlocks > 0; iGroup -= 1 ) {
        BitSet* blockBitSet = fs->groupStructures[iGroup]->mBlockBitSet;
        uint32_t bitsCount = ( uint32_t ) blockBitSet->mBitsCount;

        for ( uint32_t iBit = 0; iBit < bitsCount && neededBlocks > 0; iBit += 1 ) {
            if ( blockBitSet->getBit( iBit ) == 0 ) {
                blockBitSet->setBit( iBit );
                blocks[blocksCount-neededBlocks] = iGroup * fs->super->mBlocksPerGroup + iBit;
                fs->groupStructures[iGroup]->mBlockGroupDesc->mFreeDataBlocks -= 1;
                neededBlocks -= 1;
            }
        }
    }
    fs->super->mFreeBlocks -= blocksCount;
}
