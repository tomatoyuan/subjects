/**
 * @author See Contributors.txt for code contributors and overview of BadgerDB.
 *
 * @section LICENSE
 * Copyright (c) 2012 Database Group, Computer Sciences Department, University of Wisconsin-Madison.
 */

#include <memory>
#include <iostream>
#include "buffer.h"
#include "exceptions/buffer_exceeded_exception.h"
#include "exceptions/page_not_pinned_exception.h"
#include "exceptions/page_pinned_exception.h"
#include "exceptions/bad_buffer_exception.h"
#include "exceptions/hash_not_found_exception.h"

namespace badgerdb {
	/*
	* This is the class constructor. Allocates an array for the buffer pool with bufs page frames and a
	* corresponding BufDesc table. The way things are set up all frames will be in the clear state when the
	* buffer pool is allocated. The hash table will also start out in an empty state. We have provided the
	* constructor.
	*/
    BufMgr::BufMgr(std::uint32_t bufs)
            : numBufs(bufs) {
        bufDescTable = new BufDesc[bufs];

        for (FrameId i = 0; i < bufs; i++) {
            bufDescTable[i].frameNo = i;
            bufDescTable[i].valid = false;
        }

        bufPool = new Page[bufs];

        int htsize = ((((int) (bufs * 1.2)) * 2) / 2) + 1;
        hashTable = new BufHashTbl(htsize);  // allocate the buffer hash table

        clockHand = bufs - 1;
    }

	/*
	* Flushes out all dirty pages and deallocates the buffer pool and the BufDesc table.
	*/
    BufMgr::~BufMgr() {
		for (FrameId i = 0; i < numBufs; i++) {
			if (bufDescTable[i].dirty) {
				bufDescTable[i].file->writePage(bufPool[i]); // write back disk
			}
		}
		delete[] bufPool;
		delete[] bufDescTable;
		delete hashTable;
    }

	/*
	* Advance clock to next frame in the buffer pool.
	*/
    void BufMgr::advanceClock() {
		clockHand = (clockHand + 1) % numBufs;
    }

	/*
	* Allocates a free frame using the clock algorithm; if necessary, writing a dirty page back to disk. Throws
	* BufferExceededException if all buffer frames are pinned. This private method will get called by the
	* readPage() and allocPage() methods described below. Make sure that if the buffer frame allocated
	* has a valid page in it, you remove the appropriate entry from the hash table.
	*/
    void BufMgr::allocBuf(FrameId &frame) {
		uint32_t pin_tot = 0;
		// (pin_tot < numBufs) --> Throws BufferExceededException if all buffer frames are pinned.
		for (uint32_t i = 0; i < numBufs || pin_tot < numBufs; i++) {
			// advance clock pointer
			advanceClock();
			// Valid set?
			if (!bufDescTable[clockHand].valid) {
				frame = clockHand;
				return ;
			}
			// refbit set?
			if (bufDescTable[clockHand].refbit) {
				// update recently not be ref
				bufDescTable[clockHand].refbit = false; 
				continue;
			}
			// Page pinned?
			if (bufDescTable[clockHand].pinCnt) {
				// 统计被 pin 的缓冲帧个数
				pin_tot++;
				continue;
			}
			// 如果有没有被 pin 的缓冲帧，则有机会成功分配缓冲帧，将pin_tot标志位清空
			pin_tot = 0;
			// Dirty bit set?
			if (bufDescTable[clockHand].dirty) {
				bufDescTable[clockHand].file->writePage(bufPool[clockHand]);
				bufDescTable[clockHand].dirty = false;
			}
			// Call "Set()" on the Frame
			hashTable->remove(bufDescTable[clockHand].file,
							  bufDescTable[clockHand].pageNo);
			bufDescTable[clockHand].Clear();
			// Use Frame
			frame = clockHand;
			return ;
		}
		// throw BufferExceededException
		throw BufferExceededException();
    }

	/*
	* First check whether the page is already in the buffer pool by invoking the lookup() method, which
	* may throw HashNotFoundException when page is not in the buffer pool, on the hashtable to get a
	* frame number. There are two cases to be handled depending on the outcome of the lookup() call:
	* 	– Case 1: Page is not in the buffer pool. Call allocBuf() to allocate a buffer frame and then call
	* 	the method file->readPage() to read the page from disk into the buffer pool frame. Next, insert
	* 	the page into the hashtable. Finally, invoke Set() on the frame to set it up properly. Set() will
	* 	leave the pinCnt for the page set to 1. Return a pointer to the frame containing the page via the
	* 	page parameter.
	* 	– Case 2: Page is in the buffer pool. In this case set the appropriate refbit, increment the pinCnt
	* 	for the page, and then return a pointer to the frame containing the page via the page parameter.
	*/
    void BufMgr::readPage(File *file, const PageId pageNo, Page *&page) {
		FrameId frame;
		try {
			// page is already in the buffer pool
			hashTable->lookup(file, pageNo, frame);
			bufDescTable[frame].refbit = true;
			bufDescTable[frame].pinCnt++;
		} catch(HashNotFoundException&) { // Page is not in the buffer pool
			// allocate a buffer frame
			allocBuf(frame);
			// read the page from disk into the buffer pool
			bufPool[frame] = file->readPage(pageNo);
			// insert the page into the hashtable
			hashTable->insert(file, pageNo, frame);
			// set the frame up properly
			bufDescTable[frame].Set(file, pageNo);
		}
		page = bufPool + frame;
    }

