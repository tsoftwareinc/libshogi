/**
 *****************************************************************************

 @file       Main.cpp

 @brief      Example Shogi program built on libshogi

 @author     Hiroki Takada (http://wwww.tsoftware.jp/)

 @date       2026-11-22

 @version    $Id:$

  A complete, single-threaded Shogi engine that shows how to use libshogi:

   - search : iterative deepening alpha-beta (PVS) with a transposition
              table, null move pruning, late move reductions, killer and
              history move ordering and a capture-only quiescence search.
   - eval   : material (from the library) plus king safety and the mobility
              of the sliding pieces, computed with the library bitboards.
   - rules  : legal moves come from Position::genMove(); the program adds
              the pawn-drop-mate (uchifuzume) rule, repetition (sennichite)
              and the perpetual check rule on top of it.

  Modes (see usage()):

     shogiprogram                         USI engine (for Shogi GUIs)
     shogiprogram play [black|white] [s]  play against the program in a
                                          terminal
     shogiprogram csa <user> <pass> <host> [s]
                                          play on a CSA protocol server
     shogiprogram selfplay [games] [ms]   the program plays itself
     shogiprogram bench [depth]           fixed depth search benchmark


  ----------------------------------------------------------------------------
  RELEASE NOTE :

   DATE          REV    REMARK
  ============= ====== =======================================================
  14th Dec 2016  0.1   Initial release
  22nd Nov 2026  0.2   Rewritten: USI, terminal play, CSA, self-play, bench

 *****************************************************************************/

#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <Shogi.h>
#include <Array.h>
#include <Vector.h>
#include <CSASummary.h>
#include <CSAConnection.h>

using namespace game;

namespace {

/* ------------------------------ parameters ------------------------------- */

const char * const          EngineName      = "libshogi example 0.2";
const char * const          EngineAuthor    = "Hiroki Takada";

const int                   MaxPly          = 128;
const int                   MateScore       = 30000;
const int                   MateInMaxPly    = MateScore - MaxPly;
const int                   InfScore        = 31000;
const int                   PerpetualScore  = 20000;
const int                   MaxGamePlies    = 512;

/// Hash keys of the library do not contain the side to move
const uint64_t              SideKey         = 0x9e3779b97f4a7c15ULL;

using MoveList = foundation::Array<Move::Move, Move::Max>;

/* ------------------------------------------------------------------------- */



/* ------------------------------- utilities ------------------------------- */

using Clock = std::chrono::steady_clock;

int64_t nowMs (void)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               Clock::now().time_since_epoch()).count();
}

/// Serializes the output of the search thread and the main thread
std::mutex                  outputLock;

void say (const std::string &line)
{
    std::lock_guard<std::mutex> lock(outputLock);
    std::cout << line << std::endl;
}

/// Hash key of a position including the side to move
uint64_t fullKey (const Position &p)
{
    return p.hash() ^ (p.turn() == Color::White ? SideKey : 0);
}

/// Move without the value bits the library may store in it
Move::Move plain (Move::Move m)
{
    return static_cast<Move::Move>(m & Move::MoveMask);
}

bool isDrop    (Move::Move m) { return (m & Move::Drop)    != 0; }
bool isPromote (Move::Move m) { return (m & Move::Promote) != 0; }

/// Piece values used for move ordering, indexed by Piece & Neutral
const int                   OrderValue[16] = {
                            //  EMP   FU   KY   KE   GI   KA    HI   KI
                                  0, 100, 200, 300, 400, 800, 1000, 500,
                            //  OU      TO   NY   NK   NG   UM    RY
                                10000, 500, 500, 500, 500, 1300, 1500, 0
                            };

int pieceValue (Piece::Piece p)
{
    return OrderValue[p & Piece::Neutral];
}

/// Captured piece of a move (EMP for none)
Piece::Piece captured (const Position &p, Move::Move m)
{
    return isDrop(m) ? static_cast<Piece::Piece>(Piece::EMP)
                     : p.square(Move::to(m));
}

/* ------------------------------------------------------------------------- */



/* ------------------------- move string conversion ------------------------ */

/// Square to USI notation, e.g. SQ77 -> "7g"
std::string usiSquare (Square::Square sq)
{
    std::string s;
    s += static_cast<char>('1' + Square::toFile(sq));
    s += static_cast<char>('a' + Square::toRank(sq));
    return s;
}

/// USI letters of the pieces in hand, indexed by fundamental piece
const char                  UsiDrop[] = "?PLNSBRG";

/// USI notation of a move, e.g. "7g7f", "8h2b+", "P*5e"
std::string usiMove (Move::Move m)
{
    if (Move::isNull(m)) {
        return "resign";
    }
    if (isDrop(m)) {
        std::string s;
        s += UsiDrop[Move::from(m)];
        s += '*';
        return s + usiSquare(Move::to(m));
    }
    return usiSquare(Move::from(m)) + usiSquare(Move::to(m))
         + (isPromote(m) ? "+" : "");
}

/// All legal moves including the minor ones (e.g. a rook not promoting)
void allLegalMoves (Position &p, MoveList &m)
{
    m.setsz(0);
    p.genMove(m);
    p.minorMove(m);
}

/**
 * Position::genMove() includes dropping a pawn to give mate (uchifuzume),
 * which is illegal. Call this right after the move m was made.
 */
bool pawnDropMate (Position &p, Move::Move m)
{
    if (! isDrop(m) || Move::from(m) != Piece::FU || p.nchecks() == 0) {
        return false;
    }
    MoveList r;
    p.genMove(r);
    return r.vsize() == 0;
}

/// Check if the move is an illegal pawn drop mate (the move is not made)
bool illegalPawnDrop (Position &p, Move::Move m)
{
    if (! isDrop(m) || Move::from(m) != Piece::FU) {
        return false;
    }
    auto back = p.move(m);
    bool bad  = pawnDropMate(p, m);
    p.undo(back);
    return bad;
}

/// Find the given move among the legal moves
Move::Move findLegal (Position &p, Move::Move wanted)
{
    MoveList m;
    allLegalMoves(p, m);
    for (auto mv : m) {
        if (plain(mv) == plain(wanted)) {
            return illegalPawnDrop(p, plain(mv)) ? Move::None : plain(mv);
        }
    }
    return Move::None;
}

/// Parse "7g" into a square, SQVD on error
Square::Square parseUsiSquare (char f, char r)
{
    if (f < '1' || f > '9' || r < 'a' || r > 'i') {
        return Square::SQVD;
    }
    return (f - '1') * 9 + (r - 'a');
}

