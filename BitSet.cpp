#include <string.h>
#include <math.h>
#include <limits.h>
#include "BitSet.hpp"


static const size_t wordSize = sizeof( word_t ) * CHAR_BIT;

/*
 * Returns the index of the word that the requested @bit is part of
 */
size_t wordIndex( size_t bit )
{
    return bit / wordSize;
}

/*
 * Returns the exact position of the @bit in a word
 */
size_t wordOffset( size_t bit )
{
    return bit % wordSize;
}


/*
 * Set, clear and get bit functions
 */

bool BitSet::setBit( size_t bit )
{
    if ( bit >= mBitsCount ) {
        return false;
    }

    mBits[wordIndex( bit )] |= 1 << wordOffset( bit );
    return true;
}

bool BitSet::clearBit( size_t bit )
{
    if ( bit >= mBitsCount ) {
        return false;
    }

    mBits[wordIndex( bit )] &= ~( 1 << wordOffset( bit ) );
    return true;
}

ssize_t BitSet::getBit( size_t bit )
{
    if ( bit >= mBitsCount ) {
        return -1;
    }

    word_t value = mBits[wordIndex( bit )] & ( 1 << wordOffset( bit ) );
    return ( value >> wordOffset( bit ) );
}

void BitSet::setAll()
{
    memset( mBits, 0xff, mWordsCount * sizeof( word_t ) );
}

void BitSet::clearAll()
{
    memset( mBits, 0, mWordsCount * sizeof( word_t ) );
}


/*
 * BitSet allocation and deletion
 */

BitSet* allocBitSet( size_t bitsCount )
{
    BitSet* bitSet = new BitSet();
    bitSet->mBitsCount = bitsCount;
    bitSet->mWordsCount = ( size_t ) ceil( ( double ) ( bitsCount - 1 ) / wordSize );
    bitSet->mBits = new word_t[bitSet->mWordsCount];
    bitSet->clearAll();
    return bitSet;
}

void deleteBitSet( BitSet* bitSet )
{
    delete[] bitSet->mBits;
    delete bitSet;
}

