#ifndef BITSET_HPP
#define BITSET_HPP

#include <stdint.h>
#include <unistd.h>


// The BitSet is constructed from 32 bit words
typedef uint32_t word_t;

struct BitSet {
    bool setBit( size_t );
    bool clearBit( size_t );
    ssize_t getBit( size_t );

    void setAll();
    void clearAll();

    word_t* mBits;
    size_t mBitsCount;
    size_t mWordsCount;
};

BitSet* allocBitSet( size_t bitsCount );
void deleteBitSet( BitSet* bitSet );

#endif // BITSET_HPP