/// Parse a USI move, returns Move::None unless it is legal
Move::Move parseUsiMove (Position &p, const std::string &s)
{
    if (s.size() < 4) {
        return Move::None;
    }
    if (s[1] == '*') {
        const char *c = strchr(UsiDrop + 1, s[0]);
        auto to = parseUsiSquare(s[2], s[3]);
        if (c == nullptr || to == Square::SQVD || s.size() != 4) {
            return Move::None;
        }
        return findLegal(p, Move::drop(static_cast<Piece::Piece>(c - UsiDrop), to));
    }
    auto fm = parseUsiSquare(s[0], s[1]);
    auto to = parseUsiSquare(s[2], s[3]);
    if (fm == Square::SQVD || to == Square::SQVD) {
        return Move::None;
    }
    if (s.size() == 5 && s[4] == '+') {
        return findLegal(p, Move::promote(fm, to));
    }
    if (s.size() != 4) {
        return Move::None;
    }
    return findLegal(p, Move::move(fm, to));
}

/// Parse a CSA move ("+7776FU" or "7776FU"), Move::None unless legal
Move::Move parseCsaMove (Position &p, std::string s)
{
    if (! s.empty() && (s[0] == '+' || s[0] == '-')) {
        s = s.substr(1);
    }
    if (s.size() < 6) {
        return Move::None;
    }
    auto pc = Piece::csaToPiece(s.substr(4, 2));
    if (pc == Piece::EMP || ! isdigit(s[0]) || ! isdigit(s[1])
                         || ! isdigit(s[2]) || ! isdigit(s[3])) {
        return Move::None;
    }
    if (s[2] == '0' || s[3] == '0') {
        return Move::None;
    }
    Square::Square to = (s[2] - '1') * 9 + (s[3] - '1');
    if (s[0] == '0' && s[1] == '0') {
        return findLegal(p, Move::drop(pc & Piece::Mask, to));
    }
    if (s[0] == '0' || s[1] == '0') {
        return Move::None;
    }
    Square::Square fm = (s[0] - '1') * 9 + (s[1] - '1');
    bool promote = (pc & Piece::Neutral) != (p.square(fm) & Piece::Neutral);
    return findLegal(p, promote ? Move::promote(fm, to) : Move::move(fm, to));
}

/// Parse a move in either notation
Move::Move parseMove (Position &p, const std::string &s)
{
    auto m = parseUsiMove(p, s);
    return m != Move::None ? m : parseCsaMove(p, s);
}

/* ------------------------------------------------------------------------- */



/* ------------------------------- SFEN ------------------------------------ */

/// SFEN letters, indexed by Piece & Neutral
const char * const          SfenPiece[16] = {
                                "",   "P",  "L",  "N",  "S",  "B",  "R", "G",
                                "K", "+P", "+L", "+N", "+S", "+B", "+R", ""
                            };

/// CSA names, indexed by Piece & Neutral
const char * const          CsaPiece[16] = {
                                "",   "FU", "KY", "KE", "GI", "KA", "HI", "KI",
                                "OU", "TO", "NY", "NK", "NG", "UM", "RY", ""
                            };

/// Pieces in hand in the SFEN order
const Piece::Piece          SfenHand[] = {
                                Piece::HI, Piece::KA, Piece::KI, Piece::GI,
                                Piece::KE, Piece::KY, Piece::FU
                            };

/// Current position in SFEN
std::string toSfen (const Position &p)
{
    std::string s;
    for (int r = 0; r < 9; ++r) {
        int empty = 0;
        for (int f = 8; f >= 0; --f) {
            auto pc = p.square(f * 9 + r);
            if (pc == Piece::EMP) {
                ++empty;
                continue;
            }
            if (empty) {
                s += static_cast<char>('0' + empty);
                empty = 0;
            }
            std::string n = SfenPiece[pc & Piece::Neutral];
            if (Piece::color(pc) == Color::White) {
                for (auto &c : n) {
                    c = static_cast<char>(tolower(c));
                }
            }
            s += n;
        }
        if (empty) {
            s += static_cast<char>('0' + empty);
        }
        if (r != 8) {
            s += '/';
        }
    }
    s += p.turn() == Color::Black ? " b " : " w ";
    std::string h;
    for (auto c : {Color::Black, Color::White}) {
        for (auto pc : SfenHand) {
            int n = p.hand(c, pc);
            if (n == 0) {
                continue;
            }
            if (n > 1) {
                h += std::to_string(n);
            }
            char l = SfenPiece[pc][0];
            h += c == Color::Black ? l : static_cast<char>(tolower(l));
        }
    }
    s += h.empty() ? "-" : h;
    s += " " + std::to_string(static_cast<int>(p) + 1);
    return s;
}

/// Build a CSA game summary from SFEN fields, false on a syntax error
bool sfenToSummary (const std::string &board, const std::string &side,
                    const std::string &hands, CSASummary &g)
{
    std::istringstream rows(board);
    std::string        row;
    int                r = 0;
    while (std::getline(rows, row, '/')) {
        if (r >= 9) {
            return false;
        }
        std::string csa;
        int         files    = 0;
        bool        promoted = false;
        for (char c : row) {
            if (isdigit(c)) {
                for (int i = 0; i < c - '0'; ++i) {
                    csa += " * ";
                }
                files += c - '0';
                continue;
            }
            if (c == '+') {
                promoted = true;
                continue;
            }
            const char *l = strchr("PLNSBRGK", toupper(c));
            if (l == nullptr) {
                return false;
            }
            // the order of the letters matches the library: FU .. OU
            int pc = static_cast<int>(l - "PLNSBRGK") + 1;
            if (promoted) {
                pc |= Piece::Promoted;
                promoted = false;
            }
            csa += isupper(c) ? '+' : '-';
            csa += CsaPiece[pc];
            ++files;
        }
        if (files != 9) {
            return false;
        }
        g.position[static_cast<size_t>(r++)] = csa;
    }
    if (r != 9 || (side != "b" && side != "w")) {
        return false;
    }
    g.senteHand.clear();
    g.goteHand.clear();
    if (hands != "-") {
        int count = 0;
        for (char c : hands) {
            if (isdigit(c)) {
                count = count * 10 + (c - '0');
                continue;
            }
            const char *l = strchr("PLNSBRG", toupper(c));
            if (l == nullptr) {
                return false;
            }
            const Piece::Piece kind[] = {Piece::FU, Piece::KY, Piece::KE,
                                         Piece::GI, Piece::KA, Piece::HI,
                                         Piece::KI};
            std::string &dst = isupper(c) ? g.senteHand : g.goteHand;
            for (int i = 0; i < std::max(count, 1); ++i) {
                dst += std::string("00") + CsaPiece[kind[l - "PLNSBRG"]];
            }
            count = 0;
        }
    }
    g.firstPlayer = side == "b" ? "+" : "-";
    return true;
}

/* ------------------------------------------------------------------------- */



/* ------------------------------ evaluation ------------------------------- */

/// Squares within two steps of each square
Bitboard                    NearMask[Square::Squares];

