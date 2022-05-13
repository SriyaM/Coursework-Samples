/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */
#include <iostream>     // for cout, endl
#include "recursion.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This function takes in two arguments, street and avenue which
 * represent the current location of karel (x and y positions).
 * It returns the number of shortest ossible paths that karel can take
 * back home (at point (1,1)) by recursing through the same
 * function until karel is at the edge of the coordinate system,
 * meaning there is only one more possible shortest path to get
 * home.
 */
int countRoutes(int street, int avenue)
{
    if (street < 1 || avenue < 1) {
        error ("enter a street and avenue that is greater than or equal to one");
    }
    if (street == 1){
        if (avenue == 1) {
            return 0;
        }
        return 1;
    }
    else if (avenue == 1){
        if (street == 1) {
            return 0;
        }
        return 1;
    }
    return 1 + countRoutes(street - 1, avenue) + countRoutes(avenue, street - 1);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Test countRoutes") {
   EXPECT_EQUAL(countRoutes(2, 3), 3);
}

STUDENT_TEST("Test countRoutes") {
   EXPECT_EQUAL(countRoutes(4, 5), 59);
   EXPECT_EQUAL(countRoutes(4, 1), 1);
   EXPECT_EQUAL(countRoutes(1, 3), 1);
   EXPECT_EQUAL(countRoutes(9, 8), 13727);
   EXPECT_EQUAL(countRoutes(2, 1), 1);
   EXPECT_ERROR(countRoutes(0, 0));
}


