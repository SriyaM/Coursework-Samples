/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */

#include "pqheap.h"
#include "error.h"
#include "random.h"
#include "strlib.h"
#include "datapoint.h"
#include "testing/SimpleTest.h"
using namespace std;

const int INITIAL_CAPACITY = 10;
const int NONE = -1; // used as sentinel index

/*
 * The constructor initializes all of the member variables needed for
 * an instance of the PQHeap class. The allocated capacity
 * is initialized to a starting constant and a dynamic array of that
 * size is allocated. The number of filled slots is initially zero.
 */
PQHeap::PQHeap() {
    _numAllocated = INITIAL_CAPACITY;
    _elements = new DataPoint[_numAllocated]();
    _numFilled = 0;
}

/*
 * This deconsrtuctor deletes the memory that was
 * allocated to this instance of the PQHeap.
 */
PQHeap::~PQHeap() {
    delete[] _elements;
}

/*
 * This function adds a new datapoint, elem, to the PQHeap and
 * doesn't return anything. The function checks if the array needs to be
 * resized then adds the new element to the end of the array and increments
 * the size of the heap. It then swaps the new element with its parent until
 * it is situated in the right position in the heap (it is greater than its parent).
 */
void PQHeap::enqueue(DataPoint elem) {
    checkResize();
    _elements[size()] = elem;
    _numFilled ++;
    int currIndex = size()-1;
    while(getParentIndex(currIndex) != NONE && elem.priority < _elements[getParentIndex(currIndex)].priority){
        swap(currIndex,getParentIndex(currIndex));
        currIndex = getParentIndex(currIndex);
    }
}

/*
 * This function returns the DataPoint with the top priority.
 * This element is position at the zeroth index of the QP array.
 */
DataPoint PQHeap::peek() const {
    if (isEmpty()) {
        error("PHeap is empty!");
    }
    return _elements[0];
}

/*
 * This function returns the element with the top priority which
 * is positioned at the beginning of the QPHeap. It then moves the
 * bottom most element to the top position in the tree shifts this
 * element down until it is situated in the correct position in the
 * QPHeap (its priority is less than its left and right children).
 */
DataPoint PQHeap::dequeue() {
    DataPoint front = peek();
    _elements[0] = _elements[size()-1];
    _elements[size()-1] = {"DEQUEUED", -106};
    _numFilled --;
    int currIndex = 0;
    while (getLeftChildIndex(currIndex) != NONE){
        if (getRightChildIndex(currIndex) == NONE) {
            if (_elements[getLeftChildIndex(currIndex)].priority < _elements[currIndex].priority) {
                swap(getLeftChildIndex(currIndex), currIndex);
                currIndex = getLeftChildIndex(currIndex);
            }
            else {
                break;
            }
        }
        else if (_elements[getLeftChildIndex(currIndex)].priority < _elements[currIndex].priority && _elements[getLeftChildIndex(currIndex)].priority < _elements[getRightChildIndex(currIndex)].priority) {
            swap(getLeftChildIndex(currIndex), currIndex);
            currIndex = getLeftChildIndex(currIndex);
        }
        else if (_elements[getRightChildIndex(currIndex)].priority < _elements[currIndex].priority) {
            swap(getRightChildIndex(currIndex), currIndex);
            currIndex = getRightChildIndex(currIndex);
        }
        else {
            break;
        }
    }
    return front;
}

/*
 * This funnction checks to see if the QPHeap is empty
 * by returning whether the curren size is zero.
 */
bool PQHeap::isEmpty() const {
    return size() == 0;
}

/*
 * The count of enqueued elements is tracked in the
 * member variable _numFilled.
 */
int PQHeap::size() const {
    return _numFilled;
}

/*
 * Updates internal state to reflect that the queue is empty, e.g. count
 * of filled slots is reset to zero. The array memory stays allocated
 * at current capacity. The mark cleare position are
 * each marked with a sentinel value as a debugging aid.
 */
void PQHeap::clear() {
    for (int i = 0; i < size(); i++) {
        _elements[i] = { "CLEARED", -106 };
    }
    _numFilled = 0;
}

/*
 * Prints the contents of internal array for debugging purposes.
 */
void PQHeap::printDebugInfo(string msg) const {
    cout << msg << endl;
    for (int i = 0; i < size(); i++) {
        cout << "[" << i << "] = " << _elements[i] << endl;
}
}

/*
 * We strongly recommend implementing this helper function, which
 * should traverse the heap array and ensure that the heap property
 * holds for all elements in the array. If elements are found that
 * violate the heap property, an error should be thrown.
 */
