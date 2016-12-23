/**
 *****************************************************************************

 @file       Maic.cpp

 @brief      Example

 @author     Hiroki Takada (http://wwww.tsoftware.jp/)

 @date       2016-12-14

 @version    $Id:$

  ----------------------------------------------------------------------------
  RELEASE NOTE :

   DATE          REV    REMARK
  ============= ====== =======================================================
  14th Dec 2016  0.1   Initial release

 *****************************************************************************/

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <Shogi.h>
#include <CSAConnection.h>
#include <CSAFile.h>
#include <List.h>
#include <Array.h>
#include <Atomic.h>
#include <SpinLock.h>
#include <Semaphore.h>
#include <Thread.h>

using namespace foundation;
using namespace game;
using namespace Evaluation;



/* ------------------------------ parameters ------------------------------- */

/// 置換表サイズ
static const size_t         TPSize          = 0x800000ULL;

/// 置換表登録に関する残り深さ閾値
static const int            TPLimit         = 3;

/// スレッド数 (物理コア数と同じにする)
static const int            NumberOfThreads = 2;

/// 最大思考時間
static const time_t         ThinkingTime    = 10;

/* ------------------------------------------------------------------------- */

/* --------------------------- global  variables --------------------------- */

/// 最大の探索深さ
static int                  _searchDepth    = 10;

/// 静止探索に入る探索深さ
static int                  _stableDepth    = 3;

/// 探索終了通知用セマフォ       (Thread -> Main  )
template <typename T, typename V>
Semaphore Thread<T, V>::globalSync;

/// 探索終了通知用アトミック変数 (Main   -> Thread)
static Atomic<int>          _stopSearch(0);

/// 置換表
class  TransPosition;
static TransPosition *      _TPB            = nullptr;
static TransPosition *      _TPW            = nullptr;

/* ------------------------------------------------------------------------- */

/* -------------------------- function prototypes -------------------------- */

/// 置換表の初期化クリア
static void     initTP    (void);
static void     clearTP   (void);

/// 置換表操作
static void     recordTPB (Zobrist::key key,
                           int depth, Eval  alpha, Eval  beta, Eval  value);
static void     recordTPW (Zobrist::key key,
                           int depth, Eval  alpha, Eval  beta, Eval  value);
static bool     searchTPB (Zobrist::key key,
                           int depth, Eval &alpha, Eval &beta, Eval &value);
static bool     searchTPW (Zobrist::key key,
                           int depth, Eval &alpha, Eval &beta, Eval &value);

/// 探索
static size_t   searchMax (Position &, Array<Move::Move, Move::Max> &);
static size_t   searchMin (Position &, Array<Move::Move, Move::Max> &);
static size_t   splitMax  (Position &, Array<Move::Move, Move::Max> &);
static size_t   splitMin  (Position &, Array<Move::Move, Move::Max> &);
static Eval     searchMax (Position &, Eval alpha, Eval beta, int depth);
static Eval     searchMin (Position &, Eval alpha, Eval beta, int depth);
static Eval     quiesMax  (Position &, Eval alpha, Eval beta, int depth);
static Eval     quiesMin  (Position &, Eval alpha, Eval beta, int depth);

/* ------------------------------------------------------------------------- */



/* ------------------------- TransPosition class --------------------------- */

/// 置換表エントリ
class TransPosition : public SpinLockObject {
public:

    TransPosition () : SpinLockObject() {}

    static const uint64_t   Shift = 16;
    static const uint64_t   Mask  = 0x0000007fffff0000ULL;
    static uint64_t         index (uint64_t key) {
                                return ((key & Mask) >> Shift);
                            }

    Zobrist::key            key;
    short                   depth;
    short                   value;
    enum : short            {
                                FailHigh,
                                FailLow,
                                Unknown,
                                Determined
                            } state;
};

/* ------------------------------------------------------------------------- */


/* ----------------------------  Thread class ------------------------------ */

/// 探索スレッドに渡すパラメータ
struct threadArg {

    threadArg() {}

    threadArg(Position *ptr, Eval a, Eval b, int d) :
     p(ptr), alpha(a), beta(b), depth(d)
    {}

    Position *              p;
    Eval                    alpha;
    Eval                    beta;
    int                     depth;
};

/// 先手番局面を探索するスレッド
class threadMax : public Thread<Eval, threadArg>
{

public:

    static void globalSync() {
        Thread<Eval, threadArg>::globalSync.wait();
    }

