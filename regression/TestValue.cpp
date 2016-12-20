#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <Shogi.h>
#include <CSAFile.h>

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
        Position p(k.summary());
        for (auto m : k) {
            if (m.move[0] == '%') {
                break;
            }
            p.move(m);
            int exchange = 0;
            for (auto sq : Square::all)   {
                exchange += Evaluation::Value[p.square(sq)];
            }
            for (auto pc : Piece::hand)   {
                // for black's hands
                int bhand  = p.hand(Color::Black, pc);
                exchange  -= Evaluation::Value[Piece::polar(pc, Color::White)] * bhand;
                exchange  += Evaluation::Hands[bhand][pc];
                // for white's hands
                int whand  = p.hand(Color::White, pc);
                exchange  -= Evaluation::Value[Piece::polar(pc, Color::Black)] * whand;
                exchange  -= Evaluation::Hands[bhand][pc];
            }
            if (p.exchange() != exchange) {
                std::cout << exchange << std::endl
                          << p        << std::endl;
            }
        }
    }

    exit(EXIT_SUCCESS);

}
