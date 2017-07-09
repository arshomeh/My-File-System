#ifndef SUPER_BLOCK_HPP
#define SUPER_BLOCK_HPP

#include <stdint.h>

/*
 * The SuperBlock contains vital information
 * about the structure of the file system.
 * It has a fixed size of 512 bytes.
 */

struct SuperBlock {
    uint32_t mInodes;
    uint32_t mBlocks;
    uint32_t mFreeInodes;
    uint32_t mFreeBlocks;
    uint32_t mBlockGroups;

    uint32_t mBlockSize;
    uint32_t mInodeSize;
    uint32_t mEntrySize;
    uint32_t mBlockGroupDescSize;

    uint32_t mMaxFileName;
    uint32_t mMaxFileSize;
    uint32_t mMaxEntriesPerDir;

    uint32_t mInodesPerGroup;
    uint32_t mBlocksPerGroup;
    uint32_t mInodeBlocksPerGroup;
    uint32_t mDataBlocksPerGroup;
    uint32_t mEntriesPerBlock;

    uint32_t padding[111];  // not to be used
};

void printSuperBlock( SuperBlock );

#endif // SUPER_BLOCK_HPP