    threadMax () :
        Thread<Eval, threadArg> ()
    {}

    void run (threadArg a)
    {
        Thread<Eval, threadArg>::run(a);
    }

private:

    Eval _thread (threadArg a)
    {
        int rtn = nice(1);
        if (rtn == -1) {
            perror("nice()");
        }
        return searchMax(*a.p, a.alpha, a.beta, a.depth);
    }

};

/// 後手番局面を探索するスレッド
class threadMin : public Thread<Eval, threadArg>
{

public:

    static void globalSync() {
        Thread<Eval, threadArg>::globalSync.wait();
    }

    threadMin () :
        Thread<Eval, threadArg> ()
    {}

    void run (threadArg a)
    {
        Thread<Eval, threadArg>::run(a);
    }

private:

    Eval _thread (threadArg a)
    {
        int rtn = nice(1);
        if (rtn == -1) {
            perror("nice()");
        }
        return searchMin(*a.p, a.alpha, a.beta, a.depth);
    }

};

// タイマースレッド
class threadTimer : public Thread<int, time_t>
{

public:
    // 時計を見る間隔 [nsec]
    static const long       PollingInterval = 50000000L;
    // タイマースレッドの優先度 (最高)
    static const int        Priority        = -21;

    threadTimer () :
        Thread<int, time_t> ()
    {
        _itval.tv_sec  = 0;
        _itval.tv_nsec = PollingInterval;
    }

    void run (time_t t)
    {
        // 探索終了通知用アトミック変数をリセット
        _stopSearch.set(0);

        // スレッド起動
        Thread<int, time_t>::run(t);
    }

    void stop (void)
    {
        _stopSearch.set(1);
    }

private:

    time_t                  _limit;
    struct timespec         _itval;
    struct timespec         _rmain;

    int _thread (time_t t)
    {

        // タイマー発動時間を計算
        _limit = time(NULL) + t;

        while (_limit > time(NULL)) {
            _rmain = _itval;
            while (nanosleep(&_rmain, &_rmain) == -1);
            if (_stopSearch == 1) {
                break;
            }
        }

        // 探索終了通知用アトミック変数をセット
        _stopSearch.set(1);

        return 0;

    }

};


/* ------------------------------------------------------------------------- */



/* ---------------------------- implementations ---------------------------- */

/**
 * 置換表の初期化
 * 
 */
static void initTP (void)
{

    _TPB = new TransPosition[TPSize];
    _TPW = new TransPosition[TPSize];

    std::cout << "Hash Size : " 
              << ((TPSize * sizeof(TransPosition)) >> 19) << "MiB" << std::endl;

    clearTP();

}

/**
 * 置換表のクリア
 * 
 */
static void clearTP (void)
{

    for (size_t i = 0; i < TPSize; ++i) {
        _TPB[i].key = 0;
        _TPW[i].key = 0;
    }

}

/**
 * 置換表に登録 (先手)
 * @parm key   ハッシュキー
 * @parm depth 探索深さ
 * @parm alpha 下限値
 * @parm beta  上限値
 * @parm value 評価値
 */
static void recordTPB (Zobrist::key key,
                       int depth, Eval alpha, Eval beta, Eval value)
{

    auto _tp = _TPB;

    // 残り深さが少ないエントリは登録しない
    if ((_searchDepth - depth) < TPLimit) {
        return;
    }

    // インデックス計算
    auto idx = TransPosition::index(key);

    // Lock
    SpinLock lock(_tp[idx]);

    // 登録
    _tp[idx].key   = key;
    _tp[idx].depth = (short)(_searchDepth - depth);
    _tp[idx].value = (short)value;
    if (value >= beta) {
        // fail-high
        // 評価値は [value, +Infinity) の区間に存在する
        _tp[idx].state = TransPosition::FailHigh;
    } else
    if (value <  alpha) {
        // 下限が更新されなかった
        // 評価値は (-Infinity, value] の区間に存在する
        _tp[idx].state = TransPosition::Unknown;
    } else {
        // 決定された評価値
        _tp[idx].state = TransPosition::Determined;
    }

}

/**
 * 置換表に登録 (後手)
 * @parm key   ハッシュキー
 * @parm depth 探索深さ
 * @parm alpha 下限値
 * @parm beta  上限値
 * @parm value 評価値
 */
