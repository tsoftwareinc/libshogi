#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <Shogi.h>
#include <CSAFile.h>

using namespace game;
using namespace Piece;
using namespace Square;

static bool exitInError (const Position &p, std::string error)
{

    std::cout << p     << std::endl
              << error << std::endl;

    return false;

}


static bool checkBFU (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, BFU);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if (d != UWARD) {
            return false;
        }
    }

    return true;

}


static bool checkBKY (const Position &p, Square::Square sq, Bitboard &m)
{

    Bitboard effect = Effect::KY(Color::Black, sq, m);

    while (effect) {
        auto e = effect.pick();
        auto d = distantDirection(sq, e);
        if (d != UWARD) {
            goto error;
        }
        for (e -= d; e != sq; e-=d) {
            if (p.square(e) != EMP) {
                goto error;
            }
        }
    }

    return true;

error:
    return false;

}


static bool checkBKE (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, BKE);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if (d == UWDKR || d == UWDKL) {
            continue;
        }
        return false;
    }

    return true;

}


static bool checkBGI (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, BGI);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if ( d == UWARD || d == UWRDR ||
             d == UWRDL || d == DWRDL ||
             d == DWRDR                  ) {
            continue;
        }
        return false;
    }

    return true;

}


static bool checkBKA (const Position &p, Square::Square sq, Bitboard &m)
{

    Bitboard effect = Effect::KA(sq, m);

    while (effect) {
        auto e = effect.pick();
        auto d = distantDirection(sq, e);
        if ( d != UWRDR && d != UWRDL &&
             d != DWRDL && d != DWRDR    ) {
            goto error;
        }
        for (e -= d; e != sq; e-=d) {
            if (p.square(e) != EMP) {
                goto error;
            }
        }
    }

    return true;

error:
    return false;

}


static bool checkBHI (const Position &p, Square::Square sq, Bitboard &m)
{

    Bitboard effect = Effect::HI(sq, m);

    while (effect) {
        auto e = effect.pick();
        auto d = distantDirection(sq, e);
        if ( d != RWARD && d != UWARD &&
             d != LWARD && d != DWARD    ) {
            goto error;
        }
        for (e -= d; e != sq; e-=d) {
            if (p.square(e) != EMP) {
                goto error;
            }
        }
    }

    return true;

error:
    return false;

}


static bool checkBKI (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, BKI);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if ( d == RWARD || d == UWARD ||
             d == LWARD || d == DWARD ||
             d == UWRDR || d == UWRDL    ) {
            continue;
        }
        return false;
    }

    return true;

}


static bool checkBOU (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, BOU);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if ( d == RWARD || d == UWARD ||
             d == LWARD || d == DWARD ||
             d == UWRDR || d == UWRDL ||
             d == DWRDL || d == DWRDR    ) {
            continue;
        }
        return false;
    }

    return true;

}


static bool checkBUM (const Position &p, Square::Square sq, Bitboard &m)
{

    if (! checkBKI(sq)) {
        return false;
    }
    return checkBKA(p, sq, m);

}


static bool checkBRY (const Position &p, Square::Square sq, Bitboard &m)
{

    if (! checkBKI(sq)) {
        return false;
    }
    return checkBHI(p, sq, m);

}


static bool checkWFU (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, WFU);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if (d != DWARD) {
            return false;
        }
    }

    return true;

}


static bool checkWKY (const Position &p, Square::Square sq, Bitboard &m)
{

    Bitboard effect = Effect::KY(Color::White, sq, m);

    while (effect) {
        auto e = effect.pick();
        auto d = distantDirection(sq, e);
        if (d != DWARD) {
            goto error;
        }
        for (e -= d; e != sq; e-=d) {
            if (p.square(e) != EMP) {
                goto error;
            }
        }
    }

    return true;

error:
    return false;

}


static bool checkWKE (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, WKE);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if (d == DWDKR || d == DWDKL) {
            continue;
        }
        return false;
    }

    return true;

}


static bool checkWGI (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, WGI);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if ( d == DWARD || d == UWRDR ||
             d == UWRDL || d == DWRDL ||
             d == DWRDR                  ) {
            continue;
        }
        return false;
    }

    return true;

}


static bool checkWKI (Square::Square sq)
{

    Bitboard effect = Effect::AD(sq, WKI);

    while (effect) {
        auto e = effect.pick();
        auto d = adjacntDirection(sq, e);
        if ( d == RWARD || d == UWARD ||
             d == LWARD || d == DWARD ||
             d == DWRDR || d == DWRDL    ) {
            continue;
        }
        return false;
    }

    return true;

}