void initEvaluation (void)
{
    for (int s = 0; s < 81; ++s) {
        Bitboard b;
        for (int t = 0; t < 81; ++t) {
            if (t != s && std::abs(s / 9 - t / 9) <= 2
                       && std::abs(s % 9 - t % 9) <= 2) {
                b |= Bitboard::Square[t];
            }
        }
        NearMask[s] = b;
    }
}

/// King safety of color c (positive is good for c)
int kingSafety (const Position &p, Color::Color c)
{
    using namespace Piece;

    auto kb = p.bitboard(polar(OU, c));
    if (! kb) {
        return 0;
    }
    auto k = kb.lsb();
    auto e = Color::flip(c);

    // own generals around the king
    auto def = p.bitboard(polar(KI, c)) | p.bitboard(polar(GI, c))
             | p.bitboard(polar(TO, c)) | p.bitboard(polar(NY, c))
             | p.bitboard(polar(NK, c)) | p.bitboard(polar(NG, c));
    int  nd  = static_cast<int>((def & NearMask[k]).popcnt());
    nd       = std::min(nd, 4);

    // enemy pieces (but pawns) around the king
    auto att = p.occupied(e) & ~(p.bitboard(polar(FU, e)) |
                                 p.bitboard(polar(OU, e)));
    int  na  = static_cast<int>((att & NearMask[k]).popcnt());

    // enemy pieces in hand can be dropped near the king
    int  nh  = p.hand(e, FU) / 3;
    for (auto pc : {KY, KE, GI, KI, KA, HI}) {
        nh  += p.hand(e, pc);
    }

    // a king staying in its own camp is safer
    int  rank = Square::toRank(k);
    int  rel  = c == Color::Black ? rank : 8 - rank;

    return nd * 30 - na * 40 - nh * 8 - na * nh * 6 + (rel - 6) * 15;
}

/// Mobility of the sliding pieces of color c
int mobility (const Position &p, Color::Color c)
{
    using namespace Piece;

    const auto &occ = p.occupied();
    int         s   = 0;
    for (auto pc : {KA, UM}) {
        auto b = p.bitboard(polar(pc, c));
        while (b) {
            s += 2 * static_cast<int>(Effect::KA(b.pick(), occ).popcnt());
        }
    }
    for (auto pc : {HI, RY}) {
        auto b = p.bitboard(polar(pc, c));
        while (b) {
            s += 2 * static_cast<int>(Effect::HI(b.pick(), occ).popcnt());
        }
    }
    return s;
}

/// Evaluation from the point of view of the side to move
int evaluate (Position &p)
{
    int s = p.eval()                                         // material
          + kingSafety(p, Color::Black) - kingSafety(p, Color::White)
          + mobility  (p, Color::Black) - mobility  (p, Color::White);
    return (p.turn() == Color::Black ? s : -s) + 20;         // tempo
}

/* ------------------------------------------------------------------------- */



/* -------------------------- transposition table -------------------------- */

enum Bound : uint8_t { BoundNone, BoundUpper, BoundLower, BoundExact };

struct TTEntry {
    uint64_t                key;
    int16_t                 score;
    uint16_t                move;
    int8_t                  depth;
    uint8_t                 bound;
    uint8_t                 age;
};

class TransTable
{
public:
    void resize (size_t mb)
    {
        size_t n = 1;
        while (n * 2 * sizeof(TTEntry) <= mb * 1024 * 1024) {
            n *= 2;
        }
        _table.assign(n, TTEntry());
        _mask = n - 1;
        clear();
    }

    void clear (void)
    {
        std::fill(_table.begin(), _table.end(), TTEntry());
        _age = 0;
    }

    void newSearch (void) { ++_age; }

    const TTEntry *probe (uint64_t key) const
    {
        const auto &e = _table[key & _mask];
        return e.key == key && e.bound != BoundNone ? &e : nullptr;
    }

    void store (uint64_t key, int score, Move::Move m, int depth, Bound b)
    {
        auto &e = _table[key & _mask];
        if (e.key == key && depth < e.depth - 2 && b != BoundExact &&
            e.age == _age) {
            return;
        }
        if (e.key == key && m == Move::None) {
            m = static_cast<Move::Move>(e.move);
        }
        e.key   = key;
        e.score = static_cast<int16_t>(score);
        e.move  = static_cast<uint16_t>(m);
        e.depth = static_cast<int8_t>(std::max(-1, std::min(depth, 127)));
        e.bound = b;
        e.age   = _age;
    }

    int hashfull (void) const
    {
        int used = 0;
        for (size_t i = 0; i < 1000 && i < _table.size(); ++i) {
            used += _table[i].bound != BoundNone && _table[i].age == _age;
        }
        return used;
    }

private:
    std::vector<TTEntry>    _table;
    size_t                  _mask = 0;
    uint8_t                 _age  = 0;
};

/// Mate scores are stored relative to the node, not to the root
int scoreToTT (int s, int ply)
{
    return s >= MateInMaxPly ? s + ply : s <= -MateInMaxPly ? s - ply : s;
}

int scoreFromTT (int s, int ply)
{
    return s >= MateInMaxPly ? s - ply : s <= -MateInMaxPly ? s + ply : s;
}

/* ------------------------------------------------------------------------- */



/* --------------------------------- game ---------------------------------- */

/**
 * A game: the position and the history needed by the repetition rules.
 * keys[i] is the hash key of the i-th position and checks[i] tells if the
 * side to move was in check there.
 */
struct Game {
    Position                pos;
    std::vector<uint64_t>   keys;
    std::vector<char>       checks;
    std::vector<Move::Move> moves;      // as returned by Position::move()

    Game () { reset(); }

    void reset (void)
    {
        pos.init();
        start();
    }

    void start (void)
    {
        keys  .assign(1, fullKey(pos));
        checks.assign(1, pos.nchecks() > 0);
        moves .clear();
    }

    bool setSfen (const std::string &board, const std::string &side,
                  const std::string &hands)
    {
        CSASummary g;
        if (! sfenToSummary(board, side, hands, g)) {
            return false;
        }
        Position p(g);
        pos = p;
        start();
        return true;
    }

    void play (Move::Move m)
    {
        moves .push_back(pos.move(m));
        keys  .push_back(fullKey(pos));
        checks.push_back(pos.nchecks() > 0);
    }

    bool takeBack (void)
    {
        if (moves.empty()) {
            return false;
        }
        pos.undo(moves.back());
        moves .pop_back();
        keys  .pop_back();
        checks.pop_back();
        return true;
    }

    /// Number of times the current position has occurred
    int occurrences (void) const
    {
        int n = 0;
        for (size_t i = keys.size() % 2 == 0 ? 1 : 0; i < keys.size(); ++i) {
            n += keys[i] == keys.back();
        }
        return n;
    }
};

/* ------------------------------------------------------------------------- */



/* -------------------------------- search --------------------------------- */