static void recordTPW (Zobrist::key key,
                       int depth, Eval alpha, Eval beta, Eval value)
{

    auto _tp = _TPW;

    // 残り深さが少ないエントリは登録しない
    if ((_searchDepth - depth) < TPLimit) {
        return;
    }

    // インデックス計算
    auto idx = TransPosition::index(key);

    // Lock
    SpinLock lock(_tp[idx]);

    // 登録
    _tp[idx].key   = key;
    _tp[idx].depth = (short)(_searchDepth - depth);
    _tp[idx].value = (short)value;
    if (value <= alpha) {
        // fail-low
        // 評価値は (-Infinity, value] の区間に存在する
        _tp[idx].state = TransPosition::FailLow;
    } else
    if (value >  beta ) {
        // 上限が更新されなかった
        // 評価値は [value, +Infinity) の区間に存在する
        _tp[idx].state = TransPosition::Unknown;
    } else {
        // 決定された評価値
        _tp[idx].state = TransPosition::Determined;
    }

}

/**
 * 置換表を検索 (先手)
 * @parm key   ハッシュキー
 * @parm depth 探索深さ
 * @parm alpha 下限値 (更新用)
 * @parm beta  上限値 (更新用)
 * @parm value 評価値 (更新用)
 * @return true ハッシュエントリ有、false 無し 
 */
static bool searchTPB (Zobrist::key key,
                       int depth, Eval &alpha, Eval &beta, Eval &value)
{

    auto _tp = _TPB;

    // インデックス計算
    auto idx = TransPosition::index(key);

    // Lock
    SpinLock lock(_tp[idx]);

    // この局面のエントリか
    if (_tp[idx].key != key ) {
        return false;
    }

    // 探索が浅いエントリは信用できない (候補手のオーダリングには使える)
    if (_tp[idx].depth < (_searchDepth - depth)) {
        return false;
    }

    if (_tp[idx].state == TransPosition::Unknown) {
        // 下限が更新されず登録されたエントリの場合、
        // 評価値は (-Infinity, value] の区間に存在する。別コンテクスト
        // からこの局面を参照した場合は上限を更新できる可能性がある
        beta = min(_tp[idx].value, beta);
        // 探索は続行
        return false;
    }

    // 評価値は value もしくは [value, +Infinity) の区間に存在するので
    // value >= beta の場合必ず上限値を超える (fail-high)
    if (_tp[idx].value >= beta) {
        value = beta;
        return true;
    }

    if (_tp[idx].state == TransPosition::FailHigh) {
        // fail-high により登録されたエントリの場合、
        // 評価値は [value, +Infinity) の区間に存在するので下限を更新
        // できる可能性がある
        alpha = max(_tp[idx].value, alpha);
        // 探索は続行
        return false;
    }

    // 決定された評価値を返す
    value = _tp[idx].value;

    return true;

}

/**
 * 置換表を検索 (後手)
 * @parm key   ハッシュキー
 * @parm depth 探索深さ
 * @parm alpha 下限値 (更新用)
 * @parm beta  上限値 (更新用)
 * @parm value 評価値 (更新用)
 * @return true ハッシュエントリ有、false 無し 
 */
static bool searchTPW (Zobrist::key key,
                       int depth, Eval &alpha, Eval &beta, Eval &value)
{

    auto _tp = _TPW;

    // インデックス計算
    auto idx = TransPosition::index(key);

    // Lock
    SpinLock lock(_tp[idx]);

    // この局面のエントリか
    if (_tp[idx].key != key  ) {
        return false;
    }

    // 探索が浅いエントリは信用できない (候補手のオーダリングには使える)
    if (_tp[idx].depth < (_searchDepth - depth)) {
        return false;
    }

    if (_tp[idx].state == TransPosition::Unknown) {
        // 上限が更新されず登録されたエントリの場合、
        // 評価値は [value, +Infinity) の区間に存在する。別コンテクスト
        // からこの局面を参照した場合は下限を更新できる可能性がある
        alpha = max(_tp[idx].value, alpha);
        // 探索は続行
        return false;
    }

    // 評価値は value もしくは (-Infinity, value] の区間に存在するので
    // value <= alpha の場合必ず下限値を下回る (fail-low)
    if (_tp[idx].value <= alpha)  {
        value = alpha;
        return true;
    }

    if (_tp[idx].state == TransPosition::FailLow) {
        // fail-low により登録されたエントリの場合、
        // 評価値は (-Infinity, value] の区間に存在するので上限を更新で
        // きる可能性がある
        beta = min(_tp[idx].value, beta);
        // 探索は続行
        return false;
    }

    // 決定された評価値を返す
    value = _tp[idx].value;

    return true;

}

