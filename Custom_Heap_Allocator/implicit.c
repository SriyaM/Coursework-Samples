/*
 * Code by Sriya Mantena
 * Supplemented by CS107a starter code:
 * Used helper functions and framing provided by Andrew Benson
 *
 * This file contains several helper functions, a myinit, mymalloc, myfree, myrealloc,
 * validate_heap, and dump_heap function. The validate_heap and dump_heap functions are
 * used to debug and ensure that the internal state of the heap is valid. The rest of the
 * function are an implementation of an implicit heap structure which tracks block
 * size and status information on the heap. This implementation of the heap recycles free
 * blocks suing a first-fit approach. Additionally the mymalloc function accomodates in place
 * reallocations.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include "./allocator.h"
#include "./debug_break.h"

#define ALLOC 0
#define FREE 1
#define STATE_MASK 0x7

// Initializing global variables
typedef size_t header_t;
header_t *h_start;
header_t *h_end;


// making prototypes for helper functions
void set_size(header_t *header, size_t size);
size_t get_size(header_t *header);
void set_free(header_t *header);
void set_alloc(header_t *header);
bool is_free(header_t *header);
header_t *payload2header(void* payload);
void *header2payload(header_t *header);
header_t *next_block(header_t *header);
size_t round_up(size_t size);

/*
 * This helper function takes in a pointer to a header and a size. It
 * sets the size of the header by modifying everything except the last
 * three bits. This function does not return anything.
 */
void set_size(header_t *header, size_t size) {
    size_t allocation_bits = (*header) & STATE_MASK;  // maintains the state of the allocated bits
    *header = size | allocation_bits;
}

/*
 * This helper function takes in a pointer to a header and return the size
 * of the payload by zeroing out the last 3 significant bits.
 */
size_t get_size(header_t *header) {
    return ((*header) & (~STATE_MASK));
}

/*
 * This helper function takes in a pointer to a header and sets the last
 * bit of the header to indicate that the correpsponding payload is 
 * free. This function does not return anything.
 */
void set_free(header_t *header) {
    *header |= FREE;
}

/*
 * This helper function takes in a pointer to a header and sets the
 * last bit of the header to indicate that the corresponding payload
 * is allocated. This function does not return anything.
 */
void set_alloc(header_t *header) {
    *header &= ~1;
}

/*
 * This function takes in a pointer to a header and returns true
 * or false based on whether the least significant bit indicates that
 * the associated payload is free.
 */
bool is_free(header_t *header) {
    return ((*header) & 1) == FREE;
}

/*
 * This function takes in a pointer to a payload and returns
 * a pointer to the corresponding header.
 */
header_t *payload2header(void* payload) {
    return (header_t *)((char*)payload - sizeof(header_t));
}

/*
 * This function takes in a pointer to a header and returns
 * a pointer to the corresponding payload.
 */
void *header2payload(header_t *header) { 
    return ((char*)header + sizeof(header_t));
}

/*
 * This function takes in a pointer to a header and returns a
 * pointer to the next block on the heap.
 */
header_t *next_block(header_t *header) {
    return (header_t *)((char*)header2payload(header)+ get_size(header));
}

/*
 * This function takes in a size of a payload and rounds it up
 * based on the alignment of the heap.
 */
size_t round_up(size_t size) {
    return ((size + ALIGNMENT - 1) & ~(ALIGNMENT - 1));
}

/*
 * This function initializes the heap based on a pointer that points to the beginning
 * of the heap and a specified heap size. The function initializes global variables
 * and creates a header for the heap block. The function returns true if the initialization
 * was successful and false otherwise. This function can be used to reset the heap to 
 * an empty state.
 */
bool myinit(void *heap_start, size_t heap_size) {
    if (heap_size <= sizeof(header_t)) {
        return false;
    }

    h_start = (header_t *) heap_start;
    h_end = (header_t *) ((char*)heap_start + heap_size);

    set_size(h_start, heap_size - sizeof(header_t));
    set_free(h_start);

    return true;
}

/*
 * This function takes in a size in bytes from the user. The function accomodates
 * an allocation request of this size by allocating a block greater than or 
 * equal to the size on the heap. The function returns NULL if the request size is
 * zero, greater than the maximum request size, or if the request could not be
 * accomodated. Otherwise, the function returns a pointer to the newly allocated
 * block on the heap.
 */
