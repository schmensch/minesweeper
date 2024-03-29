#include <cctype>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

struct minesweeperBoard {
    vector<vector<bool>> bombs;
    vector<vector<int>> numAdjacentBombs;
    vector<vector<bool>> visible;
};

struct recursiveRevealMetaStruct {
    vector<vector<bool>> checkedSquares;
    minesweeperBoard board;
};

recursiveRevealMetaStruct revealSquares(recursiveRevealMetaStruct rv, int x, int y);

recursiveRevealMetaStruct checkAndReveal(recursiveRevealMetaStruct rv, int x, int y)
{
    if (x < 0 || x >= rv.board.bombs.size() || y < 0 || y >= rv.board.bombs[0].size()) {
        return rv;
    }

    if (rv.checkedSquares[x][y] == true) {
        return rv;
    }

    if (rv.board.bombs[x][y] == true) {
        return rv;
    }

    return revealSquares(rv, x, y);
}

recursiveRevealMetaStruct revealSquares(recursiveRevealMetaStruct rv, int x, int y)
{

    if (rv.board.numAdjacentBombs[x][y] > 0) {
        rv.board.visible[x][y] = true;
        return rv;
    }

    rv.board.visible[x][y] = true;
    rv.checkedSquares[x][y] = true;

    // Go around and check all adjacent cells
    rv = checkAndReveal(rv, x - 1, y + 1);
    rv = checkAndReveal(rv, x, y + 1);
    rv = checkAndReveal(rv, x + 1, y + 1);
    rv = checkAndReveal(rv, x - 1, y);
    rv = checkAndReveal(rv, x + 1, y);
    rv = checkAndReveal(rv, x - 1, y - 1);
    rv = checkAndReveal(rv, x, y - 1);
    rv = checkAndReveal(rv, x + 1, y - 1);

    return rv;
}

void printBoard(minesweeperBoard board)
{
    for (int i = 0; i < board.bombs.size(); i++) {
        for (int j = 0; j < board.bombs[0].size(); j++) {
            // DEBUG: always print all squares
            if (board.visible[i][j]) {
                // if (true) {
                if (board.numAdjacentBombs[i][j] == 0) {
                    cout << " - ";
                } else {
                    cout << " " << board.numAdjacentBombs[i][j] << " ";
                }
            } else {
                cout << " # ";
            }
        }
        cout << "\n\n";
    }
}

// Takes user input
vector<int> userInput(minesweeperBoard board)
{
    vector<int> userInput = { 0, 0 };
    int input;
    do {
        cout << "Please enter a coordinate \nEnter X:";
        cin >> input;
        input--;
        userInput[0] = input;
        cout << "Enter Y:";
        cin >> input;
        input--;
        userInput[1] = input;
    } while (userInput[0] < 0 || userInput[1] < 0 || userInput[0] >= board.bombs.size() || userInput[1] >= board.bombs[0].size());
    //       ^ Check if coordinate is inside the board
    return userInput;
}

// Generates random coordinate using <random>
vector<int> randomCoordinate(minesweeperBoard board)
{
    vector<int> random;
    random_device rand;
    mt19937 gen(rand());
    uniform_int_distribution<> dis(1, board.bombs.size());
    random.push_back(dis(gen));
    uniform_int_distribution<> cis(1, board.bombs[0].size());
    random.push_back(cis(gen));
    return random;
}

// Used for 'countAdjacentMines"
minesweeperBoard checkAndIncrease(minesweeperBoard board, int i, int j)
{
    // Check if input is on the board
    if (i < 0 || j < 0 || i >= board.bombs.size() || j >= board.bombs[0].size()) {
        return board;
    }

    board.numAdjacentBombs[i][j]++;

    return board;
}

// Takes an empty game board and fills in mines
minesweeperBoard generateMines(minesweeperBoard board, int numMines, vector<int> start)
{
    for (int i = 0; i < numMines; i++) {
        vector<int> coordinateMine = randomCoordinate(board);
        int i1 = --coordinateMine[0], i2 = --coordinateMine[1];
        if (board.bombs[i1][i2]) {
            i--;
        } else if (coordinateMine == start) {
            i--;
        } else {
            board.bombs[coordinateMine[0]][coordinateMine[1]] = true;
        }
    }

    return board;
}

