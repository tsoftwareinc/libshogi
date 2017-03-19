#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <Array.h>
#include <Shogi.h>
#include <CSAFile.h>
#include "MyPosition.h"
#include "lesserpyon/kyokumen.h"

using namespace foundation;
using namespace game;
using namespace Square;

/* ------------------------------- parameters ------------------------------ */

static const size_t         BoardSize   = 9;

static const size_t         KomaVariety = 16;

static const char *         KomaIndex[] = {
                                            "* ","FU","KY","KE","GI",
                                            "KI","KA","HI","OU","TO",
                                            "NY","NK","NG","  ","UM",
                                            "RY"
                                          };

static const Square::Square toSquare [] = {
  SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ11, SQ12, SQ13, SQ14, SQ15, SQ16, SQ17, SQ18, SQ19, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ21, SQ22, SQ23, SQ24, SQ25, SQ26, SQ27, SQ28, SQ29, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ31, SQ32, SQ33, SQ34, SQ35, SQ36, SQ37, SQ38, SQ39, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ41, SQ42, SQ43, SQ44, SQ45, SQ46, SQ47, SQ48, SQ49, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ51, SQ52, SQ53, SQ54, SQ55, SQ56, SQ57, SQ58, SQ59, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ61, SQ62, SQ63, SQ64, SQ65, SQ66, SQ67, SQ68, SQ69, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ71, SQ72, SQ73, SQ74, SQ75, SQ76, SQ77, SQ78, SQ79, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ81, SQ82, SQ83, SQ84, SQ85, SQ86, SQ87, SQ88, SQ89, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQ91, SQ92, SQ93, SQ94, SQ95, SQ96, SQ97, SQ98, SQ99, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD,
  SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD, SQVD
  };


static const unsigned char  toLesser [] = {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
        0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99
  };


static const Piece::Piece   toPiece []  = {
        Piece::EMP,     // 0
        Piece::FU,      // 1
        Piece::KY,      // 2
        Piece::KE,      // 3
        Piece::GI,      // 4
        Piece::KI,      // 5
        Piece::KA,      // 6
        Piece::HI,      // 7
        Piece::OU,      // 8
        Piece::TO,      // 9
        Piece::NY,      // 11
        Piece::NK,      // 12
        Piece::NG,      // 13
        Piece::UM,      // 14
        Piece::RY,      // 15

        Piece::EMP,     // 16

        Piece::BFU,     // 17
        Piece::BKY,     // 18
        Piece::BKE,     // 19
        Piece::BGI,     // 20
        Piece::BKI,     // 21
        Piece::BKA,     // 22
        Piece::BHI,     // 23
        Piece::BOU,     // 24
        Piece::BTO,     // 25
        Piece::BNY,     // 26
        Piece::BNK,     // 27
        Piece::BNG,     // 28
        Piece::BUM,     // 29
        Piece::BRY,     // 30

        Piece::EMP,     // 31
        Piece::EMP,     // 32

        Piece::WFU,     // 33
        Piece::WKY,     // 34
        Piece::WKE,     // 35
        Piece::WGI,     // 37
        Piece::WKI,     // 38
        Piece::WKA,     // 39
        Piece::WHI,     // 40
        Piece::WOU,     // 41
        Piece::WTO,     // 42
        Piece::WNY,     // 43
        Piece::WNK,     // 44
        Piece::WNG,     // 45
        Piece::WUM,     // 46
        Piece::WRY      // 47
  };

/* ------------------------------------------------------------------------- */



/* --------------------------- global  variables --------------------------- */

static KomaInf                      Board[BoardSize][BoardSize] =
                                    {
                                        {EKY,EKE,EGI,EKI,EOU,EKI,EGI,EKE,EKY},
                                        {EMP,EHI,EMP,EMP,EMP,EMP,EMP,EKA,EMP},
                                        {EFU,EFU,EFU,EFU,EFU,EFU,EFU,EFU,EFU},
                                        {EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP},
                                        {EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP},
                                        {EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP},
                                        {SFU,SFU,SFU,SFU,SFU,SFU,SFU,SFU,SFU},
                                        {EMP,SKA,EMP,EMP,EMP,EMP,EMP,SHI,EMP},
                                        {SKY,SKE,SGI,SKI,SOU,SKI,SGI,SKE,SKY}
                                    };

