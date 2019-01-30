//Including all the neccessary header files 
#include <stdio.h>
#include <stdlib.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"

//Struct for every frame page
typedef struct FrameDetail
{
  SM_PageHandle data;
  int pageNumber;
  int isDirty;
  int C_count;	
  int f_count;
  int No_Replace;
}FrameDetail;


int count = 0;
SM_FileHandle fh;
FrameDetail *fPtr;

void setPageFromFile(BM_BufferPool *const bm, int i, BM_PageHandle *const page, const PageNumber pageNum);

/*********************************************
  this method initialize the bufferpool and framedetail struct
********************************************/
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,const int numPages, ReplacementStrategy strategy,void *stratData)
{
	//FrameDetail *frame = NULL;
	
	CHECK_RC((bm == NULL || numPages <=0),RC_INVALID_BM)
	CHECK_RC((pageFileName == NULL),RC_FILE_NOT_FOUND)
	CHECK_RC((strategy < 0),RC_UNKNOWN_STRATEGY)
	CREATE_FRAME(FrameDetail,frame,numPages) 
        CHECK_RC(frame == NULL,RC_MEMORY_ERROR) 
        MOVE(bm->pageFile , (char *)pageFileName)
 	MOVE(bm->numPages,numPages)
        MOVE(bm->strategy,strategy)
        MOVE(bm->mgmtData,frame)
        FOR_LOOP(i,i<numPages,frame[i].data = NULL;frame[i].pageNumber = -1;frame[i].C_count = frame[i].f_count = frame[i].isDirty = 0;)
        MOVE(bm->readcount,0)
        MOVE(bm->writecount,0)
        RC(RC_OK)
  
 
}

/*********************************************
 this method shutdown the buffer pool 
  that is passed as parameter
*********************************************/
RC shutdownBufferPool(BM_BufferPool *const bm)
{
  int i=0;
	
  MOVE(FrameDetail *fd,bm->mgmtData)
  CHECK_RC(bm == NULL,RC_NULL_ERROR)
  CHECK_RC(forceFlushPool(bm) != RC_OK,RC_WRITE_ERROR)
  LOOP_WHILE(i< bm->numPages,CHECK_RC(fd[i].C_count != 0,RC_PINNEN_PAGES_IN_BUFFER);i++;)
  MOVE(fd,NULL)
  
  RC(RC_OK)
}
  

/*****************************************
  this method write all dirty page to the disk
*******************************************/
RC forceFlushPool(BM_BufferPool *const bm)
{
int i=0;
   CHECK_RC(bm == NULL,RC_NULL_ERROR)
   MOVE(FrameDetail *fd,bm->mgmtData)
   LOOP_WHILE(i<bm -> numPages,CHECK_DO((fd[i].isDirty == 1 && fd[i].C_count == 0),writeBlock (fd[i].pageNumber, &fh, fd[i].data);
                                                                                  bm->writecount++;fd[i].isDirty = 0;);i++;)
   RC(RC_OK)

}
	

/************************************************************
 this method mark the page dirty and return the pageHandle
*************************************************************/
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
  CHECK_RC((bm == NULL || page == NULL),RC_NULL_ERROR)

  int i=0;
  int numPages = bm->numPages;
  MOVE(FrameDetail *fd,bm->mgmtData)
  LOOP_WHILE((i<bm -> numPages),CHECK_DO((fd[i].pageNumber == page->pageNum),fd[i].isDirty = 1;return RC_OK;);i++;)
  RC(RC_OK)

}

/************************************************
  this method unpin the page from bufferpool and decrease the C_count
************************************************/
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
  CHECK_RC((bm == NULL || page == NULL), RC_NULL_ERROR)

  int i=0;
  int numPages = bm->numPages;
  FrameDetail *fd = bm->mgmtData;

  while(i<bm -> numPages)
    {
      if(fd[i].pageNumber == page->pageNum)
      {
        fd[i].C_count = fd[i].C_count-1;
        break;
      }
	  i++;
    }
  return RC_OK;

}

