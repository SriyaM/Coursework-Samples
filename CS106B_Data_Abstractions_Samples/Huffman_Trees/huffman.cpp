/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */

#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "testing/SimpleTest.h"
using namespace std;

/**
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * messageBits queue contains a valid sequence of encoded bits.
 *
 * Your implementation may change the messageBits queue however you like. There
 * are no requirements about what it should look like after this function
 * returns. The encoding tree should be unchanged.
 *
 * This function uses iteration to decode a sequence of bits and produce
 * a string with the resulting text.
 */
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    string result = "";
    EncodingTreeNode* curr = tree;
    while (!messageBits.isEmpty()) {
        Bit currChar = messageBits.dequeue();
        if (currChar == 0) {
            curr = curr->zero;
        }
        else if (currChar == 1) {
            curr = curr->one;
        }
        if (curr->isLeaf()) {
            result += curr->getChar();
            curr = tree;
        }
    }
    return result;
}

/**
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>.
 *
 * You can assume that the queues are well-formed and represent
 * a valid encoding tree.
 *
 * Your implementation may change the queue parameters however you like. There
 * are no requirements about what they should look like after this function
 * returns.
 *
 * This function uses recursion to build a tree given the treeShape and treeleaves.
 */
EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    if (treeShape.isEmpty()) {
        return nullptr;
    }
    Bit currChar = treeShape.dequeue();
    if (currChar == 0) {
        return new EncodingTreeNode(treeLeaves.dequeue());
    }
    else {
        return new EncodingTreeNode(unflattenTree(treeShape, treeLeaves), unflattenTree(treeShape, treeLeaves));
    }
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 * This function calls the unflattenTree and decodeText functions to
 * transform encoded data into a string.
 */
string decompress(EncodedData& data) {
    EncodingTreeNode* tree = unflattenTree(data.treeShape, data.treeLeaves);
    string message = decodeText(tree, data.messageBits);
    deallocateTree(tree);
    return message;
}


/* This helper function counts the occurences of characters in
 * the input string and makes a map with the character and the
 * number of occurences. It then takes each element from the map
 * and adds it to a priority queue based on the number of
 * occurences.
 */

void PQueueBuilder(PriorityQueue<EncodingTreeNode*>& queue, string text) {
    Map<char,int> occurences;
    for (char x : text) {
        if (occurences.containsKey(x)) {
            int curr = occurences.get(x);
            occurences.put(x, curr+1);
        }
        else {
            occurences.put(x, 1);
        }
    }

    for (char y : occurences) {
        EncodingTreeNode* curr = new EncodingTreeNode(y);
        queue.enqueue(curr, occurences.get(y));
    }
}

/**
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 *
 * This function build a huffman tree from a text input. It leverages
 * the PQBuilder helper function to build a queue of nodes with each
 * character ordered by the number of their occurences. It then dequeues
 * two elements, combines them, and enqeues the result until the tree
 * is created. The function retuens an EncodingTreeNode pointer to the
 * root.
 */
EncodingTreeNode* buildHuffmanTree(string text) {
    if (text.length() < 2) {
        error("please enter text that has at least two characters");
    }
    if (text.length() == 2 && text[0] == text [1]) {
        error("please enter at least two disntinct chracters");
    }

    PriorityQueue<EncodingTreeNode*> queue;
    PQueueBuilder(queue, text);

    while (queue.size() > 1) {
        int priority1 = queue.peekPriority();
        EncodingTreeNode* merge1 = queue.dequeue();
        int priority2 = queue.peekPriority();
        EncodingTreeNode* merge2 = queue.dequeue();
        EncodingTreeNode* merged = new EncodingTreeNode(merge1,merge2);
        queue.enqueue(merged, priority1 + priority2);
    }

    return queue.dequeue();
}

/* This helper function creates a map of characters
 * and their associated encoded sequences given an encoding
 * tree. It uses recursion to go through the tree and stores
 * the current sequence each time it reaches a leaf node. The
 * function does not return anything.
 */

void makeCharMap(Map<char,Vector<Bit>>& map, EncodingTreeNode* tree, Vector<Bit> currSeq) {
    if (tree->isLeaf()) {
        map.put(tree->getChar(), currSeq);
    }
    else {
        currSeq.add(0);
        makeCharMap(map, tree->zero,currSeq);
        currSeq.remove(currSeq.size()-1);
        currSeq.add(1);
        makeCharMap(map, tree->one,currSeq);
        currSeq.remove(currSeq.size()-1);
    }
}