static int                          InHand[EHI + 1] =
                                    {
                                        0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0
                                    };


static Array<Move::Move, Move::Max> move;
static Te                           te[Move::Max];
static size_t                       tenum;

/* ------------------------------------------------------------------------- */

/**
 * Uchifudume check
 * @param p current position
 * @param m current move
 * @return true if Uchifudume, false not
 */
static bool uchifudume (const Position &p, Move::Move m)
{

    if ((Move::Drop & m) == 0) {
        return false;
    }

    if (Move::from(m) != Piece::FU) {
        return false;
    }

    Array<Move::Move, Move::Max> _m;
    Position                     _p(p);
    _p.drop(m);
    _p.genMove(_m);

    if (_m.vsize() > 0) {
        return false;
    }

    return true;

}



/**
 * Print moves in a list (lesserpyon)
 * 
 */
static void printMoveLessr (void)
{

    for (size_t i = 0; i < tenum; ++i) {
        te[i].Print();
        std::cerr << std::endl;
    }

}



/**
 * Print moves in a list (libshogi)
 * 
 */
static void printMoveLibsh (const Position &p)
{

    for (auto m : move) {
        p.show(m);
        std::cerr << std::endl;
    }

}



/**
 * Crosschek moves of lesserpyon 
 * 
 */
static void aroundLibshogi (const Position &p, int turn)
{

    for (size_t i = 0; i < tenum; ++i) {
        bool check = false;
        for (auto m : move) {
            auto to   = toLesser[Move::to  (m)];
            auto from = toLesser[Move::from(m)];
            if (te[i].promote == 0 && (m & Move::Promote)     ) {
                continue;
            }
            if (te[i].promote      && (m & Move::Promote) == 0) {
                continue;
            }
            if (te[i].from == 0) {
                int pk = toPiece[(te[i].koma & 0xf)];
                if (pk == Move::from(m) && te[i].to == to && (m & Move::Drop)) {
                    check = true;
                    break;
                }
            } else
            if (te[i].from == from && te[i].to == to) {
                    check = true;
                    break;
            }
        }
        if (! check) {
            KomaInf       _k  = te[i].koma;
            unsigned char frm = te[i].from & 0xf;
            unsigned char dst = te[i].to   & 0xf;
            if (turn == SELF) {
                if ((frm <= 3 || dst <= 3) && (! te[i].promote) &&
                    (_k == SFU || _k == SKA || _k == SHI)          ) {
                    continue;
                }
            } else {
                if ((frm >= 7 || dst >= 7) && (! te[i].promote) &&
                    (_k == EFU || _k == EKA || _k == EHI)          ) {
                    continue;
                }
            }
            te[i].Print();
            std::cerr << std::endl << "is missing." << std::endl;
            p.show();
            printMoveLibsh(p);
            exit(EXIT_FAILURE);
        }
    }

}



/**
 * Crosschek moves of Libshogi
 * 
 */
static void aroundLesserpy (const Position &p)
{

    for (auto m : move) {
        bool check = false;
        for (size_t i = 0; i < tenum; ++i) {
            auto to   = toLesser[Move::to  (m)];
            auto from = toLesser[Move::from(m)];
            if (te[i].promote == 0 && (m & Move::Promote)     ) {
                continue;
            }
            if ((te[i].promote)    && (m & Move::Promote) == 0) {
                continue;
            }
            if (te[i].from == 0) {
                int pk = toPiece[(te[i].koma & 0xf)];
                if (pk == Move::from(m) && te[i].to == to) {
                    check = true;
                    break;
                }
            } else
            if (te[i].from == from && te[i].to == to) {
                    check = true;
                    break;
            }
        }
        if (! check) {
            if (uchifudume(p, m)) {
                continue;
            }
            MyPosition _p(p);
            _p.move(m);
            if (_p.recheck()) {
                continue;
            }
            p.show(m);
            std::cerr << std::endl << "is missing." << std::endl;
            p.show();
            printMoveLessr();
            exit(EXIT_FAILURE);
        }
    }

}



