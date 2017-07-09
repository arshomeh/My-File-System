#include <string.h>
#include "Commands.hpp"

size_t countTokensInPath( char* commandLine );
size_t countTokensInCommand( char* commandLine );

size_t tokenize( char* commandLine, char*** words, const char* delimiter, int flag )
{
    size_t wordsCounter = 0;

    if ( flag == COMMAND ) {
        wordsCounter = countTokensInCommand( commandLine );
    } else if ( flag == PATH ) {
        wordsCounter = countTokensInPath( commandLine );
    }

    if ( wordsCounter == 0 ) {
        return 0;
    }

    (*words) = new char*[wordsCounter];

    if ( (*words) == NULL ) {
        return 0;
    }

    (*words)[0] = strtok( commandLine, delimiter );

    for ( size_t i = 1; i < wordsCounter; i += 1 ) {
        (*words)[i] = strtok( NULL, delimiter );
    }

    return wordsCounter;
}

size_t countTokensInCommand( char* commandLine )
{
    size_t wordsCounter = 0;

    for ( char* c = commandLine; *c != '\0'; c += 1 ) {
        if ( *c != ' ' && *c != '\t' && *c != '\n' ) {
            wordsCounter += 1;
            c += 1;
            while ( *c != ' ' && *c != '\t' && *c != '\n' ) {
                c += 1;
            }
        }

        while ( *c == ' ' || *c == '\t' ) {
            c += 1;
            if ( *c != ' ' && *c != '\t' && *c != '\n' ) {
                c -= 1;
                break;
            }
        }
    }
    return wordsCounter;
}

size_t countTokensInPath( char* commandLine )
{
    size_t wordsCounter = 0;

    for ( char* c = commandLine; *c != '\0'; c += 1 ) {
        if ( *c != '/' && *c != '\0' ) {
            wordsCounter += 1;
            c += 1;
            while ( *c != '/' && *c != '\0' ) {
                c += 1;
            }
        }

        while ( *c == '/' && *c != '\0' ) {
            c += 1;
            if ( *c != '/' && *c != '\0' ) {
                c -= 1;
                break;
            }
        }
        if (  *c == '\0' ) {
            break;
        }
    }

    return wordsCounter;
}