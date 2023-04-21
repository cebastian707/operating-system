#include "BoundaryTag.hpp"
#include <iostream>


BoundaryTag::BoundaryTag(){
    // initialize memory array with zeros
    memset(memory, 0, SIZE * BYTES_PER_WORD);

    // set boundary tags
    memory[0] = -1 * SIZE;
    memory[SIZE - 1] = -1 * SIZE;

    // initialize pointers for free blocks
    memory[1] = -1;
    memory[2] = -1;

    freeIdx = 0;
    iterIdx = 0;

    for (int i = 0; i < SIZE; i += 4) {
        memory[i] = 16;
    }

}


void* BoundaryTag::allocate(int numBytes) {
    // Compute the number of words needed to store numBytes
    int numWords = (numBytes + BYTES_PER_WORD - 1) / BYTES_PER_WORD + 2;  // add 2 words for boundary tags

    // Find a free block with enough words
    int prevFreeIdx = 0, currFreeIdx = memory[1];
    while (currFreeIdx != -1) {
        if (memory[currFreeIdx] >= numWords) {
            break;  // found a free block that is large enough
        }
        prevFreeIdx = currFreeIdx;
        currFreeIdx = memory[currFreeIdx + 1];  // move to the next free block
    }

    if (currFreeIdx == -1) {
        return nullptr;  // no free block is large enough
    }

    // Split the free block if there are enough words left for a new free block
    int leftoverWords = memory[currFreeIdx] - numWords;
    if (leftoverWords >= 2) {  // need at least 2 words for a new free block
        int newFreeIdx = currFreeIdx + numWords;
        memory[newFreeIdx] = leftoverWords;
        memory[newFreeIdx + leftoverWords - 1] = leftoverWords;
        if (currFreeIdx == memory[1]) {
            memory[1] = newFreeIdx;  // update head of free block list if necessary
        }
        else {
            memory[prevFreeIdx + 1] = newFreeIdx;  // link the previous block to the new block
        }
    }
    else {
        numWords += leftoverWords;  // include the leftover words in the allocated block
    }

    // Allocate the block by updating the boundary tags
    memory[currFreeIdx] = -numWords;
    memory[currFreeIdx + numWords - 1] = -numWords;

    // Update the head of the free block list if necessary
    if (currFreeIdx == memory[1]) {
        memory[1] = memory[currFreeIdx + 1];
    }
    else {
        memory[prevFreeIdx + 1] = memory[currFreeIdx + 1];
    }

    return &(memory[currFreeIdx + 1]);  // return a pointer to the user block
}

void BoundaryTag::free(void* ptrToMem) {
    if (ptrToMem == nullptr) {
        return;  // do nothing if ptrToMem is nullptr
    }

    int blockStartIdx = (int*)ptrToMem - memory - 1;

    // Check if the block to free is adjacent to the left boundary tag
    bool mergeLeft = false;
    int leftBlockEndIdx = blockStartIdx - 1;
    if (leftBlockEndIdx >= 0 && memory[leftBlockEndIdx] > 0) {
        leftBlockEndIdx -= memory[leftBlockEndIdx] - 1;
        mergeLeft = true;
    }

    // Check if the block to free is adjacent to the right boundary tag
    bool mergeRight = false;
    int rightBlockStartIdx = blockStartIdx + memory[blockStartIdx] - 1;
    if (rightBlockStartIdx < SIZE && memory[rightBlockStartIdx] > 0) {
        mergeRight = true;
    }

    // Update the boundary tags of the block to free
    int blockWords = abs(memory[blockStartIdx]);
    memory[blockStartIdx] = -blockWords;
    memory[blockStartIdx + blockWords - 1] = -blockWords;

    // Coalesce with adjacent blocks if necessary
    if (mergeLeft && mergeRight) {
        // Merge with both left and right blocks
        int leftBlockWords = abs(memory[leftBlockEndIdx]);
        int rightBlockWords = abs(memory[rightBlockStartIdx]);
        int newBlockWords = blockWords + leftBlockWords + rightBlockWords;
        memory[leftBlockEndIdx - leftBlockWords + 1] = newBlockWords;
        memory[rightBlockStartIdx] = newBlockWords;
    } else if (mergeLeft) {
        // Merge with left block
        int leftBlockWords = abs(memory[leftBlockEndIdx]);
        int newBlockWords = blockWords + leftBlockWords;
        memory[leftBlockEndIdx - leftBlockWords + 1] = newBlockWords;
        memory[blockStartIdx + newBlockWords - 1] = -newBlockWords;
    } else if (mergeRight) {
        // Merge with right block
        int rightBlockWords = abs(memory[rightBlockStartIdx]);
        int newBlockWords = blockWords + rightBlockWords;
        memory[blockStartIdx] = -newBlockWords;
        memory[rightBlockStartIdx] = newBlockWords;
    } else {
        // Add to free list
        int freeListIdx = 1;
        while (memory[freeListIdx] != -1 && freeListIdx < SIZE) {
            freeListIdx += abs(memory[freeListIdx]) / BYTES_PER_WORD;
        }
        if (freeListIdx < SIZE) {
            memory[freeListIdx] = blockWords;
            memory[blockStartIdx + blockWords] = freeListIdx * BYTES_PER_WORD;
            memory[blockStartIdx + blockWords + 1] = -1;
        }
    }
}




void BoundaryTag::start() {
    iterIdx = 0;
}



void* BoundaryTag::next() {
    // Check if there are any more blocks to iterate over
    if (iterIdx == -1) {
        return nullptr;
    }

    // Save the current block's index before moving to the next block
    int prevIdx = iterIdx;

    // Move to the next block
    iterIdx += abs(memory[iterIdx]) / BYTES_PER_WORD;

    // Check if we've reached the end of the memory pool
    if (iterIdx >= SIZE || iterIdx < 0) {
        iterIdx = -1;
    }

    // Return a pointer to the start of the current block
    return &(memory[prevIdx]);
}




bool BoundaryTag::isFree( void* ptrToMem ){
    int* blockStart = (int*)ptrToMem - 1;
    return (*blockStart % 2) == 0;
}



int BoundaryTag::size(void* ptr){
    return *(int*)ptr;
}


