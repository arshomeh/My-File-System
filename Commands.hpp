#ifndef COMMANDS_HPP
#define	COMMANDS_HPP

#include "FS.hpp"


/*defines*/
#define BUFFERSIZE 256
#define PATH 2
#define COMMAND 1
#define STARTPOINT 1

/* functions */
size_t tokenize( char *, char ***, const char*, int );
char* concatPathTokens( char** tokens, uint32_t tokensCount );
void runCommand( char * );
char* commandLineLoop();
size_t getUsefulWords( int, char**, size_t, char*** );
/*functions end*/

/*commands functions */
FSInfo* commandWorkWith( FSInfo*, size_t, char** );
void commandLs 		( FSInfo*, size_t, char** );
void commandCd      ( FSInfo* fs, size_t wordsCounter, char** words );
void commandPwd		( FSInfo* );
void commandCp		( size_t, char** );
void commandMv		( size_t, char** );
void commandRm 		( size_t, char** );
void commandMkdir	( FSInfo*, size_t , char** );
void commandTouch	( FSInfo*, size_t, char** );
void commandImport	( FSInfo*, size_t, char** );
void commandExport	( FSInfo*, size_t, char** );
void commandCat 	( size_t, char** );
void commandCreate	( size_t , char** );
/*end commads functions*/

#endif /* COMMANDS_HPP */
