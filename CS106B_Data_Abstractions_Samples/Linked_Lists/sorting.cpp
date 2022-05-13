#include "listnode.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This utility function combines two linked lists
 * and returns a pointer to the combined list.
 */
ListNode* concatenate(ListNode* front1, ListNode* front2){
    ListNode* curr = front1;

    if (curr == nullptr) {
        return front2;
    }

    while(curr->next != nullptr){
        curr = curr->next;
    }
    curr->next = front2;
    return front1;
}

/*
 * This utility function splits a linked list into three
 * based on whether the number is less than, equal to, or greater
 * than the first element in the linked list.
 */
void partition(ListNode*& front, ListNode*& less, ListNode*& equal, ListNode*& greater){

    if (front == nullptr) {
        return;
    }

    int threshold = front->data;
    ListNode *curr = front;
    while (curr != nullptr) {
        ListNode* temp = curr;
        ListNode* currNext = curr->next;
        if (curr->data < threshold) {
            temp->next = less;
            less = temp;
        }
        else if (curr->data > threshold) {
            temp->next = greater;
            greater = temp;
        }
        else if (curr->data == threshold) {
            temp->next = equal;
            equal = temp;
        }
        curr = currNext;
    }

}

/*
 * This ulility function returns the last part of a
 * linked list that is not sorted and cuts off the front
 * of the link list that conatins number that are sorted.
 */
ListNode* splitRun(ListNode*& front){
    ListNode* remainder = front;
    if (front == nullptr) {
        return nullptr;
    }
    else if (front->next == nullptr) {
        return nullptr;
    }
    else {
        for (ListNode *cur = front; cur->next != nullptr; cur = cur->next) {
            if (cur->data > cur->next->data) {
                remainder = cur->next;
                cur->next = nullptr;
                return remainder;
            }
    }
    }
    return nullptr;
}


/*
 * This utility function takes in two sorted linked lists and
 * merges them into one big sorted linked list.
 */
ListNode* merge(ListNode*& front1, ListNode*& front2){
    ListNode* frontRes = nullptr;
    ListNode* cur1 = front1;
    ListNode* cur2 = front2;
    ListNode* curRes = frontRes;

    if (front1 ==  nullptr && front2 == nullptr) {
        return nullptr;
    }
    else if (front1 == nullptr){
        return front2;
    }
    else if (front2 == nullptr){
        return front1;
    }


    else {
        if (cur1->data <= cur2->data) {
            frontRes = cur1;
            cur1 = cur1->next;
        }
        else {
            frontRes = cur2;
            cur2 = cur2->next;
        }

        curRes = frontRes;

        while (!(cur2 == nullptr || cur1 == nullptr)){
        if (cur1->data <= cur2->data) {
            curRes->next = cur1;
            curRes = cur1;
            cur1 = cur1->next;
        }
        else {
            curRes->next = cur2;
            curRes = cur2;
            cur2 = cur2->next;
        }
        }
        if (cur2 == nullptr){
            curRes->next = cur1;
        }
        if (cur1 == nullptr) {
            curRes->next = cur2;
        }

    }
    return frontRes;
}

/*
 * This function splits a sorted portion
 * of the original list then merges it with the unsorted
 * portion. It continually does this until the entire
 * list is sorted.
 */
void runSort(ListNode*& front) {
    ListNode* l1 = splitRun(front);
    ListNode* l2 = front;
    if (l1 == nullptr) {
        return;
    }

    else {
        ListNode* result = nullptr;
        while (l1 != nullptr) {
            result = merge(l1, l2);
            l1 = splitRun(l2);
    }
    front = result;
    }
}

/*
 * This function recursively splits a list into thirds
 * until the lists are of length 1, then concatenates the
 * mini lists until recursively, until the orginial list
 * is build up in sorted order.
 */
void quickSort(ListNode*& front) {
    if (front == nullptr) {
        return;
    }
    else {
        ListNode* less = nullptr;
        ListNode* equal = nullptr;
        ListNode* greater = nullptr;

        partition(front, less,equal,greater);

        quickSort(less);
        quickSort(greater);

        ListNode* start = concatenate(less, equal);
        front = concatenate(start,greater);
    }
}

/* * * * * * Test Code Below This Point * * * * * */

/* TODO: Write your own student tests below to test your sorting helper
 * functions and overall sorting algorithm correctness. We have provided
 * the skeleton of some tests below to help you get started with this
 * process.
 */

/*
 * We have provided this utility function as a convenience. This
 * function traverses the linked list represented by the provided
 * front pointer and prints out the data stored in the list along
 * the way in a convenient format. You may find it to be helpful to
 * use this function as a debugging aid.
 */
