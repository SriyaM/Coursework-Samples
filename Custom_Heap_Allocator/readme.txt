File: readme.txt
Author: Sriya Mantena
----------------------

1. One major design decision that I made when it came to the implicit heap allocator was to resize blocks in the myrealloc function. 
If the current block was big enough to accommodate the user’s reallocation t request the function checked if there was a possibility 
of splitting up the free block in order to waste less memory. This allows for better heap utilization. Another design decision I made 
was to use the first-fit method when searching for free blocks in the mymalloc function. I made this decision to allow large allocations
towards the end of the heap although this method may require having to iterate over more blocks as we start searching at the beginning of 
the heap.

My allocator would show strong performance in a scenario where there are several consecutive allocation calls and the user does not use 
the reallocate and free function as much. The utilization would be very high on these types of scripts. On the other hand, this heap allocator 
would not work very well in the case that the reallocate and free functions are used heavily. This is because there will be several splinters 
created near the beginning of the heap and these free blocks cannot be coalesced within the scope of the implicit allocator which leads to a 
low utilization percentage. One attempt that I made at optimizing the compiler was the resize blocks in place in the myrealloc function. This 
ensured that if the client realloc a pointer to a much smaller size, the rest of the current block would not be wasted. The implicit allocator 
splits this block and labels it as free so that the user can use this block in consecutive function calls.

2. We assume that clients will only use myfree on heap-allocated pointers and we assume that clients will only use myrealloc on heap-allocated pointers. 
If the client tried to free a pointer that was not on the heap, the myfree function would alter memory (the header) that is not on the heap which could 
affect other variables that the client is working with on the stack. It would alter the ‘header’ of the pointer and change memory before and after 
(if coalescing) the inputted pointer. Additionally, if a client called the myrealloc function on a pointer that is not located on the heap, the myrealloc 
function would change memory that is meant to be changed outside the scope of the client's request which will cause undefined behavior in their program. 
It could change the values of variables, corrupt other functions, or cause a segfault.



Tell us about your quarter in CS107!
-----------------------------------
Although this course was challenging, I enjoyed it a lot, and feel like I have grown immensely as a programmer. I’m looking forward to taking more CS classes.



