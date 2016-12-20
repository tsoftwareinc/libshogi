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
            p.move(m);
            auto check = p.check();
            auto chkAD = p.checkAD();
            auto chkKY = p.checkKY();
            auto chkKA = p.checkKA();
            auto chkHI = p.checkHI();
            if (check) {
                auto guard  = p.turn();
                auto attack = Color::flip(guard);
                auto ou     = p.bitboard(Piece::polar(OU, guard));
                auto ocp    = p.occupied();

                if (chkAD) {
                    auto sq = chkAD.pick();
                    auto pc = p.square(sq);
                    auto ef = Effect::AD(sq, pc);
                    if (! (ou & ef)) {
                        std::cout << p  << std::endl
                                  << ou << std::endl
                                  << ef << std::endl;
                    }
                }

                if (chkKA) {
                    auto sq = chkKA.pick();
                    auto ef = Effect::KA(sq, ocp);
                    if (! (ou & ef)) {
                        std::cout << p  << std::endl
                                  << ou << std::endl
                                  << ef << std::endl;
                    }
                } else
                if (chkHI) {
                    auto sq = chkHI.pick();
                    auto ef = Effect::HI(sq, ocp);
                    if (! (ou & ef)) {
                        std::cout << p  << std::endl
                                  << ou << std::endl
                                  << ef << std::endl;
                    }
                } else
                if (chkKY) {
                    auto sq = chkKY.pick();
                    auto ef = Effect::KY(attack, sq, ocp);
                    if (! (ou & ef)) {
                        std::cout << p  << std::endl
                                  << ou << std::endl
                                  << ef << std::endl;
                    }
                }
            }
        }
    }

    exit(EXIT_SUCCESS);

}