static bool checkWUM (const Position &p, Square::Square sq, Bitboard &m)
{

    if (! checkWKI(sq)) {
        return false;
    }
    return checkBKA(p, sq, m);

}


static bool checkWRY (const Position &p, Square::Square sq, Bitboard &m)
{

    if (! checkWKI(sq)) {
        return false;
    }
    return checkBHI(p, sq, m);

}


static bool checkEffect  (const Position &p)
{

    for (auto pc : Piece::all) {

        if (pc == EMP) {
            continue;
        }

        Bitboard bbd(p.bitboard(pc));

        while (bbd) {
            auto     sq = bbd.pick();
            Bitboard e  = p.occupied();

            if (p.square(sq) != pc) {
                return exitInError(p, "Bitboard Error");
            }

            switch (pc) {

            case BFU:
                if (! checkBFU(sq)) {
                    return exitInError(p, "BFU Error");
                }
                break;
            case BKY:
                if (! checkBKY(p, sq, e)) {
                    return exitInError(p, "BKY Error");
                }
                break;
            case BKE:
                if (! checkBKE(sq)) {
                    return exitInError(p, "BKE Error");
                }
                break;
            case BGI:
                if (! checkBGI(sq)) {
                    return exitInError(p, "BGI Error");
                }
                break;
            case BKA:
                if (! checkBKA(p, sq, e)) {
                    return exitInError(p, "BKA Error");
                }
                break;
            case BHI:
                if (! checkBHI(p, sq, e)) {
                    return exitInError(p, "BHI Error");
                }
                break;
            case BKI:
                if (! checkBKI(sq)) {
                    return exitInError(p, "BKI Error");
                }
                break;
            case BOU:
                if (! checkBOU(sq)) {
                    return exitInError(p, "BOU Error");
                }
                break;
            case BTO:
                if (! checkBKI(sq)) {
                    return exitInError(p, "BTO Error");
                }
                break;
            case BNY:
                if (! checkBKI(sq)) {
                    return exitInError(p, "BNY Error");
                }
                break;
            case BNK:
                if (! checkBKI(sq)) {
                    return exitInError(p, "BNK Error");
                }
                break;
            case BNG:
                if (! checkBKI(sq)) {
                    return exitInError(p, "BNG Error");
                }
                break;
            case BUM:
                if (! checkBUM(p, sq, e)) {
                    return exitInError(p, "BUM Error");
                }
                break;
            case BRY:
                if (! checkBRY(p, sq, e)) {
                    return exitInError(p, "BRY Error");
                }
                break;
            case WFU:
                if (! checkWFU(sq)) {
                    return exitInError(p, "WFU Error");
                }
                break;
            case WKY:
                if (! checkWKY(p, sq, e)) {
                    return exitInError(p, "WKY Error");
                }
                break;
            case WKE:
                if (! checkWKE(sq)) {
                    return exitInError(p, "WKE Error");
                }
                break;
            case WGI:
                if (! checkWGI(sq)) {
                    return exitInError(p, "WGI Error");
                }
                break;
            case WKA:
                if (! checkBKA(p, sq, e)) {
                    return exitInError(p, "WKA Error");
                }
                break;
            case WHI:
                if (! checkBHI(p, sq, e)) {
                    return exitInError(p, "WHI Error");
                }
                break;
            case WKI:
                if (! checkWKI(sq)) {
                    return exitInError(p, "WKI Error");
                }
                break;
            case WOU:
                if (! checkBOU(sq)) {
                    return exitInError(p, "WOU Error");
                }
                break;
            case WTO:
                if (! checkWKI(sq)) {
                    return exitInError(p, "WTO Error");
                }
                break;
            case WNY:
                if (! checkWKI(sq)) {
                    return exitInError(p, "WNY Error");
                }
                break;
            case WNK:
                if (! checkWKI(sq)) {
                    return exitInError(p, "WNK Error");
                }
                break;
            case WNG:
                if (! checkWKI(sq)) {
                    return exitInError(p, "WNG Error");
                }
                break;
            case WUM:
                if (! checkWUM(p, sq, e)) {
                    return exitInError(p, "WUM Error");
                }
                break;
            case WRY:
                if (! checkWRY(p, sq, e)) {
                    return exitInError(p, "WRY Error");
                }
                break;
            default:
                return exitInError(p, "Undefined piece");
            }
        }
    }

    return true;

}



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
            if (! checkEffect(p)) {
                std::cout << l << " ";
                exit(EXIT_FAILURE);
            }
            if (m.move[0] == '%') {
                break;
            }
            p.move(m);
        }
    }

    exit(EXIT_SUCCESS);

}
