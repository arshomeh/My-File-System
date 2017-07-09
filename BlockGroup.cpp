#include <stdio.h>
#include <string.h>
#include "BitSet.hpp"
#include "FS.hpp"

BitSet* createBlocksBitSet( SuperBlock* super )
{
    size_t bitsCount = super->mBlocksPerGroup;
    BitSet* bitSet = allocBitSet( bitsCount );

    bitSet->clearAll();
    size_t usedBlocksCount = super->mBlocksPerGroup - super->mDataBlocksPerGroup;

    for ( size_t i = 0; i < usedBlocksCount; i += 1 ) {
        bitSet->setBit( i );
    }

    return bitSet;
}

BitSet* createInodesBitSet( SuperBlock* super )
{
    size_t bitsCount = super->mBlockSize;
    BitSet* bitSet = allocBitSet( bitsCount );

    bitSet->clearAll();
    return bitSet;
}


void readBlockGroupsStrucs( FSInfo* fs )
{
    uint32_t blockSize = fs->super->mBlockSize;
    uint32_t blockOffset = STARTING_BLOCK;
    uint32_t blocksPerGroup = fs->super->mBlocksPerGroup;
    uint32_t inodesPerGroup = fs->super->mInodesPerGroup;
    size_t blockGroupDescSize = sizeof( BlockGroupDesc );
    size_t groupsCount = fs->super->mBlockGroups;
    char* buffer = new char[blockSize];
    BlockGroupStructures** structs = new BlockGroupStructures*[groupsCount];

    for ( size_t iGroup = 0; iGroup < groupsCount; iGroup += 1 ) {
        structs[iGroup] = new BlockGroupStructures;

        structs[iGroup]->mBlockGroupDesc = new BlockGroupDesc;
        fs->ioManager->readBlock( blockOffset, buffer );
        memcpy( structs[iGroup]->mBlockGroupDesc, buffer, blockGroupDescSize );

        structs[iGroup]->mBlockBitSet = allocBitSet( blocksPerGroup );
        fs->ioManager->readBlock( blockOffset+1, buffer );
        memcpy( structs[iGroup]->mBlockBitSet->mBits, buffer, blockSize );

        structs[iGroup]->mInodeBitSet = allocBitSet( inodesPerGroup );
        fs->ioManager->readBlock( blockOffset+2, buffer );
        memcpy( structs[iGroup]->mInodeBitSet->mBits, buffer, blockSize / CHAR_BIT );

        blockOffset += fs->super->mBlocksPerGroup;
    }

    delete[] buffer;
    fs->groupStructures = structs;
}

void writeBlockGroupsStrucs( FSInfo* fs )
{
    uint32_t blockOffset = STARTING_BLOCK;
    uint32_t blockSize = fs->super->mBlockSize;
    size_t groupsCount = fs->super->mBlockGroups;
    char* buffer = new char[blockSize];
    BlockGroupStructures** structs = fs->groupStructures;

    for ( size_t iGroup = 0; iGroup < groupsCount; iGroup += 1 ) {
        memset( buffer, 0, blockSize );
        memcpy( buffer, structs[iGroup]->mBlockGroupDesc, sizeof( BlockGroupDesc ) );
        fs->ioManager->writeBlock( blockOffset, buffer );

        memset( buffer, 0, blockSize );
        memcpy( buffer, structs[iGroup]->mBlockBitSet->mBits, blockSize );
        fs->ioManager->writeBlock( blockOffset+1, buffer );

        memset( buffer, 0, blockSize );
        memcpy( buffer, structs[iGroup]->mInodeBitSet->mBits, blockSize / CHAR_BIT );
        fs->ioManager->writeBlock( blockOffset+2, buffer );

        blockOffset += fs->super->mBlocksPerGroup;
    }

    delete[] buffer;
}


void writeBlockGroup( IO_Manager* ioManager, SuperBlock* super )
{
    size_t blockSize = super->mBlockSize;
    size_t currentBlock = super->mBlocks;
    size_t blocksToWrite = super->mBlocksPerGroup;
    char* buffer = new char[blockSize];

    memset( buffer, 0, blockSize );

    for ( size_t iBlock = 0; iBlock < blocksToWrite; iBlock += 1, currentBlock += 1 ) {
        ioManager->writeBlock( currentBlock, buffer );
    }

    delete[] buffer;
}

void createBlockGroup( FSInfo* fs )
{
    SuperBlock* super = fs->super;
    IO_Manager* ioManager = fs->ioManager;
    BlockGroupDesc* blockGroupDesc = new BlockGroupDesc;
    BlockGroupStructures* newGroupStructs = new BlockGroupStructures;

    // Write block group to File System
    writeBlockGroup( ioManager, super );

    // Create block group descriptor table
    blockGroupDesc->mFreeInodes = super->mInodesPerGroup;
    blockGroupDesc->mFreeDataBlocks = super->mDataBlocksPerGroup;
    newGroupStructs->mBlockGroupDesc = blockGroupDesc;

    // Create block group block bitset
    BitSet* blocksBitSet = createBlocksBitSet( super );
    newGroupStructs->mBlockBitSet = blocksBitSet;

    // Create block group inode bitset
    BitSet* inodeBitSet = createInodesBitSet( super );
    newGroupStructs->mInodeBitSet = inodeBitSet;

    // Update SuperBlock
    super->mInodes += super->mInodesPerGroup;
    super->mBlocks += super->mBlocksPerGroup;
    super->mFreeInodes += super->mInodesPerGroup;
    super->mFreeBlocks += super->mDataBlocksPerGroup;
    super->mBlockGroups += 1;

    // Add block group structures to the existing ones
    size_t blockGroups = super->mBlockGroups;
    BlockGroupStructures** groupStructs = new BlockGroupStructures*[blockGroups];
    for ( size_t i = 0; i < blockGroups - 1; i += 1 ) {
        groupStructs[i] = fs->groupStructures[i];
    }
    groupStructs[blockGroups-1] = newGroupStructs;

    if ( fs->groupStructures != NULL ) {
        delete[] fs->groupStructures;
    }

    fs->groupStructures = groupStructs;
}
