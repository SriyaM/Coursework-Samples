/*
/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */
#include <iostream>    // for cout, endl
#include "recursion.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}

/*
 * This function takes in the Gwindow, three points of a triangle, and the order
 * of the sierpinski triangle. It recursively calculates and draws the side points
 * of the triangles and returns the number of little triangles that are in the sierpinski
 * triangle of a certain order.
 */
int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    if (order == 0){
        fillBlackTriangle(window, one, two, three);
        return 1;
    }
    else {
        return drawSierpinskiTriangle(window, GPoint((one.x + two.x)/2, (one.y + two.y)/2), GPoint((one.x + three.x)/2, (one.y + three.y)/2), one, order-1) + drawSierpinskiTriangle(window, GPoint((one.x + two.x)/2, (one.y + two.y)/2), GPoint((two.x + three.x)/2, (two.y + three.y)/2), two, order-1) + drawSierpinskiTriangle(window, three, GPoint((two.x + three.x)/2, (two.y + three.y)/2), GPoint((one.x + three.x)/2, (one.y + three.y)/2), order-1);
    }
}


/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {
    runInteractiveDemo();
}

