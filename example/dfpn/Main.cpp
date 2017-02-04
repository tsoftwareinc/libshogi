#include <iostream>
#include <iomanip>
#include <limits>
#include <Shogi.h>
#include <CSAFile.h>
#include <Array.h>

#include "CN.h"

using namespace foundation;
using namespace game;

/* ------------------------------ parameters ------------------------------- */

/// ハッシュウィンドウ
static const size_t         HashWindow       = 16;

/// ハッシュサイズ
static const size_t         HashSize         = 0x1000000ULL + HashWindow + 1;

/// 手順最大記録数
static const size_t         MaxDepth         = 5000;

// 証明数，反証数の有限最大値
template<>
const int                   CN<int>::Immense = 0x80000;

/* ------------------------------------------------------------------------- */

/* --------------------------- global  variables --------------------------- */

// 手順を記録する文字列の配列
std::string                 _sequence[MaxDepth];

// ハッシュ
class HashEntry;
static HashEntry *          _hash = nullptr;

/* ------------------------------------------------------------------------- */

/* --------------------------- HashEntry class ----------------------------- */

class HashEntry {
public:

    static const uint64_t   Shift = 16;
    static const uint64_t   Mask  = 0x000000ffffff0000ULL;
    static size_t           index (uint64_t key) {
                                return ((key & Mask) >> Shift);
                            }

    Zobrist::key            key;
    CN<int>                 pn;
    CN<int>                 dn;
};

/* ------------------------------------------------------------------------- */

/* -------------------------- function prototypes -------------------------- */

// 比較
template <typename T>
static inline T min(const T &v1, const T &v2) {return (v1 < v2 ? v1 : v2); }

// ハッシュ初期化
static void    initHash       (void);
// ハッシュクリア
static void    clearHash      (void);
// ハッシュリセット
static void    resetHash      (void);
// ハッシュ計測
static void    countHash      (void);
// ハッシュ登録
static void    PutInHash      (Zobrist::key, CN<int>,   CN<int>  );
// ハッシュ検索
static void    LookUpHash     (Zobrist::key, CN<int> &, CN<int> &);

// 手を記録する
static void    recordMove     (int, std::string);
// 手を表示する
static void    showSequence   (void);

// 子ノードの選択
static size_t  SelectChildOr  (Position &, Array<Move::Move, Move::Max> &,
                                                     CN<int> &, CN<int> &);
static size_t  SelectChildAnd (Position &, Array<Move::Move, Move::Max> &,
                                                     CN<int> &, CN<int> &);

// 子ノードの最小反証数
static CN<int> DeltaMinOr     (Position &, Array<Move::Move, Move::Max> &);
// 子ノードの最小証明数
static CN<int> DeltaMinAnd    (Position &, Array<Move::Move, Move::Max> &);

// 子ノードの証明数の和
static CN<int> PhiSumOr       (Position &, Array<Move::Move, Move::Max> &);
// 子ノードの反証数の和
static CN<int> PhiSumAnd      (Position &, Array<Move::Move, Move::Max> &);

// ノード展開
static void    MIDOr          (Position &, CN<int> &, CN<int> &, int);
static void    MIDAnd         (Position &, CN<int> &, CN<int> &, int);

/* ------------------------------------------------------------------------- */

/**
 * ハッシュ初期化
 */
static void initHash (void)
{

    _hash = new HashEntry[HashSize];

    std::cout << "Hash Size : "
              << ((HashSize * sizeof(HashEntry)) >> 20) << "MiB" << std::endl;

    clearHash();

}

/**
 * ハッシュクリア
 */
static void clearHash (void)
{

    for (size_t i = 0; i < HashSize; ++i) {
        _hash[i].key = 0;
    }

}

/**
 * ハッシュリセット
 */
static void resetHash (void)
{

    for (size_t i = 0; i < HashSize; ++i) {
        if (_hash[i].pn.isinf() || _hash[i].dn.isinf()) {
            continue;
        }
        _hash[i].key = 0;
    }

}


/**
 * ハッシュ計測
 */
static void countHash (void)
{

    size_t                  count = 0;

    for (size_t i = 0; i < HashSize; ++i) {
        if (_hash [i].key != 0) {
            ++count;
        }
    }

    std::cout << "Hash Utilization : "
              << count << "/" << HashSize << std::endl << std::endl;

}

/**
 * ハッシュ登録
 * @parm key ハッシュキー
 * @parm pn  証明数
 * @parm dn  反証数
 */
static void PutInHash (Zobrist::key key, CN<int> pn, CN<int> dn)
{

    size_t idx = HashEntry::index(key);
    size_t i;

    for (i = 0; i < HashWindow; ++i, ++idx) {
        auto _key = _hash[idx].key;
        if (_key == 0 || _key == key) {
            break;
        }
    }

    if (i >= HashWindow) {
        resetHash();
        std::cout << "Reset Hash." << std::endl;
        showSequence();
        countHash();
    }

    _hash[idx].key = key;
    _hash[idx].pn  = pn;
    _hash[idx].dn  = dn;

}

