/*
 * Code by Sriya Mantena
 * Supplemented by CS107a starter code:
 * Used helper function and framing provided by Andrew Benson
 *
 * Thsi file contains several helper function, a myinit, my malloc, myfree, my realloc,
 * validate_heap, and dump_heap function. The validate_heap and dump_heap functions are
 * used to debug and ensure that the inernal state of the heap along with the explicit doubly
 * linked list are valid. The rest of the functions are an impementation of an explicit heap allocator.
 * The structure uses block headers to store information about the size and status of the consequent
 * payload. The function recycles free blocks and uses a doubly linked list to keep track 
 * of the free blocks. The mymalloc function only searches the explicit list of free blocks and supports block 
 * splitting. The myrealloc function resizes a block in place by splitting and coalesces all the free blocks
 * to the right of the inputted pointer. The function frees the inputted pointer and coalesces one block to 
 * the right if it is free.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./allocator.h"
#include "./debug_break.h"

#define ALLOC 0
#define FREE 1
#define STATE_MASK 0x7
#define MINIMUM_BLOCK_SIZE 24

typedef size_t header_t;

// initializing global variables
header_t *h_start;
header_t *h_end;
struct node *list_start_ptr;  // pointer to nested prev and next pointers in free block

// this node is nested in the payload of free blocks
struct node {
    struct node *prev;
    struct node *next;
};

// helper functions

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
 * This function inserts a node into the doubly linked list of free
 * blocks. It covers the cases in which there is nothing in the list
 * and the case in which there are blocks that are already in the list.
 */
void add_free_block(struct node *new_free_payload) {
    // empty list case
    if (list_start_ptr == NULL) {
        new_free_payload->next = NULL;
        list_start_ptr = new_free_payload;
        new_free_payload->prev = NULL;

    } else  {
        // add block to the beginning of the list
        list_start_ptr->prev = new_free_payload;
        new_free_payload->next = list_start_ptr;
        new_free_payload->prev = NULL;
        list_start_ptr = new_free_payload;
    }
}

/*
 * This function removes a free block from the doubly linked list of blocks
 * and cover the cases in which there is only one node in the list, that the
 * node is at the beginning of the list, that the node is in the middle of the list
 * and that the node is at the end of the list.
 */
void detach_free_block(struct node *free_payload) {
    // only node case:
    if (free_payload->prev == NULL && free_payload->next == NULL) {
        list_start_ptr = NULL;
    }
    // beginning of the list
    if (free_payload->prev == NULL && free_payload->next!= NULL) {
        free_payload->next->prev = NULL;
        list_start_ptr = free_payload->next;
    }
    // >1 node, end of list
    if (free_payload->next == NULL && free_payload->prev != NULL) {
        free_payload->prev->next = NULL;
    }
    // general case - in the middle
    if (free_payload->prev != NULL && free_payload->next != NULL) {
        free_payload->prev->next = free_payload->next;
        free_payload->next->prev = free_payload->prev;
    }
}

/*
 * This helper function taks in a pointer to a left payload and
 * right payload and coaslesces them into one block. The status of the
 * block (free of allocated) will match the state of the left block.
 * This function does not coalesce the blocks in the explicit list.
 * This function does not return anything.
 */
void coalesce_block_right(header_t *left, header_t *right) {
    header_t *right_head = payload2header(right);
    size_t right_size = get_size(right_head);
    header_t *left_head = payload2header(left);
    size_t left_size = get_size(left_head);
    set_size(left_head, left_size + right_size + sizeof(header_t));
}


/*
 * This helper function takes in a pointer to a struct node on the left
 * and a struct node on the right. It coalesces these two nodes and the 
 * blocks in the heap's internal state. This function doesn't return anything.
 */
void coalesce_right(struct node *left, struct node *right) {
    coalesce_block_right((header_t *) left, (header_t *) right);
    detach_free_block(right);
}

/*
 * This funciton intializes the internal state of the heap by setting the initial
 * header block and initiatilizing the first pointer block to null. It also
 * handles handles the edge case in which the heap size is not big enough to
 * store the headers and linked list pointers.
 */
bool myinit(void *heap_start, size_t heap_size) {
    if (heap_size <= sizeof(header_t) + sizeof(struct node)) {
        return false;
    }

    h_start = (header_t *) heap_start;
    h_end = (header_t *) ((char*)heap_start + heap_size);

    set_size(h_start, heap_size - sizeof(header_t));
    set_free(h_start);

    // initialize the first node
    header_t *payload = header2payload(h_start);
    struct node* first_node = (struct node*) payload;
    first_node->prev = NULL;
    first_node->next = NULL;
    list_start_ptr = (struct node*) payload;

    return true;
}

/*
 * This function takes in a size_t which specifies that amount of bytes that
 * the client requests on the heap. It iterated through the doubly linked list 
 * of free blocks and returns a pointer to the block if it is big enough for the
 * clients request. If the free node has enough excess memory, the funcion resizes the
 * allocated block and sets the excess to free. The function returns a pointer to a
 * payload that is greater than or equal to the size of the client's request.
 */
