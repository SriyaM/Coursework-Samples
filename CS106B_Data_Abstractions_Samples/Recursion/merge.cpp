/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */
#include <iostream>    // for cout, endl
#include "queue.h"
#include "testing/SimpleTest.h"
using namespace std;

bool validateInputs(Queue<int> a) {
    if (a.isEmpty()) {
        return true;
    }
    int current = a.dequeue();
    while (!a.isEmpty()) {
        if (current > a.peek()) {
            error("Inputs are not ordered properly");
            return false;
        }
        current = a.dequeue();
    }
    return true;
}

/*
 * This function takes in two sequences of sorted intergers
 * a, and b. It validates that the squences are sorted then
 * merges them through iteration. The function serturns a single
 * sorted queue that consists of the numbers from both input
 * queues.
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    Queue<int> result;
    validateInputs(a);
    validateInputs(b);
    if (a.isEmpty()) {
        return b;
    }
    if (b.isEmpty()) {
        return a;
    }
    /* TODO: Implement this function. */
    while (!a.isEmpty() || !b.isEmpty()) {
        if (a.isEmpty()) {
            result.enqueue(b.dequeue());
            }
        else if (b.isEmpty()) {
            result.enqueue(a.dequeue());
        }
        else if (a.peek() <= b.peek()) {
            result.enqueue(a.dequeue());
            }
        else {
            result.enqueue(b.dequeue());
            }
    }
    return result;
}


/*
 * This function assumes correct functionality of the previously
 * defined binaryMerge function and makes use of this function to
 * iteratively merge a collection of sequences, starting off with
 * an empty sequence and progressively merging in one sequence at
 * a time. This function is provided for you fully implemented –
 * it does not need to modified at all.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}

/*
 * This function takes in a vector of sequences that consist of
 * sorted numbers. It recursively splits the vector into two and
 * merges each side until the vector is empty. The function returns
 * a queue with the number from at the sequences sorted.
 */
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;
    Vector<Queue<int>> leftVec = all.subList(0,all.size()/2);
    Vector<Queue<int>> RightVec = all.subList(all.size()/2);

    if (all.isEmpty()) {
        return Queue<int> {};
    }
    if (all.size() == 1) {
        return all.get(0);
    }

    Queue<int> x = recMultiMerge(leftVec);
    Queue<int> y = recMultiMerge(RightVec);
    result = binaryMerge(x,y);
    return result;
}


/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}

STUDENT_TEST("Testing helper function validateInputs used within the binarySearch") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 4, 3};
    EXPECT_ERROR(binaryMerge(a, b));
    Queue<int> c = {4, 5, 5, 5, 6};
    Queue<int> d = {1, 4, 3};
    EXPECT_ERROR(binaryMerge(c, d));
}

STUDENT_TEST("Time binaryMerge operation") {
    Vector<int> sizes = {10000000,20000000,40000000,80000000};
    for (int size : sizes) {
        //Queue<int> a = createSequence(size);
        //Queue<int> b = createSequence(size);
        //TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
        size = 1;
    }
}

STUDENT_TEST("Testing naiveMultiMerge with empty sequences and with an empty vector") {
    Vector<Queue<int>> empty_seq = {{},
                             {},
                             {},
                             {},
                             {},
                             {}
                            };
    EXPECT_EQUAL(naiveMultiMerge(empty_seq), {});
    Vector<Queue<int>> empty_vec = {};
    EXPECT_EQUAL(naiveMultiMerge(empty_vec), {});
}

STUDENT_TEST("Time naiveMultiMerge on different n values") {
    Vector<int> n_vals = {2000000, 4000000,8000000,16000000,32000000};
    int k = 10;
    for (int n : n_vals) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(input.size(), naiveMultiMerge(all));
    }
}

STUDENT_TEST("Time naiveMultiMerge on different k values") {
    Vector<int> k_vals = {2,4,8,10,20};
    int n = 8000000;
    for (int k : k_vals) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(k, naiveMultiMerge(all));
    }
}

STUDENT_TEST("several cases of recMultiMerge, compare to naiveMultiMerge") {
    int n = 400;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
    int n2 = 2;
    Queue<int> input2 = createSequence(n);
    Vector<Queue<int>> all2(n2);
    distribute(input2, all2);
    EXPECT_EQUAL(recMultiMerge(all2), naiveMultiMerge(all2));
}

STUDENT_TEST("Time recMultiMerge operation varying n") {
    Vector<int> n_vals = {1000000,2000000,4000000,8000000,16000000};
    int k = 10;
    for (int n : n_vals) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(input.size(), recMultiMerge(all));
   }
}

STUDENT_TEST("Time recMultiMerge operation varying k") {
    Vector<int> k_vals = {1,2,4,8,16};
    int n = 10000000;
    for (int k : k_vals) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(k, recMultiMerge(all));
    }
}


/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}
