#include <stdio.h>
#include <unistd.h>
#include "mylib.hpp"

ssize_t mylib::read_all( int fd, char* buffer, size_t bufferLength )
{
    size_t received = 0;   // total bytes received
    ssize_t batch = 0;      // bytes received from one read system call

    // Try to read @bufferLength bytes
    for ( received = 0; received < bufferLength; received += batch ) {
        batch = read( fd, &buffer[received], bufferLength - received );
        if ( batch == -1 ) {
            return -1;  // error in read
        } else if (batch == 0) {
            break;      // no more data to read
        }
    }
    return received;
}

ssize_t mylib::write_all( int fd, char* buffer, size_t bufferLength )
{
    size_t sent = 0;         // total bytes sent
    ssize_t batch = 0;       // bytes sent with one write system call

    // Try to write @bufferLength bytes
    for ( sent = 0 ; sent < bufferLength ; sent += batch ) {
        batch = write( fd, &buffer[sent], bufferLength - sent );
        if ( batch == -1 ) {
            return -1;  // error in write
        }
    }
    return sent;
}

unsigned int mylib::countDigits( unsigned long long num )
{
    unsigned int digits = 1;

    while ( num /= 10 ) {
        digits += 1;
    }

    return digits;
}