/**
 * Given a string and an encoding tree, encode the text using the tree
 * and return a Queue<Bit> of the encoded bit sequence.
 *
 * You can assume tree is a valid non-empty encoding tree and contains an
 * encoding for every character in the text.
 *
 * This function uses the helper function makeCharMap which contains
 * the code for each character. It then iterates through the text and
 * leverages the map to return the final encoded sequence.
 */
Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    Map<char,Vector<Bit>> charMap;
    Vector<Bit> startSeq;
    makeCharMap(charMap, tree, startSeq);
    Queue<Bit> result;
    for (char curr : text) {
       for (Bit x : charMap.get(curr)){
           result.enqueue(x);
       }
    }
    return result;
}

/* This helper function takes in the tree and the treeShape. It
 * uses recusrion to perform a pre-order traversal on the tree.
 * While traversing, the function adds a zero or one to the treeShape
 * code based on whether the current node is a leaf node or not.
 */

void traverseShape(EncodingTreeNode* tree, Queue<Bit>& treeShape) {
    if (tree != nullptr) {
        if (tree->isLeaf()) {
            treeShape.enqueue(0);
        }
        else {
            treeShape.enqueue(1);
        }
        traverseShape(tree->zero, treeShape);
        traverseShape(tree->one, treeShape);
    }
}

/* This helper function takes in the tree and treeLeaves. It
 * uses recusrion to perform an in-order traversal on the tree.
 * While traversing, the function adds a character to
 * the treeLeaves sequence if it is currently at a
 * leaf node.
 */

void traverseLeaves(EncodingTreeNode* tree, Queue<char>& treeLeaves) {
    if (tree != nullptr) {
        traverseLeaves(tree->zero, treeLeaves);
        if (tree->isLeaf()) {
            treeLeaves.enqueue(tree->getChar());
        }
        traverseLeaves(tree->one, treeLeaves);
    }
}

/**
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment writeup.
 *
 * You can assume the input queues are empty on entry to this function.
 *
 * You can assume tree is a valid well-formed encoding tree.
 *
 * This function leverages two recursive helper functions, treeShape and
 * treeLeaves to create the falttened version of the tree.
 */
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    EncodingTreeNode* currTree = tree;
    traverseShape(currTree, treeShape);
    traverseLeaves(currTree, treeLeaves);
    }

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and flattened
 * encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 *
 * This function uses the buildHuffmanTree, encodeText, and flattenTree
 * function to compress a file.
 */
EncodedData compress(string messageText) {
    EncodingTreeNode* tree = buildHuffmanTree(messageText);
    Queue<Bit> encodedText = encodeText(tree, messageText);
    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(tree, treeShape, treeLeaves);
    deallocateTree(tree);
    return {treeShape, treeLeaves, encodedText};
}

/* * * * * * Testing Helper Functions Below This Point * * * * * */

EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */
    EncodingTreeNode* T = new EncodingTreeNode('T');
    EncodingTreeNode* R = new EncodingTreeNode('R');
    EncodingTreeNode* S = new EncodingTreeNode('S');
    EncodingTreeNode* E = new EncodingTreeNode('E');
    EncodingTreeNode* RS = new EncodingTreeNode(R, S);
    EncodingTreeNode* _E = new EncodingTreeNode(RS, E);
    EncodingTreeNode* B = new EncodingTreeNode(T, _E);
    return B;
}

void deallocateTree(EncodingTreeNode* t) {
    if (t->isLeaf()) {
        delete t;
    }
    else {
        deallocateTree(t->zero);
        delete t;
        deallocateTree(t->one);
    }
}

bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {
    if (a == nullptr && b == nullptr) {
        return true;
        }
    else if (a == nullptr || b == nullptr) {
        return false;
    }
    else if (a->isLeaf() && b->isLeaf()) {
        return a->getChar() == b->getChar();
    }
    else if (a->isLeaf() || b->isLeaf()){
        return false;
    }
    else {
        if (areEqual(a->zero, b->zero) && areEqual(a->one, b->one)) {
            return true;
        }
    }
    return false;
}

/* * * * * * Test Cases Below This Point * * * * * */

STUDENT_TEST("deallocateTree, testing on sample function") {
    EncodingTreeNode* tree = createExampleTree();
    deallocateTree(tree);
}