/**
 * 先手最善手を調べる
 * @parm p 局面
 * @parm m 候補手
 * @return 最善手のインデックス
 */
static size_t searchMax (Position &p, Array<Move::Move, Move::Max> &m)
{

    size_t                  idx   = 0;
    Eval                    alpha = -Infinity;
    Eval                    beta  =  Infinity;

    for (size_t i = 0; i < m.vsize(); ++i) {
        // 局面を進める
        auto back  = p.move(m[i]);
        // 後手局面を探索
        auto value = searchMin(p, alpha, beta, 1);
        // 局面を戻す
        p.undo(back);
        // fail-high
        if (value >= beta ) {
            std::cout << "* " << value << " "
                      << p.string(m[i]) << std::endl;
            return i;
        }
        // 下限を更新
        if (value >  alpha) {
            alpha = value;
            idx   = i;
            std::cout << "  " << value << " "
                      << p.string(m[i]) << std::endl;
        }
    }

    return idx;

}

/**
 * 後手最善手を調べる
 * @parm p 局面
 * @parm m 候補手
 * @return 最善手のインデックス
 */
static size_t searchMin (Position &p, Array<Move::Move, Move::Max> &m)
{

    size_t                  idx   = 0;
    Eval                    alpha = -Infinity;
    Eval                    beta  =  Infinity;

    for (size_t i = 0; i < m.vsize(); ++i) {
        // 局面を進める
        auto back  = p.move(m[i]);
        // 先手局面を探索
        auto value = searchMax(p, alpha, beta, 1);
        // 局面を戻す
        p.undo(back);
        // fail-low
        if (value <= alpha) {
            std::cout << "* " << value << " "
                      << p.string(m[i]) << std::endl;
            return i;
        }
        // 上限を更新
        if (value <  beta ) {
            beta  = value;
            idx   = i;
            std::cout << "  " << value << " "
                      << p.string(m[i]) << std::endl;
        }
    }

    return idx;

}

/**
 * 先手最善手を調べる (並列探索)
 * @parm p 局面
 * @parm m 候補手
 * @return 最善手のインデックス
 */
static size_t splitMax (Position &p, Array<Move::Move, Move::Max> &m)
{

    size_t                  idx   = 0;
    Eval                    alpha = -Infinity;
    Eval                    beta  =  Infinity;

    // 候補手の数
    int tenum = (int)m.vsize();

    // Move::Move の上位 16 bit には評価値を書き込むことができる
    // ここではランダム探索の状態管理に使用する
    enum {
        NotYet,    // 未探索
        Done       // 探索済み
    };
    for (decltype(auto) check : m) {
        check = Move::setValue(NotYet, check);
    }

    // スレッド管理用データ
    struct Thread {
        threadMin   th;
        int         move;
    } th[NumberOfThreads];

    // 探索中スレッド管理用リスト
    List<struct Thread *> busy;

    // 空きスレッド管理用リスト
    List<struct Thread *> idle;
    for (decltype(auto) t : th) {
        idle.add(&t);
    }

    // fail-high の時には探索中のスレッドを片付けなければならない
    // fail-high になったら特別な処理を行うためのフラグ
    bool failhigh = false;

    // 全ての候補手を探索
    // done  : 探索済み
    // count : 探索スレッド起動回数
    for (int done = 0, count = 0; done < tenum; ) {

        // 空きスレッドが無い場合は終了を待つ
        // fail-high の時は探索中のスレッドを片付ける
        while (idle.size() == 0 || (failhigh && busy.size() != 0)) {

            // スレッド終了待ち
            threadMin::globalSync();

            // いずれかのスレッドが終了したので確認する
            // List 要素の削除操作を伴う場合はイテレータを使う
            for (auto itr = busy.begin(); itr != busy.end(); ++itr) {

                auto t = *itr;

                // スレッドの終了に同期を試みる
                if (! t->th.trysync()) {
                    // このスレッドはまだ探索中
                    continue;
                }

                // 探索結果を取得する
                auto value    = t->th.result();
                // 局面データ解放
                threadArg arg = t->th.argument();
                delete arg.p;

                // 探索中リストから削除
                itr.del();
                // 空きリストへ追加
                idle.add(t);
                // 探索済み + 1
                ++done;

                // fail-high 処理中か
                if (failhigh) {
                    break;
                }

                // fail-high
                if (value >= beta ) {
                    // スレッドに終了を通知
                    _stopSearch.set(1);
                    // fail-high 状態に入る
                    failhigh = true;
                    idx      = t->move;
                    std::cout << "* " << value << " "
                              << p.string(m[t->move]) << std::endl;
                    break;
                }

                // 下限更新
                if (value >  alpha) {
                    alpha = value;
                    idx   = t->move;
                    std::cout << "  " << value << " "
                              << p.string(m[t->move]) << std::endl;
                }

                // 終了通知 1 件に対して
                // 片付けは 1 スレッド
                break;
            }
        }

        // fail-high
        if (failhigh) {
            return idx;
        }

        // スレッドを起動する
        //

        // 空きスレッド取得
        auto t = idle.top();

        // 全ての手について探索スレッドを起動したか
        if (count >= tenum) {
            // 新しく探索を開始する手はない
            continue;
        }
        // 探索スレッド起動回数 + 1
        ++count;

        // 探索する手をランダムに決定する
        int idx = rand() % tenum;
        while (Move::value(m[idx]) != NotYet) {
            ++idx;
            if (idx >= tenum) {
                idx = 0;
            }
        }

        // 局面を進める
        auto back  = p.move(m[idx]);

        // 後手番局面を探索するスレッド生成
        threadArg arg(new Position(p), alpha, beta, 1);
        t->th.run(arg);
        t->move = idx;

        // 局面を戻す
        p.undo(back);

        // 選択された手を探索済みに変更
        m[idx] = Move::setValue(Done, m[idx]);

        // 探索中リストに登録
        busy.add(t);

    }

    return idx;

}