/**
 * ハッシュ検索
 * @parm key ハッシュキー
 * @parm pn  証明数
 * @parm dn  反証数
 */
static void LookUpHash (Zobrist::key key, CN<int> &pn, CN<int> &dn)
{

    size_t idx = HashEntry::index(key);

    for (size_t i = 0; i < HashWindow; ++i, ++idx) {
        if (_hash[idx].key == key) {
            pn = _hash[idx].pn;
            dn = _hash[idx].dn;
            return;
        }
    }

    pn = 1;
    dn = 1;

}

/**
 * 手順を記録する
 * @parm dx   局面
 * @parm move 指し手文字列
 */
static void recordMove (int dx, std::string move)
{

    _sequence[dx    ] = move;
    _sequence[dx + 1].erase();

}

/**
 * 手順を表示する
 */
static void showSequence (void)
{
    for (auto str : _sequence) {
        if (str.empty()) {
            break;
        }
        std::cout << str << " ";
    }
    std::cout << std::endl;
}

/**
 * 子ノードの選択 (子ノードが OR)
 * @parm p   局面
 * @parm m   候補手
 * @parm pn  証明数
 * @parm dn2 二番目に小さい反証数
 * @return 手のインデックス
 */
static size_t SelectChildOr (Position &p, Array<Move::Move, Move::Max> &m,
                                                 CN<int> &pn, CN<int> &dn2)
{

    CN<int>                 dn1;
    size_t                  best = 0;

    pn.infinity();

    for (size_t i = 0; i < m.vsize(); ++i) {

        // 子局面を作る
        auto back = p.move(m[i]);

        // ハッシュ検索
        CN<int> _pn, _dn;
        LookUpHash(p.hash(), _pn, _dn);

        // 局面を戻す
        p.undo(back);

        if (_dn < dn1) {
            best = i;
            pn   = _pn;
            dn2  = dn1;
            dn1  = _dn;
        } else
        if (_dn < dn2) {
            dn2  = _dn;
        }

    }

    return best;
    
}

/**
 * 子ノードの選択 (子ノードが AND)
 * @parm p   局面
 * @parm m   候補手
 * @parm dn  反証数
 * @parm pn2 二番目に小さい証明数
 * @return 手のインデックス
 */
static size_t SelectChildAnd (Position &p, Array<Move::Move, Move::Max> &m,
                                                  CN<int> &dn, CN<int> &pn2)
{

    CN<int>                 pn1;
    size_t                  best = 0;

    dn.infinity();

    for (size_t i = 0; i < m.vsize(); ++i) {

        // 子局面を作る
        auto back = p.move(m[i]);

        // ハッシュ検索
        CN<int> _pn, _dn;
        LookUpHash(p.hash(), _pn, _dn);

        // 局面を戻す
        p.undo(back);

        if (_pn < pn1) {
            best = i;
            dn   = _dn;
            pn2  = pn1;
            pn1  = _pn;
        } else
        if (_pn < pn2) {
            pn2  = _pn;
        }

    }

    return best;
    
}

/**
 * 子ノードの最小反証数 (子ノードが OR)
 * @parm p   局面
 * @parm m   候補手
 * @return 最小値
 */
static CN<int> DeltaMinOr (Position &p, Array<Move::Move, Move::Max> &m)
{

    CN<int> minimum;

    for (auto move : m) {
        // 子局面を作る
        auto back = p.move(move);

        // ハッシュ検索
        CN<int> _pn, _dn;
        LookUpHash(p.hash(), _pn, _dn);

        // 局面を戻す
        p.undo(back);

        minimum = min(_dn, minimum);

    }

    return minimum;

}

/**
 * 子ノードの最小証明数 (子ノードが AND)
 * @parm p   局面
 * @parm m   候補手
 * @return 最小値
 */
static CN<int> DeltaMinAnd (Position &p, Array<Move::Move, Move::Max> &m)
{

    CN<int> minimum;

    for (auto move : m) {
        // 子局面を作る
        auto back = p.move(move);

        // ハッシュ検索
        CN<int> _pn, _dn;
        LookUpHash(p.hash(), _pn, _dn);

        // 局面を戻す
        p.undo(back);

        minimum = min(_pn, minimum);

    }

    return minimum;

}

/**
 * 子ノードの証明数の和 (子ノードが OR)
 * @parm p   局面
 * @parm m   候補手
 * @return 証明数の和
 */
static CN<int> PhiSumOr (Position &p, Array<Move::Move, Move::Max> &m)
{

    CN<int> sum(0);

    for (auto move : m) {
        // 子局面を作る
        auto back = p.move(move);

        // ハッシュ検索
        CN<int> _pn, _dn;
        LookUpHash(p.hash(), _pn, _dn);

        // 局面を戻す
        p.undo(back);

        sum += _pn;

    }

    return sum;

}

/**
 * 子ノードの反証数の和 (子ノードが AND)
 * @parm p   局面
 * @parm m   候補手
 * @return 反証数の和
 */