STUDENT_TEST("areEqual, empty trees") {
    EncodingTreeNode* a = nullptr;
    EncodingTreeNode* b = nullptr;
    EXPECT_EQUAL(areEqual(a, b), true);
}

STUDENT_TEST("areEqual, simple tree and empty tree") {
    EncodingTreeNode* a = nullptr;
    EncodingTreeNode* b = nullptr;

    EncodingTreeNode* A = new EncodingTreeNode('A');
    EncodingTreeNode* B = new EncodingTreeNode('B');
    EncodingTreeNode* simple = new EncodingTreeNode(A,B);
    EXPECT_EQUAL(areEqual(a, simple), false);
    deallocateTree(simple);
}

STUDENT_TEST("areEqual, simple tree duplicate and swaps") {

    EncodingTreeNode* A = new EncodingTreeNode('A');
    EncodingTreeNode* B = new EncodingTreeNode('B');
    EncodingTreeNode* simple = new EncodingTreeNode(A,B);
    EncodingTreeNode* A2 = new EncodingTreeNode('A');
    EncodingTreeNode* B2 = new EncodingTreeNode('B');
    EncodingTreeNode* simple2 = new EncodingTreeNode(A2,B2);
    EncodingTreeNode* A3 = new EncodingTreeNode('A');
    EncodingTreeNode* B3 = new EncodingTreeNode('B');
    EncodingTreeNode* simple3 = new EncodingTreeNode(B3,A3);
    EXPECT_EQUAL(areEqual(simple, simple3), false);
    EXPECT_EQUAL(areEqual(simple, simple2), true);
    deallocateTree(simple);
    deallocateTree(simple2);
    deallocateTree(simple3);
}

STUDENT_TEST("areEqual, simple tree and standard tree") {
    EncodingTreeNode* tree = createExampleTree();
    EncodingTreeNode* tree2 = createExampleTree();

    EncodingTreeNode* A = new EncodingTreeNode('A');
    EncodingTreeNode* B = new EncodingTreeNode('B');
    EncodingTreeNode* simple = new EncodingTreeNode(A,B);
    EXPECT_EQUAL(areEqual(simple, tree), false);
    EXPECT_EQUAL(areEqual(tree, tree2), true);


    EncodingTreeNode* R = new EncodingTreeNode('R');
    EncodingTreeNode* S = new EncodingTreeNode('S');
    EncodingTreeNode* RS = new EncodingTreeNode(R, S); //subtree
    EXPECT_EQUAL(areEqual(tree, RS), false);

    deallocateTree(simple);
    deallocateTree(tree);
    deallocateTree(tree2);
    deallocateTree(RS);
}

STUDENT_TEST("decodeText, empty message") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { };
    EXPECT_EQUAL(decodeText(tree, messageBits), "");

    deallocateTree(tree);
}

STUDENT_TEST("decodeText, single character") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 0 }; // T
    EXPECT_EQUAL(decodeText(tree, messageBits), "T");

    messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    deallocateTree(tree);
}

STUDENT_TEST("decodeText, long messages") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1};
    EXPECT_EQUAL(decodeText(tree, messageBits), "TREESTREETS");

    messageBits = { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "SEEEEET");

    deallocateTree(tree);
}

STUDENT_TEST("decodeText, incomplete bits") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; // T
    EXPECT_EQUAL(decodeText(tree, messageBits), "SEEEEE");


    deallocateTree(tree);
}

STUDENT_TEST("unflattenTree, empty tree") {
    Queue<Bit>  treeShape  = { };
    Queue<char> treeLeaves = { };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, nullptr));
}

STUDENT_TEST("unflattenTree, small tree") {
    EncodingTreeNode* S = new EncodingTreeNode('S');
    EncodingTreeNode* M = new EncodingTreeNode('M');
    EncodingTreeNode* reference = new EncodingTreeNode(S,M);; // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 0 };
    Queue<char> treeLeaves = {'S', 'M'};
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

STUDENT_TEST("unflattenTree, large tree, track in debugger") {
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 1, 0, 0};
    Queue<char> treeLeaves = { 'A', 'B', 'C', 'D', 'E'};
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    deallocateTree(tree);
}

STUDENT_TEST("decompress, example 1 from warmup") {
    EncodedData data = {
        {1, 0, 1, 1, 0, 0, 0}, // treeShape
        { 'A', 'D', 'B', 'N'},  // treeLeaves
        { 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0} // messageBits
    };

    EXPECT_EQUAL(decompress(data), "BANDANA");
}

