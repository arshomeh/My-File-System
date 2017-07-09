#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include "Commands.hpp"


static FSInfo* fs = NULL;

int main( int argc, char const *argv[] )
{
	while ( true ) {
		char* commandLine;

		if ( fs != NULL ) {
			printf( "%s", fs->currentDirName );
		}
		printf( " $ " );

		if ( ( commandLine = commandLineLoop() ) == NULL ) {
			free( commandLine );
			putchar('\n');
			break;
		}

		runCommand( commandLine );
		free( commandLine );
	}

	if ( fs != NULL ) {
		closeFS( fs );
	}

	return 0;
}

char* commandLineLoop()
{
	char* moreCommandLine = NULL;
	char* commandLine = (char* ) malloc ( sizeof( char ) * BUFFERSIZE );
	char* buffer = (char*) malloc ( sizeof( char )* BUFFERSIZE );

	commandLine[0] = '\0';
	size_t commandLineSize = BUFFERSIZE;

	while ( true ){

		if( fgets( buffer, BUFFERSIZE, stdin ) == NULL ){
			free( commandLine );
			free( buffer );
			return NULL;
		}

		size_t lastChar = strlen( buffer ) - 1;
		
		if ( buffer[lastChar] == '\n' ) {
			strcat( commandLine, buffer );
			break;
		} else {
			commandLineSize += BUFFERSIZE;
			moreCommandLine = (char*) realloc( commandLine, sizeof( char ) * commandLineSize );
			if ( moreCommandLine !=NULL ) {
      			 commandLine=moreCommandLine;
      			 strcat( commandLine, buffer );
    	 	}
		}
	}

	free( buffer );
	return commandLine;
}

void runCommand( char* commandLine )
{
	char** words = NULL;
	size_t wordsCounter;
	wordsCounter = tokenize( commandLine, &words, " \n\t", COMMAND );

	if ( wordsCounter == 0 && words != NULL ) {
			delete[] words;
			return;
	}

	char* command = words[0];

	if ( !strcmp( command, "mfs_workwith" ) ) {
		fs = commandWorkWith( fs, wordsCounter, words );
	} else if ( strcmp( command, "mfs_ls" ) == 0 ) { 
		commandLs( fs, wordsCounter, words );
	} else if ( !strcmp( command, "mfs_cd" ) ) {
		commandCd( fs, wordsCounter, words );
	} else if ( !strcmp( command, "mfs_pwd" ) ) {
		commandPwd( fs );
	} else if ( !strcmp( command, "mfs_cp" ) ) {
		commandCp( wordsCounter, words );
	} else if ( !strcmp( command, "mfs_mv" ) ) {
		commandMv( wordsCounter, words );
	} else if ( !strcmp( command, "mfs_rm" ) ) {
		commandRm( wordsCounter, words );
	} else if ( !strcmp( command, "mfs_mkdir" ) ) {
		commandMkdir( fs, wordsCounter, words );
	} else if ( !strcmp( command, "mfs_touch" ) ) {
		commandTouch( fs, wordsCounter, words );
	} else if ( !strcmp( command, "mfs_import" ) ) {
		commandImport( fs, wordsCounter, words );
	} else if ( !strcmp( command, "mfs_export" ) ) {
		commandExport( fs, wordsCounter, words );
	} else if ( !strcmp( command, "mfs_cat" ) ) {
		commandCat( wordsCounter, words );
	} else if ( !strcmp( command, "mfs_create" ) ) {
		commandCreate ( wordsCounter, words );
	} else {
		fprintf( stderr, "Error: Unrecognized command!\n" );
	}

	delete[] words;
}

size_t getUsefulWords( int optInd, char** words, size_t wordsCount, char*** usefulWords )
{
	int usefulWordsCount = 0;
	size_t counter = 0;

	usefulWordsCount = ( int ) wordsCount - optInd;
	if ( usefulWordsCount <= 0 ) {
		return 0;
	}

	(*usefulWords) = new char* [usefulWordsCount];

	for ( size_t i = ( size_t ) optInd; i < wordsCount; i += 1 ) {
		(*usefulWords)[counter] = new char[strlen( words[i] ) + 1];
		strcpy( (*usefulWords)[counter], words[i] );
		counter += 1;
	}

	return ( size_t ) usefulWordsCount;
}


void commandCp( size_t wordsCounter, char** words )
{
	int iflag = 0, lflag = 0, rflag = 0, c;
	char* destination;
	char** sourses;
	char** usefulWords;

	optind = 0;

	while ( ( c = getopt( ( int ) wordsCounter, words, "ilr" ) ) != -1 ) {
		switch ( c ) {
			case 'i':
				iflag = 1;
				break;
			case 'l':
				lflag = 1;
				break;
			case 'r':
				rflag = 1;
				break;
			default:
				break;
		}
	}

	size_t usefulWordsCounter = 0;
	usefulWordsCounter = getUsefulWords( optind, words, wordsCounter, &usefulWords );

	printf( "Options are i: %d l: %d r: %d\n", iflag, lflag, rflag );

	if ( usefulWordsCounter >= 2 ) {
		destination = usefulWords[usefulWordsCounter -1];
		printf( "destination is \n%s\n", destination );
		sourses = new char* [usefulWordsCounter -1];

		for ( size_t i = 0; i < usefulWordsCounter-1; i += 1 ) {
			sourses[i] = usefulWords[i];
		}
		printf( "sourses are \n" );
		for ( size_t i = 0; i < usefulWordsCounter-1; i += 1 ) {
			printf( "%s\n", sourses[i] );
		}
		delete[] sourses;
	}

	for ( size_t i = 0; i < usefulWordsCounter; i += 1 ) {
		delete[] usefulWords[i];
	}

	delete[] usefulWords;
	return;
}

