//Filename:                fileSystem.c 
//Team Members:            Divya Machenahalli Lokesh and Siddrameshwar Kadagad
//UTD_ID:                  2021496022 and 2021491485
//NetID:                   dxm190018 and sxk190071
//Class:                   CS 5348.001
//Project:                 project3
/***********************************************************************
 
 
 
 This program allows user to do two things: 
   1. initfs: Initilizes the file system and redesigning the Unix file system to accept large 
      files of up tp 4GB, expands the free array to 152 elements, expands the i-node array to 
      200 elemnts, doubles the i-node size to 64 bytes and other new features as well.
   2. Quit: save all work and exit the program.
   
 User Input:
     - initfs (file path) (# of total system blocks) (# of System i-nodes)
     - q
     
 File name is limited to 14 characters.
 ***********************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>


#define FREE_SIZE 152  
#define I_SIZE 200
#define BLOCK_SIZE 1024    
#define ADDR_SIZE 11
#define INPUT_SIZE 256


// Superblock Structure

typedef struct {
  unsigned short isize;
  unsigned short fsize;
  unsigned short nfree;
  unsigned short ninode;
  unsigned short inode[I_SIZE];
  char flock;
  char ilock;
  unsigned short fmod;
  unsigned short time[2];
  unsigned int free[FREE_SIZE]; // to make size 1024.
} superblock_type;


// Declaring superblock
superblock_type superBlock;

// I-Node Structure

//Do we really use size in inode??
//What should we do about modtime and actime?
//How are they initializing root_inode?
typedef struct {
unsigned short flags;
unsigned short nlinks;
unsigned short uid;
unsigned short gid;
unsigned int size;
unsigned int addr[ADDR_SIZE];
unsigned short actime[2];
unsigned short modtime[2];
} inode_type; 


// Declaring I-Node
inode_type inode;

//this inode is inode number or inode address?
typedef struct {
  unsigned short inode;
  unsigned char filename[14];
} dir_type;

// Declaring root
dir_type root; //each entry in root directory will hold max of 14characters long file name and 2byes inode address

int fileDescriptor ;		//file descriptor 
const unsigned short inode_alloc_flag = 0100000; 
const unsigned short dir_flag = 040000; 
const unsigned short dir_large_file = 010000; 
const unsigned short dir_small_file = 010000; 
const unsigned short dir_access_rights = 000777; // User, Group, & World have all access privileges 
const unsigned short file_flag = 000000; 
const unsigned short small_file_flag = 000000; 
const unsigned short large_file_flag = 010000; 
const unsigned short file_access_rights = 000777;
const unsigned short INODE_SIZE = 64; // inode has been doubled

int cur_dir_inode_num;    //Hopefully keeps track of current directory.
char cur_dir[1000];
int dir_index=0;

int initfs(char* path, unsigned int total_blcks,unsigned int total_inodes);
void add_block_to_free_list( int blocknumber , unsigned int *empty_buffer );
void create_root();
int getInode();
inode_type addFreeBlockToAddr(inode_type inode, int no_free_blocks);
int preInitialization();
int getFreeBlock();
void printInode(inode_type inode_);
void printSuperBlock();
inode_type getInodeWithNum(int num);
void findLocWhereCurrDirFileShouldGo(int cur_dir_inode_num);
void printFirst5RootDirectories();
void ls();
void mkdir();
int inodeOfFile(char *file_name); 
void printContentOfFile(int inode_num);
void cd();
void rmFile();
void rmDir();
void addToFreeBlockArrayWithoutBuffer(int block_number);
void freeInode(int inode_num);
void addInodeToFreeArray(int inode_num);
int getinodeNumberAfterDeleteFile();
void printBlock(int blockNum);
void printPwd();

int main() {
 
  char input[INPUT_SIZE];
  char *splitter;
  unsigned int numBlocks = 0, numInodes = 0;
  char *filepath;
  printf("Size of super block = %d , size of i-node = %d\n",(int)sizeof(superBlock), (int)sizeof(inode));
  printf("Enter command:\n");
  
  // while adding a new file or dir check if the file/dir exist 
  // check if the inode val is 0. Then set that val to inode number.  
  
  // Method that we can use  -> ftell -> current position of pointer
  // If open function has not been used it will mess things up. Catch that exception. Have a method or if else condition to take care of it. 
  
  //Change all the address types to long or better
  //Check the flags. Set the flags. 
  //We have to keep track of current directory. How are we doing it?
  //Inode address will be Inode_num-1
  //Fix the compiler errors first of all. 
  //Can we copy one Struct into another just by Assigning?
  //Need a fuction to print the contents of super block
  //Also need a function to print any block for that matter
  //fsize if shot but we are not using it anyway
  
  //In large file how many data_block number can be fit in a 1 data_block?
  
  //We can print the number of blocks were used during an operation. also when we add it back to free array we can keep track of the numbers. 
  
  //We shouldn't fill the addr array with free block. If the directory or the file requires it only then we should add the free blocks. 
  
  //When we deallocate an inode we need to set the first bit to 0
  
  //Questions
  //Will the cursor move after the read command. 
  //
  
  //if ninode is full we will add the free inodes to the queue. 
  
  //If at any time, fileDescriptor < 3 then it should print out that scenario. 
  
  // Check if Dir/file/special_file
  /*
    if((flags & 060000) == 040000){
        printf("flags- inode is a dir_access_rights\n");
    } else if((flags & 060000) == 0) {
        printf("flags- inode is a file\n");
    } else {
        printf("flags -inode is a special file\n");
    }
    
    // Checks if large or small
    if((flags & 010000) == 010000) {
        printf("inode points to large file\n");
    } else {
        printf("inode points to small file\n");
    }
	*/
  while(1) {
		  
		// Take commands from user till the input is 'q' 
		
		//Takes input as string until it encounters a new line character. 
		scanf(" %[^\n]s", input);
		
		// Split the user input string into words which are seperated by space ' '
		splitter = strtok(input," ");
		
		// if the first word of the input command is initfs, execute this if block
		if(strcmp(splitter, "initfs") == 0){
		
			preInitialization();
			splitter = NULL;
						   
		} else if (strcmp(splitter, "q") == 0) {
			//Exit the program when the input command is 'q'
			//Before quitting the program setting the cursor offset by BLOCK_SIZE to 1st Block which is super block
		   lseek(fileDescriptor, BLOCK_SIZE, 0);
		   // saving the contents of superBlock 
		   write(fileDescriptor, &superBlock, BLOCK_SIZE);
		   //Should we close the file here? Because it wasn't closed in the given code
		   close(fileDescriptor);
		   return 0;
		 
		} else if(strcmp(splitter, "cpin") == 0) {
			char *externalFile = strtok(NULL, " ");
			char *v6_file = strtok(NULL, " ");
			
			char *dirpathcpin = v6_file;
			int index1=0;
			char cpinfileName[100];
			inode_type inode_of_dir;
			int inode_num_of_dir;
			int dir_error_flag=0;
			if (*dirpathcpin++=='/')
			{
				int temp=cur_dir_inode_num;
				cur_dir_inode_num=1;
				while(*dirpathcpin){
					cpinfileName[index1]=*dirpathcpin++;
					index1++;
					if(cpinfileName[index1-1]=='/')
					{
						cpinfileName[index1-1]='\0';
						inode_num_of_dir = inodeOfFile(cpinfileName);
						inode_of_dir = getInodeWithNum(inode_num_of_dir);
						if((inode_of_dir.flags & 060000) == 040000) {
						cur_dir_inode_num = inode_num_of_dir;
						}
						else {
								printf("invalid direcotry name\n");
								cur_dir_inode_num=temp;
								dir_error_flag=1;
								break;
							}	
						while(index1>=0)
						{
							cpinfileName[index1]='\0';
							index1--;
						}
						index1=0;	
					}
				}
				cur_dir_inode_num=temp;
			
			}
			else{
				inode_num_of_dir=cur_dir_inode_num;
				*dirpathcpin--;
				while(*dirpathcpin){
					cpinfileName[index1]=*dirpathcpin++;
					index1++;
					
				}
				cpinfileName[index1]='\0';
			}
			
			if(dir_error_flag==0)
			{
			FILE* fin=fopen(externalFile,"r");
			//Check if the given file already exists// If so we would have to over lap the 
			// Also check if inode corresponding to filename is 0 or not. If it is zero then replace that with new inode. 
			
			if (fin==NULL)
			{
				printf("File not found.");
			} else {
				printf("copying data...\n");
				fseek(fin, 0L, SEEK_END);
				long int filesize=ftell(fin);	
				//printf("filesize is %ld", filesize);
				fclose(fin);
				
				// get an inode
					// consier the scenario when inode array is empty
				int cur_file_inode_num = getInode();
				//printf("cur_file_inode_num is %d\n", cur_file_inode_num);
				int inode_address = (cur_file_inode_num-1)* INODE_SIZE + 2 * BLOCK_SIZE;
				//printf("inode_address is %d\n", inode_address);
				
				//We will have to write the inode not get one!. We only need the inode number to which 
				//we want to write the contents to. 
				inode_type new_file_inode;
				new_file_inode.size = filesize;
				
				int number_of_free_block_req;
				if(filesize % BLOCK_SIZE  == 0){
					number_of_free_block_req = filesize/BLOCK_SIZE;
				} else {
					number_of_free_block_req = filesize/BLOCK_SIZE + 1;
				}
				//printf("number_of_free_block_req is %d\n", number_of_free_block_req);
				
				new_file_inode = addFreeBlockToAddr(new_file_inode, number_of_free_block_req);
				FILE* fin=fopen(externalFile,"r");
				int index = 0;
				int num_of_char_debug = 0;
				//printf(" writing the contents to blocks\n");
				
				if(number_of_free_block_req > ADDR_SIZE) {
					//big file		
					int count = 0;
					int count2 = 0;
					int block_num1;
					int currBlock;
					new_file_inode.flags = inode_alloc_flag | file_flag | large_file_flag | file_access_rights;
					//printf("*************Big file*****************\n");
					//printf("Number of free block required is -> %d\n", number_of_free_block_req);
					while(count < number_of_free_block_req && count < ((ADDR_SIZE-1) * (BLOCK_SIZE/sizeof(int)))){
						if(count % (BLOCK_SIZE/sizeof(int)) == 0) {
							block_num1 = new_file_inode.addr[index];
							//printf("\tBlock_num1 is %d and block number is %d  indes is %d\n", count, block_num1, index);
							count2 = 0;
							index++;
						}
						//Change data type of everything
						//printf("block_num - %d, BLOCK_SIZE - %d, count2 - %d, sizeof(int) -%d, ", block_num1, BLOCK_SIZE, count2, sizeof(int));
						
						//printf("block address is %ld   ", ((block_num1 * BLOCK_SIZE) + (count2 * sizeof(int))));
						//What is the input type for lseek. 
						lseek(fileDescriptor, ((block_num1 * BLOCK_SIZE) + (count2 * sizeof(int))), 0);
						read(fileDescriptor, &currBlock, sizeof(int));
						lseek(fileDescriptor, currBlock * BLOCK_SIZE, 0);
						//printf("Block is %d and block number is %d\n", count, currBlock);
						int byteNum = 0;
						char c = getc(fin);						
						while(byteNum < 1023 && c!=EOF) {
							write(fileDescriptor, &c, 1);
							c = getc(fin);
							//printf("%c", c);							
							num_of_char_debug++;	
							byteNum++;							
						}
						if(c!=EOF)
							write(fileDescriptor, &c, 1);
						count++;
						count2++;
					}
					
					
					if(count < number_of_free_block_req) {
						number_of_free_block_req -= count;
						count = 0;
						count2 = 0;						
						int count3 = 0;
						int count4 = 0;
						int block_num = new_file_inode.addr[index];
						int cur_block;
						int block_write;
						int data_block;
						//printf("Beginning of extra large part. And block num is %d\n", block_num);
						while(count < number_of_free_block_req){
							if(count % ((BLOCK_SIZE/sizeof(int)) * (BLOCK_SIZE/sizeof(int))) == 0) {
								lseek(fileDescriptor, (block_num * BLOCK_SIZE) + (count2 * sizeof(int)), 0);
								read(fileDescriptor, &cur_block, sizeof(int));
								//lseek(fileDescriptor, cur_block * BLOCK_SIZE, 0);
								count2++;
								count3 = 0;
								//printf("copying data...\t");
							}
							if(count % (BLOCK_SIZE/sizeof(int)) == 0){
								lseek(fileDescriptor, ((cur_block * BLOCK_SIZE) + (count3 * sizeof(int))), 0);
								read(fileDescriptor, &block_write, sizeof(int));
								count3++;
								count4 = 0;
								
							}
							lseek(fileDescriptor, ((block_write * BLOCK_SIZE) + (count4 * sizeof(int))), 0);
							read(fileDescriptor, &data_block, sizeof(int));
							lseek(fileDescriptor, data_block * BLOCK_SIZE, 0);
							//printf("Count is %d and block number is %d\n", count, data_block);
							int byteNum = 0;
							char c = getc(fin);						
							while(byteNum < 1023 && c!=EOF) {
								write(fileDescriptor, &c, 1);
								c = getc(fin);
								//printf("%c", c);							
								num_of_char_debug++;	
								byteNum++;							
							}
							if(c!=EOF)
								write(fileDescriptor, &c, 1);
							count++;
							count4++;
						}
					}
					
				} else {
					//small file					
					new_file_inode.flags = inode_alloc_flag | file_flag | small_file_flag | file_access_rights;
										
					//read(fileDescriptor, &new_file_inode, INODE_SIZE);
					//We need to add this inode to the current directory.					
					//write this inode 									
					
					while(index < number_of_free_block_req ) {
						int currBlock = new_file_inode.addr[index];
						lseek(fileDescriptor, currBlock * BLOCK_SIZE, SEEK_SET);
						//printf("block number - %d. contents of this block are below", currBlock);
						//We can split this into last block and other blocks. 
						int byteNum = 0;
						char c = getc(fin);						
						while(byteNum < 1023 && c!=EOF) {
							write(fileDescriptor, &c, 1);
							c = getc(fin);
							//printf("%c", c);							
							num_of_char_debug++;	
							byteNum++;							
						}
						if(c!=EOF)
							write(fileDescriptor, &c, 1);
						//write(fileDescriptor, &c, 1);
						
						index++;
					}
					
				}
				
				fclose(fin);
				//printf("num_of_char_debug is %d\n", num_of_char_debug);
				lseek(fileDescriptor, inode_address, SEEK_SET);
				write(fileDescriptor, &new_file_inode, INODE_SIZE);	
				//have to add this inode to current directory
				//long int address =  findLocWhereCurrDirFileShouldGo(cur_dir_inode_num);
				findLocWhereCurrDirFileShouldGo(inode_num_of_dir);
				//printf("+1\t");
				dir_type dir_file;
				//printf("+2\t");
				dir_file.inode = cur_file_inode_num;
				//printf("+3\t");
				int i =0;
				index1=0;
				while(cpinfileName[index1]  != '\0') {
					 dir_file.filename[i++] = cpinfileName[index1];
					 index1+=1;
				}
				//printf("\n i is %d", i);
				//printf("+4\t");
				dir_file.filename[i] = '\0';
				//lseek(fileDescriptor, address, 0);
				//printf("+5\t");
				//printf("inode number of file is %d. fileName is %s", dir_file.inode, dir_file.filename);
				write(fileDescriptor, &dir_file, 16);
				
				
				printf("cpin has been executed\n");
				/*
				printf("addr array of the file is \n");
				i =0;
				//printf("curent index is %d\n", addr_index);
				while(i < ADDR_SIZE){
					printf("addr[%d] - %d\t",i,new_file_inode.addr[i]);
					i++;
				} */
				
				//printFirst5RootDirectories();			
				
				// find the number of free blocks that you need
					// if the free array gets empty write code for that
				// ADDR array will point to the free block that we have got
				// copy the the contents from the file 1block at a time to freeblocks
				// Add F_END at the end of last free block. 
								
				//First find in which directory we are currently in 
				// we will assume this is the first command and root will our directory
				
				//Get an inode for 
				
				//Find the empty directory/file-name place
				
				//Get an inode for the file
				
				//keep writing to free blocks and keep the address of the free blocks in an array
				//
				//things to keep in mind is we have to free blocks using super blocks. Once the first free array is empty we have to write code to get next set of free blocks list
			}
			}
			
		} else if(strcmp(splitter, "cpout") == 0) {
			char *v6_file = strtok(NULL, " ");
			char *externalFile = strtok(NULL, " ");
			
			int temp=cur_dir_inode_num;
			char *dirpathcpin = v6_file;
			int index1=0;
			char cpinfileName[100];
			inode_type inode_of_dir;
			int inode_num_of_dir;
			int dir_error_flag=0;
			if (*dirpathcpin++=='/')
			{
				cur_dir_inode_num=1;
				while(*dirpathcpin){
					cpinfileName[index1]=*dirpathcpin++;
					index1++;
					if(cpinfileName[index1-1]=='/')
					{
						cpinfileName[index1-1]='\0';
						inode_num_of_dir = inodeOfFile(cpinfileName);
						inode_of_dir = getInodeWithNum(inode_num_of_dir);
						if((inode_of_dir.flags & 060000) == 040000) {
						cur_dir_inode_num = inode_num_of_dir;
						}
						else {
								printf("invalid direcotry name\n");
								cur_dir_inode_num=temp;
								dir_error_flag=1;
								break;
							}	
						while(index1>=0)
						{
							cpinfileName[index1]='\0';
							index1--;
						}
						index1=0;	
					}
				}
				
			
			}
			else{
				*dirpathcpin--;
				while(*dirpathcpin){
					cpinfileName[index1]=*dirpathcpin++;
					index1++;
				}
				cpinfileName[index1]='\0';
			}
			
			if(dir_error_flag==0)
			{
			//Will fopen read a file with a file? Or just file name
			int fd_external = open(externalFile, O_RDWR | O_CREAT, 0700);
			//FILE* fout=fopen(externalFile,"w");
			//v6_file - Will it have a file path from the 
			
			int inode_num_of_file = inodeOfFile(cpinfileName);
			//Also check inode_num_of_file is a file
			if(inode_num_of_file > 0 ) {
				printf("copying data...\n");
				inode_type inode_of_file = getInodeWithNum(inode_num_of_file);
				if((inode_of_file.flags & 060000) != dir_flag ) {
					//looping through addr array			
					//Get the inode of the given file name. What if the v-6 file doesn't exist? print that file doesn't exist. 			
					//Assuming small size for the directory			
					//looping through addr array //looping through dir_data blocks 
					//Use the existing code		
					//Need to change inode name below 
					
					//data block of current file
					int num_of_data_block;
					if(inode_of_file.size % BLOCK_SIZE == 0) {
						num_of_data_block = inode_of_file.size/BLOCK_SIZE;
					} else {
						num_of_data_block = inode_of_file.size/BLOCK_SIZE + 1;
					}
					int addr_index = 0;
					
					//again assuming small file
					//printf("\n inode_of_file.size is %d\n", inode_of_file.size);
					//printf("printing the contents while copying\n");
					char copy_char;
					
					if((inode_of_file.flags & large_file_flag) == large_file_flag) {
						//large file
						int count = 0;
						int count2 = 0;
						int block_num; 
						int curr_block;
						while(count < num_of_data_block && count < ((ADDR_SIZE-1) * (BLOCK_SIZE/sizeof(int)))) {
							if(count % (BLOCK_SIZE/sizeof(int)) == 0) {
								//debugging
								
								block_num = inode_of_file.addr[addr_index];
								if(block_num == 0) {
									int i =0;
									printf("curent index is %d\n", addr_index);
									while(i < ADDR_SIZE){
										printf("addr[%d] - %d\t",i,inode_of_file.addr[i]);
										i++;
									}
									printf("size in the file inode is not correct"); // Change the print statement in the future
									break;
								}
								addr_index++;
								count2 =0;
							}
							lseek(fileDescriptor, (block_num * BLOCK_SIZE) + (count2 * sizeof(int)), 0);
							read(fileDescriptor, &curr_block, sizeof(int));
							if(curr_block == 0) {
								//printf("It was here\n");
								printf("size in the file inode is not correct"); // Change the print statement in the future
								break;
							}
							int i=0;
							//printf("Block %d and block num is %d\n", count, curr_block);
							lseek(fileDescriptor, curr_block * BLOCK_SIZE, 0);
							while(i< BLOCK_SIZE) {
								read(fileDescriptor, &copy_char, 1);
								if(copy_char == '\0') {
									//write(fd_external, &copy_char, 1);
									break;
								}
								//printf("%c", copy_char);
								write(fd_external, &copy_char, 1);
								i++;
							}
							count++;
							count2++;
						}
						
						if(count < num_of_data_block) {
							num_of_data_block -=  count;
							count = 0;
							count2 = 0;
							int count3 = 0;
							int count4 = 0;
							int block_num = inode_of_file.addr[addr_index];
							int cur_block;
							int block_write;
							int dataBlock;
							//printf("reading extra large part of file. block_num is %d\n", block_num);
							while(count < num_of_data_block) {
								if(count % ((BLOCK_SIZE/sizeof(int)) * (BLOCK_SIZE/sizeof(int))) == 0) {
									lseek(fileDescriptor, (block_num * BLOCK_SIZE) + (count2 * sizeof(int)), 0);
									read(fileDescriptor, &cur_block, sizeof(int));
									//lseek(fileDescriptor, cur_block * BLOCK_SIZE, 0);
									count2++;
									count3 = 0;
									//printf("copying data blocks...\t");
								}
								if(count % (BLOCK_SIZE/sizeof(int)) == 0) {									
									lseek(fileDescriptor, (cur_block * BLOCK_SIZE) + (count3 * sizeof(int)), 0);
									read(fileDescriptor, &block_write, sizeof(int));
									count3++;
									count4 = 0;
								}
								lseek(fileDescriptor, (block_write * BLOCK_SIZE) + (count4 * sizeof(int)), 0);
								read(fileDescriptor, &dataBlock, sizeof(int));
								lseek(fileDescriptor, dataBlock * BLOCK_SIZE, 0);
								int i=0;
								//printf("Block %d and block num is %d\n", count, dataBlock);
								while(i< BLOCK_SIZE) {
									read(fileDescriptor, &copy_char, 1);
									if(copy_char == '\0') {
										//write(fd_external, &copy_char, 1);
										break;
									}
									//printf("%c", copy_char);
									write(fd_external, &copy_char, 1);
									i++;
								}								
								count++;
								count4++;
							}
						}
						
					} else {
						//small file
						while(addr_index < num_of_data_block) {
							//check till the empty block
							
							int dir_block_num = inode_of_file.addr[addr_index];
							lseek(fileDescriptor, dir_block_num * BLOCK_SIZE, 0);
							if(dir_block_num == 0) {
								printf("size in the file inode is not correct"); // Change the print statement in the future
								break;
							}
							int i=0;
							//printf("Block %d and block num is %d\n", addr_index, dir_block_num);
							while(i< BLOCK_SIZE) {
								read(fileDescriptor, &copy_char, 1);
								if(copy_char == '\0') {
									//write(fd_external, &copy_char, 1);
									break;
								}
								//printf("%c", copy_char);
								write(fd_external, &copy_char, 1);
								i++;
							}
							addr_index++;
							//loop through the directories in the dir_block_num
							/**
							while(false) {
								//dir_type cur_loop_dir = 
								//see if this directory has the file that you looking for. 
							}
							**/				
						}
					}
						
				} else {
					printf("not a valid file name\n");
				}

				
				
				//loop through all the directories and search for the file that you are looking for
				
				//How do we read data from file system?
				// May be we can have a pointer to the address of the starting address (like FILE* or char*) and then use read command to get the data from the block. Or We can character(character pointer) to point to it and read the content interatively
			} 
			close(fd_external);		
			printf("cpout has been executed\n");
			cur_dir_inode_num=temp;
			}
			
		} else if(strcmp(splitter, "mkdir") == 0) {
			mkdir();
		} else if(strcmp(splitter, "rm") == 0) {
			rmFile();
		} else if(strcmp(splitter, "ls") == 0) {
			ls();
		} else if(strcmp(splitter, "pwd") == 0) {
			printPwd();
		} else if(strcmp(splitter, "cd") == 0) {
			cd();
		} else if(strcmp(splitter, "rmdir") == 0) {
			rmDir();			
			//go to all subdirectory and delete all the files and directories
			//Can not delete root 
			//delete dir/files you have set all the blocks free and then to free list. 
			// Free inodes and if the ilist size changes- update that in the super block			
			
		} else if(strcmp(splitter, "open") == 0) {
			char *filepath = strtok(NULL, " ");
			//fileDescriptor = open(fileName, O_RDWR);
			if(access(filepath, F_OK) != -1) {
				//Check if the filepath exists
			  //Check if the read/write access for the filepath for the current user does not exist 
			  fileDescriptor = open(filepath, O_RDWR, 0600);
			  if( fileDescriptor == -1){
			  
				 printf("\n filesystem already exists but open() failed with error [%s]\n", strerror(errno));
				 return 1;
			  }
			  //read/write access for filepath is available for the current user
			  // One the following 3 methods is getting stuck? What could be the reason for that?
			  // Initialise superBlock
				//printf("fd is %d\n", fileDescriptor);
			  
				lseek(fileDescriptor, BLOCK_SIZE, 0);
				read(fileDescriptor, &superBlock, BLOCK_SIZE);				
				//printSuperBlock();
				cur_dir_inode_num = 1;
				cur_dir[dir_index]='/';
				//Set current directory. 				
				printf("filesystem %s open\n", filepath);
				//printf("****\n");
			  // Initialise rootDir			  
			  //printFirst5RootDirectories();		  
		    } else {
				printf("Given FileSystem doesn't exist");
			}
		} else if(strcmp(splitter, "print") == 0) {
			char *blockNum = strtok(NULL, " ");
			int block_num = atoi(blockNum);
			printBlock(block_num);
		} else {
			printf("incorrect command. Try again\n");
		}
	}
}