/**
 * Test delChck()
 *
 */
static void deleteCheck (MyPosition &p)
{

    if (p.nchecks() != 1) {
        return;
    }

    Array<Move::Move, Move::Max> _m;
    p.delChck(_m);

    auto sq = (p.check()).lsb();

    for (auto m : move) {
        if (Move::to(m) == sq) {
            bool check = false;
            for (auto d : _m) {
                if (m == d) {
                    check = true;
                    break;
                }
            }
            if (! check) {
                p.show(m);
                std::cerr << std::endl << "is missing on delChck()." << std::endl;
                p.show();
                exit(EXIT_FAILURE);
            }
        }
    }
}



/**
 * Convert CSA koma string to numeric koma index
 * @param str CSA koma string
 * @return numeric koma index number
 */
static KomaInf komaIndex (const std::string &str)
{

    for (size_t i = 0; i < KomaVariety; ++i) {
        if (str == KomaIndex[i]) {
            return static_cast<KomaInf>(i);
        }
    }

    return static_cast<KomaInf>(0);

}



/**
 * Convert CSA move string to Te of lesserpyon
 * @param str CSA move string
 * @param k   current board
 * @return move
 */
static void parseMove (const std::string &move, const Kyokumen &k, Te &te)
{

    int                     from;
    int                     to;
    unsigned char           turn;

    // check turn
    turn = move.substr(0, 1) == "+" ? SELF : ENEMY;

    // set from and to
    from        = ((move.substr(1, 1))[0] - '0') << 4;
    from       += ( move.substr(2, 1))[0] - '0';
    to          = ((move.substr(3, 1))[0] - '0') << 4;
    to         += ( move.substr(4, 1))[0] - '0';
    te.from     = static_cast<unsigned char>(from);
    te.to       = static_cast<unsigned char>(to);

    // koma index
    KomaInf kindx = komaIndex(move.substr(5, 2));

    // set koma
    if (te.from != 0) {
        te.koma    = k.ban[te.from];
    } else {
        te.koma    = kindx | turn;
    }

    // set capture
    te.capture  = k.ban[te.to];

    // reset value
    te.value    = 0;

    // set promote
    if (kindx > 8 && ! (te.koma & PROMOTED)) {
        te.promote = true;
    } else {
        te.promote = false;
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
    std::ifstream ifs(argv[1]);
    std::string   l;
    while (! (std::getline(ifs, l)).eof()) {
        CSAFile f(l);
        // libshogi
        MyPosition p(f.summary());
        // lesserpyon
        Kyokumen k(0, Board, InHand);
        int turn = SELF;
        for (auto m : f) {
            if (m.move[0] == '%') {
                break;
            }
            // Generate legal moves
            // libshogi
            move.setsz(0);
            p.genFast(move);
            // lesserpyon
            tenum = k.MakeLegalMoves(turn, te);
            //
            // Check uniqueness
            //
            for (auto mv1 : move) {
                int count = 0;
                for (auto mv2 : move) {
                    if (mv1 == mv2) {
                        ++count;
                    }
                }
                if (count != 1) {
                    printMoveLibsh(p);
                    std::cerr << p << std::endl;
                    exit(EXIT_SUCCESS);
                }
            }
            //
            // Cross check
            // 
            aroundLibshogi(p, turn);
            aroundLesserpy(p);
            deleteCheck(p);
            // Proceed
            // libshogi
            p.move(m);
            // lesserpyon
            parseMove(m.move, k, te[0]);
            k.Move(turn, te[0]);
            turn ^= (SELF | ENEMY);
        }
    }

    exit(EXIT_SUCCESS);

}