struct Limits {
    int                     depth    = MaxPly - 1;
    int64_t                 softMs   = 0;       // don't start a new iteration
    int64_t                 hardMs   = 0;       // abort the search
    uint64_t                nodes    = 0;       // 0 for no limit
    bool                    infinite = false;
};

/// Set by the USI front end to interrupt the search
std::atomic<bool>           stopRequest(false);

/// Cleared by "ponderhit" to start the clock of a pondering search
std::atomic<bool>           pondering(false);
std::atomic<int64_t>        searchStart(0);

struct SearchResult {
    Move::Move              best  = Move::None;
    int                     score = 0;
    int                     depth = 0;
    uint64_t                nodes = 0;
};

class Searcher
{
public:
    explicit Searcher (TransTable &tt) : _tt(tt) {}

    /// Search the current position of the game
    SearchResult think (const Game &game, const Limits &lim, bool verbose);

private:
    int  _search     (int alpha, int beta, int depth, int ply, bool nullOk);
    int  _qsearch    (int alpha, int beta, int ply, int qdepth);
    bool _repetition (int ply, int &score) const;
    bool _illegalDrop(Move::Move m);
    void _checkTime  (void);
    void _push       (void);
    void _pop        (void);
    void _order      (MoveList &m, int *score, Move::Move ttMove, int ply);
    void _report     (int depth, int score, int64_t elapsed);

    TransTable &            _tt;
    Position                _p;
    Limits                  _lim;
    bool                    _verbose  = false;
    bool                    _aborted  = false;
    uint64_t                _nodes    = 0;
    int                     _seldepth = 0;
    int                     _rootDepth = 0;
    int                     _barrier  = 0;      // repetition can't cross it
    Move::Move              _rootBest = Move::None;

    std::vector<uint64_t>   _keys;
    std::vector<char>       _checks;

    Move::Move              _killer [MaxPly][2];
    int                     _history[64][Square::Squares];
    Move::Move              _pv     [MaxPly][MaxPly];
    int                     _pvLen  [MaxPly];
};

void Searcher::_push (void)
{
    _keys  .push_back(fullKey(_p));
    _checks.push_back(_p.nchecks() > 0);
}

void Searcher::_pop (void)
{
    _keys  .pop_back();
    _checks.pop_back();
}

void Searcher::_checkTime (void)
{
    if (stopRequest) {
        _aborted = true;
        return;
    }
    if (_lim.nodes && _nodes >= _lim.nodes) {
        _aborted = true;
        return;
    }
    if (! _lim.infinite && ! pondering &&
        nowMs() - searchStart >= _lim.hardMs) {
        _aborted = true;
    }
}

/**
 * Repetition (sennichite). A repeated position is a draw unless one side
 * has been checking all the time, in which case that side loses.
 */
bool Searcher::_repetition (int ply, int &score) const
{
    int n = static_cast<int>(_keys.size()) - 1;
    for (int j = n - 4; j >= _barrier; j -= 2) {
        if (_keys[static_cast<size_t>(j)] != _keys.back()) {
            continue;
        }
        bool checkedUs   = true;            // the opponent kept checking
        bool checkedThem = true;            // we kept checking
        for (int i = j + 2; i <= n; i += 2) {
            checkedUs   = checkedUs   && _checks[static_cast<size_t>(i)];
        }
        for (int i = j + 1; i <  n; i += 2) {
            checkedThem = checkedThem && _checks[static_cast<size_t>(i)];
        }
        score = checkedUs   ?  PerpetualScore - ply :
                checkedThem ? -PerpetualScore + ply : 0;
        return true;
    }
    return false;
}

/**
 * The move was just made. Dropping a pawn to give mate is illegal.
 */
bool Searcher::_illegalDrop (Move::Move m)
{
    return pawnDropMate(_p, m);
}

/// Index to the history table: board piece, or 32 + piece for drops
int historyIndex (const Position &p, Move::Move m)
{
    return isDrop(m) ? 32 + Move::from(m) : p.square(Move::from(m));
}

void Searcher::_order (MoveList &m, int *score, Move::Move ttMove, int ply)
{
    for (size_t i = 0; i < m.vsize(); ++i) {
        auto mv = plain(m[i]);
        auto cp = captured(_p, mv);
        int  s;
        if (mv == ttMove) {
            s = 1 << 30;
        } else if (cp != Piece::EMP) {
            s = (1 << 28) + pieceValue(cp) * 16
              - pieceValue(_p.square(Move::from(mv))) / 16;
        } else if (isPromote(mv)) {
            s = (1 << 27) + pieceValue(_p.square(Move::from(mv)));
        } else if (mv == _killer[ply][0]) {
            s = (1 << 26) + 1;
        } else if (mv == _killer[ply][1]) {
            s = (1 << 26);
        } else {
            s = _history[historyIndex(_p, mv)][Move::to(mv)];
        }
        score[i] = s;
    }
}

/// Move the best scored move of m[i..] to m[i]
void pickNext (MoveList &m, int *score, size_t i)
{
    size_t b = i;
    for (size_t j = i + 1; j < m.vsize(); ++j) {
        if (score[j] > score[b]) {
            b = j;
        }
    }
    std::swap(m[i],     m[b]);
    std::swap(score[i], score[b]);
}

int Searcher::_qsearch (int alpha, int beta, int ply, int qdepth)
{
    _pvLen[ply] = ply;
    if ((++_nodes & 1023) == 0) {
        _checkTime();
    }
    if (_aborted) {
        return 0;
    }
    _seldepth = std::max(_seldepth, ply);

    bool inCheck = _p.nchecks() > 0;
    if (ply >= MaxPly - 1) {
        return inCheck ? 0 : evaluate(_p);
    }

    int best = -InfScore;
    int stand = 0;
    if (! inCheck) {
        stand = evaluate(_p);
        if (stand >= beta) {
            return stand;
        }
        alpha = std::max(alpha, stand);
        best  = stand;
        if (qdepth >= 10) {
            return best;
        }
    }

    MoveList m;
    _p.genMove(m);
    int score[Move::Max];
    _order(m, score, Move::None, ply);

    for (size_t i = 0; i < m.vsize(); ++i) {
        pickNext(m, score, i);
        auto mv = plain(m[i]);
        auto cp = captured(_p, mv);
        if (! inCheck) {
            if (cp == Piece::EMP) {
                continue;
            }
            // a captured piece goes to the hand: it counts twice
            if (stand + 2 * pieceValue(cp) + 200 <= alpha) {
                continue;
            }
        }
        auto back = _p.move(mv);
        if (_illegalDrop(mv)) {
            _p.undo(back);
            continue;
        }
        _push();
        int v = -_qsearch(-beta, -alpha, ply + 1, qdepth + 1);
        _pop();
        _p.undo(back);
        if (_aborted) {
            return 0;
        }
        if (v > best) {
            best = v;
            if (v > alpha) {
                alpha = v;
                if (v >= beta) {
                    break;
                }
            }
        }
    }

    // no legal move: mated (in Shogi this is a loss even without check)
    if (best == -InfScore) {
        return -MateScore + ply;
    }
    return best;
}

