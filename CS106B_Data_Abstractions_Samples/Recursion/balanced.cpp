/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "testing/SimpleTest.h"

using namespace std;

/*
 * This function takes in a string and recurses through
 * it, deleting characters that are not brackets. Once it
 * has recursed through the entire original string, a string
 * with only the bracketing characters is returned.
 */
string operatorsFrom(string str) {
    /* TODO: Implement this function. */
    if (str.empty()) {
            return "";
        }
    else if(str[0] == '[' ||
               str[0] == '{' ||
               str[0] == '(' ||
               str[0] == '}' ||
               str[0] == ')' ||
               str[0] == ']') {
        return str[0] + operatorsFrom(str.substr(1));
    }
    else {
        return operatorsFrom(str.substr(1));
        };
}

/*
 * This function takes in a string, ops, which contains
 * only bracketing characters. It recurses through the string,
 * attempting to find matching sets of brackets then deleting them.
 * If all are deleted at the end (all sets of brackets are matched)
 * it returns true. If there is a bracketing character that does not
 * fit into a set, the function returns false.
 */
    bool operatorsAreMatched(string ops) {
    if (ops.empty()) {
            return true;
        }
    else if(ops.find("()") != string::npos) {
        return operatorsAreMatched(ops.erase(ops.find("()"), 2));
    }
    else if (ops.find("[]") != string::npos) {
        return operatorsAreMatched(ops.erase(ops.find("[]"), 2));
    }
    else if (ops.find("{}") != string::npos) {
        return operatorsAreMatched(ops.erase(ops.find("{}"), 2));
    }
    else {
        return false;
    }
    }

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}

STUDENT_TEST("operatorsFrom on varying examples, all brackets, mixed, no brackets") {
    EXPECT_EQUAL(operatorsFrom("hisasd((an[3]"), "(([]");
    EXPECT_EQUAL(operatorsFrom("{hisasd((an[3]}}"), "{(([]}}");
    EXPECT_EQUAL(operatorsFrom("(){}"), "(){}");
    EXPECT_EQUAL(operatorsFrom("hiuuhiu"), "");
}

STUDENT_TEST("isBalanced test on many examples") {
    string example ="hey {} my (name is ;;[]{sriya} and I like) CS";
    EXPECT(isBalanced(example));
    string example2 ="[{}}((({})))";
    EXPECT(!isBalanced(example2));
    string example3 ="";
    EXPECT(isBalanced(example3));
}