STUDENT_TEST("decompress, example 2 from warmup") {
    EncodedData data = {
        {1, 0, 1, 0, 0}, // treeShape
        {'E', 'W', 'K'},  // treeLeaves
        { 1, 1, 0, 1, 0, 0, 1, 1} // messageBits
    };

    EXPECT_EQUAL(decompress(data), "KEWEK");
}


STUDENT_TEST("encodeText, several examples") {
    EncodingTreeNode* tree = createExampleTree();
    Queue<Bit> encoded = encodeText(tree, "TREE");
    EXPECT_EQUAL(decodeText(tree, encoded), "TREE");

    Queue<Bit> encoded2 = encodeText(tree, "T");
    EXPECT_EQUAL(decodeText(tree, encoded2), "T");

    Queue<Bit> encoded3 = encodeText(tree, "TREESTREETS");
    EXPECT_EQUAL(decodeText(tree, encoded3), "TREESTREETS");

    Queue<Bit> encoded4 = encodeText(tree, "SSSS");
    EXPECT_EQUAL(decodeText(tree, encoded4), "SSSS");

    Queue<Bit> encoded5 = encodeText(tree, "");
    EXPECT_EQUAL(decodeText(tree, encoded5), "");

    deallocateTree(tree);
}

STUDENT_TEST("flattenTree, empty tree") {
    EncodingTreeNode* reference = nullptr;
    Queue<Bit>  expectedShape  = { };
    Queue<char> expectedLeaves = { };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);
}

STUDENT_TEST("flattenTree, test 3 node tree") {
    EncodingTreeNode* A = new EncodingTreeNode('A');
    EncodingTreeNode* B = new EncodingTreeNode('B');
    EncodingTreeNode* reference = new EncodingTreeNode(A,B);
    Queue<Bit>  expectedShape  = {1, 0, 0};
    Queue<char> expectedLeaves = {'A', 'B'};

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

STUDENT_TEST("flattenTree, testing unequal trees") {
    EncodingTreeNode* A = new EncodingTreeNode('A');
    EncodingTreeNode* B = new EncodingTreeNode('B');
    EncodingTreeNode* reference = new EncodingTreeNode(A,B);
    EncodingTreeNode* wrongReference = createExampleTree();
    Queue<Bit>  expectedShape  = {1, 0, 0};
    Queue<char> expectedLeaves = {'A', 'B'};

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);
    EncodingTreeNode* result = unflattenTree(treeShape, treeLeaves);

    EXPECT_EQUAL(areEqual(wrongReference,result), false);

    deallocateTree(reference);
    deallocateTree(wrongReference);
    deallocateTree(result);
}

STUDENT_TEST("buildHuffmanTree, tiny tree") {
    EncodingTreeNode* A = new EncodingTreeNode('A');
    EncodingTreeNode* B = new EncodingTreeNode('B');
    EncodingTreeNode* reference = new EncodingTreeNode(B,A);
    EncodingTreeNode* tree = buildHuffmanTree("BA");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

STUDENT_TEST("buildHuffmanTree, large tree, confirmed in debug") {
    EncodingTreeNode* tree = buildHuffmanTree("BOOKKEEPER");
    deallocateTree(tree);
}

STUDENT_TEST("buildHuffmanTree, medium tree, confirmed in debug") {
    EncodingTreeNode* tree = buildHuffmanTree("RACECAR");
    deallocateTree(tree);

}
STUDENT_TEST("buildHuffmanTree, medium tree no duplicates, confirmed in debug") {
    EncodingTreeNode* tree = buildHuffmanTree("CRASH");
    deallocateTree(tree);
}

STUDENT_TEST("buildHuffmanTree, small tree, confirmed in debug") {
    EncodingTreeNode* tree = buildHuffmanTree("BBA");
    deallocateTree(tree);
}

STUDENT_TEST("buildHuffmanTree, invalid tree raises error") {
    EXPECT_ERROR(buildHuffmanTree(""));
    EXPECT_ERROR(buildHuffmanTree("A"));
    EXPECT_ERROR(buildHuffmanTree("BB"));
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeShape   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeShape, treeShape);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "Nana Nana Nana Nana Nana Nana Nana Nana Batman"
        "Research is formalized curiosity. It is poking and prying with a purpose. – Zora Neale Hurston",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(input, output);
    }
}
