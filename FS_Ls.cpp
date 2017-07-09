#include <stdio.h>
#include <string.h>
#include "Commands.hpp"
#include "UnorderedList.hpp"

UnorderedList<DirEntry*>* getOrderedListByName( DirEntry* , size_t );
UnorderedList<DirEntry*>* getOrderedListByTime( FSInfo* fs, DirEntry* dirTable, size_t tableSize );
UnorderedList<DirEntry*>* getUnOrdereddList( DirEntry* , size_t );

void lsDirFS ( FSInfo* fs, Inode* inode, int aflag, int rflag, int lflag, int Uflag, int dflag );
void lsFileFS ( FSInfo* fs, Inode* inode, int lflag, char* path );

void getNonHiddenFiles ( UnorderedList<DirEntry*>* );
void getDirFiles( UnorderedList<DirEntry*>* );

UnorderedList<DirEntry*>* getOrderedListByName( DirEntry* dirTable, size_t tableSize )
{
	UnorderedList<DirEntry*>* list = new UnorderedList<DirEntry*>;

	if ( list->empty() ) {
		list->insert( &( dirTable[0] ) );
	}

	for ( int j = 1; j < ( int ) tableSize; j += 1 ) {
		bool isin = false;
		size_t size = list->size();

		for ( size_t i = 0; i < size; i += 1 ) {
			if ( strcmp( dirTable[j].mName, list->get(i)->mName ) < 0 ) {
				list->insert( i, &( dirTable[j] ) );
				isin = true;
				break;
			}
		}
		if ( !isin ) {
			list->insert( &( dirTable[j] ) );
		}
	}
	return list;
 }

UnorderedList<DirEntry*>* getOrderedListByTime( FSInfo* fs, DirEntry* dirTable, size_t tableSize )
{
	UnorderedList<DirEntry*>* list = new UnorderedList<DirEntry*>;

	if ( list->empty() ) {
		list->insert( &( dirTable[0] ) );
	}

	for ( int j = 1; j < ( int ) tableSize; j += 1 ) {
		bool isin = false;
		size_t size = list->size();
		Inode* dirInodeToInsert = readInode( fs, dirTable[j].mInode );
		for ( size_t i = 0; i < size; i += 1 ) {
			Inode* dirInodeInList = readInode( fs, list->get(i)->mInode );
			if ( dirInodeToInsert->mModificationTime > dirInodeInList->mModificationTime ) {
				list->insert( i, &( dirTable[j] ) );
				isin = true;
				delete dirInodeInList;
				break;
			}
			delete dirInodeInList;
		}
		delete dirInodeToInsert;
		if ( !isin ) {
			list->insert( &( dirTable[j] ) );
		}
	}
	return list;
 }

UnorderedList<DirEntry*>* getUnOrdereddList( DirEntry* dirTable, size_t tableSize ) 
{
	UnorderedList<DirEntry*>* list = new UnorderedList<DirEntry*>;

	for ( int j = 1; j < (int) tableSize; j += 1 ) {
			list->insert( &( dirTable[j] ) );
	}

	return list;
}


void getNonHiddenFiles( UnorderedList<DirEntry*>* list ) 
{	
	size_t size = list->size();

	if ( size == 0 ) {
		return;
	}

	for ( int i = 0; i < ( int ) size; i += 1 ) {
		if ( list->get( ( size_t ) i )->mName[0] == '.' ) {
			list->erase( ( size_t ) i );
			i = -1;
			size = list->size();
		}
	}

	return;
}

void getDirFiles( UnorderedList<DirEntry*>* list ) {

	size_t size = list->size();

	if ( size == 0 ) {
		return;
	}

	for ( int i = 0; i < ( int ) size; i += 1 ) {
		if ( list->get( ( size_t ) i )->mType != DIR_TYPE ) {
			list->erase( ( size_t ) i );
			i = -1;
			size = list->size();
		}
	}
	
	return;
}

void commandLs( FSInfo* fs, size_t wordsCounter, char** words )
{
	 if ( fs == NULL ) {
        fprintf( stderr, "Error: No open file system found!\n" );
        return;
    }

	int aflag = 0, rflag = 0, lflag = 0, Uflag = 0, dflag = 0, c;
	optind = 0;
	
	while ( ( c = getopt( ( int ) wordsCounter, words, "arlUd" ) ) != -1 ) {
		switch ( c ) {
			case 'a':
				aflag = 1;
				break;
			case 'r':
				rflag = 1;
				break;
			case 'l':
				lflag = 1;
				break;
			case 'U':
				Uflag = 1;
				break;
			case 'd':
				dflag = 1;
				break;
			default:
				break;
		}
	}

	size_t usefulWordsCounter = 0;
	char** usefulWords;
	usefulWordsCounter = getUsefulWords( optind, words, wordsCounter, &usefulWords );

	if ( usefulWordsCounter == 0 ) {
		Inode* dirInode = NULL;
		dirInode = readInode( fs, fs->currentDirInode->mInodeId );
		lsDirFS (fs, dirInode, aflag, rflag, lflag, Uflag, dflag );
		delete dirInode;
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
		    		Inode* inode = readInode( fs, dirInodeId );
		    		if( usefulWordsCounter > 1 ){
		    			printf( "/%s\n", usefulWords[i] );
		    		}
		    		lsDirFS( fs, inode, aflag, rflag, lflag, Uflag, dflag );
		    		delete inode;
		    	} else {
		    		Inode* inode = readInode( fs, fileInodeId );
		    		lsFileFS(fs, inode, lflag, usefulWords[i] );
		    		delete inode;
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

void lsDirFS ( FSInfo* fs, Inode* inode, int aflag, int rflag, int lflag, int Uflag, int dflag )
{
	DataAndPointersBlocks blocksInfo = getDataAndPointrerBlocks( fs, inode );
	DirEntry* entries = readDirEntries( fs, inode, blocksInfo.dataBlocks );
	uint32_t entriesTableSize = inode->mSize * fs->super->mEntriesPerBlock;
	UnorderedList<DirEntry*>* list;

	if ( Uflag == 1 ) {
		list = getOrderedListByTime( fs, entries, entriesTableSize );
	} else {
		list = getOrderedListByName( entries, entriesTableSize );
	}

	if ( aflag == 0 ) {
		getNonHiddenFiles( list );
	}

	if ( dflag == 1 ) {
		getDirFiles( list );
	}

	if ( lflag == 1 ) {
		size_t size = list->size();

		for ( size_t i = 0; i < size; i+= 1 ) {
			DirEntry* entry = list->get( i );

			if ( entry->mOccupied == 1  ) {
				Inode* fileInode = readInode( fs, list->get(i)->mInode );

				printfInode( fileInode );
				printf( "%s\n", entry->mName );
				delete fileInode;
			}
		}
	} else {
		size_t size = list->size();

		for ( size_t i = 0; i < size; i += 1 ) {
			DirEntry* entry = list->get( i );

			if ( entry->mOccupied == 1  ) {
				printf( "%s\n", entry->mName );
			}
		}
	}

	freeDirEntries( entries, entriesTableSize );
	delete[] blocksInfo.dataBlocks;
	delete[] blocksInfo.pointerBlocks;
	delete list;
}

void lsFileFS ( FSInfo* fs, Inode* inode, int lflag, char* path )
{	
	if ( lflag == 1 ) {
		Inode* fileInode = readInode( fs, inode->mInodeId );
		printf( "%s | ", path );
		printfInode( fileInode );
	} else {
		printf( "%s\n", path );
	}
	return;
}