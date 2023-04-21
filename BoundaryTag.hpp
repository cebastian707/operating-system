#ifndef _BoundaryTag_hpp
#define _BoundaryTag_hpp

class BoundaryTag {
    enum { SIZE = 4096, BYTES_PER_WORD = sizeof(int), FREE_OVERHEAD = 4 };
public:
    BoundaryTag();
    void* allocate( int numBytes ); // allocate a block of memory with "numBytes" bytes
    void free( void* ptrToMem );    // recycle the memory that "ptrToMem" points to.

    // The following two functions, "start" and "next", together make
    // it possible to scan the entire memory, block by block, starting
    // with the first block. Here is how they could be used:
    // ...
    // BoundaryTag *bt = new BoundaryTag();
    // ...
    // int totalBytes = 0;
    // bt->start();
    // while( void *ptrToMemBlock = bt->next() )
    //     totalBytes += bt->size( ptrToMemBlock );
    // ...

    // The statement "bt->start()" indicates the user's intention to
    // iterate over the memory blocks. After this call, successive
    // calls to "bt->next()" should return pointers to the adjacent
    // memory bocks, starting with the first block. "bt->next()"
    // returns NULL to indicate that the scan is complete. That is,
    // all memory blocks have been returned to the user through calls
    // to "bt->next(). Member-function "size", given a pointer to a
    // memory block that "next()" returns, provides the size of that
    // block. This is the actual block size including the tags and the
    // over-allocation bytes, if any. Variable "totalBytes" in the
    // previous code-segment should contains the size of the memory
    // pool, in bytes, after the while-loop. Refer to the drivers that
    // you have been given for more applications of these functions.

    void start();
    void* next();
    bool isFree( void* ptrToMem );
    int size( void* ptr);

private:
    int memory[ SIZE ];
    int freeIdx;
    int iterIdx;

    // You are not required to implement the following functions.
    void addToFreeList( void* ptrToMem, int size );
    void deleteFromFreeList( void* ptrToMem );
    int* makeBusy( void* ptr, int numBytes );
    int bytesToInt( int );
    void makeFree( int*, int );
};
#endif