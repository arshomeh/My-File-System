OBJS = BitSet.o Inodes.o DirEntry.o FS_Cd_Pwd.o FS_Close.o FS_Create.o FS_Import.o FS_Ls.o FS_Mkdir.o FS_Open.o Inode.o Blocks.o SuperBlock.o Tokens.o IO_Manager.o mylib.o BlockGroup.o Dirs.o FS_Export.o FS_Touch.o main.o
SOURCE	= BitSet.cpp DirEntry.cpp FS_Cd_Pwd.cpp FS_Close.cpp FS_Create.cpp FS_Import.cpp FS_Ls.cpp FS_Mkdir.cpp FS_Open.cpp Inode.cpp Inodes.cpp SuperBlock.cpp Tokens.cpp IO_Manager.cpp mylib.cpp BlockGroup.cpp Dirs.cpp FS_Export.cpp FS_Touch.cpp main.cpp
HEADER  = BaseList.hpp BitSet.hpp Commands.hpp DirEntry.hpp FS.hpp Inode.hpp SuperBlock.hpp IO_Manager.hpp mylib.hpp UnorderedList.hpp

OUT = mfs
CC = g++
CFLAGS = -Wall -c -lm -g3
LFLAGS = -Wall -lm -g3

all: $(OBJS)
	$(CC) $(DEBUG) $(LFLAGS) $(OBJS) -o $(OUT)

# compile seperately

IO_Manager.o : IO_Manager.cpp
	$(CC) $(CFLAGS) IO_Manager.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

mylib.o: mylib.cpp
	$(CC) $(CFLAGS) mylib.cpp

BitSet.o: BitSet.cpp
	$(CC) $(CFLAGS) BitSet.cpp

Inodes.o: Inodes.cpp
	$(CC) $(CFLAGS) Inodes.cpp

DirEntry.o: DirEntry.cpp
	$(CC) $(CFLAGS) DirEntry.cpp

FS_Cd_Pwd.o: FS_Cd_Pwd.cpp
	$(CC) $(CFLAGS) FS_Cd_Pwd.cpp

FS_Close.o: FS_Close.cpp
	$(CC) $(CFLAGS) FS_Close.cpp

FS_Create.o: FS_Create.cpp
	$(CC) $(CFLAGS) FS_Create.cpp

FS_Import.o: FS_Import.cpp
	$(CC) $(CFLAGS) FS_Import.cpp

FS_Export.o: FS_Export.cpp
	$(CC) $(CFLAGS) FS_Export.cpp

FS_Ls.o: FS_Ls.cpp
	$(CC) $(CFLAGS) FS_Ls.cpp

FS_Mkdir.o: FS_Mkdir.cpp
	$(CC) $(CFLAGS) FS_Mkdir.cpp

FS_Open.o: FS_Open.cpp
	$(CC) $(CFLAGS) FS_Open.cpp

FS_Touch.o: FS_Touch.cpp
	$(CC) $(CFLAGS) FS_Touch.cpp

Inode.o: Inode.cpp
	$(CC) $(CFLAGS) Inode.cpp

Blocks.o: Blocks.cpp
	$(CC) $(CFLAGS) Blocks.cpp

BlockGroup.o: BlockGroup.cpp
	$(CC) $(CFLAGS) BlockGroup.cpp

Dirs.o: Dirs.cpp
	$(CC) $(CFLAGS) Dirs.cpp

SuperBlock.o: SuperBlock.cpp
	$(CC) $(CFLAGS) SuperBlock.cpp

Tokens.o: Tokens.cpp
	$(CC) $(CFLAGS) Tokens.cpp

# clean house
clean:
	rm -f $(OBJS)

cleanall:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
