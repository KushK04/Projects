/* Kush Khanna
 * CS106B: Programming Abstractions
 * This file accomplishes the tasks relating to boggle
 * in assignment three.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "gridlocation.h"
#include "grid.h"
#include "set.h"
#include "lexicon.h"
#include "testing/SimpleTest.h"
using namespace std;

/* This function takes a string and returns the point value
 * of the string based on the boggle guidelines.
 */
int points(string str) {
    if (str.length() <= 3){
        return 0;
    } else {
        return str.length() - 3;
    }
    return 0;
}

/* This function takes a gridLocation and returns the possile neighbors
 * to that location.
 */
Set<GridLocation> validNeighbors(Grid<char>& board, GridLocation cur, Set<GridLocation> visitedMoves){
    Set<GridLocation> neighbors;
    GridLocation north = {cur.row + 1, cur.col};
    GridLocation northEast = {cur.row + 1, cur.col + 1};
    GridLocation northWest = {cur.row + 1, cur.col - 1};
    GridLocation south = {cur.row - 1, cur.col};
    GridLocation southEast = {cur.row - 1, cur.col + 1};
    GridLocation southWest = {cur.row - 1, cur.col - 1};
    GridLocation east = {cur.row, cur.col + 1};
    GridLocation west = {cur.row, cur.col - 1};
    if(board.inBounds(north) && !visitedMoves.contains(north)){
        neighbors.add(north);
    }
    if(board.inBounds(northEast) && !visitedMoves.contains(northEast)){
        neighbors.add(northEast);
    }
    if(board.inBounds(northWest) && !visitedMoves.contains(northWest)){
        neighbors.add(northWest);
    }
    if(board.inBounds(south) && !visitedMoves.contains(south)){
        neighbors.add(south);
    }
    if(board.inBounds(southEast) && !visitedMoves.contains(southEast)){
        neighbors.add(southEast);
    }
    if(board.inBounds(southWest) && !visitedMoves.contains(southWest)){
        neighbors.add(southWest);
    }
    if(board.inBounds(east) && !visitedMoves.contains(east)){
        neighbors.add(east);
    }
    if(board.inBounds(west) && !visitedMoves.contains(west)){
        neighbors.add(west);
    }
    return neighbors;
}

/* This function takes a location on a grid and returns a set of
 * strings that contain all the possible words made starting from
 * that location.
 */
Set<string> findWords(Grid<char>& board, Lexicon& lex, string currentWord, GridLocation currentLocation, Set<GridLocation> visitedMoves){
    Set<string> wordList;
    currentWord += board[currentLocation];
    if(!lex.containsPrefix(currentWord)){
        return wordList;
    } else {
        if (currentWord.length() >= 4 && lex.contains(currentWord)){
            wordList.add(currentWord);
        }
        Set<GridLocation> neighbors = validNeighbors(board, currentLocation, visitedMoves);
        visitedMoves.add(currentLocation);
        for(GridLocation neighbor: neighbors){
            wordList += findWords(board,lex,currentWord,neighbor,visitedMoves);
        }
    }
    return wordList;
}

/* This function scores an inputted boggle board.
 */
int scoreBoard(Grid<char>& board, Lexicon& lex) {
    int score = 0;
    Set<string> scoredWords;
    for (int i = 0; i < board.numRows(); i++){
        for (int j = 0; j < board.numCols(); j++){
            Set<GridLocation> moves = {};
            GridLocation currentLocation = {i,j};
            string currentWord = "";
            scoredWords += findWords(board, lex, currentWord, currentLocation, moves);
        }
    }
    for(string words: scoredWords){
        score += points(words);
    }
    return score;
}

/* * * * * * Test Cases * * * * * */

/* Test helper function to return shared copy of Lexicon. Use to
 * avoid (expensive) re-load of word list on each test case. */
Lexicon& sharedLexicon() {
    static Lexicon lex("res/EnglishWords.txt");
    return lex;
}

PROVIDED_TEST("Load shared Lexicon, confirm number of words") {
    Lexicon lex = sharedLexicon();
    EXPECT_EQUAL(lex.size(), 127145);
}

PROVIDED_TEST("Test point scoring") {
    EXPECT_EQUAL(points("and"), 0);
    EXPECT_EQUAL(points("quad"), 1);
    EXPECT_EQUAL(points("quint"), 2);
    EXPECT_EQUAL(points("sextet"), 3);
    EXPECT_EQUAL(points("seventh"), 4);
    EXPECT_EQUAL(points("supercomputer"), 10);
}

