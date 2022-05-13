/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


/* This function takes a grid of booleans which is the maze and the current
 * grid location. It checks to see that each of the neighboring grid
 * locations is in bounds and is a corridor and returns a set of grid
 * locations that meet these conditions.
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    Set<GridLocation> potentialNeighbors;
    GridLocation N = {cur.row + 1, cur.col};
    potentialNeighbors.add(N);
    GridLocation S = {cur.row - 1, cur.col};
    potentialNeighbors.add(S);
    GridLocation E = {cur.row, cur.col + 1};
    potentialNeighbors.add(E);
    GridLocation W = {cur.row, cur.col - 1};
    potentialNeighbors.add(W);
    for (GridLocation x : potentialNeighbors) {
        if (maze.inBounds(x) && maze[x]) {
            neighbors.add(x);
        }

    }
    return neighbors;
}

/* This function takes in a grid of booleans which is the maze and
 * a stack of gridlocations which represents a path through the maze.
 * The function checks if the grid location at the top of the stack
 * is at the end of the maze, creates a set to validate that there
 * is no loop in the path, and uses the generateValidMoves function
 * to make sure that each position is in a corridor and adjacent to
 * the previous posision. Finally, the function checks if the path
 * begins at the entrance of the maze by peeking at the last remaining
 * element in the stack. The function will throw an error if any of these
 * conditions are violated. If they are not, it will run to completion.
 */
void validatePath(Grid<bool>& maze, Stack<GridLocation> path) {
    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1};

    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit");
    }

    Set<GridLocation> uniquePositions;
    uniquePositions.add(path.peek());

    while (path.size() > 1) {
        GridLocation currPos = path.pop();
        GridLocation prevPos = path.peek();
        if (generateValidMoves(maze, currPos).contains(prevPos)) {
            if (!uniquePositions.contains(prevPos)) {
                uniquePositions.add(prevPos);

        }
            else {
                error("Path contains a loop");
            }
    }
        else {
            error("Path contains an invalid move");
        }
    }

    GridLocation mazeEntrance = {0,0};
    if (path.pop() != mazeEntrance) {
        error("Path does not begin at maze entrance");
    }

    /* If you find a problem with the path, call error() to report it.
     * If the path is a valid solution, then this function should run to completion
     * without throwing any errors.
     */
}

/* This function takes a grid of booleans which is the maze. It draws
 * the maze and iterates through a queue of possible paths which
 * increase in size with each iteration. Once a path that ends at
 * the maze exit is found, the function returns the path (a stack
 * of grid locations).
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> path;
    Queue<Stack<GridLocation>> possiblePaths;
    Stack<GridLocation> entryPath = { {0,0} };
    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1};
    possiblePaths.enqueue(entryPath);

    while(possiblePaths.size() > 0) {
        Stack<GridLocation> currPath = possiblePaths.dequeue();
        MazeGraphics::highlightPath(currPath, "pink", 0);

        if (currPath.peek() == mazeExit){
            return currPath;
        }

        Set<GridLocation> possibleMoves = generateValidMoves(maze,currPath.peek());
        Set<GridLocation> pastSpots;
        Stack<GridLocation> pathCopy = currPath;
        while (!pathCopy.isEmpty()) {
            pastSpots.add(pathCopy.pop());
        }

        for (GridLocation move : possibleMoves) {
            if(!pastSpots.contains(move)) {
                Stack<GridLocation> newPath = currPath;
                newPath.push(move);
                possiblePaths.enqueue(newPath);
            }
        }
    }
    return path;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for medium maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for large maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}

PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// STUDENT TEST CASES

STUDENT_TEST("generateValidMoves in the center of a 1x1 grid with no walls") {
    Grid<bool> maze = {{true}};
    GridLocation center = {0, 0};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves in the corner of a 5x5 grid with walls") {
    Grid<bool> maze = {{true, true, true, false, true},
                       {true, true, true, false, true},
                       {true, true, true, true, true},
                       {true, true, true, true, true},
                       {true, true, true, true, true}};
    GridLocation corner = {0, 4};
    Set<GridLocation> expected = {{1,4}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

STUDENT_TEST("generateValidMoves in the center of a 5x5 grid with walls") {
    Grid<bool> maze = {{true, true, true, false, true},
                       {true, true, false, false, true},
                       {true, false, true, false, true},
                       {true, true, false, true, true},
                       {true, true, true, true, true}};
    GridLocation corner = {2, 2};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

STUDENT_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, true, true, false, true},
                       {true, true, true, false, true},
                       {true, false, true, true, true},
                       {true, true, false, true, true},
                       {true, true, true, true, true}};
    Stack<GridLocation> one_away_from_exit = { {0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {2, 3}, {2, 4}, {3, 4} };
    Stack<GridLocation> has_loop = { {0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {1, 2}, {2, 2}, {2, 3}, {2, 4}, {3, 4}, {4, 4} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {0, 2}, {0 ,3}, {0, 4}, {1, 4}, {2 ,4}, {3, 4}, {4, 4} };

    EXPECT_ERROR(validatePath(maze, one_away_from_exit));
    EXPECT_ERROR(validatePath(maze, has_loop));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
}

PROVIDED_TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, true, true, false, true},
                       {true, true, true, false, true},
                       {true, false, true, true, true},
                       {true, true, false, true, true},
                       {true, true, true, true, true}};
    Stack<GridLocation> soln = { {0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {2, 3}, {2, 4}, {3, 4}, {4, 4} };
    EXPECT_NO_ERROR(validatePath(maze, soln));
}