int Searcher::_search (int alpha, int beta, int depth, int ply, bool nullOk)
{
    if (depth <= 0) {
        return _qsearch(alpha, beta, ply, 0);
    }

    _pvLen[ply] = ply;
    if ((++_nodes & 1023) == 0) {
        _checkTime();
    }
    if (_aborted) {
        return 0;
    }

    bool pvNode = beta - alpha > 1;
    if (ply > 0) {
        int rep;
        if (_repetition(ply, rep)) {
            return rep;
        }
        // mate distance pruning
        alpha = std::max(alpha, -MateScore + ply);
        beta  = std::min(beta,   MateScore - ply - 1);
        if (alpha >= beta) {
            return alpha;
        }
    }
    if (ply >= MaxPly - 1) {
        return evaluate(_p);
    }

    // transposition table
    uint64_t   key    = _keys.back();
    Move::Move ttMove = Move::None;
    if (auto e = _tt.probe(key)) {
        ttMove = static_cast<Move::Move>(e->move);
        int s  = scoreFromTT(e->score, ply);
        if (! pvNode && e->depth >= depth &&
            (e->bound == BoundExact                ||
            (e->bound == BoundLower && s >= beta ) ||
            (e->bound == BoundUpper && s <= alpha))) {
            return s;
        }
    }

    bool inCheck    = _p.nchecks() > 0;
    int  staticEval = inCheck ? -InfScore : evaluate(_p);

    // reverse futility pruning
    if (! pvNode && ! inCheck && depth <= 3 &&
        std::abs(beta) < MateInMaxPly && staticEval - 150 * depth >= beta) {
        return staticEval;
    }

    // null move pruning
    if (! pvNode && nullOk && ! inCheck && depth >= 3 &&
        staticEval >= beta && std::abs(beta) < MateInMaxPly) {
        auto turn    = _p.turn();
        int  barrier = _barrier;
        _p.turn(Color::flip(turn));
        _push();
        _barrier     = static_cast<int>(_keys.size()) - 1;
        int R        = 2 + depth / 4;
        int v        = -_search(-beta, -beta + 1, depth - 1 - R, ply + 1, false);
        _barrier     = barrier;
        _pop();
        _p.turn(turn);
        if (_aborted) {
            return 0;
        }
        if (v >= beta) {
            return v >= MateInMaxPly ? beta : v;
        }
    }

    MoveList m;
    _p.genMove(m);
    int score[Move::Max];
    _order(m, score, ttMove, ply);

    int        best     = -InfScore;
    int        oldAlpha = alpha;
    Move::Move bestMove = Move::None;
    int        legal    = 0;

    for (size_t i = 0; i < m.vsize(); ++i) {
        pickNext(m, score, i);
        auto mv    = plain(m[i]);
        bool quiet = captured(_p, mv) == Piece::EMP && ! isPromote(mv);

        auto back  = _p.move(mv);
        if (_illegalDrop(mv)) {
            _p.undo(back);
            continue;
        }
        ++legal;
        bool givesCheck = _p.nchecks() > 0;

        // futility pruning of quiet moves near the leaves
        if (quiet && ! inCheck && ! givesCheck && legal > 1 && depth <= 2 &&
            best > -MateInMaxPly && staticEval + 200 * depth <= alpha) {
            _p.undo(back);
            continue;
        }

        _push();
        int ext      = givesCheck && ply + depth < 2 * _rootDepth ? 1 : 0;
        int newDepth = depth - 1 + ext;
        int v;
        if (legal == 1) {
            v = -_search(-beta, -alpha, newDepth, ply + 1, true);
        } else {
            // late move reductions
            int R = 0;
            if (quiet && depth >= 3 && legal > 3 && ! inCheck && ! givesCheck) {
                R = 1 + (legal > 10) + (depth > 6);
            }
            v = -_search(-alpha - 1, -alpha, newDepth - R, ply + 1, true);
            if (v > alpha && R > 0) {
                v = -_search(-alpha - 1, -alpha, newDepth, ply + 1, true);
            }
            if (v > alpha && v < beta) {
                v = -_search(-beta, -alpha, newDepth, ply + 1, true);
            }
        }
        _pop();
        _p.undo(back);
        if (_aborted) {
            return 0;
        }

        if (v > best) {
            best = v;
            if (v > alpha) {
                alpha    = v;
                bestMove = mv;
                // principal variation
                _pv[ply][ply] = mv;
                for (int j = ply + 1; j < _pvLen[ply + 1]; ++j) {
                    _pv[ply][j] = _pv[ply + 1][j];
                }
                _pvLen[ply] = std::max(_pvLen[ply + 1], ply + 1);
                if (ply == 0) {
                    _rootBest = mv;
                }
                if (v >= beta) {
                    if (quiet) {
                        if (_killer[ply][0] != mv) {
                            _killer[ply][1] = _killer[ply][0];
                            _killer[ply][0] = mv;
                        }
                        int &h = _history[historyIndex(_p, mv)][Move::to(mv)];
                        h = std::min(h + depth * depth, 1 << 20);
                    }
                    break;
                }
            }
        }
    }

    if (legal == 0) {
        return -MateScore + ply;            // no legal move: lost
    }
    if (best == -InfScore) {
        best = alpha;                       // everything was pruned
    }

    Bound b = best >= beta ? BoundLower :
              best > oldAlpha ? BoundExact : BoundUpper;
    _tt.store(key, scoreToTT(best, ply), bestMove, depth, b);
    return best;
}

void Searcher::_report (int depth, int score, int64_t elapsed)
{
    if (! _verbose) {
        return;
    }
    std::ostringstream s;
    s << "info depth " << depth << " seldepth " << _seldepth;
    if (std::abs(score) >= MateInMaxPly) {
        int plies = MateScore - std::abs(score);
        s << " score mate " << (score > 0 ? plies : -plies);
    } else {
        s << " score cp " << score;
    }
    s << " nodes " << _nodes
      << " nps "   << (_nodes * 1000 / static_cast<uint64_t>(std::max<int64_t>(elapsed, 1)))
      << " time "  << elapsed
      << " hashfull " << _tt.hashfull()
      << " pv";
    for (int i = 0; i < _pvLen[0]; ++i) {
        s << " " << usiMove(_pv[0][i]);
    }
    say(s.str());
}

