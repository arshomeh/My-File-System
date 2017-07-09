#ifndef MY_LIB_HPP
#define	MY_LIB_HPP

#include <sys/types.h>

namespace mylib {

    // Returns the minimum of @val1 and @val2
    template <typename T>
    T min( T val1, T val2 )
    {
        return ( val1 < val2 ? val1 : val2 );
    }

    // Swaps @a and @b
    template<typename T>
    void swap(T &a, T &b)
    {
        T c(a);
        a = b;
        b = c;
    }

    // Reads at most @bufferLength bytes from @fd and store them in @buffer
    ssize_t read_all( int fd, char* buffer, size_t bufferLength );
    
    // Writes 'bufferLength' bytes from 'buffer' to 'fd'
    ssize_t write_all( int fd, char* buffer, size_t bufferLength );

    // Returns the number of digits of @num
    unsigned int countDigits( unsigned long long num );

}


#endif	/* MY_LIB_HPP */