void rmFile(){
	int inode_num_of_file = getinodeNumberAfterDeleteFile();
	if(inode_num_of_file != 0) {
		freeInode(inode_num_of_file);		 
	}	
}

void printPwd()
{
	printf("%s\n",cur_dir);
}

void rmDir() {
	int inode_num_of_file = getinodeNumberAfterDeleteDir();
	if(inode_num_of_file != 0) {
		freeInode(inode_num_of_file);		 
	}
}

void freeInode(int inode_num) {
	if(inode_num == 0)
		return;
	//printf("Adding to free list -> %d inode\n", inode_num);
	//Go through addr array. Free a block. Add the block to free list. set the addr[index] val to zero	
	inode_type inode_ = getInodeWithNum(inode_num);
	int i =0;
	int block_num;
	if((inode_.flags & large_file_flag) == small_file_flag) {
		//small file 
		for(i = 0; i < ADDR_SIZE; i++) {
			if(inode_.addr[i] == 0){
				break;
			}			
			block_num = inode_.addr[i];
			addToFreeBlockArrayWithoutBuffer(block_num);
			inode_.addr[i] = 0;
			//No need to have buffer right?
		}
	} else {
		//large file
		int j = 0;
		int curr_block;
		for(i = 0; i < ADDR_SIZE-1; i++) {
			if(inode_.addr[i] == 0){
				break;
			}
			block_num = inode_.addr[i];
			for(j = 0; j < BLOCK_SIZE/sizeof(int); j++){
				lseek(fileDescriptor, ((block_num * BLOCK_SIZE) + (j * sizeof(int))), 0);
				read(fileDescriptor, &curr_block, sizeof(int));
				if(curr_block == 0){
					break;
				}
				//printf("\t i is %d, j is %d, block num %d\n", i, j, curr_block);
				addToFreeBlockArrayWithoutBuffer(curr_block);
			}
			//printf("i is %d, block num %d\n", i, block_num);
			addToFreeBlockArrayWithoutBuffer(block_num);
			inode_.addr[i] = 0;
		}
		if(inode_.addr[ADDR_SIZE-1] != 0) {
			int data_block;
			int dataBlock2;
			int k;
			block_num = inode_.addr[ADDR_SIZE-1];
			for(i = 0; i < BLOCK_SIZE/sizeof(int); i++) {
				lseek(fileDescriptor, ((block_num * BLOCK_SIZE) + (i * sizeof(int))), 0);
				read(fileDescriptor, &curr_block, sizeof(int));
				//printf("deallocating data blocks...\t");
				if(curr_block == 0){
					break;
				}
				for(j = 0; j < BLOCK_SIZE/sizeof(int); j++){
					
					lseek(fileDescriptor, ((curr_block * BLOCK_SIZE) + (j * sizeof(int))), 0);
					read(fileDescriptor, &data_block, sizeof(int));
					if(data_block == 0){
						break;
					}
					for(k = 0; k < BLOCK_SIZE/sizeof(int); k++){
						lseek(fileDescriptor, ((data_block * BLOCK_SIZE) + (k * sizeof(int))), 0);
						read(fileDescriptor, &dataBlock2, sizeof(int));
						if(dataBlock2 == 0){
							break;
						}
						addToFreeBlockArrayWithoutBuffer(dataBlock2);
					}
					//printf("\t\t i is %d, j is %d, block num %d\n", i, j, data_block);
					addToFreeBlockArrayWithoutBuffer(data_block);
				}
				//printf("\t i is %d, block num %d\n", i, curr_block);
				addToFreeBlockArrayWithoutBuffer(curr_block);
			}
			//printf("block num %d\n", block_num);
			addToFreeBlockArrayWithoutBuffer(block_num);
			inode_.addr[ADDR_SIZE-1] = 0;
		}
	
	}
	
	//Set inode allocate bit to zero. Add it the inode array if possible. 
	inode_.flags = inode_.flags & 011111;
	
	//Need to write this inode contents to its location. 
	lseek(fileDescriptor, 2 * BLOCK_SIZE + (inode_num - 1)*INODE_SIZE, 0);
	write(fileDescriptor, &inode_, INODE_SIZE);	
		
	//add inode to the free list if possible
	addInodeToFreeArray(inode_num);
	printf(" was successfully deleted\n");
}