void *mymalloc(size_t requested_size) {
    if (requested_size == 0 || requested_size > MAX_REQUEST_SIZE) {
        return NULL;
    }
    requested_size = round_up(requested_size);
    if (requested_size < MINIMUM_BLOCK_SIZE) {
        requested_size = MINIMUM_BLOCK_SIZE;
    }
    struct node *curr = list_start_ptr;
    while (curr != NULL) {  // iterate through all the free blocks
        size_t curr_size = get_size(payload2header((header_t *)curr));
        if (curr_size >= requested_size) {
            detach_free_block(curr);
            // checks if the block is big enough to split
            if (curr_size - requested_size > sizeof(header_t) + sizeof(struct node)) {
                // make a new header
                header_t *new_block_ptr = (header_t *) (((char *)curr) + requested_size);
                set_size(new_block_ptr, curr_size - sizeof(header_t) - requested_size);
                set_free(new_block_ptr);
                // add the new free block to the linked list
                add_free_block((struct node *)header2payload(new_block_ptr));
            } else {
                // if the requested size is not enough, give the excess to the user
                requested_size = get_size(payload2header(curr));
            }
            set_size(payload2header((header_t *) curr), requested_size);
            set_alloc(payload2header((header_t *)curr));
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

/*
 * This function takes in a pointer to a payload and frees the current block. If there
 * is a neighbhoring free block to the right, the function coalesces the newly freed block
 * with the block to its right. The function does not do anything is the inputted pointer
 * is equal to NULL.
 */
void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    set_free(payload2header(ptr));
    struct node *curr_node = (struct node *) ptr;
    add_free_block(curr_node);
    if (next_block(payload2header(ptr)) != h_end) {   // if the node to the right is free
        if (is_free(next_block(payload2header(ptr)))) {
            coalesce_right(curr_node, (struct node*)header2payload(next_block(payload2header(ptr))));
    }
    }
}

/*
 * This function takes in a pointer to an allocated block on the heap along with a 
 * new requested size. Th function returns the same pointer if there is enough space within
 * the current block or in neighbhoring free blocks (it coalesces neighboring pointers to the
 * right of the current pointer). If there is not enough space to accomodate the request after
 * the function allocates, the function calls the mymalloc function and copies the data
 * at the inputted pointer to a new location on the heap. The function returns NULL if the request
 * cannot be accomodated or if the new_size parameter is equal to zero. The function simply calls
 * my malloc if the inputted pointer is NULL.
 */
void *myrealloc(void *old_ptr, size_t new_size) {
    new_size = round_up(new_size);
    if (new_size < MINIMUM_BLOCK_SIZE) {
        new_size = MINIMUM_BLOCK_SIZE;
    }
    // edge cases
    if (old_ptr == NULL) {
        return mymalloc(new_size);
    }
    if (old_ptr != NULL && new_size == 0) {
        myfree(old_ptr);
        return NULL;
    }
    size_t curr_size = get_size(payload2header(old_ptr));

    // check if the request is smaller that the size
    if (curr_size >= new_size) {
        if (curr_size - new_size > sizeof(header_t) + sizeof(struct node)) {
            header_t *new_block_ptr = (header_t *) (((char *)old_ptr) + new_size);
            set_size(new_block_ptr, curr_size - sizeof(header_t) - new_size);
            set_free(new_block_ptr);
            add_free_block((struct node *)header2payload(new_block_ptr));
            set_size(payload2header(old_ptr), new_size);
        }
        return old_ptr;
    } else {
        // try coalescing free blocks and checking if theres enough space
        size_t total_size = curr_size;
        header_t *next = next_block(payload2header(old_ptr));
        while (next != h_end && is_free(next)) {
            coalesce_block_right(old_ptr, header2payload(next));
            detach_free_block((struct node *)header2payload(next));
            total_size = total_size + sizeof(header_t) + get_size(next);
            if (total_size >= new_size) {
                // call the myrealloc function again to check for block splitting
                myrealloc(old_ptr, new_size);
                return old_ptr;
            }
            next = next_block(next);
        }
    }
    // coalescing did not work -- must find new place on heap with malloc
    void * new_ptr = mymalloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    memcpy(new_ptr, old_ptr, new_size);
    myfree(old_ptr);
    return new_ptr;
}

/*
 * This function iterate through the heap and validates the iternal state.
 * It checks both the implicit and explicit list of blocks by validating
 * the alignment, the header information, the total size, and making sure
 * that the pointers of the doubly linked list point to appropriate places.
 * If there is an error, the function returns false and sets a breakpoint.
 * If no errors are found, the function returns true.
 */
bool validate_heap() {
    // validating the internal state of the implicit list
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
    // checking the internal state of the doubly linked list
    struct node *curr_node = list_start_ptr;
    // checking that the previous of the first list is set to null
    if (curr_node->prev != NULL) {
        breakpoint();
        printf("the previous of the first node is not set to NULL");
        return false;
    }
    while (curr_node->next != NULL) {
        if (!is_free(payload2header((header_t *)curr_node))) {
            printf("There is an allocated block in the linked list of free blocks");
            breakpoint();
            return false;
        }
        if (curr_node->next != NULL && curr_node->next->prev != curr_node) {
            printf("Error in linked list, next pointer of previous does not point to current");
            breakpoint();
            return false;
        }
        if (curr_node->next == curr_node || curr_node->prev == curr_node) {
            breakpoint();
            printf("Doubly linked list contains a self-loop");
            return false;
        }
        curr_node = curr_node->next;
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
        printf("%p %s Size: %zu\n", curr, is_free(curr)? "FREE" : "ALLOC", get_size(curr));
        curr = next_block(curr);
    }

    printf("explicit linked list\n");
    struct node *curr_node = list_start_ptr;
    while (curr_node != NULL) {
        printf("Curr Location: %p   Next: %p, Prev: %p, HeaderSize = %zu\n", curr_node, curr_node->next,
                curr_node->prev, get_size(payload2header((header_t *)curr_node)));
        curr_node = curr_node->next;
    }
}