void printList(ListNode* front) {
   cout << "{";
   for (ListNode *cur = front; cur != nullptr; cur = cur->next) {
       cout << cur->data;
       if (cur->next != nullptr) {
           cout << ", ";
       }
   }
   cout << "}" << endl;
}

/*
 * This utility function deallocates the memory for all the nodes in a
 * given linked list. It can be used to recycle the memory allocated
 * during a test case and avoid memory leaks.
 */
void deallocateList(ListNode* front) {
    if (front == nullptr) {
        return;
    }
    else if (front->next == nullptr) {
        delete front;
    }
    else {
        ListNode *trash = front;
        delete front;
        while (trash != nullptr){
            trash = trash->next;
            delete trash;
    }
    }
    return;
}

/*
 * This utility function is given a vector of values and constructs
 * an equivalent linked list containing the same values in the same order.
 * It can be used to construct a linked list to use as an input to
 * a function being tested.
 */
ListNode* createList(Vector<int> values) {
    if (values.isEmpty()){
        return nullptr;
    }
    ListNode* front =  new ListNode();
    ListNode* current = front;
    for (int i = 0; i < values.size()-1; i++) {
        current->data = values[i];
        ListNode* next =  new ListNode();
        current->next = next;
        current = current->next;
    }
    current->data = values[values.size()-1];
    current->next = nullptr;
    return front;
}

/*
 * This utility function compares a linked list and a vector for
 * equivalence and returns true if both contain the same values in the
 * same order. It can be used to confirm a linked list's contents match
 * the expected.
 */
bool areEquivalent(ListNode* front, Vector<int> v) {
    int index = 0;
    for (ListNode *cur = front; cur != nullptr; cur = cur->next) {
        if (index >= v.size()) {
            return false;
        }
        if (cur->data != v[index]){
            return false;
        }
        index ++;
    }
    if (index < v.size()) {
        return false;
    }
    return true;
}


STUDENT_TEST("Testing createList and DeAllocate") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1,2,3,4};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    printList(list);
    deallocateList(list);

}

STUDENT_TEST("partition, small list") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {5, 6, 5, 3};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);
    ListNode* less = nullptr;
    ListNode* equal = nullptr;
    ListNode* greater = nullptr;

    /* Sort the list */
    partition(list,less,equal,greater);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(less, {3}));
    EXPECT(areEquivalent(equal, {5,5}));
    EXPECT(areEquivalent(greater, {6}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(less);
    deallocateList(equal);
    deallocateList(greater);
}

STUDENT_TEST("concatenate, standard example") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {5, 6, 5, 3};
    Vector<int> values2 = {5, 6, 3};
    /* Create equivalent list to use as test input */
    ListNode* list1 = createList(values);
    ListNode* list2 = createList(values2);

    /* Sort the list */
    ListNode* newList = concatenate(list1,list2);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(newList, {5,6,5,3,5,6,3}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(newList);
}

STUDENT_TEST("concatenate, one empty list") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {2,2};
    Vector<int> values2 = {};
    /* Create equivalent list to use as test input */
    ListNode* list1 = createList(values);
    ListNode* list2 = createList(values2);

    /* Sort the list */
    ListNode* newList = concatenate(list1,list2);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(newList, {2,2}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(newList);
}

STUDENT_TEST("concatenate, empty lists") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {};
    Vector<int> values2 = {};
    /* Create equivalent list to use as test input */
    ListNode* list1 = createList(values);
    ListNode* list2 = createList(values2);

    /* Sort the list */
    ListNode* newList = concatenate(list1,list2);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(newList, {}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(newList);
}

STUDENT_TEST("partition, two values") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {5, 2};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);
    ListNode* less = nullptr;
    ListNode* equal = nullptr;
    ListNode* greater = nullptr;

    /* Sort the list */
    partition(list,less,equal,greater);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(less, {2}));
    EXPECT(areEquivalent(equal, {5}));
    EXPECT(areEquivalent(greater, {}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(less);
    deallocateList(equal);
    deallocateList(greater);
}

STUDENT_TEST("partition, empty") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);
    ListNode* less = nullptr;
    ListNode* equal = nullptr;
    ListNode* greater = nullptr;

    /* Sort the list */
    partition(list,less,equal,greater);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(less, {}));
    EXPECT(areEquivalent(equal, {}));
    EXPECT(areEquivalent(greater, {}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(less);
    deallocateList(equal);
    deallocateList(greater);
}

STUDENT_TEST("partition, single number") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {3};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);
    ListNode* less = nullptr;
    ListNode* equal = nullptr;
    ListNode* greater = nullptr;

    /* Sort the list */
    partition(list,less,equal,greater);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(less, {}));
    EXPECT(areEquivalent(equal, {3}));
    EXPECT(areEquivalent(greater, {}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(less);
    deallocateList(equal);
    deallocateList(greater);
}

PROVIDED_TEST("Confirm correctness of test utility functions.") {
    /* Manually construct testList 1 -> 2 -> 3 */
    ListNode* testList = nullptr;
    testList = new ListNode(3, testList);
    testList = new ListNode(2, testList);
    testList = new ListNode(1, testList);

    /* Test equality comparison function. */
    EXPECT(areEquivalent(testList, {1, 2, 3}));
    EXPECT(!areEquivalent(testList, {1, 2}));
    EXPECT(!areEquivalent(testList, {1, 2, 3, 4}));
    EXPECT(!areEquivalent(testList, {1, 3, 2}));

    /* Test create list function. */
    ListNode* studentList = createList({1, 2, 3, 4});
    printList(studentList);
    ListNode* cur = studentList;
    for (int i = 1; i <= 4; i++) {
        EXPECT(cur != nullptr);
        EXPECT_EQUAL(cur->data, i);
        cur = cur->next;
    }

    /* Test deallocate list functions. There should be no memory leaks from this test. */
    deallocateList(studentList);
    deallocateList(testList);
}

STUDENT_TEST("Testing splitRun, empty lists") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    ListNode* remainderLoc = splitRun(list);

    /* Confirm sorted list matches sorted vector */
    EXPECT_EQUAL(remainderLoc ,list);

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("Testing splitRun, singleton list") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    ListNode* remainderLoc = splitRun(list);

    /* Confirm sorted list matches sorted vector */
    EXPECT_EQUAL(remainderLoc ,nullptr);

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("Testing splitRun, sorted list") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1,2,3,4,5,7,8,8,9,9};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    ListNode* remainderLoc = splitRun(list);

    /* Confirm sorted list matches sorted vector */
    EXPECT_EQUAL(remainderLoc ,nullptr);

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("Testing splitRun, list in reverse order") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {5,4,3,2,1};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);
    /* Sort the list */
    ListNode* remainderLoc = splitRun(list);

    /* Confirm sorted list matches sorted vector */
    printList(list);
    printList(remainderLoc);

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
    deallocateList(remainderLoc);
}