SearchResult Searcher::think (const Game &game, const Limits &lim, bool verbose)
{
    SearchResult r;
    _p        = game.pos;
    _lim      = lim;
    _verbose  = verbose;
    _aborted  = false;
    _nodes    = 0;
    _seldepth = 0;
    _barrier  = 0;
    _keys     = game.keys;
    _checks   = game.checks;
    memset(_killer,  0, sizeof(_killer));
    memset(_history, 0, sizeof(_history));
    _tt.newSearch();

    // legal root moves
    MoveList root;
    _p.genMove(root);
    std::vector<Move::Move> legal;
    for (auto mv : root) {
        auto back = _p.move(plain(mv));
        if (! _illegalDrop(plain(mv))) {
            legal.push_back(plain(mv));
        }
        _p.undo(back);
    }
    if (legal.empty()) {
        r.score = -MateScore;
        return r;                           // lost: resign
    }
    r.best = legal[0];

    // with only one legal move there is nothing to think about
    int maxDepth = legal.size() == 1 ? 0 : lim.depth;

    int score = 0;
    for (int depth = 1; depth <= maxDepth; ++depth) {
        _rootDepth = depth;
        _rootBest  = Move::None;
        _seldepth  = 0;

        // aspiration window
        int delta = 60;
        int alpha = depth >= 4 ? std::max(score - delta, -InfScore) : -InfScore;
        int beta  = depth >= 4 ? std::min(score + delta,  InfScore) :  InfScore;
        int v;
        while (true) {
            v = _search(alpha, beta, depth, 0, false);
            if (_aborted) {
                break;
            }
            if (v <= alpha) {
                alpha = std::max(v - delta, -InfScore);
            } else if (v >= beta) {
                beta  = std::min(v + delta,  InfScore);
            } else {
                break;
            }
            delta *= 3;
        }

        // a move searched completely in an aborted iteration is usable
        if (_rootBest != Move::None) {
            r.best = _rootBest;
        }
        if (_aborted) {
            break;
        }

        score   = v;
        r.score = v;
        r.depth = depth;
        int64_t elapsed = nowMs() - searchStart;
        _report(depth, v, elapsed);

        if (! lim.infinite && ! pondering) {
            if (elapsed >= lim.softMs) {
                break;
            }
            if (std::abs(v) >= MateInMaxPly && depth > MateScore - std::abs(v)) {
                break;
            }
        }
    }

    // wait for "stop" or "ponderhit" instead of returning early
    while ((lim.infinite || pondering) && ! stopRequest) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    r.nodes = _nodes;
    return r;
}

/// Time budget from the clock (all in milliseconds)
Limits timeLimits (int64_t time, int64_t inc, int64_t byoyomi, int64_t margin)
{
    Limits l;
    int64_t total = time + byoyomi + inc - margin;
    int64_t soft  = time / 40 + inc * 9 / 10 + byoyomi - margin;
    int64_t hard  = time / 8  + inc + byoyomi - margin;
    hard   = std::max<int64_t>(std::min(hard, total), 10);
    soft   = std::max<int64_t>(std::min(soft, hard), 10);
    // with byoyomi only, the time is lost unless used
    if (time <= 0 && byoyomi > 0) {
        soft = hard;
    }
    l.softMs = soft * 6 / 10;
    l.hardMs = hard;
    return l;
}

/// Fixed time per move
Limits fixedTime (int64_t ms)
{
    Limits l;
    l.softMs = ms;
    l.hardMs = ms;
    return l;
}

/* ------------------------------------------------------------------------- */



/* ------------------------------ USI engine ------------------------------- */

class UsiEngine
{
public:
    UsiEngine () : _searcher(_tt) { _tt.resize(_hashMb); }
    ~UsiEngine () { _stop(); }

    void run (void);

private:
    void _stop     (void);
    void _position (std::istringstream &is);
    void _go       (std::istringstream &is);
    void _option   (std::istringstream &is);

    TransTable              _tt;
    Searcher                _searcher;
    Game                    _game;
    std::thread             _thread;
    size_t                  _hashMb   = 64;
    int64_t                 _margin   = 300;
    bool                    _hashDirty = false;
};

void UsiEngine::_stop (void)
{
    if (_thread.joinable()) {
        stopRequest = true;
        _thread.join();
    }
    stopRequest = false;
    pondering   = false;
}

void UsiEngine::_option (std::istringstream &is)
{
    std::string token, name, value;
    is >> token;                            // "name"
    while (is >> token && token != "value") {
        name += (name.empty() ? "" : " ") + token;
    }
    is >> value;
    if (name == "USI_Hash") {
        _hashMb    = static_cast<size_t>(std::max(1, atoi(value.c_str())));
        _hashDirty = true;
    } else if (name == "NetworkDelay") {
        _margin = std::max(0, atoi(value.c_str()));
    }
}

void UsiEngine::_position (std::istringstream &is)
{
    std::string token;
    is >> token;
    if (token == "startpos") {
        _game.reset();
        is >> token;                        // "moves" if any
    } else if (token == "sfen") {
        std::string board, side, hands, number;
        is >> board >> side >> hands >> number;
        if (! _game.setSfen(board, side, hands)) {
            say("info string invalid sfen");
            _game.reset();
            return;
        }
        is >> token;                        // "moves" if any
    }
    while (is >> token) {
        auto m = parseUsiMove(_game.pos, token);
        if (m == Move::None) {
            say("info string illegal move " + token);
            return;
        }
        _game.play(m);
    }
}

void UsiEngine::_go (std::istringstream &is)
{
    int64_t     time[2] = {0, 0}, inc[2] = {0, 0};
    int64_t     byoyomi = 0, movetime = 0;
    Limits      lim;
    bool        ponder  = false;
    std::string token;
    while (is >> token) {
        if      (token == "btime")    { is >> time[Color::Black]; }
        else if (token == "wtime")    { is >> time[Color::White]; }
        else if (token == "binc")     { is >> inc [Color::Black]; }
        else if (token == "winc")     { is >> inc [Color::White]; }
        else if (token == "byoyomi")  { is >> byoyomi;            }
        else if (token == "movetime") { is >> movetime;           }
        else if (token == "depth")    { is >> lim.depth; lim.infinite = false; }
        else if (token == "nodes")    { is >> lim.nodes;          }
        else if (token == "infinite") { lim.infinite = true;      }
        else if (token == "ponder")   { ponder = true;            }
        else if (token == "mate")     {
            say("checkmate notimplemented");
            return;
        }
    }

    auto us = _game.pos.turn();
    if (! lim.infinite) {
        Limits t = movetime > 0 ? fixedTime(std::max<int64_t>(movetime - _margin, 10))
                 : time[us] || inc[us] || byoyomi
                            ? timeLimits(time[us], inc[us], byoyomi, _margin)
                            : fixedTime(lim.depth < MaxPly - 1 || lim.nodes
                                        ? INT64_MAX / 2 : 5000);
        lim.softMs = t.softMs;
        lim.hardMs = t.hardMs;
    }
    lim.depth = std::max(1, std::min(lim.depth, MaxPly - 1));

    if (_hashDirty) {
        _tt.resize(_hashMb);
        _hashDirty = false;
    }

    stopRequest = false;
    pondering   = ponder;
    searchStart = nowMs();
    Game game   = _game;
    _thread = std::thread([this, game, lim] {
        auto r = _searcher.think(game, lim, true);
        std::string line = "bestmove " + usiMove(r.best);
        say(line);
    });
}

