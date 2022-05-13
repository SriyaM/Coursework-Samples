/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */

#include "pqarray.h"
#include "error.h"
#include "random.h"
#include "strlib.h"
#include "datapoint.h"
#include "testing/SimpleTest.h"
using namespace std;

// program constant
static const int INITIAL_CAPACITY = 10;

/*
 * The constructor initializes all of the member variables needed for
 * an instance of the PQArray class. The allocated capacity
 * is initialized to a starting constant and a dynamic array of that
 * size is allocated. The number of filled slots is initially zero.
 */
PQArray::PQArray() {
    _numAllocated = INITIAL_CAPACITY;
    _elements = new DataPoint[_numAllocated](); // allocated zero'd memory
    _numFilled = 0;
}

/* The destructor is responsible for cleaning up any resources
 * used by this instance of the PQArray class. The array
 * memory that was allocated for the PQArray is deleted here.
 */
PQArray::~PQArray() {
    delete[] _elements;
}

/*
 * This function enqueues a new element in the proper position in the queue.
 * It first checks to see if the array is at capacity and resizes accorindgly.
 * Then, it adds the new element to the end of the array and swaps it with the
 * element to its left until the priority value of the new element is less than
 * that of the element to its left. Lastly, it increments the _numFilled variable.
 */
void PQArray::enqueue(DataPoint elem) {
    checkResize();
    _elements[size()] = elem;
    _numFilled ++;
    for (int i = size() - 1; i > 0; i--){
        if (_elements[i].priority > _elements[i-1].priority) {
            swap(i, i-1);
        }
    }
}

/*
 * The count of enqueued elements is tracked in the
 * member variable _numFilled.
 */
int PQArray::size() const {
    return _numFilled;
}

/*
 * The array elements are stored in decreasing order of priority value.
 * The element at index 0 is the least urgent (largest priority value)
 * and the element in the last-filled index is the most urgent
 * (minimum priority value), this element is frontmost. peek returns
 * the frontmost element (most urgent, minimum priority value).
 */
DataPoint PQArray::peek() const {
    if (isEmpty()) {
        error("PQueue is empty!");
    }
    return _elements[size() - 1];
}

/*
 * This function returns the value of the frontmost element and removes
 * it from the queue.  Because the frontmost element was at the
 * last-filled index, decrementing filled count is sufficient to remove it.
 */
DataPoint PQArray::dequeue() {
    DataPoint front = peek();
    _numFilled--;
    // we could leave contents of dequeued array element as-is, but overwriting
    // with sentinel value can help with debugging. If your code were to
    // accidentally access an element at index that has been dequeued, you
    // will see sentinel value and this will be a sign of how your code has
    // gone wrong. We recommend you also use this same strategy for PQHeap
    // as a debugging aid.
    _elements[_numFilled] = {"DEQUEUED", -106};
    return front;
}

/*
 * Returns true if no elements in the queue, false otherwise
 */
bool PQArray::isEmpty() const {
    return size() == 0;
}

/*
 * Updates internal state to reflect that the queue is empty, e.g. count
 * of filled slots is reset to zero. The array memory stays allocated
 * at current capacity. The previously stored elements do not need to
 * be cleared; the slots would be overwritten when additional elements
 * are enqueued, but as a defensive programming technique, we mark
 * each with a sentinel value as a debugging aid.
 */
void PQArray::clear() {
    for (int i = 0; i < size(); i++) {
        _elements[i] = { "CLEARED", -106 };
    }
    _numFilled = 0;
}

/*
 * Private member function. This helper exchanges the element at
 * indexA with the element at indexB.
 */
void PQArray::swap(int indexA, int indexB) {
    DataPoint tmp = _elements[indexA];
    _elements[indexA] = _elements[indexB];
    _elements[indexB] = tmp;
}

/*
 * Private member function. This helper function checks if the
 * number of filled elements equals the current capacity of the array.
 * If it does, it doubles the capacity of the array.
 */