void PQHeap::validateInternalState() const {
    if (_numFilled > _numAllocated) {
        error("Too many elements in not enough space!");
    }

    if(size() == 1){
        return;
    }

    //start from root and go to last node
    for (int i=0; i<=(size()-2)/2; i++) {
        //check if the left child exists (no gaps) and is smaller than the parent
        if (_elements[getLeftChildIndex(i)].priority < _elements[i].priority) {
            printDebugInfo("validateInternalState");
            error("PQArray has elements out of order at index " + integerToString(getLeftChildIndex(i)));
        }
        //check if the right child exists, and if it is smaller than the parent
        if (getRightChildIndex(i) != NONE && _elements[getRightChildIndex(i)].priority < _elements[i].priority) {
            printDebugInfo("validateInternalState");
            error("PQArray has elements out of order at index " + integerToString(getRightChildIndex(i)));
        }
    }

}

/*
 * Private member function. This helper function checks if the
 * number of filled elements equals the current capacity of the array.
 * If it does, it doubles the capacity of the array.
 */
void PQHeap::checkResize() {
    if (size() == _numAllocated) {
        DataPoint* bigger = new DataPoint[2*_numAllocated]();
        for (int i = 0; i < size(); i++) {
            bigger[i] = _elements[i];
        }
        delete[] _elements;
        _elements = bigger;
        _numAllocated *= 2;

    }
}

/*
 * Private member function. This helper exchanges the element at
 * indexA with the element at indexB.
 */