void UsiEngine::run (void)
{
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream is(line);
        std::string        cmd;
        is >> cmd;
        if (cmd == "usi") {
            say(std::string("id name ")   + EngineName);
            say(std::string("id author ") + EngineAuthor);
            say("option name USI_Hash type spin default 64 min 1 max 4096");
            say("option name USI_Ponder type check default false");
            say("option name NetworkDelay type spin default 300 min 0 max 10000");
            say("usiok");
        } else if (cmd == "isready") {
            _stop();
            if (_hashDirty) {
                _tt.resize(_hashMb);
                _hashDirty = false;
            }
            say("readyok");
        } else if (cmd == "setoption") {
            _option(is);
        } else if (cmd == "usinewgame") {
            _stop();
            _tt.clear();
        } else if (cmd == "position") {
            _stop();
            _position(is);
        } else if (cmd == "go") {
            _stop();
            _go(is);
        } else if (cmd == "stop") {
            _stop();
        } else if (cmd == "ponderhit") {
            searchStart = nowMs();
            pondering   = false;
        } else if (cmd == "gameover") {
            _stop();
        } else if (cmd == "quit") {
            break;
        } else if (cmd == "d" || cmd == "show") {
            // not USI: show the position for debugging
            std::lock_guard<std::mutex> lock(outputLock);
            std::cout << _game.pos << toSfen(_game.pos) << std::endl;
        }
    }
    _stop();
}

/* ------------------------------------------------------------------------- */



/* --------------------------- terminal play ------------------------------- */

/// Result of a game from the point of view of the side to move
enum class Outcome { None, Lost, Won, Draw };

/// Check if the game is over at the current position
Outcome gameOver (Game &g)
{
    MoveList m;
    g.pos.genMove(m);
    int legal = 0;
    for (auto mv : m) {
        legal += ! illegalPawnDrop(g.pos, plain(mv));
    }
    if (legal == 0) {
        return Outcome::Lost;
    }
    if (g.occurrences() >= 4) {
        // perpetual check: the side that kept checking loses
        size_t n = g.keys.size() - 1, j = n;
        while (j >= 2 && (j -= 2, g.keys[j] != g.keys[n])) {}
        bool checkedUs = true, checkedThem = true;
        for (size_t i = j + 2; i <= n; i += 2) checkedUs   = checkedUs   && g.checks[i];
        for (size_t i = j + 1; i <  n; i += 2) checkedThem = checkedThem && g.checks[i];
        return checkedUs ? Outcome::Won : checkedThem ? Outcome::Lost : Outcome::Draw;
    }
    if (g.moves.size() >= MaxGamePlies) {
        return Outcome::Draw;
    }
    return Outcome::None;
}

const char * colorName (Color::Color c)
{
    return c == Color::Black ? "Black (Sente)" : "White (Gote)";
}

void printHelp (void)
{
    std::cout
        << "Enter a move in USI (7g7f, 8h2b+, P*5e) or CSA (7776FU) notation.\n"
        << "Commands: moves  - list the legal moves\n"
        << "          undo   - take back your last move\n"
        << "          hint   - let the program suggest a move\n"
        << "          sfen   - print the position in SFEN\n"
        << "          resign - give up\n"
        << "          quit   - leave the program\n";
}

int playMode (Color::Color human, int64_t ms)
{
    TransTable tt;
    tt.resize(64);
    Searcher   searcher(tt);
    Game       g;

    std::cout << "You play " << colorName(human) << ". The program thinks "
              << ms / 1000.0 << " s per move. Type 'help' for help.\n";

    while (true) {
        std::cout << g.pos;
        auto over = gameOver(g);
        if (over != Outcome::None) {
            auto mover = g.pos.turn();
            if (over == Outcome::Draw) {
                std::cout << "Draw (repetition or move limit).\n";
            } else {
                auto winner = over == Outcome::Won ? mover : Color::flip(mover);
                std::cout << colorName(winner) << " wins.\n";
            }
            return EXIT_SUCCESS;
        }

        if (g.pos.turn() != human) {
            std::cout << "Thinking..." << std::endl;
            searchStart = nowMs();
            auto r = searcher.think(g, fixedTime(ms), false);
            std::cout << "Program plays " << usiMove(r.best) << " ("
                      << g.pos.string(r.best) << "), eval " << r.score
                      << ", depth " << r.depth << "\n\n";
            g.play(r.best);
            continue;
        }

        std::cout << colorName(human) << " to move" << (g.pos.nchecks() ? " (check!)" : "")
                  << "> " << std::flush;
        std::string line;
        if (! std::getline(std::cin, line)) {
            return EXIT_SUCCESS;
        }
        std::istringstream is(line);
        std::string        cmd;
        if (! (is >> cmd)) {
            continue;
        }
        if (cmd == "quit" || cmd == "exit") {
            return EXIT_SUCCESS;
        } else if (cmd == "help" || cmd == "?") {
            printHelp();
        } else if (cmd == "resign") {
            std::cout << colorName(Color::flip(human)) << " wins.\n";
            return EXIT_SUCCESS;
        } else if (cmd == "sfen") {
            std::cout << "sfen " << toSfen(g.pos) << "\n";
        } else if (cmd == "undo") {
            if (g.moves.size() >= 2) {
                g.takeBack();
                g.takeBack();
            } else {
                std::cout << "Nothing to take back.\n";
            }
        } else if (cmd == "moves") {
            MoveList m;
            allLegalMoves(g.pos, m);
            for (auto mv : m) {
                std::cout << usiMove(mv) << " ";
            }
            std::cout << "\n";
        } else if (cmd == "hint") {
            searchStart = nowMs();
            auto r = searcher.think(g, fixedTime(ms / 2), false);
            std::cout << "Hint: " << usiMove(r.best) << " ("
                      << g.pos.string(r.best) << ")\n";
        } else {
            auto m = parseMove(g.pos, cmd);
            if (m == Move::None) {
                std::cout << "Illegal move or unknown command: " << cmd << "\n";
                continue;
            }
            std::cout << "\n";
            g.play(m);
        }
    }
}

/* ------------------------------------------------------------------------- */



/* -------------------------- CSA server client ---------------------------- */

