#ifndef FS_HPP
#define FS_HPP

#include <limits.h>
#include <stdint.h>
#include "Inode.hpp"
#include "BitSet.hpp"
#include "DirEntry.hpp"
#include "IO_Manager.hpp"
#include "SuperBlock.hpp"

#define UINT32_SIZE 4
#define CHAR_SIZE   1

#define STARTING_BLOCK                  1
#define MAINTENANCE_BLOCKS_PER_GROUP    3


struct BlockGroupDesc {
    uint32_t mFreeInodes;
    uint32_t mFreeDataBlocks;
};

struct BlockGroupStructures {
    BitSet* mBlockBitSet;
    BitSet* mInodeBitSet;
    BlockGroupDesc* mBlockGroupDesc;
};

struct FSInfo {
    IO_Manager* ioManager;
    SuperBlock* super;
    BlockGroupStructures** groupStructures;
    Inode* currentDirInode;
    char* currentDirName;
};

struct FSOptions {
    uint32_t mBlockSize;
    uint32_t mMaxFileName;
    uint32_t mMaxFileSize;
    uint32_t mMaxEntriesPerDir;
};

struct DataAndPointersBlocks {
    uint32_t* dataBlocks;
    uint32_t dataBlocksCount;
    uint32_t* pointerBlocks;
    uint32_t pointerBlocksCount;
};


SuperBlock* createSuperBlock( FSOptions options );
void readSuperBlock( FSInfo* );
void writeSuperBlock( FSInfo* );
void writeBlockGroup( IO_Manager*, SuperBlock* );
void createBlockGroup( FSInfo* );
void writeBlockGroupsStrucs( FSInfo* );
void readBlockGroupsStrucs( FSInfo* );
uint32_t getInode( FSInfo* fs );
void getBlocks( FSInfo* fs, uint32_t* blocks, uint32_t blocksCount );
uint32_t findBlockOfInode( FSInfo* fs, uint32_t inode, uint32_t* );
Inode* readInode( FSInfo* fs, uint32_t inodeId );
void writeInode( FSInfo* fs, Inode* inodeData );
uint32_t createNewDir( FSInfo* fs, uint32_t parentInodeId );
void writeDirEntries( FSInfo* fs, DirEntry* entries, Inode* inode, uint32_t* dataBlocks );
DirEntry* readDirEntries( FSInfo* fs, Inode* inode, uint32_t* dataBlocks );
uint32_t neededDataBlocksCount( uint32_t fileSize, uint32_t blockSize );
uint32_t neededPointerBlocksCount( uint32_t blockCount, uint32_t blockSize );
void storeDataBlocksIds( FSInfo* fs, Inode* inodeData, uint32_t* dataBlocks,
        uint32_t* pointerBlocks, uint32_t blocksCount, uint32_t pointerBlocksCount );
void copyBlocks( IO_Manager* inputManager, IO_Manager* outputManager,
        uint32_t* inputBlocks, uint32_t* outputBlocks, uint32_t fileSize );
void writeFileBlocks( FSInfo* fs, IO_Manager* fIOManager, Inode* inode, uint32_t* inputBlocks );
uint32_t importFile( FSInfo* fs, char* fileName, uint32_t parentId );
DataAndPointersBlocks getDataAndPointrerBlocks( FSInfo* fs, Inode* inode );
void exportFromFileSystem( FSInfo* fs, char* toFile, Inode* inodeFromFile );
void closeFS( FSInfo* fs );
uint32_t getInodeOfDir( FSInfo* fs, Inode* currentInode, char** pathTokens, uint32_t pathIndex, uint32_t pathEnd );
uint32_t getInodeOfFile( FSInfo* fs, Inode* currentInode, char** pathTokens, uint32_t pathIndex, uint32_t pathEnd );
void addEntryToDir( FSInfo* fs, Inode* inode, DirEntry* entry );

#endif  // FS_HPP