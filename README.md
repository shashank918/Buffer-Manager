# Buffer Manager


The following is the Description and Instructions for Running the code.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

1. For Running the code,

              a. Navigate to the directory where the programming Assignment is downloaded.

              b. Right click in the same directory and Open Terminal.

              c. Now execute make clean to clean any Object files that were already present.

              d. Type make -f Makefile to Run the make file and to complie the files.

              e. Run the command ./TestAssn2 to get the out put for the Programming Assignment

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

2. Page Replacement Strategies Used,

              a. We have implemented 2 Replacement Strategies in this Assignment

              b. FIFO and LRU

              c. The functions provided in the TestAssn2 shall be executing the respective Tests.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

3. Description for the Respective Functions:

 

       Name: markDirty

*****************************

Get the page handle.

Where the handle is returned mark the page/frame as dirty.


       Name: unpinPage

*****************************

Check if the page is dirty.

If the page is dirty, write change to file and it is unpinned from the buffer pool.


       Name: forcePage

******************************

Check the frame that should be written to file which is on disk.

The page contents that should be written to the file on disk is written.


       Name: pinPage

********************************

Check for an empty frame.

For the replacement find an empty page and use the replacement strategy, if not empty see if it is there in buffer already.

Add the required timestamp for the page.

By invoking the strategy we can get the corresponding page.

Replace that with new page and make necessary changes to the parameters of replacement.

 
       Name: InitBufferPool

********************************

Initialize bufferpool.

If the bufferpool already exists, it is shared with the new pool handler.


       Name: ShutDown BufferPool

*********************************

Check if there are any dirty page frame before shuttingdown.

If there are any dirty page it should be written to the disc first.

Then shutdown the bufferpool


       Name: ForceFlush BufferPool

**********************************

find and check the current buffer pool.

If there is any dirty page frame and its bit is 0 write that to the disc.


       Name: getFrameContents

************************************

Take input as the buffer pool and get the page frame for that buffer pool.

copy the page number of the frames into an array and return it.


       Name: getDirtyFlags

********************************************

Take input as the buffer pool and get the page frame for that buffer pool.

copy the dirty flags of the frames into an array and return it.

 
       Name: getNumReadIO

****************************************************

Take input as the buffer pool and get the page frame for that buffer pool.

Find the total read IO for the pool and return it.

 
       Name: getNumWriteIO

****************************************************

Take input as the buffer pool and get the page frame for that buffer pool.

Find the total write IO for the pool and return it.

 

       Name: getFixCounts

***********************************************

Take input as the buffer pool and get the page frame for that buffer pool.

copy the fixcounts of the frames into an array and return it.