/**
 * 後手最善手を調べる (並列探索)
 * @parm p 局面
 * @parm m 候補手
 * @return 最善手のインデックス
 */
static size_t splitMin (Position &p, Array<Move::Move, Move::Max> &m)
{

    size_t                  idx   = 0;
    Eval                    alpha = -Infinity;
    Eval                    beta  =  Infinity;

    // 候補手の数
    int tenum = (int)m.vsize();

    // Move::Move の上位 16 bit には評価値を書き込むことができる
    // ここではランダム探索の状態管理に使用する
    enum {
        NotYet,    // 未探索
        Done       // 探索済み
    };
    for (decltype(auto) check : m) {
        check = Move::setValue(NotYet, check);
    }

    // スレッド管理用データ
    struct Threads {
        threadMax   th;
        int         move;
    } th[NumberOfThreads];

    // 探索中スレッド管理用リスト
    List<struct Threads *> busy;

    // 空きスレッド管理用リスト
    List<struct Threads *> idle;
    for (decltype(auto) t : th) {
        idle.add(&t);
    }

    // fail-low の時には探索中のスレッドを片付けなければならない
    // fail-low になったら特別な処理を行うためのフラグ
    bool faillow = false;

    // 全ての候補手を探索
    // done  : 探索済み
    // count : 探索スレッド起動回数
    for (int done = 0, count = 0; done < tenum; ) {

        // 空きスレッドが無い場合は探索中スレッドの終了を待つ
        // fail-low 時には探索中スレッドを全て片付ける
        while (idle.size() == 0 || (faillow && busy.size() != 0)) {

            // スレッド終了待ち
            threadMax::globalSync();

            // いずれかのスレッドが終了したので確認する
            // List 要素の削除操作を伴う場合はイテレータを使う
            for (auto itr = busy.begin(); itr != busy.end(); ++itr) {

                auto t = *itr;

                // スレッドの終了に同期を試みる
                if (! t->th.trysync()) {
                    // このスレッドはまだ探索中
                    continue;
                }

                // 探索結果を取得する
                auto value    = t->th.result();
                // 局面データ解放
                threadArg arg = t->th.argument();
                delete arg.p;

                // 探索中リストから削除
                itr.del();
                // 空きリストへ追加
                idle.add(t);
                // 探索済み + 1
                ++done;

                // fail-low 処理中か
                if (faillow) {
                    break;
                }

                // fail-low
                if (value <= alpha) {
                    // スレッドに終了を通知
                    _stopSearch.set(1);
                    // fail-low 状態に入る
                    faillow = true;
                    idx     = t->move;
                    std::cout << "* " << value << " "
                              << p.string(m[t->move]) << std::endl;
                    break;
                }

                // 上限更新
                if (value <  beta ) {
                    beta  = value;
                    idx   = t->move;
                    std::cout << "  " << value << " "
                              << p.string(m[t->move]) << std::endl;
                }

                // 終了通知 1 件に対して
                // 片付けは 1 スレッド
                break;
            }
        }

        // fail-low
        if (faillow) {
            return idx;
        }

        // スレッドを起動する
        //

        // 空きスレッド取得
        auto t = idle.top();

        // 全ての手について探索スレッドを起動したか
        if (count >= tenum) {
            // 新しく探索を開始する手は無い
            continue;
        }
        // 探索スレッド起動回数 + 1
        ++count;

        // 探索する手をランダムに選択
        int idx = rand() % tenum;
        while (Move::value(m[idx]) != NotYet) {
            ++idx;
            if (idx >= tenum) {
                idx = 0;
            }
        }

        // 局面を進める
        auto back  = p.move(m[idx]);

        // 先手番局面を探索するスレッド開始
        threadArg arg(new Position(p), alpha, beta, 1);
        t->th.run(arg);
        t->move = idx;

        // 局面を戻す
        p.undo(back);

        // 選択された手を探索済みに変更
        m[idx] = Move::setValue(Done, m[idx]);

        // 探索中リストに登録
        busy.add(t);

    }

    return idx;

}