/*********************************************************
 this method write dirty page back to disk from bufferpool
**********************************************************/
RC writeDirtyPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    if(bm !=NULL && page != NULL){

     int i;
     for(i=0;i<bm -> numPages;i++)
    {
    if(((FrameDetail  *)bm->mgmtData)[i].pageNumber == page->pageNum)
    {
     ensureCapacity(((FrameDetail  *)bm->mgmtData)[i].pageNumber, &fh);
     writeBlock (((FrameDetail  *)bm->mgmtData)[i].pageNumber, &fh, ((FrameDetail  *)bm->mgmtData)[i].data);
     bm->writecount++;
     ((FrameDetail  *)bm->mgmtData)[i].isDirty = 0;

     return RC_OK;
    }
  }

}
else{
  return RC_NULL_ERROR;
}
}



/***************************************
 this method write page back to disk from bufferpool
****************************************/
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
  CHECK_RC((bm == NULL || page == NULL), RC_NULL_ERROR)

  if(writeDirtyPage(bm,page)!=RC_OK)
  {
	return RC_WRITE_ERROR;
	}
	else{
		return RC_OK;
	}

}

/***************************************
 First In First out Stratergy
****************************************/
void FIFO(BM_BufferPool *const bm, FrameDetail *fd)
{
  int i;
  MOVE(int pages,bm -> numPages)
  int previous = bm->readcount%pages;
  MOVE(fPtr,(FrameDetail *) bm->mgmtData)
  
  for(i=0; i<pages; i++){
    if(fPtr[previous].C_count == 0)
	  {
      if(fPtr[previous].isDirty == 1){
        openPageFile (bm->pageFile, &fh);
        writeBlock (fPtr[previous].pageNumber, &fh, fPtr[previous].data);
        INCR(bm -> writecount,1);
	    }
    	 MOVE(fPtr[previous].data,fd->data)
    	 MOVE(fPtr[previous].pageNumber,fd->pageNumber)
    	 MOVE(fPtr[previous].isDirty,fd->isDirty)
    	 MOVE(fPtr[previous].C_count,fd->C_count)
    	 break;
	  }
	  else
	  {
       INCR(previous,1);
       CHECK_DO((previous%pages == 0),previous=0;)
	  }
  }
}


/***************************************
 Least Recently Used Stratergy
****************************************/
void LRU(BM_BufferPool *const bm, FrameDetail *fd)
{
  MOVE(fPtr,(FrameDetail *) bm->mgmtData)
  int pages = bm->numPages;
  int previous;
  int least;
  FOR_LOOP(i, i<pages,CHECK_DO((fPtr[i].C_count == 0),previous= i;least = fPtr[i].f_count;break;))
   
    for(int i=previous+1; i<pages; i++)
    {
      CHECK_DO((fPtr[i].f_count < least),previous = i;least = fPtr[i].f_count;)
    }
    CHECK_DO(fPtr[previous].isDirty == 1,openPageFile (bm->pageFile, &fh);writeBlock (fPtr[previous].pageNumber, &fh, fPtr[previous].data);
             bm -> writecount++;)
    MOVE(fPtr[previous].data,fd->data)
    MOVE(fPtr[previous].pageNumber,fd->pageNumber)
    MOVE(fPtr[previous].isDirty,fd->isDirty)
    MOVE(fPtr[previous].C_count,fd->C_count)
    MOVE(fPtr[previous].f_count,fd->f_count)
}
 

RC callPageReplacement(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum){

       FrameDetail *fr;

       fr = (FrameDetail *)malloc(sizeof(FrameDetail));

       fr->data = (SM_PageHandle) malloc(PAGE_SIZE);

    openPageFile (bm->pageFile, &fh);

    readBlock(pageNum, &fh, fr->data);

    MOVE(fr->pageNumber,pageNum)

    MOVE(fr->isDirty,0)

    MOVE(fr->C_count,1)

    MOVE(fr->No_Replace,0)

    MOVE(page->pageNum,pageNum)

    MOVE(page->data,fr->data)

    bm->readcount++;

    count++;

    int replacementAlgo = bm->strategy;

 

 

       if(replacementAlgo == RS_FIFO){

        FIFO(bm,fr);

    }

    else if (replacementAlgo == RS_LRU){

                  MOVE(fr->f_count,count)

                    LRU(bm,fr);

    }else{

        return RC_NULL_ERROR;

    }

}

/***************************************************************

this function pins the page with different replacement strategy (FIFO/LRU)

*******************************************************************/

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)

