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
using namespace Piece;
using namespace Square;

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
        CSAFile  k(l);
        MyPosition p(k.summary());
        for (auto m : k) {
            if (m.move[0] == '%') {
                break;
            }
            p.move(m);
            auto check = p.check();
            if (check) {
                Array<Move::Move, Move::Max> move;
                p.genMove(move);
                for (auto _m : move) {
                    MyPosition _p(p);
                    _p.move(_m);
                    auto _check = _p.recheck();
                    if (_check) {
                        std::cout <<  p << std::endl;
                        _p.show(_m);
                        std::cout << _p << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }

    exit(EXIT_SUCCESS);

}
