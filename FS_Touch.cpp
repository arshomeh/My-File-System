#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Commands.hpp"

void touch_FS ( FSInfo* fs, uint32_t inodeId, int aflag, int mflag );


void commandTouch( FSInfo* fs, size_t wordsCounter, char** words ) //9
{
	if ( fs == NULL ) {
		fprintf( stderr, "Error: No open file system found!\n" );
		return;
	}

	int aflag = 0, mflag = 0, c;
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
	
	if ( usefulWordsCounter == 0 ) {
		fprintf( stderr, "Error: Missing files\n" );
        return;
	}

	if ( usefulWordsCounter > 0 ) {
		for ( size_t i = 0; i < usefulWordsCounter; i += 1 ) {
			 char** pathTokens = NULL;
		 	 uint32_t tokensCount = 0;
		 	 Inode* dirInode = NULL;
		 	 Inode* fileInode = NULL;
		 	 uint32_t dirInodeId;
		 	 uint32_t fileInodeId = ( uint32_t )  -1;
		     tokensCount = ( uint32_t ) tokenize( usefulWords[i], &pathTokens, "/", PATH );

		    if ( !strcmp( usefulWords[i] , "/" ) ) {
		        dirInodeId = 0;
		    } else {
		        if ( usefulWords[i][0] == '/' ) {
		            dirInode = readInode( fs, 0 );
		        } else {
		            dirInode = readInode( fs, fs->currentDirInode->mInodeId );
		        }
		        dirInodeId = getInodeOfDir( fs, dirInode, pathTokens, 0, tokensCount-1 );
		        fileInodeId = getInodeOfFile( fs, dirInode, pathTokens, 0, tokensCount-1 );
		        delete dirInode;
		        delete fileInode;
		    }

		    if ( dirInodeId == ( uint32_t ) -1 && fileInodeId == ( uint32_t ) -1 ) {
		         fprintf( stderr, "Error: Invalid path %s\n", usefulWords[i]  );
		    } else {
		    	if ( dirInodeId != ( uint32_t ) -1 ) {
		    		touch_FS( fs, dirInodeId, aflag, mflag );
		    	} else {
		    		touch_FS( fs, fileInodeId, aflag, mflag );
		    	}
		    }
		    delete[] pathTokens;
		}

		for ( size_t i = 0; i < usefulWordsCounter; i += 1 ) {
			delete[] usefulWords[i];
		}

		delete[]  usefulWords;
	}
	return;
}

void touch_FS ( FSInfo* fs, uint32_t inodeId, int aflag, int mflag )
{
	Inode* inode = readInode( fs, inodeId );
	uint32_t ttime = ( uint32_t ) time( NULL );

	if ( aflag == 0 && mflag == 0 ) {
		inode->mModificationTime = ttime;
		inode->mAccessTime = ttime;
	} else if ( aflag == 1 && mflag == 0 ) {
		inode->mAccessTime = ttime;
	} else if ( aflag == 0 && mflag == 1 ) {
		inode->mModificationTime = ttime;
	} else if ( aflag == 1 && mflag == 1 ) {
		inode->mModificationTime = ttime;
		inode->mAccessTime = ttime;
	}

	writeInode( fs, inode );

	delete inode;
	return;
}