#include "storage_mgr.h"
#include "dberror.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

FILE *pageFile;

void initStorageManager(void){
printf("Storage manager has been initialised\n\n");
//We don't have any global data structure to initialise
}

/*createPageFile method used to Create file if not exists*/

RC createPageFile(char *fileName)
{
FILE *filepointer=NULL;
//check if file exists or not
struct stat check;
int exist = stat(fileName,&check);
if(exist == 0)
 {
 printf("File, %s already exists \n\n", fileName);
 return RC_OK;
 }

OPEN(filepointer,fileName,"w+");

CHECK_DO((filepointer==NULL),printf("File creation failed\n\n");return RC_FILE_CREATION_FAILED);
 printf("File, %s has been created\n\n", fileName);
 char *input = (char*) malloc(PAGE_SIZE);
 int i=0;
for(i=0; i<PAGE_SIZE; i++)
 {
 input[i]='\0';
 }
fwrite(input,sizeof(char),PAGE_SIZE,filepointer);
free(input);
CLOSE(filepointer);
return RC_OK;

}

/*openPageFile method used open file to read or write*/
RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{
	pageFile = fopen(fileName, "r");
	if(pageFile == NULL) 
		return RC_FILE_NOT_FOUND;
	
	fHandle->fileName = fileName;
	fHandle->curPagePos = 0;
	fseek(pageFile, 0L, SEEK_END);
	int totalSize = ftell(pageFile);
	fseek(pageFile, 0L, SEEK_SET);
	fHandle->totalNumPages = totalSize/ PAGE_SIZE;  
	
	struct stat fileInfo;
	if(fstat(fileno(pageFile), &fileInfo) < 0)    
		return RC_ERROR;
	fHandle->totalNumPages = fileInfo.st_size/ PAGE_SIZE;

		// Closing file stream so that all the buffers are flushed. 
		fclose(pageFile);
		return RC_OK;
}

/* closePageFile method closes the file and frees the memory*/
RC closePageFile(SM_FileHandle *fHandle)
{

FILE *filepointer=NULL;
int fileclosed;
int a;
a=0;

CHECK_DO(fHandle == NULL,return RC_FILE_HANDLE_NOT_INIT;)
 else
 filepointer=fHandle->mgmtInfo;
 (CLOSE(filepointer)==0) ? (printf("file, %s has been closed \n\n",fHandle->fileName), a=1) : a++ ;
  if(a=1)
  return RC_OK;
  else
  return RC_FILE_NOT_CLOSED;
}

/*destroyPageFile method deletes the file from memory*/

RC destroyPageFile(char *fileName)
{
int filedestroyed;
int a;
a=0;
filedestroyed = remove(fileName);
(filedestroyed==0) ? (printf("file, %s has been Destroyed \n\n",fileName), a=1) : a++ ;
 if(a=1)
 return RC_OK;
 else
 return RC_FILE_NOT_FOUND;
}


/*readBlock method reads the block from file and loads the data into memory*/


RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
			
		FILE *fp=fopen(fHandle->fileName,"r+");
		fHandle->mgmtInfo=fp;
		
		if(fHandle == NULL) 
			return RC_FILE_HANDLE_NOT_INIT;
	
		if(!fp)	//check if the page is closed, if so, then open
		{
			fp= fopen(fHandle->fileName,"r+");
			fHandle->mgmtInfo=fp;
		}
		
		int rc=fseek(fp,(pageNum*PAGE_SIZE),SEEK_SET);
		
		if(rc==0){
			fread(memPage,sizeof(char),PAGE_SIZE,fp);
			fHandle->curPagePos=ftell(fp) ;//pageNum;
			fclose(fp);
			return RC_OK;
		}else
			return RC_FILE_READ_FAILED;
}

//getBlockPos method returns the poistion at which the fHandle is present
  int getBlockPos (SM_FileHandle *fHandle){
    int pos = fHandle->curPagePos;
    return pos;
  }

//readFirstBlock method reads the First block of the File
// and stores content to memPage pointer
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
//We shall now call the readblock method to read the first block
//while passing first argument as zero
return readBlock(0,fHandle,memPage);
}