// Goes to every mine and increases the fields around it
minesweeperBoard countAdjacentMines(minesweeperBoard board)
{
    for (int i = 0; i < board.bombs.size(); i++) {
        for (int j = 0; j < board.bombs[0].size(); j++) {
            // Take all mines and add 1 to adjacent coordinates
            if (board.bombs[i][j]) {
                // Go around the cell and check all adjacent cells
                board = checkAndIncrease(board, i - 1, j + 1);
                board = checkAndIncrease(board, i, j + 1);
                board = checkAndIncrease(board, i + 1, j + 1);
                board = checkAndIncrease(board, i - 1, j);
                board = checkAndIncrease(board, i + 1, j);
                board = checkAndIncrease(board, i - 1, j - 1);
                board = checkAndIncrease(board, i, j - 1);
                board = checkAndIncrease(board, i + 1, j - 1);
            }
        }
    }

    return board;
}

// Checks for victory
bool victoryCheck(minesweeperBoard board, int numMines)
{
    int areaBoard = board.visible.size() * board.visible[0].size();
    int areaBoardWOMines = areaBoard - numMines;
    int counter = 0;
    for (int i = 0; i < board.visible.size(); i++) {
        for (int j = 0; j < board.visible[0].size(); j++) {
            if (board.visible[i][j]) {
                counter++;
            }
        }
    }
    return areaBoardWOMines == counter;
}

int main()
{

    // main gameloop
    bool gameloop = true;
    while (gameloop) {

        // Introduction
        cout << "Welcome to MINESWEEPER \n\nPlease select a difficulty: \n\t Normal [N] \n\t Hard [H] \n\t Expert [E]\n";

        // Set up variables + Evil hack to check whether difficulty is set
        char userDifficulty;
        int boardSizeX, boardSizeY, numMines = -1;

        // Sets boardSizeX, boardSizeY and numMines according to player input
        while (numMines == -1) {
            cin >> userDifficulty;
            userDifficulty = tolower(userDifficulty);
            switch (userDifficulty) {
            case 'n':
                boardSizeX = boardSizeY = 8;
                numMines = 10;
                break;
            case 'h':
                boardSizeX = boardSizeY = 16;
                numMines = 40;
                break;
            case 'e':
                boardSizeX = 16;
                boardSizeY = 30;
                numMines = 99;
                break;
            default:
                cout << "Enter a valid answer!\n";
                break;
            }
        }

        // Creates board
        minesweeperBoard board;

        // Zero the board vector
        board.bombs.resize(boardSizeX);
        board.numAdjacentBombs.resize(boardSizeX);
        board.visible.resize(boardSizeX);

        for (int i = 0; i < boardSizeX; i++) {

            board.bombs[i].resize(boardSizeY);
            board.numAdjacentBombs[i].resize(boardSizeY);
            board.visible[i].resize(boardSizeY);

            for (int j = 0; j < boardSizeY; j++) {
                board.bombs[i][j] = false;
                board.numAdjacentBombs[i][j] = 0;
                board.visible[i][j] = false;
            }
        }

        // First move
        cout << "Make first move \n";

        vector<int> firstMove = userInput(board);

        board = generateMines(board, numMines, firstMove);
        board = countAdjacentMines(board);

        vector<vector<bool>> checkedSquares;
        checkedSquares.resize(boardSizeX);
        for (int i = 0; i < boardSizeX; i++) {
            checkedSquares[i].resize(boardSizeY);
            for (int j = 0; j < boardSizeY; j++) {
                checkedSquares[i][j] = false;
            }
        }

        recursiveRevealMetaStruct boardWrapped;
        boardWrapped.board = board;
        boardWrapped.checkedSquares = checkedSquares;

        boardWrapped = revealSquares(boardWrapped, firstMove[0], firstMove[1]);
        board = boardWrapped.board;
        printBoard(board);

        bool move = true;

        while (move) {
            vector<int> userCoordinate = userInput(board);
            if (!board.bombs[userCoordinate[0]][userCoordinate[1]]) {
                boardWrapped = revealSquares(boardWrapped, userCoordinate[0], userCoordinate[1]);
                board = boardWrapped.board;
                printBoard(board);
            } else {
                cout << "Game Over!\n";
                move = false;
            }
            if (true) {
                victoryCheck(board, numMines);
            }
        }

        // Quits / Restarts gameloop
        while (true) {
            cout << "Play Again? \nY/N" << endl;
            char input;
            cin >> input;
            input = tolower(input);
            if (input == 'y') {
                break;
            } else if (input == 'n') {
                return 0;
            } else {
                cout << "Enter valid answer!" << endl;
            }
        }
    }
    return 0;
}