int csaMode (const std::string &user, const std::string &pass,
             const std::string &host, int64_t ms)
{
    TransTable tt;
    tt.resize(256);
    Searcher   searcher(tt);

    CSAConnection csa(host);
    csa.connect();
    csa.login(user, pass);
    CSASummary summary = csa.newGame();
    if (! csa.accept()) {
        std::cerr << "The game was rejected." << std::endl;
        return EXIT_FAILURE;
    }

    Game g;
    Position start(summary);
    g.pos = start;
    g.start();
    for (auto m : summary.sequence) {
        auto mv = parseCsaMove(g.pos, m.move);
        if (mv != Move::None) {
            g.play(mv);
        }
    }
    auto myturn = Position::myTurn(summary);

    foundation::Vector<std::string> message;
    while (true) {
        std::cout << g.pos << std::endl;
        if (g.pos.turn() == myturn) {
            searchStart = nowMs();
            auto r = searcher.think(g, fixedTime(ms), false);
            if (r.best == Move::None) {
                csa.resign();
                break;
            }
            csa.send(g.pos.string(r.best));
            g.play(r.best);
            continue;
        }
        csa.receive(message);
        if (message.vsize() == 0 ||
            (message[0][0] != '+' && message[0][0] != '-')) {
            for (auto l : message) {
                std::cout << l << std::endl;
            }
            break;
        }
        // the server echoes our moves
        if ((message[0][0] == '+') == (myturn == Color::Black)) {
            continue;
        }
        auto mv = parseCsaMove(g.pos, message[0].substr(0, 7));
        if (mv == Move::None) {
            std::cerr << "Unexpected move: " << message[0] << std::endl;
            break;
        }
        g.play(mv);
    }
    while (csa.message()) {
        csa.receive(message);
        if (message.vsize() == 0) {
            break;
        }
        for (auto l : message) {
            std::cout << l << std::endl;
        }
    }
    csa.write("kifu.csa");
    csa.logout();
    csa.close();
    return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------------- */



/* ------------------------- self play and bench --------------------------- */

int selfPlayMode (int games, int64_t ms)
{
    TransTable tt;
    tt.resize(64);
    Searcher   searcher(tt);
    int        wins[2] = {0, 0}, draws = 0;

    for (int n = 0; n < games; ++n) {
        Game g;
        tt.clear();
        Outcome over;
        while ((over = gameOver(g)) == Outcome::None) {
            searchStart = nowMs();
            auto r = searcher.think(g, fixedTime(ms), false);
            if (findLegal(g.pos, r.best) == Move::None) {
                std::cerr << "BUG: illegal move " << usiMove(r.best)
                          << " in " << toSfen(g.pos) << std::endl;
                return EXIT_FAILURE;
            }
            g.play(r.best);
        }
        std::string result;
        if (over == Outcome::Draw) {
            ++draws;
            result = "draw";
        } else {
            auto winner = over == Outcome::Won ? g.pos.turn()
                                               : Color::flip(g.pos.turn());
            ++wins[winner];
            result = winner == Color::Black ? "black wins" : "white wins";
        }
        std::cout << "game " << n + 1 << ": " << result << " in "
                  << g.moves.size() << " plies" << std::endl;
    }
    std::cout << "black " << wins[Color::Black] << ", white "
              << wins[Color::White] << ", draws " << draws << std::endl;
    return EXIT_SUCCESS;
}

int benchMode (int depth)
{
    const char * const positions[] = {
        "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1",
        "lnsgk2nl/1r4gs1/p1pppp1pp/1p4p2/7P1/2P6/PP1PPPP1P/1SG4R1/LN2KGSNL b Bb 17",
        "ln1g3nl/1ks1g1r2/1ppppsbpp/p4pp2/7P1/P1P1P1P2/1PBPSP2P/2K1G2R1/LNSG3NL b - 31",
        "l6nl/5+P1gk/2np1S3/p1p4Pp/3P2Sp1/1PPb2P1P/P5GS1/R8/LN4bKL w RGgsn5p 1",
    };
    TransTable tt;
    tt.resize(64);
    Searcher   searcher(tt);
    uint64_t   nodes = 0;
    int64_t    start = nowMs();
    for (auto s : positions) {
        std::istringstream is(s);
        std::string board, side, hands;
        is >> board >> side >> hands;
        Game g;
        g.setSfen(board, side, hands);
        tt.clear();
        Limits lim;
        lim.depth  = depth;
        lim.softMs = lim.hardMs = INT64_MAX / 2;
        searchStart = nowMs();
        auto r = searcher.think(g, lim, false);
        nodes += r.nodes;
        std::cout << "bestmove " << usiMove(r.best) << " score " << r.score
                  << " nodes " << r.nodes << "  (" << s << ")" << std::endl;
    }
    int64_t ms = std::max<int64_t>(nowMs() - start, 1);
    std::cout << "total nodes " << nodes << ", " << ms << " ms, "
              << nodes * 1000 / static_cast<uint64_t>(ms) << " nps" << std::endl;
    return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------------- */

void usage (const char *name)
{
    std::cerr
        << "Usage:\n"
        << "  " << name << "                          USI engine (for Shogi GUIs)\n"
        << "  " << name << " play [black|white] [sec] play against the program\n"
        << "  " << name << " csa <user> <pass> <host> [sec]\n"
        << "  " << name << "                          play on a CSA server\n"
        << "  " << name << " selfplay [games] [msec]  the program plays itself\n"
        << "  " << name << " bench [depth]            search benchmark\n";
}

}   // namespace



/**
 * Main function
 * @param argc number of command option when invoked.
 * @param argv strings of comannd options in array.
 * @return status code reflecting normal end or error end.
 */
int main (int argc, char *argv[])
{
    // initialize the library and the evaluation
    Shogi::initialize();
    Position::setValue  (Evaluation::Value);
    Position::handsValue(Evaluation::Hands);
    initEvaluation();

    std::string mode = argc > 1 ? argv[1] : "usi";

    if (mode == "usi") {
        if (isatty(STDIN_FILENO)) {
            std::cerr << "USI engine. Type 'usi' to start, or run '"
                      << argv[0] << " play' to play in this terminal."
                      << std::endl;
        }
        UsiEngine engine;
        engine.run();
        return EXIT_SUCCESS;
    }
    if (mode == "play") {
        auto human = Color::Black;
        if (argc > 2 && std::string(argv[2]) == "white") {
            human = Color::White;
        }
        double sec = argc > 3 ? atof(argv[3]) : 5.0;
        return playMode(human, static_cast<int64_t>(sec * 1000));
    }
    if (mode == "csa" && argc >= 5) {
        double sec = argc > 5 ? atof(argv[5]) : 10.0;
        return csaMode(argv[2], argv[3], argv[4],
                       static_cast<int64_t>(sec * 1000) - 500);
    }
    if (mode == "selfplay") {
        int     games = argc > 2 ? atoi(argv[2]) : 1;
        int64_t ms    = argc > 3 ? atoll(argv[3]) : 200;
        return selfPlayMode(games, ms);
    }
    if (mode == "bench") {
        return benchMode(argc > 2 ? atoi(argv[2]) : 7);
    }

    usage(argv[0]);
    return EXIT_FAILURE;
}