void PQHeap::swap(int indexA, int indexB) {
    DataPoint tmp = _elements[indexA];
    _elements[indexA] = _elements[indexB];
    _elements[indexB] = tmp;
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the parent of the
 * specified child index. If this child has no parent, return
 * the sentinel value NONE.
 */
int PQHeap::getParentIndex(int child) const {
    int parent = (child-1)/2;
    if (parent < 0 || parent >= size()) {
        return NONE;
    }
    return parent;
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the left child of the
 * specified parent index. If this parent has no left child, return
 * the sentinel value NONE.
 */
int PQHeap::getLeftChildIndex(int parent) const {
    int child = (parent * 2) + 1;
    if (child < 0 || child >= size()) {
        return NONE;
    }
    return child;
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the right child of the
 * specified parent index. If this parent has no right child, return
 * the sentinel value NONE.
 */
int PQHeap::getRightChildIndex(int parent) const {
    int child = (parent * 2) + 2;
    if (child < 0 || child >= size()) {
        return NONE;
    }
    return child;
}

/* * * * * * Test Cases Below This Point * * * * * */


STUDENT_TEST("PQHeap, Testing enqueue sorting with equal priority values") {
    PQHeap pq;

    pq.enqueue( { "", -7 } );
    pq.enqueue( { "", -7 } );
    pq.enqueue( { "",-7 } );
    pq.enqueue( { "",-7 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.validateInternalState();
    pq.printDebugInfo("After enqueuing all elements");
}

STUDENT_TEST("PQHeap, Testing enqueue with various edge cases") {
    PQHeap pq;

    pq.enqueue( { "", 0 } );
    pq.enqueue( { "", -150 } );
    pq.enqueue( { "", 8 } );
    pq.enqueue( { "", 8 } );
    pq.enqueue( { "", 8 } );
    pq.enqueue( { "", -149 } );
    pq.enqueue( { "", -149 } );
    pq.enqueue( { "", 432 } );
    EXPECT_EQUAL(pq.size(), 8);
    pq.validateInternalState();
    pq.printDebugInfo("After enqueuing all elements");
}

STUDENT_TEST("PQHeap basic enqueue, example from writeup") {
    PQHeap pq;

    pq.enqueue( { "Zoe", -3 } );
    pq.enqueue( { "Elmo", 10 } );
    pq.enqueue( { "Bert", 6 } );
    EXPECT_EQUAL(pq.size(), 3);
    pq.validateInternalState();
    pq.printDebugInfo("After enqueue 3 elements");

    pq.enqueue( { "Kermit", 5 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.validateInternalState();
    pq.printDebugInfo("After enqueue one more");
}

STUDENT_TEST("PQHeap mix of operations, validate state at every step") {
    PQHeap pq;
    EXPECT_ERROR(pq.peek());
    EXPECT_ERROR(pq.dequeue());
    pq.validateInternalState();
    pq.enqueue( { "", -3 } );
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 1);
    pq.clear();
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 0);
    pq.enqueue( { "", 10 } );
    pq.enqueue( { "", 6 } );
    EXPECT_EQUAL(pq.dequeue(), { "", 6 });
    pq.validateInternalState();
    pq.enqueue( { "Kermit", -2 } );
    EXPECT_EQUAL(pq.isEmpty(), false);
    EXPECT_EQUAL(pq.size(), 2);
    pq.validateInternalState();
}

STUDENT_TEST("PQHeap, enqueue, validate at every step") {
    PQHeap pq;

    pq.enqueue({ "a", 1.618 });
    pq.validateInternalState();
    pq.enqueue({ "b", 3.14 });
    pq.validateInternalState();
    pq.enqueue({ "c", 1.618 });
    pq.validateInternalState();
    pq.enqueue({ "d", -1 });
    pq.printDebugInfo("4 elements enqueued");
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 4);
}

STUDENT_TEST("PQArray, test enlarge array memory") {
    for (int size = 5; size <= 500; size *= 5) {
        PQHeap pq;

        for (int i = 1; i <= size; i++) {
            pq.enqueue({"", double(i) });
        }
        pq.validateInternalState();
    }
}


STUDENT_TEST("PQArray: operations size/isEmpty/clear") {
    PQHeap pq;

    EXPECT(pq.isEmpty());
    pq.clear();
    EXPECT_EQUAL(pq.isEmpty(), pq.size() == 0);
    pq.enqueue({ "", 7 });
    EXPECT_EQUAL(pq.size(), 1);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 2);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 3);
    pq.clear();
    pq.validateInternalState();
    pq.printDebugInfo("empty queue");
    EXPECT(pq.isEmpty());
    EXPECT_EQUAL(pq.size(), 0);
}

STUDENT_TEST("PQArray, mixed operations") {
    PQHeap pq;

    pq.enqueue( { "d", -3 } );
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 1);
    pq.clear();
    pq.validateInternalState();
    EXPECT_EQUAL(pq.isEmpty(), true);
    EXPECT_EQUAL(pq.size(), 0);
    pq.enqueue( { "k", -20 } );
    pq.enqueue( { "", -10 } );
    pq.enqueue( { "HELOO", -19 } );
    EXPECT_EQUAL(pq.dequeue(), {"k", -20 });
    EXPECT_EQUAL(pq.size(), 2);
    pq.printDebugInfo("After dequeuing elements");
}

STUDENT_TEST("PQArray: dequeue or peek on empty queue raises error") {
    PQHeap pq;
    DataPoint point = { "Programming Abstractions", 106 };

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.dequeue();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.clear();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

STUDENT_TEST("PQArray, dequeue, validate at every step") {
    PQHeap pq;

    pq.enqueue({ "e", 2.718 });
    pq.enqueue({ "pi", 3.14 });
    pq.enqueue({ "phi", 1.618 });

    for (int i = 0; i < 3; i++) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

STUDENT_TEST("PQArray, sequence of mixed operations") {
    PQHeap pq;
    int size = 30;
    double val = 0;

    for (int i = 0; i < size; i++) {
        pq.enqueue({"", --val });
    }
    val = 0;
    for (int i = 0; i < pq.size(); i++) {
        DataPoint front = pq.peek();
        EXPECT_EQUAL(pq.dequeue(), front);
        pq.enqueue( { "", ++val });
    }
    EXPECT_EQUAL(pq.size(), size);
    val = 0;
    while (!pq.isEmpty()) {
        DataPoint expected = { "", ++val };
        EXPECT_EQUAL(pq.dequeue(), expected);
    }
}

STUDENT_TEST("PQArray stress test, cycle many random elements in and out") {
    PQHeap pq;
    int n = 0, maxEnqueues = 1000;
    double sumEnqueued = 0, sumDequeued = 0;

    setRandomSeed(42); // make test behavior deterministic

    DataPoint mostUrgent = { "", 0 };
    pq.enqueue(mostUrgent);
    while (true) {
        if (++n < maxEnqueues && randomChance(0.9)) {
            DataPoint elem = { "", randomInteger(-10, 10) + 0.5 };
            if (elem.priority < mostUrgent.priority) {
                mostUrgent = elem;
            }
            sumEnqueued += elem.priority;
            pq.enqueue(elem);
        } else {
            DataPoint elem = pq.dequeue();
            sumDequeued += elem.priority;
            EXPECT_EQUAL(elem, mostUrgent);
            if (pq.isEmpty()) break;
            mostUrgent = pq.peek();
        }
    }
    EXPECT_EQUAL(sumEnqueued, sumDequeued);
}


/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("PQHeap example from writeup, validate each step") {
    PQHeap pq;
    Vector<DataPoint> input = {
        { "R", 4 }, { "A", 5 }, { "B", 3 }, { "K", 7 }, { "G", 2 },
        { "V", 9 }, { "T", 1 }, { "O", 8 }, { "S", 6 } };

    pq.validateInternalState();
    for (DataPoint dp : input) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}