PROVIDED_TEST("Test scoreBoard, board contains no words, score of zero") {
    Grid<char> board = {{'B','C','D','F'}, //no vowels, no words
                        {'G','H','J','K'},
                        {'L','M','N','P'},
                        {'Q','R','S','T'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 0);
}

PROVIDED_TEST("Test scoreBoard, board contains one word, score of 1") {
    Grid<char> board = {{'C','_','_','_'},
                        {'Z','_','_','_'},
                        {'_','A','_','_'},
                        {'_','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, alternate paths for same word, still score of 1") {
    Grid<char> board = {{'C','C','_','_'},
                        {'C','Z','C','_'},
                        {'_','A','_','_'},
                        {'R','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, small number of words in corner of board") {
    Grid<char> board = {{'L','I','_','_'},
                        {'M','E','_','_'},
                        {'_','S','_','_'},
                        {'_','_','_','_'}};
    Set<string> words = {"SMILE", "LIMES", "MILES", "MILE", "MIES", "LIME", "LIES", "ELMS", "SEMI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()),  2 + 2 + 2 + 1 + 1 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, small number of words") {
    Grid<char> board = {{'E','Z','R','R'},
                        {'O','H','I','O'},
                        {'N','J','I','H'},
                        {'Y','A','H','O'}};
    Set<string> words = { "HORIZON", "OHIA", "ORZO", "JOHN", "HAJI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 4 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, medium number of words") {
    Grid<char> board = {{'O','T','H','X'},
                        {'T','H','T','P'},
                        {'S','S','F','E'},
                        {'N','A','L','T'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 76);
}

PROVIDED_TEST("Test scoreBoard, full board, large number of words") {
    Grid<char> board = {{'E','A','A','R'},
                        {'L','V','T','S'},
                        {'R','A','A','N'},
                        {'O','I','S','E'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 234);
}

STUDENT_TEST("More tests for points") {
    EXPECT_EQUAL(points(""), 0);
    EXPECT_EQUAL(points("Two"), 0);
    EXPECT_EQUAL(points("Three"), 2);
    EXPECT_EQUAL(points("Four"), 1);
    EXPECT_EQUAL(points("Supercalifragilisticexpialidocious"), 31);
}


STUDENT_TEST("Test validNeighbors, 8 neighbors, middle"){
    Grid<char> board = {{'A','B','C',},
                        {'D','E','F',},
                        {'H','I','J',}};
    Set<GridLocation> moves;
    GridLocation currentLocation = {1,1};
    Set<GridLocation> neighbors = validNeighbors(board, currentLocation, moves);
    EXPECT_EQUAL(neighbors.size(), 8);
}

STUDENT_TEST("Test validNeighbors, 3 neighbors, corner"){
    Grid<char> board = {{'A','B','C',},
                        {'D','E','F',},
                        {'H','I','J',}};
    Set<GridLocation> moves;
    GridLocation currentLocation = {0,0};
    Set<GridLocation> neighbors = validNeighbors(board, currentLocation, moves);
    EXPECT_EQUAL(neighbors.size(), 3);
}

STUDENT_TEST("Test validNeighbors, 5 neighbors, side"){
    Grid<char> board = {{'A','B','C',},
                        {'D','E','F',},
                        {'H','I','J',}};
    Set<GridLocation> moves;
    GridLocation currentLocation = {0,1};
    Set<GridLocation> neighbors = validNeighbors(board, currentLocation, moves);
    EXPECT_EQUAL(neighbors.size(), 5);
}

STUDENT_TEST("Test validNeighbors, 6 neighbors, middle, already passed two locations"){
    Grid<char> board = {{'A','B','C',},
                        {'D','E','F',},
                        {'H','I','J',}};
    Set<GridLocation> moves;
    GridLocation a = {0,0};
    GridLocation b = {2,2};
    moves.add(a);
    moves.add(b);
    GridLocation currentLocation = {1,1};
    Set<GridLocation> neighbors = validNeighbors(board, currentLocation, moves);
    EXPECT_EQUAL(neighbors.size(), 6);
}

STUDENT_TEST("Test findWords, 3 possible words"){
    Grid<char> board = {{'C','A','T',},
                        {'A','L','L',},
                        {'R','L','_',}};
    GridLocation currentLocation = {0,0};
    string currentWord = "";
    Set<GridLocation> moves = {};
    Set<string> wordList = findWords(board, sharedLexicon(), currentWord, currentLocation, moves);
    EXPECT_EQUAL(wordList.size(), 3);
}

STUDENT_TEST("Test findWords, 3 possible words"){
    Grid<char> board = {{'L','I','_','_'},
                        {'M','E','_','_'},
                        {'_','S','_','_'},
                        {'_','_','_','_'}};
    GridLocation currentLocation = {0,0};
    string currentWord = "";
    Set<GridLocation> moves = {};
    Set<string> wordList = findWords(board, sharedLexicon(), currentWord, currentLocation, moves);
    EXPECT_EQUAL(wordList.size(), 3);
}

STUDENT_TEST("Test findWords, 2 possible words"){
    Grid<char> board = {{'E','Z','R','R'},
                        {'O','H','I','O'},
                        {'N','J','I','H'},
                        {'Y','A','H','O'}};
    GridLocation currentLocation = {1,3};
    string currentWord = "";
    Set<GridLocation> moves = {};
    Set<string> wordList = findWords(board, sharedLexicon(), currentWord, currentLocation, moves);
    EXPECT_EQUAL(wordList.size(), 2);
}

STUDENT_TEST("Test scoreBoard, board contains 0 words, score of 0") {
    Grid<char> board = {{'_','_','_','_'},
                        {'_','_','_','_'},
                        {'_','_','_','_'},
                        {'_','_','_','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 0);
}

STUDENT_TEST("Test scoreBoard, board contains 2 words, score of 2") {
    Grid<char> board = {{'L','O','V','E'},
                        {'_','_','_','_'},
                        {'_','_','_','_'},
                        {'D','O','G','S'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 2);
}

STUDENT_TEST("Test scoreBoard, board contains 6 words, score of 7") {
    Grid<char> board = {{'S','T','A','R'},
                        {'_','_','_','T'},
                        {'_','_','_','_'},
                        {'_','_','_','_'}};
        EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 7);
}


Set<GridLocation> findNeighbors(GridLocation currentLocation, Grid<bool>& g, Set<GridLocation> pastMoves){
    Set<GridLocation> validNeighbors;
    GridLocation north = {currentLocation.row + 1, currentLocation.col};
    GridLocation south = {currentLocation.row - 1, currentLocation.col};
    GridLocation east = {currentLocation.row, currentLocation.col + 1};
    GridLocation west = {currentLocation.row, currentLocation.col - 1};
    if (g.inBounds(north) && !pastMoves.contains(north)) {
        validNeighbors.add(north);
    }
    if (g.inBounds(south) && !pastMoves.contains(south)) {
        validNeighbors.add(south);
    }
    if (g.inBounds(east) && !pastMoves.contains(east)) {
        validNeighbors.add(east);
    }
    if (g.inBounds(west) && !pastMoves.contains(west)) {
        validNeighbors.add(west);
    }
    return validNeighbors;
}

char direction(GridLocation currentLocation, GridLocation nextLocation){
    if (nextLocation.row > currentLocation.row){
        return 'n';
    } else if (nextLocation.row < currentLocation.row){
        return 's';
    } else if (nextLocation.col > currentLocation.col){
        return 'e';
    } else {
        return 'w';
    }
}

Vector<string> allPathsHelper(Grid<bool>& g, GridLocation currentLocation, string currentPath, Set<GridLocation> moves){
    Vector<string> paths;
    if(currentLocation.row == g.numRows() - 1 && currentLocation.col == g.numCols() - 1){
        paths.add(currentPath);
    } else {
        Set<GridLocation> neighbors = findNeighbors(currentLocation, g, moves);
        for(GridLocation neighbor: neighbors){
            moves.add(currentLocation);
            currentPath += direction(currentLocation, neighbor);
            allPathsHelper(g,neighbor, currentPath, moves);
        }
    }
    return paths;
}

Vector<string> allPaths(Grid<bool>& g){
    GridLocation beginning = {0,0};
    string startingPath = {};
    Set<GridLocation> pathMoves = {};
    allPathsHelper(g, beginning, startingPath, pathMoves);
    return allPathsHelper(g, beginning, startingPath, pathMoves);;
}

STUDENT_TEST(""){
    Grid<bool> maze = {{false, false, false},
                       {false, false, false},
                       {false, false, false}};
    Vector<string> paths = allPaths(maze);
    for(string s: paths){
        cout << s << endl;
    }
}
