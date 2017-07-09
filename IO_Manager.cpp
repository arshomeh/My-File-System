#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "mylib.hpp"
#include "IO_Manager.hpp"

#define SUCCESS 0
#define SEEK_ERROR -1
#define IO_ERROR -2


IO_Manager::IO_Manager( size_t blockSize )
    : mPathname( NULL ), mFd( -1 ), mBlockSize( blockSize )
{
}

IO_Manager::~IO_Manager()
{
}

void IO_Manager::setBlockSize( size_t blockSize )
{
    mBlockSize = blockSize;
}

size_t IO_Manager::getBlockSize()
{
    return mBlockSize;
}

int IO_Manager::openFile( char* pathname )
{
    if ( ( mFd = open( pathname, O_RDWR | O_CREAT, 0600 ) ) == -1 ) {
        return -1;
    }
    
    mPathname = new char[strlen( pathname ) + 1];
    if ( mPathname == NULL ) {
        return ENOMEM;
    }

    strcpy( mPathname, pathname );
    return SUCCESS;
}

int IO_Manager::fileExists( char* pathname )
{
    int fd = open( pathname, O_CREAT | O_WRONLY | O_EXCL );
    int retVal = -1;

    if ( fd < 0 ) {
        if ( errno == EEXIST ) {
            // The file already exists
            retVal = 0;
        }
    } else {
        remove( pathname );
        retVal = IO_ERROR;
    }

    return retVal;
}

off_t IO_Manager::getFileSize()
{
    if ( mFd != -1 ) {
        return lseek( mFd, 0, SEEK_END );
    }

    return SEEK_ERROR;
}

void IO_Manager::cleanup()
{
    if ( mFd != -1 ) {
        close( mFd );
    }

    if ( mPathname != NULL ) {
        delete[] mPathname;
    }    
}

int IO_Manager::readBlock( size_t iBlock, void* block )
{
    off_t offset = iBlock * mBlockSize;  // position in file

    if ( lseek( mFd, offset, SEEK_SET ) == ( off_t ) -1 ) {
        return SEEK_ERROR;
    }

    if ( mylib::read_all( mFd, ( char* ) block, mBlockSize ) == -1 ) {
        return IO_ERROR;
    }

    return SUCCESS;
}

int IO_Manager::readBlock( size_t iBlock, void* block, size_t size )
{
    off_t offset = iBlock * mBlockSize;  // position in file

    if ( lseek( mFd, offset, SEEK_SET ) == ( off_t ) -1 ) {
        return SEEK_ERROR;
    }

    if ( mylib::read_all( mFd, ( char* ) block, size ) == -1 ) {
        return IO_ERROR;
    }

    return SUCCESS;
}

int IO_Manager::writeBlock( size_t iBlock, void* block )
{
    off_t offset = iBlock * mBlockSize;  // position in file

    if ( lseek( mFd, offset, SEEK_SET ) == ( off_t ) -1 ) {
        return SEEK_ERROR;
    }

    if ( mylib::write_all( mFd, ( char* ) block, mBlockSize ) == -1 ) {
        return IO_ERROR;
    }

    return SUCCESS;
}

int IO_Manager::writeBlock( size_t iBlock, void* block, size_t size )
{
    off_t offset = iBlock * mBlockSize;  // position in file

    if ( lseek( mFd, offset, SEEK_SET ) == ( off_t ) -1 ) {
        return SEEK_ERROR;
    }

    if ( mylib::write_all( mFd, ( char* ) block, size ) == -1 ) {
        return IO_ERROR;
    }

    return SUCCESS;
}