void addInodeToFreeArray(int inode_num){
	if(superBlock.ninode < I_SIZE) {
		superBlock.inode[superBlock.ninode] = inode_num;
		superBlock.ninode++;
	}
}

int getinodeNumberAfterDeleteFile(){
	char *v6_file = strtok(NULL, " ");
	inode_type current_dir_inode = getInodeWithNum(cur_dir_inode_num);
	int i = 0;
	printf("%s", v6_file);
	
	while(i < ADDR_SIZE) {
		int block_number = current_dir_inode.addr[i];
		lseek(fileDescriptor, block_number * BLOCK_SIZE, 0);		
		int j = 0;
		dir_type iter_dir_cur;
		while(j < BLOCK_SIZE/16){				
			read(fileDescriptor, &iter_dir_cur, 16);				
			if(iter_dir_cur.filename[0] == '\0') {
				printf(" rm: %s file not found in the directory\n", v6_file);
				return 0;
			} else if(strcmp(iter_dir_cur.filename, v6_file) == 0) {
				if(iter_dir_cur.inode == 0) {
					printf(" rm: %s file not found in the directory\n", v6_file);
					return 0;
				}
				//Make sure while adding a file or folder if the file previously existed, then use the same dir and set the new inode number for that. 
				//Check if inode of the v6_file is a file or directory. 
				inode_type file_or_dir = getInodeWithNum(iter_dir_cur.inode);
				if((file_or_dir.flags & 060000) == dir_flag ) {
					printf(" rm: cannot remove %s: Is a directory\n", v6_file);
					return 0;
				}
				//printf("\n %s found in the current dir. inode number is %d\n", v6_file, iter_dir_cur.inode);
				int inode_num = iter_dir_cur.inode;					
				iter_dir_cur.inode = 0;
				lseek(fileDescriptor, block_number * BLOCK_SIZE + j * 16, 0);
				write(fileDescriptor, &iter_dir_cur, 16);
				return inode_num;
			}
			j++;
		}
		i++;
	}
	//Find dir in the current directory which has this file name
	//get inode number. Set the val in the dir.inode to zero
	
	//Go through all the addr array and free the block and set the addr[i] val to 0;
	//once all the blocks are freed, set the allocate bit to zero. add the node to inode array if it is not full;
	
	
}