/**
 * 先手番局面の評価値を探索により決定する (Max)
 * @parm p     局面
 * @parm alpha 下限値
 * @parm beta  上限値
 * @parm depth 探索の深さ
 * @return 評価値
 */
static Eval searchMax (Position &p, Eval alpha, Eval beta, int depth)
{

    // 評価値の初期値
    Eval vmax = -Infinity;

    // 置換表検索
    if (searchTPB(p.hash(), depth, alpha, beta, vmax)) {
        return vmax;
    }

    // 終了通知があるか
    if (_stopSearch == 1) {
        return beta;
    }

    // 末端なので静止探索の結果を返す
    if (depth > _stableDepth) {
        return quiesMax(p, alpha, beta, depth); 
    }

    // 先手番の候補手 (ほぼ合法手) を生成
    // 該当する手がなかった場合は詰み (-Infinity)
    Array<Move::Move, Move::Max> m;
    p.genFastB(m);

    for (auto move : m) {

        // 玉を取る手か
        if (p.dustyB(move)) {
            vmax = Infinity;
            break;
        }

        // 局面を進める
        auto back  = p.move(move);
        // 後手番で探索
        auto value = searchMin(p, max(vmax, alpha), beta, depth + 1);
        // 局面を戻す
        p.undo(back);

        // fail-high
        if (value >= beta) {
            // 置換表に登録
            recordTPB(p.hash(), depth, alpha, beta, value);
            return beta;
        }

        // 下限更新か
        if (value > vmax ) {
            vmax = value;
        }

    }

    // 置換表に登録
    recordTPB(p.hash(), depth, alpha, beta, vmax);

    return vmax;

}

/**
 * 後手番局面の評価値を探索により決定する (Min)
 * @parm p     局面
 * @parm alpha 下限値
 * @parm beta  上限値
 * @parm depth 探索の深さ
 * @return 評価値
 */
static Eval searchMin (Position &p, Eval alpha, Eval beta, int depth)
{

    // 評価値の初期値
    Eval vmin = Infinity;

    // 置換表検索
    if (searchTPW(p.hash(), depth, alpha, beta, vmin)) {
        return vmin;
    }

    // 終了通知があるか
    if (_stopSearch == 1) {
        return alpha;
    }

    // 末端なので静止探索の結果を返す
    if (depth > _stableDepth) {
        return quiesMin(p, alpha, beta, depth); 
    }

    // 後手番の候補手 (ほぼ合法手) を生成
    // 該当する手がなかった場合は詰み (Infinity)
    Array<Move::Move, Move::Max> m;
    p.genFastW(m);

    for (auto move : m) {

        // 玉を取る手か
        if (p.dustyW(move)) {
            vmin = -Infinity;
            break;
        }

        // 局面を進める
        auto back  = p.move(move);
        // 先手番で探索
        auto value = searchMax(p, alpha, min(vmin, beta), depth + 1);
        // 局面を戻す
        p.undo(back);

        // fail-low
        if (value <= alpha) {
            // 置換表に登録
            recordTPW(p.hash(), depth, alpha, beta, value);
            return alpha;
        }

        // 上限更新か
        if (value < vmin  ) {
            vmin = value;
        }

    }

    // 置換表に登録
    recordTPW(p.hash(), depth, alpha, beta, vmin);

    return vmin;

}