//readLastBlock method reads the Last block of the File
// and stores content to memPage pointer
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  //We shall now call the readblock method to read the first block
  //while passing first argument as totalNumPages-1
return readBlock(((fHandle->totalNumPages)-1),fHandle,memPage);
}

//readPreviousBlock method reads previous block while having the current position-1
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
FILE *filepointer;
int num;
filepointer = fHandle->mgmtInfo;
num=(fHandle->curPagePos)-1;

return readBlock(num, fHandle, memPage);
}

//readCurrentBlock method reads previous block while having the current position
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  FILE *filepointer;
  int num;
  filepointer = fHandle->mgmtInfo;
  num=(fHandle->curPagePos);

  return readBlock(num, fHandle, memPage);
}
//readNextBlock method reads previous block while having the current position+1
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  FILE *filepointer;
  int num;
  filepointer = fHandle->mgmtInfo;
  num=(fHandle->curPagePos)+1;
  return readBlock(num, fHandle, memPage);
}

/*writeBlock method writes a data from memory to page file that is present on disk and page number is passed in the parameter*/
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
FILE *fp=fopen(fHandle->fileName,"r+");
		fHandle->mgmtInfo=fp;
	/*	
		if(pageNum<0 || pageNum>fHandle->totalNumPages || fp==NULL){
			return RC_WRITE_FAILED;
		}
		*/
		if(!fp)	//check if the page is closed, if so, then open
		{
			fp= fopen(fHandle->fileName,"r+");
			fHandle->mgmtInfo=fp;
		}
		
		if(fseek(fp,PAGE_SIZE*(pageNum),SEEK_SET)!=0)
		{
			return RC_FILE_OFFSET_FAILED;
		}
		else{
			int rc = fwrite(memPage,sizeof(char),PAGE_SIZE,fp);
			if(rc != PAGE_SIZE){
				return RC_WRITE_FAILED;
			}else{
				fclose(fp);
				fHandle->curPagePos=ftell(fp);
				return RC_OK;
			}
		}

}


/*writeCurrentBlock method writes a data from memory to current page file*/
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage){
	int pageNum;

		MOVE(pageNum,fHandle->curPagePos);
		return writeBlock(pageNum, fHandle, memPage);
}

/*appendEmptyBlock method appends a new page at the end of the file*/
RC appendEmptyBlock(SM_FileHandle *fHandle){

		FILE *Fptr;
                OPEN(Fptr,fHandle->fileName,"a");
		ALLOC(input,PAGE_SIZE);
		FOR_LOOP(i,i<PAGE_SIZE,MOVE(input[i],'\0');)
	        CHECK_RC(fwrite(input,sizeof(char),PAGE_SIZE,Fptr) != PAGE_SIZE,RC_WRITE_FAILED)
		free(input);
		INCR(fHandle->totalNumPages,1);
		CHECK_RC(CLOSE(Fptr) != 0,RC_FILE_NOT_CLOSED)
		return RC_OK;
}


/*ensureCapacity method ensures the number of pages in the file*/
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle){
		FILE *fp;
	
	if(fHandle==NULL){
		return RC_FILE_HANDLE_NOT_INIT;
	}
	
	fp=fHandle->mgmtInfo;
	if(!fp){
		fp = fopen(fHandle->fileName,"r+");
	}
	
	fseek(fp, 0L, SEEK_END);
	int pagesInFile = ftell(fp)/PAGE_SIZE;
	
	if(ftell(fp)%PAGE_SIZE > 0){
		pagesInFile=pagesInFile+1;
	}
	
	if(pagesInFile < numberOfPages){
		
		int pagesToAdd = numberOfPages - pagesInFile;
		int totalSize = (pagesToAdd)*PAGE_SIZE;
		int i=0;
		
		while(i<totalSize){
			fprintf(fp, "%c", '\0');
			i++;
		}
		
		fHandle->totalNumPages = (fHandle->totalNumPages)+pagesToAdd;
		return RC_OK;
	}
}

