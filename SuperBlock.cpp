#include <stdio.h>
#include <string.h>
#include "FS.hpp"

/*
 * Write SuperBlocks to the first block of FS
 */
void writeSuperBlock( FSInfo* fs )
{
    IO_Manager* ioManager = fs->ioManager;
    SuperBlock* super = fs->super;
    uint32_t blockSize = super->mBlockSize;
    char* buffer = new char[blockSize];

    memset( buffer, 0, blockSize );
    memcpy( buffer, super, sizeof( SuperBlock ) );
    ioManager->writeBlock( 0, buffer );
    delete[] buffer;
}

/*
 * Read SuperBlocks from the first block of FS
 */
void readSuperBlock( FSInfo* fs )
{
    SuperBlock* super = new SuperBlock;

    fs->ioManager->readBlock( 0, super, sizeof( SuperBlock ) );
    fs->super = super;
}

/*
 * Create SuperBlock struct based on given FS options
 */
SuperBlock* createSuperBlock( FSOptions options )
{
    uint32_t blockSize = options.mBlockSize;
    SuperBlock* super = new SuperBlock;

    super->mInodes = 0;
    super->mBlocks = 1;
    super->mFreeInodes = 0;
    super->mFreeBlocks = 0;
    super->mBlockGroups = 0;

    super->mBlockSize = blockSize;
    super->mInodeSize = sizeof( Inode );
    super->mBlockGroupDescSize = sizeof( BlockGroupDesc );
    super->mMaxFileName = options.mMaxFileName;
    super->mMaxFileSize = options.mMaxFileSize;
    super->mMaxEntriesPerDir = options.mMaxEntriesPerDir;
    super->mInodesPerGroup = blockSize;
    super->mBlocksPerGroup = blockSize * CHAR_BIT;
    super->mInodeBlocksPerGroup = super->mInodeSize;
    super->mDataBlocksPerGroup = super->mBlocksPerGroup -
            (MAINTENANCE_BLOCKS_PER_GROUP + super->mInodeBlocksPerGroup );
    memset( super->padding, 0, sizeof( super->padding ) );

    super->mEntrySize = UINT32_SIZE + 2 * CHAR_SIZE + super->mMaxFileName;
    super->mEntriesPerBlock = super->mBlockSize / ( super->mEntrySize + 1 );

    return super;
}

/*
 * Debugging Purpose
 */
void printSuperBlock( SuperBlock super )
{
    printf( "Total inodes: %d\n", super.mInodes );
    printf( "Total blocks: %d\n", super.mBlocks );
    printf( "Total blockgroups: %d\n", super.mBlockGroups );
    printf( "\n" );
    printf( "Free inodes: %d\n", super.mFreeInodes );
    printf( "Free blocks: %d\n", super.mFreeBlocks );
    printf( "\n" );
    printf( "Block size: %d\n", super.mBlockSize );
    printf( "Inode size: %d\n", super.mInodeSize );
    printf( "Emtry size: %d\n", super.mEntrySize );
    printf( "Blockgroup descriptor size: %d\n", super.mBlockGroupDescSize );
    printf( "\n" );
    printf( "Inodes per group: %d\n", super.mInodesPerGroup );
    printf( "Blocks per group: %d\n", super.mBlocksPerGroup );
    printf( "Inode blocks per group: %d\n", super.mInodeBlocksPerGroup );
    printf( "Data blocks per group: %d\n", super.mDataBlocksPerGroup );
    printf( "Entries per block: %d\n", super.mEntriesPerBlock );
    printf( "\n" );
    printf( "Max file name: %d\n", super.mMaxFileName );
    printf( "Max file size: %d\n", super.mMaxFileSize );
    printf( "Max entries per directory: %d\n", super.mMaxEntriesPerDir );
    printf( "\n" );
}