//Combine getinodeNumberAfterDeleteFile and getinodeNumberAfterDeleteDir
int getinodeNumberAfterDeleteDir(){
	char *v6_file = strtok(NULL, " ");
	inode_type current_dir_inode = getInodeWithNum(cur_dir_inode_num);
	int i = 0;
	printf("%s", v6_file);
	while(i < ADDR_SIZE) {
		int block_number = current_dir_inode.addr[i];
		lseek(fileDescriptor, block_number * BLOCK_SIZE, 0);		
		int j = 0;
		dir_type iter_dir_cur;
		while(j < BLOCK_SIZE/16){
		    
			read(fileDescriptor, &iter_dir_cur, 16);
			
			if(iter_dir_cur.filename[0] == '\0') {
				printf(" rm: %s not found in the directory\n", v6_file);
				return 0;
			} else if(strcmp(iter_dir_cur.filename, v6_file) == 0) {
				if(iter_dir_cur.inode == 0) {
					printf("rm: %s not found in the directory\n", v6_file);
					return 0;
				}
				//Make sure while adding a file or folder if the file previously existed, then use the same dir and set the new inode number for that. 
				
				//Check if inode of the v6_file is a file or directory. 
				inode_type file_or_dir = getInodeWithNum(iter_dir_cur.inode);
				if((file_or_dir.flags & 060000) == file_flag ) {
					printf(" rm: cannot remove %s: Is a file\n", v6_file);
					return 0;
				}
				//printf("\n %s found in the current dir. inode number is %d\n", v6_file, iter_dir_cur.inode);
				int inode_num = iter_dir_cur.inode;
				
				iter_dir_cur.inode = 0;
				lseek(fileDescriptor, block_number * BLOCK_SIZE + j * 16, 0);
				write(fileDescriptor, &iter_dir_cur, 16);
				return inode_num;
			}
			j++;
		}
		i++;
	}
}