static CN<int> PhiSumAnd (Position &p, Array<Move::Move, Move::Max> &m)
{

    CN<int> sum(0);

    for (auto move : m) {
        // 子局面を作る
        auto back = p.move(move);

        // ハッシュ検索
        CN<int> _pn, _dn;
        LookUpHash(p.hash(), _pn, _dn);

        // 局面を戻す
        p.undo(back);

        sum += _dn;

    }

    return sum;

}

/**
 * OR ノードの展開
 * @parm p  局面
 * @parm pn 証明数
 * @parm dn 反証数
 * @parm dx 探索深さ
 */
static void MIDOr (Position &p, CN<int> &pn, CN<int> &dn, int dx)
{

    // 1. ハッシュを引く
    CN<int> _pn, _dn;
    LookUpHash(p.hash(), _pn, _dn);
    if (pn <= _pn  || dn <= _dn ) {
        pn = _pn;
        dn = _dn;
        return;
    }

    // 2. 王手生成
    Array<Move::Move, Move::Max> m;
    p.genChckB(m);
    p.minorMove(m);

    // 候補手数
    int nmove = (int) m.vsize();

    // 不詰み (王手がない)
    if (nmove == 0) {
        pn.infinity();
        dn = 0;
        PutInHash(p.hash(), pn, dn);
        return;
    }

    // 3. ハッシュによるサイクル回避
    PutInHash(p.hash(), CN<int>(), CN<int>(0));

    // 4. 多重反復深化
    while (1) {

        CN<int> dmin = DeltaMinAnd(p, m);
        CN<int> psum = PhiSumAnd  (p, m);

        if (pn <= dmin || dn <= psum) {
            pn = dmin;
            dn = psum;
            PutInHash(p.hash(), pn, dn);
            return;
        }

        CN<int> _pn2;
        size_t best = SelectChildAnd(p, m, _dn, _pn2);
        _dn = (dn - psum) + _dn;
        _pn = min(pn, _pn2 + CN<int>(1));

        // 手を記録
        recordMove(dx, p.string(m[best]));
        // 子局面を作る
        auto back = p.move(m[best]);
        // AND ノードを展開
        MIDAnd(p, _pn, _dn, dx + 1);
        // 局面を戻す
        p.undo(back);

    }

}

/**
 * AND ノードの展開
 * @parm p  局面
 * @parm pn 証明数
 * @parm dn 反証数
 * @parm dx 探索深さ
 */
static void MIDAnd (Position &p, CN<int> &pn, CN<int> &dn, int dx)
{

    // 1. ハッシュを引く
    CN<int> _pn, _dn;
    LookUpHash(p.hash(), _pn, _dn);
    if (pn <= _pn  || dn <= _dn ) {
        pn = _pn;
        dn = _dn;
        return;
    }

    // 2. 応手生成
    Array<Move::Move, Move::Max> m;
    p.genMoveW(m);
    p.minorMove(m);

    // 候補手数
    int nmove = (int) m.vsize();

    // 詰み (応手がない)
    if (nmove == 0) {
        // 打ち歩詰め確認
        if (p.uchifuzume()) {
            pn.infinity();
            dn = 0;
        } else {
            pn = 0;
            dn.infinity();
        }
        PutInHash(p.hash(), pn, dn);
        return;
    }

    // 3. ハッシュによるサイクル回避
    PutInHash(p.hash(), CN<int>(), CN<int>(0));

    // 4. 多重反復深化
    while (1) {

        CN<int> dmin = DeltaMinOr(p, m);
        CN<int> psum = PhiSumOr  (p, m);

        if (dn <= dmin || pn <= psum) {
            dn = dmin;
            pn = psum;
            PutInHash(p.hash(), pn, dn);
            return;
        }

        CN<int> _dn2;
        size_t best = SelectChildOr(p, m, _pn, _dn2);
        _pn = (pn - psum) + _pn;
        _dn = min(dn, _dn2 + CN<int>(1));

        // 手を記録
        recordMove(dx, p.string(m[best]));
        // 子局面を作る
        auto back = p.move(m[best]);
        // OR ノードを展開
        MIDOr(p, _pn, _dn, dx + 1);
        // 局面を戻す
        p.undo(back);

    }
}

/**
 * Main function
 * @param argc number of command options when invoked
 * @param argv strings of command options in array
 * @return status code reflecting normal end or error end.
 */
int main (int argc, char * argv[])
{

    // 実行時引数を確認
    if (argc != 2) {
        std::cerr << "Usage : "
                  << argv[0] << "<User Name> <CSA File>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 将棋ライブラリ初期化
    Shogi::initialize();

    // 駒の価値を設定
    Position::setValue  (Evaluation::Value);
    Position::handsValue(Evaluation::Hands);

    // ハッシュ初期化
    initHash();

    // 棋譜読み込み
    CSAFile k(argv[1]);

    // 局面のインスタンスを生成
    Position p(k.summary());
    std::cout << p << std::endl;

    // 詰め探索
    CN<int> pn, dn;
    MIDOr(p, pn, dn, 0);

    // ハッシュ計測
    countHash();

    // 結果表示
    std::cout << "pn = " << pn << std::endl
              << "dn = " << dn << std::endl;
    showSequence();

    exit(EXIT_SUCCESS);

}