void PQArray::checkResize() {
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
 * Prints the contents of internal array for debugging purposes.
 */
void PQArray::printDebugInfo(string msg) const {
    cout << msg << endl;
    for (int i = 0; i < size(); i++) {
        cout << "[" << i << "] = " << _elements[i] << endl;
    }
}

/*
 * Confirm the internal state of member variables appears valid.
 * In this case, check that the elements in the array are stored in
 * priority order. Report an error if problem iis found.
 */
void PQArray::validateInternalState() const {
    /*
     * If there are more elements than spots in the array, we have a problem.
     */
    if (_numFilled > _numAllocated) error("Too many elements in not enough space!");

    /* Loop over the elements in the array and compare priority of
     * neighboring elements. If current element has larger priority
     * than the previous, array elements are out of order with respect
     * to our intention. Use error to report this problem.
     */
    for (int i = 1; i < size(); i++) {
        if (_elements[i].priority > _elements[i-1].priority) {
            printDebugInfo("validateInternalState");
            error("PQArray has elements out of order at index " + integerToString(i));
        }
    }
}

/* * * * * * Test Cases Below This Point * * * * * */

void fillQueue(PQArray& pq, int n) {
    pq.clear(); // start with empty queue
    for (int i = 0; i < n; i++) {
        pq.enqueue({ "", randomReal(0, 10) });
    }
}

void emptyQueue(PQArray& pq, int n) {
    for (int i = 0; i < n; i++) {
        pq.dequeue();
    }
}

STUDENT_TEST("PQArray, Testing enqueue sorting with equal priority values") {
    PQArray pq;

    pq.enqueue( { "Bob", -3 } );
    pq.enqueue( { "Katie", -3 } );
    pq.enqueue( { "Sandra",-3 } );
    pq.enqueue( { "Olivia",-3 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.validateInternalState();
    pq.printDebugInfo("After enqueuing all elements");
}

STUDENT_TEST("PQArray, Validating dequeuing function") {
    PQArray pq;

    pq.enqueue( { "d", -3 } );
    pq.enqueue( { "b", 1 } );
    pq.enqueue( { "s", 0 } );
    pq.enqueue( { "k",-2 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.dequeue();
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 3);
    pq.printDebugInfo("After dequeuing elements");
}

STUDENT_TEST("PQArray, mixed operations") {
    PQArray pq;

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

STUDENT_TEST("PQArray timing test, fillQueue and emptyQueue") {
    PQArray pq;
    Vector<int> enqueueSizes = {2500, 5000, 10000, 20000};
    Vector<int> dequeueSizes = {12000, 25000, 50000, 100000};

    for (int i : enqueueSizes) {
        TIME_OPERATION(i, fillQueue(pq, i));
        pq.clear();
    }
    for (int j : dequeueSizes) {
        fillQueue(pq, j);
        TIME_OPERATION(j, emptyQueue(pq, j));
    }
}


/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("PQArray example from writeup") {
    PQArray pq;

    pq.enqueue( { "Zoe", -3 } );
    pq.enqueue( { "Elmo", 10 } );
    pq.enqueue( { "Bert", 6 } );
    EXPECT_EQUAL(pq.size(), 3);
    pq.printDebugInfo("After enqueue 3 elements");

    pq.enqueue( { "Kermit", 5 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.printDebugInfo("After enqueue one more");

    DataPoint removed = pq.dequeue();
    DataPoint expected = { "Zoe", -3 };
    EXPECT_EQUAL(removed, expected);
    pq.printDebugInfo("After dequeue one");
}

PROVIDED_TEST("PQArray, enqueue only, validate at every step") {
    PQArray pq;

    pq.enqueue({ "e", 2.718 });
    pq.validateInternalState();
    pq.enqueue({ "pi", 3.14 });
    pq.validateInternalState();
    pq.enqueue({ "phi", 1.618 });
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 3);
}

PROVIDED_TEST("PQArray: operations size/isEmpty/clear") {
    PQArray pq;

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
    EXPECT(pq.isEmpty());
    EXPECT_EQUAL(pq.size(), 0);
}

PROVIDED_TEST("PQArray: dequeue or peek on empty queue raises error") {
    PQArray pq;
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

PROVIDED_TEST("PQArray, dequeue, validate at every step") {
    PQArray pq;

    pq.enqueue({ "e", 2.718 });
    pq.enqueue({ "pi", 3.14 });
    pq.enqueue({ "phi", 1.618 });

    for (int i = 0; i < 3; i++) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

PROVIDED_TEST("PQArray, test enlarge array memory") {
    for (int size = 5; size <= 500; size *= 5) {
        PQArray pq;

        for (int i = 1; i <= size; i++) {
            pq.enqueue({"", double(i) });
        }
        pq.validateInternalState();

        for (int i = 1; i <= size; i++) {
            DataPoint expected = {"", double(i) };
            EXPECT_EQUAL(pq.dequeue(), expected);
        }
    }
}

PROVIDED_TEST("PQArray, sequence of mixed operations") {
    PQArray pq;
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

PROVIDED_TEST("PQArray stress test, cycle many random elements in and out") {
    PQArray pq;
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

PROVIDED_TEST("PQArray timing test, fillQueue and emptyQueue") {
    PQArray pq;
    int n = 20000;

    TIME_OPERATION(n, fillQueue(pq, n));
    TIME_OPERATION(n, emptyQueue(pq, n));
}
