#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "IO_Manager.hpp"
#include "FS.hpp"


uint32_t neededDataBlocksCount( uint32_t fileSize, uint32_t blockSize )
{
	return ( uint32_t ) ceil( ( double ) fileSize / blockSize );
}

uint32_t neededPointerBlocksCount( uint32_t blockCount, uint32_t blockSize )
{
	uint32_t pointersPerBlock = blockSize / UINT32_SIZE - 1;
	int restBlocks = blockCount - ( DATA_BLOCKS - 1 );

	if ( restBlocks <= 0 ) {
		return 0;
	}

	double dPointerBlocks = ( double ) ( restBlocks ) / pointersPerBlock;
	return ( uint32_t ) ceil( dPointerBlocks );

}

DataAndPointersBlocks getDataAndPointrerBlocks( FSInfo* fs, Inode* inode )
{
	DataAndPointersBlocks blocksInfo = { NULL, 0, NULL, 0 };
	IO_Manager* ioManager = fs->ioManager;
	uint32_t i = 0, j = 0, counter = 0;
	uint32_t blockSize = fs->super->mBlockSize;
	uint32_t fileSize = ( inode->mType != DIR_TYPE ) ?
			inode->mSize : inode->mSize * blockSize;
	uint32_t dataBlocksCount = blocksInfo.dataBlocksCount =
			neededDataBlocksCount( fileSize, blockSize );
	uint32_t pointerBlocksCount = blocksInfo.pointerBlocksCount =
			neededPointerBlocksCount( dataBlocksCount, blockSize );
	uint32_t remDataBlocksCount = dataBlocksCount;
	uint32_t bufferSize = blockSize / UINT32_SIZE;
	uint32_t pointersBuffer[bufferSize];
	uint32_t* dataBlocks = blocksInfo.dataBlocks = new uint32_t[dataBlocksCount];

	for ( i = 0; i < DATA_BLOCKS - 1 && i < remDataBlocksCount; i += 1 ){
		dataBlocks[i] = inode->mDataBlocks[i];
		counter += 1;

	}
	remDataBlocksCount -= i;
	if ( remDataBlocksCount <= 0 ){
		return blocksInfo;
	}

	uint32_t* pointerBlocks = blocksInfo.pointerBlocks = new uint32_t[pointerBlocksCount];
	pointerBlocks[0] = inode->mDataBlocks[i] ;

	for ( j = 0; j < pointerBlocksCount; j++ ) {
		ioManager->readBlock( pointerBlocks[j], pointersBuffer );
		for ( i = 0; i < bufferSize-1 && i < remDataBlocksCount; i += 1 ) {
			dataBlocks[counter] = pointersBuffer[i];
			counter += 1;
		}
		remDataBlocksCount -= i;
		if ( remDataBlocksCount > 0 ) {
			pointerBlocks[j+1] = pointersBuffer[bufferSize-1];
		}
		memset( pointersBuffer, 0, blockSize );
	}
	return blocksInfo;
}

void storeDataBlocksIds( FSInfo* fs, Inode* inodeData, uint32_t* dataBlocks,
		uint32_t* pointerBlocks, uint32_t blocksCount, uint32_t pointerBlocksCount )
{
	uint32_t i = 0, j = 0, counter = 0;
	IO_Manager* ioManager = fs->ioManager;
	uint32_t blockSize = fs->super->mBlockSize;
	uint32_t bufferSize = blockSize / UINT32_SIZE;
	uint32_t remainingBlocksCount = blocksCount;
	uint32_t* pointersBuffer = new uint32_t[bufferSize];

	memset( pointersBuffer, 0, blockSize );

	for ( i = 0; i < ( DATA_BLOCKS - 1 ) && i < remainingBlocksCount; i += 1 ) {
		inodeData->mDataBlocks[i] = dataBlocks[i];
		counter += 1;
	}

	remainingBlocksCount -= i;
	if ( remainingBlocksCount == 0 ) {
		writeInode( fs, inodeData );
		delete[] pointersBuffer;
		return;
	}

	inodeData->mDataBlocks[DATA_BLOCKS - 1] = pointerBlocks[0];
	writeInode( fs, inodeData );

	for ( i = 0; i < pointerBlocksCount; i += 1 ) {
		for ( j = 0; j < ( bufferSize - 1 ) && remainingBlocksCount > 0; j += 1 ) {
			pointersBuffer[j] = dataBlocks[counter];
			counter += 1;
			remainingBlocksCount -= 1;
		}

		if ( remainingBlocksCount > 0 ) {
			pointersBuffer[bufferSize-1] = pointerBlocks[i+1];
		}
		ioManager->writeBlock( pointerBlocks[i], pointersBuffer );
		memset( pointersBuffer, 0, blockSize );
	}

	delete[] pointersBuffer;
}

void copyBlocks( IO_Manager* inputManager, IO_Manager* outputManager,
		uint32_t* inputBlocks, uint32_t* outputBlocks, uint32_t fileSize )
{
    uint32_t blockSize = ( uint32_t ) inputManager->getBlockSize();
	uint32_t blocksCount = neededDataBlocksCount( fileSize, blockSize );
	char* buffer = new char[blockSize];
	memset( buffer, 0, blockSize );

	for ( size_t i = 0; i < blocksCount - 1; i += 1) {
		inputManager->readBlock( inputBlocks[i], buffer );
		outputManager->writeBlock( outputBlocks[i], buffer );
	}

	uint32_t size = fileSize - ( blockSize * ( blocksCount - 1 ) );
	inputManager->readBlock( inputBlocks[blocksCount-1], buffer, size );
	outputManager->writeBlock( outputBlocks[blocksCount-1], buffer, size );

	delete[] buffer;
}

/*
 * Write file's blocks to file system.
 */
void writeFileBlocks( FSInfo* fs, IO_Manager* fIOManager, Inode* inode, uint32_t* inputBlocks )
{
    uint32_t blockSize = fs->super->mBlockSize;
	uint32_t fileSize = ( uint32_t ) fIOManager->getFileSize();
    uint32_t dataBlocksCount = neededDataBlocksCount( fileSize, blockSize );
	uint32_t pointerBlocksCount = neededPointerBlocksCount( dataBlocksCount, blockSize );
	uint32_t* dataBlocks = new uint32_t[dataBlocksCount];
	uint32_t* pointerBlocks = new uint32_t[pointerBlocksCount];
	IO_Manager* fsIOManager = fs->ioManager;

	getBlocks( fs, dataBlocks, dataBlocksCount );
	getBlocks( fs, pointerBlocks, pointerBlocksCount );
	storeDataBlocksIds( fs, inode, dataBlocks, pointerBlocks,
			dataBlocksCount, pointerBlocksCount );

	if ( inputBlocks != NULL ) {
		// Write only if there is anything to write
		copyBlocks( fIOManager, fsIOManager, inputBlocks, dataBlocks, fileSize );
	}

	delete[] pointerBlocks;
	delete[] dataBlocks;
}

//void writeFileBlocks( FSInfo* fs, Inode* from, Inode* to, uint32_t* inputBlocks )
//{
//
//}