void printContentOfFile(int inode_num) {
	inode_type current_dir_inode = getInodeWithNum(inode_num);
}

//setting the pointer to the location where the dir should go
//change method name.
void findLocWhereCurrDirFileShouldGo(int cur_dir_inode_num){
	inode_type cur_dir_inode = getInodeWithNum(cur_dir_inode_num);
	int i = 0;
	while( i < ADDR_SIZE) {
		int block_number = cur_dir_inode.addr[i];
		//printf("block_number is %d, inode num %d \n", block_number, cur_dir_inode_num);
		lseek(fileDescriptor, block_number * BLOCK_SIZE, 0);		
		int j = 0;
		dir_type iter_dir_cur;
		while(j < BLOCK_SIZE/16){
		    
			read(fileDescriptor, &iter_dir_cur, 16);
			j++;
			if(iter_dir_cur.filename[0] == '\0') {
				//printf("\nIt was here\n");
				//printf("SEEK_CUR is %ld\n", SEEK_CUR);
				lseek(fileDescriptor, -16, SEEK_CUR);
				//printf("+0\t");
				return;
			}
			//How do we know if this directory is empty?
			//iter_dir_cur = 
			//ftell
			//rewind by sizeof(dir_type)
			//ftell
		}
		i++;
	}
	//change the return type.
	
	//Loopthrough all the addr array
	//loopthrough all the dir in the data block. 
	return;
}

int inodeOfFile(char *file_name) {
	inode_type current_dir_inode = getInodeWithNum(cur_dir_inode_num);
	int i = 0;
	//printf("Listing of files in the directory\n");
	while( i < ADDR_SIZE) {
		int block_number = current_dir_inode.addr[i];
		lseek(fileDescriptor, block_number * BLOCK_SIZE, 0);		
		int j = 0;
		dir_type iter_dir_cur;
		while(j < BLOCK_SIZE/16){		    
			read(fileDescriptor, &iter_dir_cur, 16);			
			if(iter_dir_cur.filename[0] == '\0') {
				printf("%s file not found in the current directory", file_name);
				return 0;
			} else if(strcmp(iter_dir_cur.filename, file_name) == 0) {
				return iter_dir_cur.inode;
			}
			j++;
		}
		i++;
	}	
}

void cd() {
	char *dirname = strtok(NULL, " ");
	char *dirpath = dirname;
	int index=0;
	char fileName[100];
	inode_type inode_of_dir;
	int inode_num_of_dir;
	if (*dirpath++=='/')
	{
		cur_dir_inode_num=1;
		while(*dirpath){
			fileName[index]=*dirpath++;
			index++;
			if(fileName[index-1]=='/')
			{
				fileName[index-1]='\0';
				inode_num_of_dir = inodeOfFile(fileName);
				inode_of_dir = getInodeWithNum(inode_num_of_dir);
				if((inode_of_dir.flags & 060000) == 040000) {
				cur_dir_inode_num = inode_num_of_dir;
				}
				else {
					    printf("invalid direcotry name\n");
						return;
					 }	
				while(index>=0)
				{
					fileName[index]='\0';
					index--;
				}
				index=0;	
			}
		}
		inode_num_of_dir = inodeOfFile(fileName);
		inode_of_dir = getInodeWithNum(inode_num_of_dir);
		if((inode_of_dir.flags & 060000) == 040000) {
				//printf("given dir is correct\n");
				cur_dir_inode_num = inode_num_of_dir;
		}
		else {
		printf("invalid direcotry name\n");
		return;
		}	
		
		dir_index=-1;
		while(*dirname){
			dir_index++;
			cur_dir[dir_index]=*dirname++;
			
		}
	
	}
	
	else{
	//inode_type cur_dir_inode = getInodeWithNum(cur_dir_inode_num);
			inode_num_of_dir = inodeOfFile(dirname);
			inode_of_dir = getInodeWithNum(inode_num_of_dir);
	if((inode_of_dir.flags & 060000) == 040000) {
		//printf("given dir is correct\n");
		cur_dir_inode_num = inode_num_of_dir;	
		if(strcmp(dirname,"..")!=0 && strcmp(dirname,".")!=0){
		    if(cur_dir[dir_index]!='/')
		    {
		    	dir_index++;
		    	cur_dir[dir_index]='/';
		    	while(*dirname){
		    		dir_index++;
		    		cur_dir[dir_index]=*dirname++;
		    	}
		    }
		    else{
		    	while(*dirname){
		    		dir_index++;
		    		cur_dir[dir_index]=*dirname++;
		    	}
		    }
		}
		else if (strcmp(dirname,"..")==0){
			while(cur_dir[dir_index]!='/')
			{
				cur_dir[dir_index]='\0';
				dir_index--;
			}
			if (dir_index!=0){
				cur_dir[dir_index]='\0';
				dir_index--;
			}
		}
	
		
	} else {
		printf("invalid direcotry name\n");
	}	
	//check if this is 
	} 
}

