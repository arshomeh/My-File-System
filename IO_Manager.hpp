#ifndef IO_MANAGER_HPP
#define IO_MANAGER_HPP

class IO_Manager {
public:
    IO_Manager( size_t blockSize );
    virtual ~IO_Manager();

    void setBlockSize( size_t );
    size_t getBlockSize();

    int openFile( char* );
    int fileExists( char* );
    off_t getFileSize();

    int readBlock( size_t, void* );
    int readBlock( size_t, void*, size_t );

    int writeBlock( size_t, void* );
    int writeBlock( size_t, void*, size_t );

    void cleanup();

private:
    char*  mPathname;
    int    mFd;

    size_t mBlockSize;
};

#endif // IO_MAMAGER_HPP
