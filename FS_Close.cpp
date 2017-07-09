#include "FS.hpp"

void freeFSInfo( FSInfo* fs )
{
    size_t groupsCount = fs->super->mBlockGroups;

    for ( size_t iGroup = 0; iGroup < groupsCount; iGroup += 1 ) {
        delete fs->groupStructures[iGroup]->mBlockGroupDesc;
        deleteBitSet( fs->groupStructures[iGroup]->mBlockBitSet );
        deleteBitSet( fs->groupStructures[iGroup]->mInodeBitSet );
        delete fs->groupStructures[iGroup];
    }
    delete[] fs->groupStructures;

    fs->ioManager->cleanup();
    delete fs->ioManager;
    delete[] fs->currentDirName;
    delete fs->currentDirInode;
    delete fs->super;
    delete fs;
}

void closeFS( FSInfo* fs )
{
    writeSuperBlock( fs );
    writeBlockGroupsStrucs( fs );
    freeFSInfo( fs );
}