{

  if(page == NULL || bm == NULL )

    return RC_PIN_PAGE_PARAMETER_ERROR;

      // if bm.mgmtData initial element's pageNumber is -1,

    //then control enters if condition

    if(((FrameDetail *)bm->mgmtData)[0].pageNumber == -1)

    {

         setPageFromFile(bm, 0, page, pageNum);

        bm->readcount = 0; // initialize readCount value to 0

        count = 0; //initialize count to 0

        ((FrameDetail *)bm->mgmtData)[0].f_count = count; //assign count to f_count

     return RC_OK;

    }

    else

    {

       int i=0;

       int buffer_check = 0;

       for(i=0;i<bm -> numPages;i++)

       {

         if(((FrameDetail *)bm->mgmtData)[i].pageNumber != -1)

         {

               if(((FrameDetail *)bm->mgmtData)[i].pageNumber == pageNum)

               {

             ((FrameDetail *)bm->mgmtData)[i].C_count++;

                    MOVE(buffer_check,1)

                    count++;

                  if(bm->strategy == RS_LRU)

               ((FrameDetail *)bm->mgmtData)[i].f_count = count;

                  MOVE(page->pageNum,pageNum)

                  page->data = ((FrameDetail *)bm->mgmtData)[i].data;

               break;

               }

         }

         else

         {

            setPageFromFile(bm, i, page, pageNum);

             bm->readcount++;

             count++;

                 if(bm->strategy == RS_LRU)

               ((FrameDetail *)bm->mgmtData)[i].f_count = count;

               buffer_check = 1;

               break;

         }

       }

 

 

         if(buffer_check == 0)

     callPageReplacement(bm, page, pageNum);

         return RC_OK;

      }

}

 

void setPageFromFile(BM_BufferPool *const bm, int i, BM_PageHandle *const page, const PageNumber pageNum)

{

              MOVE(fPtr,(FrameDetail *)bm->mgmtData)

       openPageFile (bm->pageFile, &fh);

   MOVE(fPtr[i].data,(SM_PageHandle) malloc(PAGE_SIZE))

   readBlock(pageNum, &fh, fPtr[i].data);

   MOVE(fPtr[i].pageNumber,pageNum)

  MOVE(fPtr[i].C_count,1)

   MOVE(fPtr[i].No_Replace,0)

   MOVE(page->pageNum,pageNum)

  MOVE(page->data,fPtr[i].data)

}

 


 

/************************************************
  this method returns the frame contents
************************************************/
PageNumber *getFrameContents(BM_BufferPool *const bm)
{
int i; int pages = bm->numPages;
CREATE_FRAME(PageNumber,pageNo,sizeof(int) * pages);
FOR_LOOP(i,i<pages,pageNo[i] = ((FrameDetail *)bm->mgmtData)[i].pageNumber;)
return pageNo;
}
	

/************************************************
  this method returns a boolean array of the dirty page flags
************************************************/
bool *getDirtyFlags (BM_BufferPool *const bm)
{
if(bm == NULL){
return (bool *)RC_INVALID_BM;
}
int pages = bm->numPages;
int i;     
CREATE_FRAME(bool,dirtyPage,pages)
for(i=0;i<pages;i++)
{
if(((FrameDetail  *)bm->mgmtData)[i].isDirty == 1)
{
dirtyPage[i]= 1;
}
else
{
dirtyPage[i]=0;
}
}
return dirtyPage;
}
 
 
/************************************************
 this method returns fix count of all the page
************************************************/
int *getFixCounts (BM_BufferPool *const bm)
{
    int i=0;
    int pages = bm->numPages;
    CREATE_FRAME(int,fixCount,pages)
    FOR_LOOP(i,i<pages,fixCount[i] = ((FrameDetail  *)bm->mgmtData)[i].C_count;)
return fixCount;
}
 
/************************************************
  this method return number of time file read
*************************************************/

int getNumReadIO (BM_BufferPool *const bm)
{
int readCount = bm->readcount+1;
CHECK_DO(readCount < 0,return RC_READ_COUNT_ERROR;) 
}
 
/************************************************
  this method return number of time file write
************************************************/
int getNumWriteIO (BM_BufferPool *const bm)
{
int writeCount = bm->writecount;
CHECK_DO(writeCount < 0,return RC_WRITE_COUNT_ERROR;) 
}