	/*
	* Decrements the pinCnt of the frame containing (file, PageNo) and, if dirty == true, sets the dirty
	* bit. Throws PAGENOTPINNED if the pin count is already 0. Does nothing if page is not found in the
	* hash table lookup.
	*/
    void BufMgr::unPinPage(File *file, const PageId pageNo, const bool dirty) {
		FrameId frame;
		try {
			hashTable->lookup(file, pageNo, frame);
			if (bufDescTable[frame].pinCnt == 0) {
				throw PageNotPinnedException(file->filename(), pageNo, frame);
			}
			bufDescTable[frame].pinCnt--;
			if (dirty) {
				bufDescTable[frame].dirty = true;
			}
		} catch(HashNotFoundException&) {
			// Oh no! It's so dangerous!
		}
    }

	/*
	* Should scan bufTable for pages belonging to the file. For each page encountered it should: (a) if the
	* page is dirty, call file->writePage() to flush the page to disk and then set the dirty bit for the page
	* to false, (b) remove the page from the hashtable (whether the page is clean or dirty) and (c) invoke
	* the Clear() method of BufDesc for the page frame.
	* Throws PagePinnedException if some page of the file is pinned. Throws BadBufferException if an
	* invalid page belonging to the file is encountered.
	*/
    void BufMgr::flushFile(const File *file) {
		// scan bufTable for pages
		for (uint32_t i = 0; i < numBufs; i++) {
			// belonging to the file
			if (bufDescTable[i].file == file) {
				// more safe
				// a buffer is found whose valid is false
				if (!bufDescTable[i].valid) {
					throw BadBufferException(i, 
											 bufDescTable[i].dirty,
											 bufDescTable[i].valid, 
											 bufDescTable[i].refbit);
				} 
				// a page which is not expected to be pinned in the buffer pool is found to be pinned
				if (bufDescTable[i].pinCnt) {
					throw PagePinnedException(file->filename(),
											  bufDescTable[i].pageNo, 
											  i);
				}

				// (a) flush the page to disk
				if (bufDescTable[i].dirty) {
					bufDescTable[i].file->writePage(bufPool[i]);
					bufDescTable[i].dirty = false;
				}
				// (b) remove the page from the hashtable
				hashTable->remove(file, bufDescTable[i].pageNo);
				// (c) invoke the Clear() method of BufDesc for the page frame
				bufDescTable[i].Clear();
			}
		}
    }

	/*
	* The first step in this method is to to allocate an empty page in the specified file by invoking the
	* file->allocatePage() method. This method will return a newly allocated page. Then allocBuf()
	* is called to obtain a buffer pool frame. Next, an entry is inserted into the hash table and Set() is
	* invoked on the frame to set it up properly. The method returns both the page number of the newly
	* allocated page to the caller via the pageNo parameter and a pointer to the buffer frame allocated for
	* the page via the page parameter.
	*/
    void BufMgr::allocPage(File *file, PageId &pageNo, Page *&page) {
		FrameId frame;
		// obtain a buffer pool frame
		allocBuf(frame);
		// allocate an empty page in the specified file
		bufPool[frame] = file->allocatePage();
		// returns both the page number
		pageNo = bufPool[frame].page_number();
		hashTable->insert(file, pageNo, frame);
		bufDescTable[frame].Set(file, pageNo);
		// a pointer to the buffer frame allocated for the page via the page parameter
		page = bufPool + frame;
    }

	/*
	* This method deletes a particular page from file. Before deleting the page from file, it makes sure that
	* if the page to be deleted is allocated a frame in the buffer pool, that frame is freed and correspondingly
	* entry from hash table is also removed.
	*/
    void BufMgr::disposePage(File *file, const PageId PageNo) {
		FrameId frame;
		try {
			hashTable->lookup(file, PageNo, frame);
			hashTable->remove(file, PageNo);
			bufDescTable[frame].Clear();
		} catch(HashNotFoundException&) {
			// not found
		}
		file->deletePage(PageNo);
    }

    void BufMgr::printSelf(void) {
        BufDesc *tmpbuf;
        int validFrames = 0;

        for (std::uint32_t i = 0; i < numBufs; i++) {
            tmpbuf = &(bufDescTable[i]);
            std::cout << "FrameNo:" << i << " ";
            tmpbuf->Print();

            if (tmpbuf->valid == true)
                validFrames++;
        }

        std::cout << "Total Number of Valid Frames:" << validFrames << "\n";
    }

}