void *mymalloc(size_t requested_size) {
    if (requested_size == 0 || requested_size > MAX_REQUEST_SIZE) {
        return NULL;
    }
    size_t new_size = round_up(requested_size);
    header_t *curr = h_start;
    while (curr != h_end) {
        size_t curr_size = get_size(curr);
        if (is_free(curr) && curr_size >= new_size) {  // if this block is free and big enough
            if (curr_size - new_size <= sizeof(header_t)) {  // seeing if the block is big enough to split
                new_size = curr_size;  // if the block is not big enough to split, give the user extra bytes
            } else {  // split the block and make a new header
                header_t *new_block_ptr = (header_t *) ((char*)header2payload(curr) + new_size);
                set_size(new_block_ptr, curr_size - sizeof(header_t) - new_size);
                set_free(new_block_ptr);
            }
            set_size(curr, new_size);
            set_alloc(curr);
            return header2payload(curr);
        }
        curr = next_block(curr);
    }
    return NULL;  // the whole heap was iterated through (no free block big enough for request).
}

/*
 * This function sets the block specified by the inputted ptr to free.
 * This function does not return anything.
 */
void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    set_free(payload2header(ptr));
}

/*
 * This function takes in a ptr to a previosuly allocated block on the 
 * heap and new size request. If the new size request can fit within the 
 * block specified by the old ptr, the function split the block if there
 * is excess bytes and returns the old ptr. Otherwise, the function copies
 * the old data to a new location on the heap and frees the old ptr. If the
 * inputted old_ptr is NULL, the function call is equivalent to mymalloc
 * and if the size is equal to zero (and the old_ptr is not NULL), the function
 * simply frees the old pointer and returns NULL.
 */
void *myrealloc(void *old_ptr, size_t new_size) {
    new_size = round_up(new_size);
    // edge cases
    if (old_ptr == NULL) {
        return mymalloc(new_size);
    }
    // edge case
    if (old_ptr != NULL && new_size == 0) {
        myfree(old_ptr);
        return NULL;
    }
    // If the current block is bg enough, return the same pointer
    header_t *old_head = payload2header(old_ptr);
    int leftover_bytes = get_size(old_head) - new_size;
    if (leftover_bytes >= 0) {
        if (leftover_bytes > sizeof(header_t)) {  // if it is worth splitting the current block
            header_t *new_block_ptr = (header_t *) ((char*)header2payload(old_head) + new_size);
            set_size(new_block_ptr, get_size(old_head) - sizeof(header_t) - new_size);
            set_free(new_block_ptr);
            set_size(old_head, new_size);
        }
        return old_ptr;
    }
    // Find a new block on the heap of sufficient size
    void* new_ptr = mymalloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    memcpy(new_ptr, old_ptr, new_size);
    myfree(old_ptr);
    return new_ptr;
}

/*
 * This function iterates through the heap and validates it's internal state.
 * It checks that the information contained within the headers makes sense, that
 * the data is aligned, and that the total bytes in the header matches the total
 * bytes that are allocated. If the end of the heap is not reached by following
 * the size values in the headers, a error is also raised. If none of these errors
 * are raised the function returns true, otherwise, it returns false.
 */
bool validate_heap() {
    size_t total_used = 0;
    header_t *curr = h_start;
    while (curr < h_end) {
        if (get_size(curr) > (char*)h_end - (char*)h_start) {  // checking that size in header makes sense
            printf("the size of the header at location %p is invalid\n", curr);
            breakpoint();
            return false;
        }
        if (!is_free(curr) && get_size(curr) > MAX_REQUEST_SIZE) {
            printf("the size of the allocated block %p is invalid", curr);
            breakpoint();
            return false;
        }
        if ((uintptr_t)curr % ALIGNMENT != 0) {  // verifying alignment
            printf("the alignment of the heap is off at %p\n", curr);
            breakpoint();
            return false;
        }
        if (!is_free(curr)) {
            total_used += get_size(curr);  // counting total bytes used 
        }
        curr = next_block(curr);
    }
    if (curr != h_end) {  // if the end of the heap is not reached by following headers
        printf("%s", "the whole heap is not accounted for\n");
        breakpoint();
        return false;
    }
    if (total_used > (char*)h_end - (char*)h_start) {  // checking that a reasonable number of bytes is allocated
        printf("There are too many bytes allocated\n");
        breakpoint();
        return false;
    }
    return true;
}

/* Function: dump_heap
 * -------------------
 * This function prints out the the block contents of the heap.  It is not
 * called anywhere, but is a useful helper function to call from gdb when
 * tracing through programs.  It prints out the total range of the heap, and
 * information about each block within it.
 */
void dump_heap() {
    header_t *curr = h_start;
    while (curr != h_end) {
        printf("%p %s Size: %zu\n", curr, is_free(curr)? "FREE" : "ALLOC:", get_size(curr));
        curr = next_block(curr);
    }
}
