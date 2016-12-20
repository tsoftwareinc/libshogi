#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <Array.h>
#include <Shogi.h>
#include <CSAFile.h>

using namespace foundation;
using namespace game;

/* ------------------------------- parameters ------------------------------ */

/* ------------------------------------------------------------------------- */



/* --------------------------- global  variables --------------------------- */

static Array<Move::Move, Move::Max> move;

/* ------------------------------------------------------------------------- */

/**
 * Print moves in a list
 */
static void printMove (const Position &p)
{

    for (auto m : move) {
        p.show(m);
        std::cout << std::endl;
    }

}



/**
 * Main function
 * @param argc number of command option when invoked.
 * @param argv strings of comannd options in array.
 * @return status code reflecting normal end or error end.
 */
int main (int argc, char *argv[])
{

    // check argument
    if (argc != 2) {
        std::cerr << "Command Error." << std::endl;
        exit(EXIT_FAILURE);
    }

    // initialization
    Shogi::initialize();

    // set piece values
    Position::setValue  (Evaluation::Value);
    Position::handsValue(Evaluation::Hands);

    // read kifu file
    CSAFile f(argv[1]);
    Position p(f.summary());
    p.show();

    for (int i = 0; i < 5000000; ++i) {
        move.setsz(0);
        p.genMove(move);
    }

    std::cout << "Number of moves :" << move.vsize() << std::endl;
    printMove(p);

    exit(EXIT_SUCCESS);

}