/**
 * 先手番局面の静的な評価値を探索により決定する (Max)
 * @parm p     局面
 * @parm alpha 下限値
 * @parm beta  上限値
 * @parm depth 探索の深さ
 * @return 評価値
 */
static Eval quiesMax (Position &p, Eval alpha, Eval beta, int depth)
{

    // 評価値の初期値は静的評価値
    // 上限が現在の静的評価値以下の場合探索しない
    Eval vmax = p.eval();

    // fail-high (standpat)
    if (beta <= vmax) {
        return beta;
    }

    // 置換表検索
    if (searchTPB(p.hash(), depth, alpha, beta, vmax)) {
        return vmax;
    }

    // 終了通知があるか
    if (_stopSearch == 1) {
        return beta;
    }

    // 末端なので静的評価値を返す
    // searchTPB() == false の場合 vmax は更新されていない
    if (depth > _searchDepth) {
        return vmax;
    }

    // 先手番で駒を取る手、玉手を回避する手のみを生成 (ほぼ合法手)
    // 該当する手がなかった場合は静的評価値がこの局面の評価値となる
    Array<Move::Move, Move::Max> m;
    p.genCaptB(m);

    for (auto move : m) {

        // 玉を取る手か
        if (p.dustyB(move)) {
            vmax = Infinity;
            break;
        }

        // 局面を進める
        auto back  = p.move(move);
        // 後手番で静止探索
        auto value = quiesMin(p, max(vmax, alpha), beta, depth + 1);
        // 局面を戻す
        p.undo(back);

        // fail-high
        if (value >= beta) {
            // 置換表に登録
            // depth > _stableDepth なので通常探索がこのエントリを
            // 参照することはない
            recordTPB(p.hash(), depth, alpha, beta, value);
            return beta;
        }

        // 下限更新か
        if (value > vmax) {
            vmax = value;
        }

    }

    // 置換表に登録
    // depth > _stableDepth なので通常探索がこのエントリを
    // 参照することはない
    recordTPB(p.hash(), depth, alpha, beta, vmax);

    return vmax;

}

/**
 * 後手番局面の静的な評価値を探索により決定する (Min)
 * @parm p     局面
 * @parm alpha 下限値
 * @parm beta  上限値
 * @parm depth 探索の深さ
 * @return 評価値
 */
static Eval quiesMin (Position &p, Eval alpha, Eval beta, int depth)
{

    // 評価値の初期値は静的評価値
    // 下限が現在の静的評価値以上の場合探索しない
    Eval vmin = p.eval();

    // fail-low (standpat)
    if (alpha >= vmin) {
        return alpha;
    }

    // 置換表検索
    if (searchTPW(p.hash(), depth, alpha, beta, vmin)) {
        return vmin;
    }

    // 終了通知があるか
    if (_stopSearch == 1) {
        return alpha;
    }

    // 末端なので静的評価値を返す
    // searchTPW() == false の場合 vmin は更新されていない
    if (depth > _searchDepth) {
        return vmin;
    }

    // 後手番で駒を取る手、玉手を回避する手のみを生成 (ほぼ合法手)
    // 該当する手がなかった場合は静的評価値がこの局面の評価値となる
    Array<Move::Move, Move::Max> m;
    p.genCaptW(m);

    for (auto move : m) {

        // 玉を取る手か
        if (p.dustyW(move)) {
            vmin = -Infinity;
            break;
        }

        // 局面を進める
        auto back  = p.move(move);
        // 先手番で静止探索
        auto value = quiesMax(p, alpha, min(vmin, beta), depth + 1);
        // 局面を戻す
        p.undo(back);

        // fail-low
        if (value <= alpha) {
            // 置換表に登録
            // depth > _stableDepth なので通常探索がこのエントリを
            // 参照することはない
            recordTPW(p.hash(), depth, alpha, beta, value);
            return alpha;
        }

        // 上限更新か
        if (value < vmin) {
            vmin = value;
        }

    }

    // 置換表に登録
    // depth > _stableDepth なので通常探索がこのエントリを
    // 参照することはない
    recordTPW(p.hash(), depth, alpha, beta, vmin);

    return vmin;

}