void ls(){
	//Can you diff color to the files and folders. Too much I know:P But we can try. 
	inode_type cur_dir_inode = getInodeWithNum(cur_dir_inode_num);
	int i = 0;
	//printf("Listing of files in the directory\n");
	while( i < ADDR_SIZE) {
		int block_number = cur_dir_inode.addr[i];
		lseek(fileDescriptor, block_number * BLOCK_SIZE, 0);		
		int j = 0;
		dir_type iter_dir_cur;
		while(j < BLOCK_SIZE/16){
		    
			read(fileDescriptor, &iter_dir_cur, 16);
			j++;
			if(iter_dir_cur.filename[0] == '\0') {
				//printf("\nIt was here\n");
				//printf("SEEK_CUR is %ld\n", SEEK_CUR);
				//lseek(fileDescriptor, -16, SEEK_CUR);
				printf("\n");
				return;
			} else if((iter_dir_cur.filename[0] != '.') && (iter_dir_cur.inode != 0)) {
				
				printf("%s  ", iter_dir_cur.filename);
			}
			//How do we know if this directory is empty?
			//iter_dir_cur = 
			//ftell
			//rewind by sizeof(dir_type)
			//ftell
		}
		i++;
	}
	//change the return type.
	
	//Loopthrough all the addr array
	//loopthrough all the dir in the data block. 
	return;
}

void mkdir(){
	//what if there are more than 1 names?
	//
	char *v6_dir = strtok(NULL, " ");
	int cur_inode_num = getInode();
	//printf("cur_inode_num is %d\n", cur_inode_num);
	int inode_address = (cur_inode_num-1)* INODE_SIZE + 2 * BLOCK_SIZE;
	inode_type new_dir_inode;
	new_dir_inode.flags = inode_alloc_flag | dir_flag | dir_small_file | dir_access_rights; 
	//new_file_inode.size = filesize;  Dont know what to set this val to
	new_dir_inode.addr[0] = getFreeBlock();	
	new_dir_inode.nlinks = 0; 
    new_dir_inode.uid = 0;
    new_dir_inode.gid = 0;
    new_dir_inode.size = INODE_SIZE;
	int i =0;
	for( i = 1; i < ADDR_SIZE; i++ ) {
		new_dir_inode.addr[i] = 0;
	}
	
	new_dir_inode.actime[0] = 0;
	new_dir_inode.modtime[0] = 0;
	new_dir_inode.modtime[1] = 0;
	
	dir_type cur_dir;
	
	cur_dir.inode = cur_inode_num;
	cur_dir.filename[0] = '.';
    cur_dir.filename[1] = '\0';
	
	dir_type parent_dir;
	parent_dir.inode = cur_dir_inode_num;
	parent_dir.filename[0] = '.';
    parent_dir.filename[1] = '.';
    parent_dir.filename[2] = '\0';
	
	lseek(fileDescriptor, inode_address, 0);
	write(fileDescriptor, &new_dir_inode, INODE_SIZE);
	
	lseek(fileDescriptor, new_dir_inode.addr[0] * BLOCK_SIZE, 0);  
	write(fileDescriptor, &cur_dir, 16);
	write(fileDescriptor, &parent_dir, 16);
	
	// create a directory entry into current directory
	// add '.' and '..' into the 
	
	findLocWhereCurrDirFileShouldGo(cur_dir_inode_num);
	dir_type dir_file;
	dir_file.inode = cur_inode_num;
	i =0;
	while(*v6_dir != '\0') {
		 dir_file.filename[i++] = *v6_dir++;
	}
	dir_file.filename[i] = '\0';
	//lseek(fileDescriptor, address, 0);
	
	//printf("inode number of file is %d. fileName is %s", dir_file.inode, dir_file.filename);
	write(fileDescriptor, &dir_file, 16);
	
	printf("Directory created \n");
	
}

inode_type addFreeBlockToAddr(inode_type inode, int no_free_blocks) {
	//printf("Adding free blocks to array\n");
	int index = 0;
	if(no_free_blocks < ADDR_SIZE) {
		//small file
		while(index < no_free_blocks) {
			int freeBlockNo = getFreeBlock();
			inode.addr[index++] = freeBlockNo;
		}
	} else {
		//large file
		int inode_count = 0;
		int num; 
		int count3 =0;		
		//till the last but one index of addr array
		while(inode_count < no_free_blocks && inode_count < ((ADDR_SIZE-1) * (BLOCK_SIZE/sizeof(int))))  {
			//assuming we are storing block numbers in int
			if(inode_count % (BLOCK_SIZE/sizeof(int)) == 0){
				num = getFreeBlock();
				if(num == 0){
					printf("Not enough free space left\n");
					break;
				}
				//for some block it is still coming as 0
				//if(index > 0)
					//printBlock(inode.addr[index - 1]);
				inode.addr[index] = num;
				lseek(fileDescriptor, num * BLOCK_SIZE, 0);
				//printf("addr index %d and block is %d\n", index, num);
				index++;
				
				count3 =0;
			}
			int freeBlockNo = getFreeBlock();
			if(freeBlockNo == 0){
				printf("Not enough free space left\n");
				break;
			}
			//printf("\tcount is %d block num is %d\n", inode_count, freeBlockNo);
			lseek(fileDescriptor, ((num * BLOCK_SIZE) + (count3 *sizeof(int))), 0);
			write(fileDescriptor, &freeBlockNo, sizeof(int));
			
			/*
			write(fileDescriptor, &freeBlockNo, sizeof(int));
			lseek(fileDescriptor, -sizeof(int), SEEK_CUR);
			read(fileDescriptor, &freeBlockNo, sizeof(int) );
			printf("read %d\n", freeBlockNo);*/
			count3++;
			inode_count++;			
		}
		//debuggin
		/*
		lseek(fileDescriptor, -(sizeof(int)*no_free_blocks), SEEK_CUR);
		inode_count = 0;
		int dummy;
		printf("\nprinting all values again\n");
		while(inode_count < no_free_blocks) {
			read(fileDescriptor, &dummy, sizeof(int) );
			printf("%d\n", dummy);
			inode_count++;
		} */
		printf("\n");
		//printBlock(inode.addr[index -1]);	
		
		if(inode_count < no_free_blocks){
			//printf("********************************* Extra large file ***************************\n");
			no_free_blocks -=  inode_count;
			inode_count = 0;
			//printf("index is %d\n", index);
			inode.addr[index] = getFreeBlock();
			int cur_block;
			int count2 = 0;
			int count3 = 0;
			int count4 = 0;
			int block_num;
			int dataBlock;
			
			if(inode.addr[index] == 0){
				printf("Not enough free space left\n");
			}
			//last index of addr array
			while(inode_count < no_free_blocks){		
				if(inode_count % ((BLOCK_SIZE/sizeof(int)) * (BLOCK_SIZE/sizeof(int))) == 0) {
					cur_block = getFreeBlock();
					if(cur_block == 0){
						printf("Not enough free space left\n");
						break;
					}
					lseek(fileDescriptor, (inode.addr[index] * BLOCK_SIZE) + (count2 * sizeof(int)), 0);
					write(fileDescriptor, &cur_block, sizeof(int));
					count2++;
					count3 = 0;
					//printf("adding data blocks...\t");					
				}
				
				if(inode_count % (BLOCK_SIZE/sizeof(int)) == 0 ) {
					
					block_num = getFreeBlock();
					if(block_num == 0){
						printf("Not enough free space left\n");
						break;
					}
					
					//printf("count is %d block num is %d\n", inode_count, block_num);
					lseek(fileDescriptor, ((cur_block * BLOCK_SIZE) + (count3 * sizeof(int))), 0);
					write(fileDescriptor, &block_num, sizeof(int));
					count3++;
					count4 =0;
				}
				
				dataBlock = getFreeBlock();
				if(block_num == 0){
					printf("Not enough free space left\n");
					break;
				}
				
				lseek(fileDescriptor, ((block_num * BLOCK_SIZE) + (count4 * sizeof(int))), 0);
				write(fileDescriptor, &dataBlock, sizeof(int));
				count4++;
				inode_count++;
				
			}
		}
		
	}
	//printf("\n");
	return inode;
}


