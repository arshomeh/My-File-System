#ifndef DIR_ENTRY
#define DIR_ENTRY

#include <stdint.h>

struct DirEntry {
    uint32_t mInode;
    char mOccupied;
    char mType;
    char* mName;
};

struct EntriesPosition {
    int file;
    int directory;
    int link;
};

void readEntryFromBlock( DirEntry* pEntry, char* addr, size_t maxNameLength );
void writeEntryToBlock( DirEntry* pEntry, char* addr );

EntriesPosition findPosition ( DirEntry* entries, char* name, uint32_t entriesCount );
void resetEntry( DirEntry* entries, uint32_t maxFilename );
void copyEntry( DirEntry* destination, DirEntry* source, uint32_t maxFilename );
void copyDirEntries( DirEntry* entriesTo, DirEntry* entriesFrom,
        uint32_t entriesToCount, uint32_t entriesFromCount, uint32_t maxFilename );
void removeDirEntry( DirEntry* entries, char* name, uint32_t entriesCount,
        uint32_t maxFilename );
DirEntry* createBlankEntries( uint32_t entriesCount, uint32_t maxFileName );
void freeDirEntries( DirEntry* entries, size_t entriesCount );

#endif // DIR_ENTRY
