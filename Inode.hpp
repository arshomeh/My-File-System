#ifndef INODE_HPP
#define INODE_HPP

#include <stdint.h>

#define DIR_SIZE    4096

#define FILE_TYPE   1
#define LINK_TYPE   2
#define DIR_TYPE    3

#define DATA_BLOCKS 8


/*
 * Inode Structure
 *
 * @mLink
 * * Links to a file share the same Inode
 * -> File type: Links to file
 * -> Dir type: Directory entries
 *
 * @mSize
 * -> File type: Size in bytes
 * -> Dir type: Data blocks occupied by the directory
 */
struct Inode {
    uint32_t mInodeId;
    uint32_t mLinks;
    uint32_t mSize;
    uint32_t mType;
    uint32_t mParentId;
    uint32_t mCreationTime;
    uint32_t mAccessTime;
    uint32_t mModificationTime;
    uint32_t mDataBlocks[DATA_BLOCKS];
};

Inode* createInode( uint32_t inodeId, uint32_t links, uint32_t size,
        uint32_t type, uint32_t parentId, uint32_t time );
void changeInodeSize( Inode* inode, uint32_t size );
void changeInodeAccessTime( Inode* inode, uint32_t time );
void changeInodeModificationTime( Inode* inode, uint32_t time );
struct tm* getTime( uint32_t now );
void printfInode( Inode* inode );

#endif // INODE_HPP
