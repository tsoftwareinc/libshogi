#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <Array.h>
#include <Shogi.h>
#include <CSAFile.h>
#include "MyPosition.h"

using namespace foundation;
using namespace game;
using namespace Square;

/* ------------------------------- parameters ------------------------------ */

/* ------------------------------------------------------------------------- */



/* --------------------------- global  variables --------------------------- */

/* ------------------------------------------------------------------------- */

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
    std::ifstream ifs(argv[1]);
    std::string   l;
    while (! (std::getline(ifs, l)).eof()) {
        CSAFile f(l);
        MyPosition p(f.summary());
        for (auto m : f) {
            MyPosition _p(p);
            Array<Move::Move, Move::Max> move;
            _p.genMove (move);
            _p.minorMove(move);
            for (auto _m : move) {
                auto back = _p.move(_m);
                _p.undo(back);
                if (p.hash() != _p.hash()) {
                    _p.show(_m);
                    std::cout << std::endl
                              << "Hash mismatch"
                              << std::endl
                              <<  p << std::endl
                              << _p << std::endl;
                }
                if (p != _p) {
                    _p.show(_m);
                    std::cout << std::endl
                              <<  p << std::endl
                              << _p << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            //
            p.move(m);
        }
    }

    exit(EXIT_SUCCESS);

}