/**
 * Main function
 * @param argc number of command option when invoked
 * @param argv strings of comannd options in array
 * @return status code reflecting normal end or error end.
 */
int main (int argc, char *argv[])
{

    Vector<std::string>     message;
    bool                    debug = false;

    // 実行時引数を確認
    if (argc == 2) {
        // デバッグモード
        debug = true;
    } else
    if (argc != 4) {
        std::cerr << "%s <ユーザ名> <パスワード> <サーバ>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 将棋ライブラリ初期化
    Shogi::initialize();

    // 駒の価値を設定
    Position::setValue  (Value);
    Position::handsValue(Hands);

    // 置換表初期化
    initTP();

    // デバッグモード
    if (debug) {
        // 棋譜読み込み
        CSAFile  k(argv[1]);
        Position p(k.summary());
        // 候補手作成
        Array<Move::Move, Move::Max> move;
        p.genMove(move);
        // タイマースレッド起動
        //threadTimer timer;
        //timer.run(ThinkingTime);
        // 解析
        size_t best;
        if (move.vsize() > 1) {
            if (p.turn() == Color::Black) {
                best = searchMax(p, move);
            } else {
                best = searchMin(p, move);
            }
        } else {
            best = 0;
        }
        // タイマースレッド終了
        //timer.stop();
        //timer.sync();
        // 結果表示
        std::cout << p.string(move[best]) << std::endl
                  << p                    << std::endl;
        // 終了
        exit(EXIT_SUCCESS);
    }

    // CSA サーバへ接続
    CSAConnection csa(argv[3]);
    csa.connect(); 

    // CSA サーバへログイン
    csa.login(argv[1], argv[2]);

    // 手合い待ち
    CSASummary summary = csa.newGame();
    if (! csa.accept()) {
        std::cerr << "拒否されました" << std::endl;
        exit(EXIT_FAILURE);
    }

    // サーバからの情報を元に局面を作成
    Position p(summary);
    std::cout << p << std::endl;

    // プログラムの手番を確認
    Color::Color myturn = Position::myTurn(summary);

    while (1) {

        // プログラムの手番かどうか
        if (p.turn() == myturn) {

            // プログラムの手番

            // 候補手作成
            Array<Move::Move, Move::Max> move;
            p.genMove(move);

            // 詰み確認 - 指せる手がない場合は詰み
            if (move.vsize() == 0) {
                // 投了
                csa.resign();
                break;
            }

            // タイマースレッド起動
            threadTimer timer;
            timer.run(ThinkingTime);

#ifdef _RANDOM_PLAY
            // ランダムに手を選択する
            // 
            int best = rand() % (int)move.vsize();
#else
            // 探索の結果が最も良い手を選択する
            // 
            size_t best;
            if (move.vsize() > 1) {
                if (myturn == Color::Black) {
                    best = splitMax(p, move);
                } else {
                    best = splitMin(p, move);
                }
            } else {
                best = 0;
            }
#endif

            // 指し手送信
            csa.send(p.string(move[best]));

            // タイマースレッド終了
            timer.stop();
            timer.sync();

            // 局面を進める
            p.move(move[best]);

        } else {

            // 相手の手番

            // 相手の手を待つ
            csa.receive(message);

            // 先頭文字が '+' '-' でないときは指し手ではない
            if (message[0][0] != '+' && message[0][0] != '-') {
                // 送信されてきたメッセージを表示
                for (auto l : message) {
                    std::cout << l << std::endl;
                }
                break;
            }

            // プログラムの手はスキップ
            const char turnchk[] = {'+', '-'};
            if (turnchk[myturn] == message[0][0]) {
                continue;
            }

            // 相手の手を表示
            std::cout << message[0] << std::endl;

            // 局面を進める
            p.move(message[0]);

        }

        // 現局面を表示
        std::cout << p << std::endl;
    }

    // メッセージを全て受信
    while (csa.message()) {
        csa.receive(message);
        if (message.vsize() == 0) {
            break;
        }
        for (auto l : message) {
            std::cout << l << std::endl;
        }
    } 

    // 棋譜ファイルの書き込み
    csa.write("kifu.csa");

    // ログアウト
    csa.logout();

    // 通信終了
    csa.close();

    exit(EXIT_SUCCESS);

}