//Verify this method later
int getFreeBlock() {
	superBlock.nfree--;
	if(superBlock.nfree != 0) {
		int result = superBlock.free[superBlock.nfree];
		if(result == 0){
			printf("No free data blocks left\n");
			return 0;
		}			
		return result;
	} else {
		//Get the contents from superBlock.free[0] and set nfree = first int of superBlock.free[0] add it to super block. Add next FREE_SIZE or whatever number is there at 0th index
		//return the superBlock.free[0]
		int block_num = superBlock.free[superBlock.nfree];
		lseek(fileDescriptor, block_num * BLOCK_SIZE, 0);
		int num_buffer;
		read(fileDescriptor, &num_buffer, sizeof(int));
		superBlock.nfree = num_buffer;
		int i =0;
		while(i < FREE_SIZE){
			read(fileDescriptor, &num_buffer, sizeof(int));
			superBlock.free[i] = num_buffer;
			i++;
		}
			
		return block_num;
	}	
}

//already have this method
void addToFreeBlockArrayWithoutBuffer(int block_number) {
	//Checking if free array is full
	if ( superBlock.nfree == FREE_SIZE ) {
		//free array is full. We will copy the content of super block free array to new block. 
		
		//creating new block.
		int free_list_data[BLOCK_SIZE / 4], i;
		free_list_data[0] = FREE_SIZE;
		
		//copy the content of super block array array to new block.
		for ( i = 0; i < BLOCK_SIZE / 4; i++ ) {
		   if ( i < FREE_SIZE ) {
			 free_list_data[i + 1] = superBlock.free[i];
		   } else {
			 free_list_data[i + 1] = 0; // getting rid of junk data in the remaining unused bytes
		   }
		}
		lseek( fileDescriptor, (block_number) * BLOCK_SIZE, 0 );
		write( fileDescriptor, free_list_data, BLOCK_SIZE );
		superBlock.nfree = 0;		
	}
	superBlock.free[superBlock.nfree] = block_number;  // Assigning free block to free array
	++superBlock.nfree;
}

int getInode(){
	if(superBlock.ninode == 0) {
		//go through the ilist and add 100 free inodes to the array
		//Go through the ilist and find the unallocated inodes. add them to inode array. 
		int count;
		int inodeNumber;
		inode_type inode;
		lseek(fileDescriptor, 2 * BLOCK_SIZE + INODE_SIZE, 0);			
		//It shouldn't go beyond the allocated number of inodes. i guess that number is superBlock.isize * (BLOCK_SIZE/ sizeof(inode))
		//should make sure all the inodes in the last i-list block was initialized properly. 
		int total_num_of_inode = superBlock.isize * (BLOCK_SIZE/ INODE_SIZE);
		for(count = 0, inodeNumber = 2;  (count < I_SIZE) && (inodeNumber <= total_num_of_inode); inodeNumber++) {
			read(fileDescriptor, &inode, INODE_SIZE);
			if((inode.flags & 0100000) != 0100000 ) {
				superBlock.inode[count] = inodeNumber;
				count++;
			}
		}
		superBlock.ninode = count;		
	} 
	superBlock.ninode--;
	return superBlock.inode[superBlock.ninode];	
}

inode_type getInodeWithNum(int num){
	inode_type new_inode;
	lseek(fileDescriptor, 2* BLOCK_SIZE + (num-1)* INODE_SIZE, 0);
	read(fileDescriptor, &new_inode, INODE_SIZE);
	return new_inode;	
}
/*
void addInode(inode_type inode) {
	if(superBlock.ninode != 100) {
	    //We should take care about this
		//superBlock.inode[superBlock.ninode++] = inode;
	}
} */

int preInitialization(){

  char *n1, *n2;
  unsigned int numBlocks = 0, numInodes = 0;
  char *filepath;
  // input command for initilization is of the form initfs filePath numOfFreeBlocks(n1) numOfInodes(n2)
  // we have already cosidered 1st word. The below 3 lines of code will consider next 3 words. 
  filepath = strtok(NULL, " ");
  n1 = strtok(NULL, " ");
  n2 = strtok(NULL, " ");
         
  // Check if the filepath already exists.
  if(access(filepath, F_OK) != -1) {
      //filepath already exists
	  
	  //Check if the read/write access for the filepath for the current user does not exist 
	  fileDescriptor = open(filepath, O_RDWR, 0600);
      if(fileDescriptor == -1){
      
         printf("\n filesystem already exists but open() failed with error [%s]\n", strerror(errno));
         return 1;
      }
	  //printf("fileDescriptor is %d \n", fileDescriptor);
	  //read/write access for filepath is available for the current user
      printf("filesystem already exists and the same will be used.\n");
  
  } else {
		//filepath does not exist
		
		// Checking if either of n1 or n2 is null. If so, command to initialize the filesystem is incomplete/not-correct
		if (!n1 || !n2){
			printf(" All arguments(path, number of inodes and total number of blocks) have not been entered\n");
		} else {
			numBlocks = atoi(n1);
			numInodes = atoi(n2);
			//Both n1 and n2 exist. Both of them converted to int from String.
			
			//Initializing FileSystem
			if( initfs(filepath, numBlocks, numInodes )){
			    printf("The file system is initialized\n");	
			} else {
				printf("Error initializing file system. Exiting... \n");
				return 1;
			}
		}
  }
}

int initfs(char* path, unsigned int blocks,unsigned int inodes) {
   //BLOCK_SIZE is in bytes. To have integer equivalent we divide by 4
   unsigned int buffer[BLOCK_SIZE/4];
   int bytes_written;
   
   unsigned int i = 0;
   superBlock.fsize = blocks;
   unsigned int inodes_per_block= BLOCK_SIZE/INODE_SIZE;
   
   //Implementing 'ceil' fuction
   if((inodes%inodes_per_block) == 0)
   superBlock.isize = inodes/inodes_per_block;
   else
   superBlock.isize = (inodes/inodes_per_block) + 1;
   fileDescriptor = open(path,O_RDWR|O_CREAT,0700);
   if(fileDescriptor == -1)
       {
           printf("\n open() failed with the following error [%s]\n",strerror(errno));
           return 0;
       }
   //printf("fileDescriptor 2 is %d \n", fileDescriptor);
       
   for (i = 0; i < FREE_SIZE; i++)
      superBlock.free[i] =  0;			//initializing free array to 0 to remove junk data. free array will be stored with data block numbers shortly.
    
   superBlock.nfree = 0;
   superBlock.ninode = I_SIZE;
   
   for (i = 0; i < I_SIZE; i++)
	    superBlock.inode[i] = i + 1;		//Initializing the inode array to inode numbers
   
   superBlock.flock = 'a'; 					//flock,ilock and fmode are not used.
   superBlock.ilock = 'b';					
   superBlock.fmod = 0;
   superBlock.time[0] = 0;
   superBlock.time[1] = 1970;
   
   //Offsetting by BLOCK_SIZE from beginning. Cursor will now point to superblock
   lseek(fileDescriptor, BLOCK_SIZE, SEEK_SET);
   //write the contents of superBlock to file
   write(fileDescriptor, &superBlock, BLOCK_SIZE); // writing superblock to file system
   
   // writing zeroes to all inodes in ilist
   for (i = 0; i < BLOCK_SIZE/4; i++) 
   	  buffer[i] = 0;
   
   //Allocating the inode blocks and initializing all values to 0 	
   for (i = 0; i < superBlock.isize; i++)
   	  write(fileDescriptor, buffer, BLOCK_SIZE);
   
   //Nnumber of data block
   int data_blocks = blocks - 2 - superBlock.isize;
   int data_blocks_for_free_list = data_blocks - 1; 
   
   // Create root directory
   create_root();
 
   for ( i = 2 + superBlock.isize + 1; i < blocks; i++ ) //data_blocks_for_free_list should not be data_blocks -1 as the free data blocks start after root data block
   {
      add_block_to_free_list(i , buffer);
   }
   //printSuperBlock();
   return 1;
}