void commandMv( size_t wordsCounter, char** words ) //6
{
	int iflag = 0, c;
	char* destination;
	char** sourses;
	char** usefulWords;

	optind = 0;

	while ( ( c = getopt( ( int ) wordsCounter, words, "i" ) ) != -1 ) {
		switch ( c ) {
			case 'i':
				iflag = 1;
				break;
			default:
				break;
		}
	}

	size_t usefulWordsCounter = 0;
	usefulWordsCounter = getUsefulWords( optind, words, wordsCounter, &usefulWords);

	printf( "Options are i: %d\n", iflag );

	if ( usefulWordsCounter >= 2) {
		destination = usefulWords[usefulWordsCounter -1];
		printf( "destination is \n%s\n", destination );
		sourses = new char* [usefulWordsCounter -1];
		for ( size_t i = 0; i < usefulWordsCounter-1; i+= 1) {
			sourses[i] = usefulWords[i];
		}
		printf( "sourses are \n" );
		for ( size_t i = 0; i < usefulWordsCounter-1; i += 1 ) {
			printf("%s\n", sourses[i] );
		}
		delete[] sourses;
	}

	for ( size_t i = 0; i < usefulWordsCounter; i += 1 ) {
		delete[] usefulWords[i];
	}

	delete[]  usefulWords;
	return;
}

void commandRm( size_t wordsCounter, char** words ) //7
{
	int iflag = 0, rflag = 0, c;
	char** usefulWords;

	optind = 0;

	while ( ( c = getopt( ( int ) wordsCounter, words, "ir" ) ) != -1 ) {
		switch ( c ) {
			case 'i':
				iflag = 1;
				break;
			case 'r':
				rflag = 1;
				break;
			default:
				break;
		}
	}

	size_t usefulWordsCounter = 0;
	usefulWordsCounter = getUsefulWords( optind, words, wordsCounter, &usefulWords );

	printf( "Options are i: %d r:%d\n", iflag, rflag );

	for ( size_t i = 0; i < usefulWordsCounter; i += 1 ) {
		delete[] usefulWords[i];
	}

	delete[]  usefulWords;
	return;
}

void commandTouch( size_t wordsCounter, char** words )
{
	int  aflag = 0, mflag = 0, c;
	char** usefulWords;

	optind = 0;

	while ( ( c = getopt( ( int ) wordsCounter, words, "am" ) ) != -1 ) {
		switch ( c ) {
			case 'a':
				aflag = 1;
				break;
			case 'm':
				mflag = 1;
				break;
			default:
				break;
		}
	}

	size_t usefulWordsCounter = 0;
	usefulWordsCounter = getUsefulWords( optind, words, wordsCounter, &usefulWords );

	printf( "Options are a: %d m:%d\n", aflag, mflag );

	for ( size_t i = 0; i < usefulWordsCounter; i += 1 ) {
		delete[] usefulWords[i];
	}

	delete[]  usefulWords;
	return;
}

void commandCat( size_t wordsCounter, char** words ) //12
{
	int iflag, oflag;
	iflag = oflag = 0;
	int c;
	size_t inputFilesStart = 0;
	size_t outputFileStart = 0;

	optind = 0;

	while ( ( c = getopt( ( int ) wordsCounter, words, "io" ) ) != -1 ) {
		switch ( c ) {
			case 'i':
				iflag += 1;
				inputFilesStart = optind;
				break;
			case 'o':
				oflag += 1;
				outputFileStart = optind;
				break;
			default:
				break;
		}
	}

	bool comdition1 = inputFilesStart > outputFileStart;      // an to -i einai meta to -o
	bool comdition2 = outputFileStart != (wordsCounter - 1 ); // an exei dosei parapanw output files
	bool comdition3 = outputFileStart != wordsCounter; 		  // kai den eimaste se periptosi pou den exoume output file
															  // to con2 kai con3 prepei na isxioun tautoxrona

	if ( iflag != 1 || oflag != 1 || comdition1 || ( comdition2 && comdition3  ) ) {
		return;
	}

	char** inputFiles;
	char* outputFile;
	size_t filesCounter = ( outputFileStart - 1 ) - inputFilesStart;

	if (outputFileStart == wordsCounter) { // an exei dosei -o mono
		outputFile = NULL;
	} else {
		outputFile = words[ wordsCounter-1 ];
		printf( "outputFile \n%s\n----------------\ninputFiles\n", outputFile);
	}

	if ( ( outputFileStart - 1 ) == inputFilesStart ) { // an exei dosei -i mono
		inputFiles = NULL;
	} else {
		inputFiles = new char* [filesCounter];
		for ( size_t i = 3, counter = 0; counter < filesCounter; i += 1, counter += 1 ) {
			inputFiles [counter] = words[i];
		}
		for ( size_t i = 0; i < filesCounter; i += 1 ){
			printf( "%s\n", inputFiles[i] );
		}
		delete[] inputFiles;
	}

	return;
}