STUDENT_TEST("Testing splitRun, list of duplicates") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {4,4,4,4};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    ListNode* remainderLoc = splitRun(list);

    /* Confirm sorted list matches sorted vector */
    EXPECT_EQUAL(remainderLoc ,nullptr);

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}


STUDENT_TEST("Testing merge, empty lists") {
    /* Initialize vector to sequence of values */
    Vector<int> values1 = {};
    Vector<int> values2 = {};

    /* Create equivalent list to use as test input */
    ListNode* list1 = createList(values1);
    ListNode* list2 = createList(values2);

    /* Sort the list */
    ListNode* actual = merge(list1, list2);

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(actual, {}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list1);
    deallocateList(list2);
    deallocateList(actual);
}

STUDENT_TEST("Testing merge, normal lists") {
    /* Initialize vector to sequence of values */
    Vector<int> values1 = {1,2,3,5};
    Vector<int> values2 = {3,4};

    /* Create equivalent list to use as test input */
    ListNode* list1 = createList(values1);
    ListNode* list2 = createList(values2);

    /* Sort the list */
    ListNode* actual = merge(list1, list2);

    /* Confirm sorted list matches sorted vector */
    printList(actual);
    EXPECT(areEquivalent(actual, {1,2,3,3,4,5}));

    /* Avoid memory leaks by deallocating list */
    deallocateList(actual);
}

STUDENT_TEST("runSort, sorted values") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1, 2, 4, 3};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("runSort, random values") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {3,5,6,7,5};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("runSort, duplicate values") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {5,5,5,5};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("runSort, two values") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {6,5};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("runSort, single element") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {2};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("quickSort, empty list") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}


STUDENT_TEST("quickSort, reverse elements") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {5,4,3,2,1};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("quickSort, sorted elements") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1,2,3,4,5};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("quickSort, duplicate elements") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1,1,1};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

STUDENT_TEST("quickSort, positive and negative numbers") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {-1,5,6,-3,2,7,8,-3,10};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();
    printList(list);
    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}


STUDENT_TEST("This is a skeleton to demonstrate an end-to-end student test.") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1, 2, 3, 4};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

/*
 * The provided test case demonstrates a comparsion between the same
 * algorithm (quicksort) operating on a linked list versus a vector.
 */
PROVIDED_TEST("Time linked list quicksort vs vector quicksort") {
    int startSize = 500;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }

        /* NOTE: This test does not check correctness of the linked list sort
         * function. It only times the two operations to compare relative speed. */
        TIME_OPERATION(n, quickSort(list));
        TIME_OPERATION(n, v.sort());        /* Standard vector sort operation is backed
                                               with quicksort algorithm. */

        deallocateList(list);
    }
}

