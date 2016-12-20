#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <Shogi.h>
#include <CSAFile.h>
#include "MyPosition.h"

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

            // move
            p.move(m);

            // check out situation
            auto guard  = p.turn();
            auto attack = Color::flip(guard);
            auto ou     = p.bitboard(Piece::polar(OU, guard));
            auto ousq   = ou.lsb();
            auto pin    = (~ p.pin());

            // check out pin
            while (pin) {
                auto sq = pin.pick();
                auto oc = p.occupied() ^ Bitboard::Square[sq];
                auto dr = distantDirection(ousq, sq);

                // direction to the pinned piece from ou
                if (dr == Square::NOFDR) {
                    std::cout << l       << std::endl
                              << p.pin() << std::endl
                              << p       << std::endl;
                    exit(EXIT_FAILURE);
                }

                // check out the attacking piece
                Piece::Piece pc;
                do {
                    sq += dr;
                    pc  = p.square(sq);
                } while (pc == Piece::EMP);

                // effect without pinned piece
                pc &= Piece::Mask;
                Bitboard ef;
                if (pc == Piece::KY) {
                    ef = Effect::KY(attack, sq, oc);
                } else
                if (pc == Piece::KA) {
                    ef = Effect::KA(sq, oc);
                } else
                if (pc == Piece::HI) {
                    ef = Effect::HI(sq, oc);
                } else {
                    std::cout << l       << std::endl
                              << p.pin() << std::endl
                              << p       << std::endl;
                    exit(EXIT_FAILURE);
                }

                // the attacker must gives OU check when
                // removing the pinned piece
                if (! (ef & ou)) {
                    std::cout << l       << std::endl
                              << p.pin() << std::endl
                              << p       << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    exit(EXIT_SUCCESS);

}
