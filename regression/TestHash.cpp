#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <Array.h>
#include <HashTree.h>
#include <Shogi.h>
#include <CSAFile.h>
#include "MyPosition.h"


namespace foundation {

template <>
HashNode <game::Zobrist::key, MyPosition>::HashNode () :
_key(0x8000000000000000ULL), _left(0), _right(0)
{}

}

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

    HashTree<Zobrist::key, MyPosition> h;

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
            // check hash integrity
            if (! p.chkHash()) {
                std::cout << p << std::endl;
                exit(EXIT_FAILURE);
            }
            // check hash uniqueness
            auto        key = p.hash();
            MyPosition _p;
            if (h.find(key, _p)) {
                if (p != _p) {
                    std::cout << "Hash collision : " << std::endl
                              <<  p  << std::endl
                              << _p  << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                _p = p;
                h.add(key, _p);
            }
            // move            
            p.move(m);
        }
    }

    exit(EXIT_SUCCESS);

}
