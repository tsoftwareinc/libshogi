#ifndef _MY_POSITION_H
#define _MY_POSITION_H

#include <Common.h>
#include <Array.h>
#include <Shogi.h>

using namespace game;

class MyPosition : public Position
{

public:
    /// Default constructor
    MyPosition () {}

    /// Copy constructor
    MyPosition (const Position &p  ) : Position(p) {}

    /// Constructor takes CSA game summary
    MyPosition (const CSASummary &g) : Position(g) {}

    /// All pieces giving check
    const Bitboard &        check   (void) const;

    /// Check effect on OU of last player
    const Bitboard &        recheck (void);

    /// Adjacent checking pieces
    const Bitboard &        checkAD (void) const;

    /// Checking KY
    const Bitboard &        checkKY (void) const;

    /// Checking KA
    const Bitboard &        checkKA (void) const;

    /// Checking HI
    const Bitboard &        checkHI (void) const;

    /// Pin
    const Bitboard &        pin     (void);

    /// Check hash
    bool                    chkHash (void);

    /// Check if identical
    bool                    operator== (const MyPosition &rhs);
    bool                    operator!= (const MyPosition &rhs);

    /// Assign
    MyPosition &            operator=  (const MyPosition &rhs);

};

#endif