// Add Data blocks to free list
void add_block_to_free_list(int block_number,  unsigned int *empty_buffer){
  //Checking if free array is full
  if ( superBlock.nfree == FREE_SIZE ) {
	//free array is full. We will copy the content of super block free array to new block. 
	
	//creating new block.
    int free_list_data[BLOCK_SIZE / 4], i;
    free_list_data[0] = FREE_SIZE;
    
	//copy the content of super block array array to new block.
    for ( i = 0; i < BLOCK_SIZE / 4; i++ ) {
       if ( i < FREE_SIZE ) {
         free_list_data[i + 1] = superBlock.free[i];
       } else {
         free_list_data[i + 1] = 0; // getting rid of junk data in the remaining unused bytes
       }
    }
    //setting offset to the current data block
    lseek( fileDescriptor, (block_number) * BLOCK_SIZE, 0 );
    write( fileDescriptor, free_list_data, BLOCK_SIZE ); // Writing current free list to the block that has to be added to free blocks
    
    superBlock.nfree = 0;
    
  } else {
	 
    //setting offset to the current data block
	lseek( fileDescriptor, (block_number) * BLOCK_SIZE, 0 );
	//writing empty_buffer to the file
    write( fileDescriptor, empty_buffer, BLOCK_SIZE );  // writing 0 to data block to get rid of exiting data
  }

  superBlock.free[superBlock.nfree] = block_number;  // Assigning free block to free array
  ++superBlock.nfree;
}

// Create root directory
void create_root() {
  // first data block is after 2 + number of blocks for inode
  int root_data_block = 2 + superBlock.isize; // Allocating first data block to root directory
  int i;
  
  root.inode = 1;   // root directory's inode number is 1.
  root.filename[0] = '.';
  root.filename[1] = '\0';
  
  //Setting the flags in the inode
  inode.flags = inode_alloc_flag | dir_flag | dir_large_file | dir_access_rights;   		// flag for root directory 
  inode.nlinks = 0; 
  inode.uid = 0;
  inode.gid = 0;
  inode.size = INODE_SIZE; // inode size is INODE_SIZE
  //setting the root inode to point to root data block
  inode.addr[0] = root_data_block;
  
  //Initializing ADDR array to 0
  for( i = 1; i < ADDR_SIZE; i++ ) {
    inode.addr[i] = 0;
  }
  
  inode.actime[0] = 0;
  inode.modtime[0] = 0;
  inode.modtime[1] = 0;
  
  //Setting the cursor offset by 2 BLOCK_SIZE from 0. Cursor will now point to first I-Node block
  lseek(fileDescriptor, 2 * BLOCK_SIZE, 0);
  //writing the contents of 'inode' which is of size 'INODE_SIZE' into first I-Node
  write(fileDescriptor, &inode, INODE_SIZE);   // 
  
  //Setting the cursor ofsset by root_data_block from 0. Cursor will now point to first Data block
  lseek(fileDescriptor, root_data_block * BLOCK_SIZE, 0);  
  write(fileDescriptor, &root, 16);
  
  // Since this is root, setting the first 2 directories to '.' which is current directory
  root.filename[0] = '.';
  root.filename[1] = '.';
  root.filename[2] = '\0';
  //writing the contents of root directory into first data block
  write(fileDescriptor, &root, 16);
  
  cur_dir_inode_num = 1;
  cur_dir[dir_index]='/';
 
}

void printInode(inode_type inode_){
	printf("Printing the contents of inode\n");
    printf("flags is %d \n", inode_.flags);
    printf("nlinks is %d \n", inode_.nlinks);
    printf("uid is %d \n", inode_.uid);
    printf("gid is %d \n", inode_.gid);
    printf("size is %d \n", inode_.size);
    printf("actime is %d \n", inode_.actime);
    printf("modtime is %d \n", inode_.modtime);
    
    if((inode_.flags & 060000) == 040000){
        printf("inode is a directory\n");
    } else if((inode_.flags & 060000) == 0) {
        printf("inode is a file\n");
    } else {
        printf("inode is a special file\n");
    }
    
    // Checks if large or small
    if((inode_.flags & 010000) == 010000) {
        printf("inode points to large file\n");
    } else {
        printf("inode points to small file\n");
    }
    
    printf("Contents of addr array is \n");
    int i =0;
    while(i < ADDR_SIZE) {
        printf("addr[%d] -> %d \t", i, inode_.addr[i]);
        i++;
    }
	printf("\n");
    
}

void printSuperBlock(){
	printf("Printing the contents of superBlock\n");
	printf("superBlock.isize is %d \n", superBlock.isize);
	printf("superBlock.fsize is %d \n", superBlock.fsize);
	printf("superBlock.nfree is %d \n", superBlock.nfree);
	printf("superBlock.ninode is %d \n", superBlock.ninode);
	printf("superBlock.flock is %c \n", superBlock.flock);
	printf("superBlock.ilock is %c \n", superBlock.ilock);
	printf("superBlock.fmod is %d \n", superBlock.fmod);
	printf("superBlock.time is %d \n\n", superBlock.time);
	
	int i =0; 
	printf("Printing the contents of first 20 superBlock inode[] array\n");
	while(i < 20) {		
		printf("inode[%d] -> %d \t", i, superBlock.inode[i]);
		i++;
	}
	i=0;
	printf("Printing the contents of first 20 superBlock free[] array\n");
	while(i < 20) {		
		printf("free[%d] -> %d \t", i, superBlock.free[i]);
		i++;
	}
	
	printf("\n");
}

void printFirst5RootDirectories(){
	printf("Printing first 5 directories of Root\n");
	lseek(fileDescriptor, 2 * BLOCK_SIZE, 0);
	inode_type root;
	read(fileDescriptor, &root, INODE_SIZE);
	int root_block = root.addr[0];
	lseek(fileDescriptor, root_block * BLOCK_SIZE, 0);
	printf("file pointer is at - ");
	int i = 0;
	dir_type iter_dir;
	for(i =0; i < 5; i++) {
		read(fileDescriptor, &iter_dir, 16);
		printf("dir %d, filename is %s, inode Number is %d ", i, iter_dir.filename,  iter_dir.inode);
		if(iter_dir.filename[0] == '\0') {
			printf("directory is empty\n");
		}else {
			printf("directory is not empty\n");
		}
	}		
}

void printBlock(int blockNum){
	printf("printing contents of block %d\n", blockNum);
	lseek(fileDescriptor, blockNum * BLOCK_SIZE, 0);
	int i = 0;
	int currNum;
	while(i < (BLOCK_SIZE/sizeof(int))) {
		read(fileDescriptor, &currNum, sizeof(int));
		printf("i is %d, number is %d\n", i, currNum);
		i++;
	}	
	
}