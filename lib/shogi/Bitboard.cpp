/**
 *****************************************************************************

 @file       Bitboard.cpp

 @brief      Bitboard implementation
  
 @author     Hiroki Takada (http://wwww.tsoftware.jp/)

 @date       2016-03-31

 @version    $Id:$


  Copyright 2014, 2015, 2016 Hiroki Takada

  This file is part of libshogi.

  libshogi is free software: you can redistribute it and/or modify it under
  the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  libshogi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with libshogi. If not, see <http://www.gnu.org/licenses/>.
   

  ----------------------------------------------------------------------------
  RELEASE NOTE :

   DATE          REV    REMARK
  ============= ====== =======================================================
  31st Mar 2016  0.1   Initial release
 
 *****************************************************************************/ 

#include <Bitboard.h>

// begin namespace 'game'
namespace game {

/* ------------------------------ parameters ------------------------------- */

// filled bitboard
constexpr Bitboard          Bitboard::Fill 
                                (0x7fffffffffffffffULL, 0x000000000003ffffULL);

// zero bitboard
constexpr Bitboard          Bitboard::Zero
                                (0x0000000000000000ULL, 0x0000000000000000ULL);

// wall srrounding the board
constexpr Bitboard          Bitboard::Wall
                                (0x406030180c0603ffULL, 0x000000000003ff01ULL);

// moat srrounding the board
constexpr Bitboard          Bitboard::Moat
                                (0x3f9fcfe7f3f9fc00ULL, 0x00000000000000feULL);

// square bitboard
static constexpr Bitboard   SQ11(0x0000000000000001ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ12(0x0000000000000002ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ13(0x0000000000000004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ14(0x0000000000000008ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ15(0x0000000000000010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ16(0x0000000000000020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ17(0x0000000000000040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ18(0x0000000000000080ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ19(0x0000000000000100ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ21(0x0000000000000200ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ22(0x0000000000000400ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ23(0x0000000000000800ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ24(0x0000000000001000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ25(0x0000000000002000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ26(0x0000000000004000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ27(0x0000000000008000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ28(0x0000000000010000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ29(0x0000000000020000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ31(0x0000000000040000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ32(0x0000000000080000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ33(0x0000000000100000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ34(0x0000000000200000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ35(0x0000000000400000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ36(0x0000000000800000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ37(0x0000000001000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ38(0x0000000002000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ39(0x0000000004000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ41(0x0000000008000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ42(0x0000000010000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ43(0x0000000020000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ44(0x0000000040000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ45(0x0000000080000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ46(0x0000000100000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ47(0x0000000200000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ48(0x0000000400000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ49(0x0000000800000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ51(0x0000001000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ52(0x0000002000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ53(0x0000004000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ54(0x0000008000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ55(0x0000010000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ56(0x0000020000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ57(0x0000040000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ58(0x0000080000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ59(0x0000100000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ61(0x0000200000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ62(0x0000400000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ63(0x0000800000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ64(0x0001000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ65(0x0002000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ66(0x0004000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ67(0x0008000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ68(0x0010000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ69(0x0020000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ71(0x0040000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ72(0x0080000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ73(0x0100000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ74(0x0200000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ75(0x0400000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ76(0x0800000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ77(0x1000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ78(0x2000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ79(0x4000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   SQ81(0x0000000000000000ULL, 0x0000000000000001ULL);
static constexpr Bitboard   SQ82(0x0000000000000000ULL, 0x0000000000000002ULL);
static constexpr Bitboard   SQ83(0x0000000000000000ULL, 0x0000000000000004ULL);
static constexpr Bitboard   SQ84(0x0000000000000000ULL, 0x0000000000000008ULL);
static constexpr Bitboard   SQ85(0x0000000000000000ULL, 0x0000000000000010ULL);
static constexpr Bitboard   SQ86(0x0000000000000000ULL, 0x0000000000000020ULL);
static constexpr Bitboard   SQ87(0x0000000000000000ULL, 0x0000000000000040ULL);
static constexpr Bitboard   SQ88(0x0000000000000000ULL, 0x0000000000000080ULL);
static constexpr Bitboard   SQ89(0x0000000000000000ULL, 0x0000000000000100ULL);
static constexpr Bitboard   SQ91(0x0000000000000000ULL, 0x0000000000000200ULL);
static constexpr Bitboard   SQ92(0x0000000000000000ULL, 0x0000000000000400ULL);
static constexpr Bitboard   SQ93(0x0000000000000000ULL, 0x0000000000000800ULL);
static constexpr Bitboard   SQ94(0x0000000000000000ULL, 0x0000000000001000ULL);
static constexpr Bitboard   SQ95(0x0000000000000000ULL, 0x0000000000002000ULL);
static constexpr Bitboard   SQ96(0x0000000000000000ULL, 0x0000000000004000ULL);
static constexpr Bitboard   SQ97(0x0000000000000000ULL, 0x0000000000008000ULL);
static constexpr Bitboard   SQ98(0x0000000000000000ULL, 0x0000000000010000ULL);
static constexpr Bitboard   SQ99(0x0000000000000000ULL, 0x0000000000020000ULL); 
constexpr Bitboard          Bitboard::Square [Square::Squares] = { 
                                 SQ11,  SQ12,  SQ13,  SQ14,  SQ15,
                                 SQ16,  SQ17,  SQ18,  SQ19,
                                 SQ21,  SQ22,  SQ23,  SQ24,  SQ25,
                                 SQ26,  SQ27,  SQ28,  SQ29,
                                 SQ31,  SQ32,  SQ33,  SQ34,  SQ35,
                                 SQ36,  SQ37,  SQ38,  SQ39,
                                 SQ41,  SQ42,  SQ43,  SQ44,  SQ45,
                                 SQ46,  SQ47,  SQ48,  SQ49,
                                 SQ51,  SQ52,  SQ53,  SQ54,  SQ55,
                                 SQ56,  SQ57,  SQ58,  SQ59,
                                 SQ61,  SQ62,  SQ63,  SQ64,  SQ65,
                                 SQ66,  SQ67,  SQ68,  SQ69,
                                 SQ71,  SQ72,  SQ73,  SQ74,  SQ75,
                                 SQ76,  SQ77,  SQ78,  SQ79,
                                 SQ81,  SQ82,  SQ83,  SQ84,  SQ85,
                                 SQ86,  SQ87,  SQ88,  SQ89,
                                 SQ91,  SQ92,  SQ93,  SQ94,  SQ95,
                                 SQ96,  SQ97,  SQ98,  SQ99
                            };

constexpr Bitboard          Bitboard::Invert [Square::Squares] = { 
                                ~SQ11, ~SQ12, ~SQ13, ~SQ14, ~SQ15,
                                ~SQ16, ~SQ17, ~SQ18, ~SQ19,
                                ~SQ21, ~SQ22, ~SQ23, ~SQ24, ~SQ25,
                                ~SQ26, ~SQ27, ~SQ28, ~SQ29,
                                ~SQ31, ~SQ32, ~SQ33, ~SQ34, ~SQ35,
                                ~SQ36, ~SQ37, ~SQ38, ~SQ39,
                                ~SQ41, ~SQ42, ~SQ43, ~SQ44, ~SQ45,
                                ~SQ46, ~SQ47, ~SQ48, ~SQ49,
                                ~SQ51, ~SQ52, ~SQ53, ~SQ54, ~SQ55,
                                ~SQ56, ~SQ57, ~SQ58, ~SQ59,
                                ~SQ61, ~SQ62, ~SQ63, ~SQ64, ~SQ65,
                                ~SQ66, ~SQ67, ~SQ68, ~SQ69,
                                ~SQ71, ~SQ72, ~SQ73, ~SQ74, ~SQ75,
                                ~SQ76, ~SQ77, ~SQ78, ~SQ79,
                                ~SQ81, ~SQ82, ~SQ83, ~SQ84, ~SQ85,
                                ~SQ86, ~SQ87, ~SQ88, ~SQ89,
                                ~SQ91, ~SQ92, ~SQ93, ~SQ94, ~SQ95,
                                ~SQ96, ~SQ97, ~SQ98, ~SQ99
                            };


// bitboard with a filled rank
static constexpr Bitboard   RNK1B(0x0040201008040201ULL << 0,
                                  0x0000000000000201ULL << 0        );
static constexpr Bitboard   RNK2B(0x0040201008040201ULL << 1,
                                  0x0000000000000201ULL << 1        );
static constexpr Bitboard   RNK3B(0x0040201008040201ULL << 2,
                                  0x0000000000000201ULL << 2        );
static constexpr Bitboard   RNK4B(0x0040201008040201ULL << 3,
                                  0x0000000000000201ULL << 3        );
static constexpr Bitboard   RNK5B(0x0040201008040201ULL << 4,
                                  0x0000000000000201ULL << 4        );
static constexpr Bitboard   RNK6B(0x0040201008040201ULL << 5,
                                  0x0000000000000201ULL << 5        );
static constexpr Bitboard   RNK7B(0x0040201008040201ULL << 6,
                                  0x0000000000000201ULL << 6        );
static constexpr Bitboard   RNK8B(0x0040201008040201ULL << 7,
                                  0x0000000000000201ULL << 7        );
static constexpr Bitboard   RNK9B(0x0040201008040201ULL << 8,
                                  0x0000000000000201ULL << 8        );
constexpr Bitboard          Bitboard::Rank[Square::Ranks] = {
                                RNK1B, RNK2B, RNK3B, RNK4B, RNK5B,
                                RNK6B, RNK7B, RNK8B, RNK9B
                            };

// bitboards with a filled column
static constexpr Bitboard   FIL1B(0x00000000000001ffULL << (9 * 0),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL2B(0x00000000000001ffULL << (9 * 1),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL3B(0x00000000000001ffULL << (9 * 2),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL4B(0x00000000000001ffULL << (9 * 3),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL5B(0x00000000000001ffULL << (9 * 4),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL6B(0x00000000000001ffULL << (9 * 5),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL7B(0x00000000000001ffULL << (9 * 6),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL8B(0x0000000000000000ULL,           
                                  0x00000000000001ffULL << (9 * 0)  );
static constexpr Bitboard   FIL9B(0x0000000000000000ULL,            
                                  0x00000000000001ffULL << (9 * 1)  );
constexpr Bitboard          Bitboard::File[Square::Files] = {
                                FIL1B, FIL2B, FIL3B, FIL4B, FIL5B,
                                FIL6B, FIL7B, FIL8B, FIL9B
                            };

// vertical shifts
const int                   Bitboard::VShift [Square::Squares] = { 
                                 1,  1,  1,  1,  1,  1,  1,  1,  1,
                                10, 10, 10, 10, 10, 10, 10, 10, 10,
                                19, 19, 19, 19, 19, 19, 19, 19, 19,
                                28, 28, 28, 28, 28, 28, 28, 28, 28,
                                37, 37, 37, 37, 37, 37, 37, 37, 37,
                                46, 46, 46, 46, 46, 46, 46, 46, 46,
                                55, 55, 55, 55, 55, 55, 55, 55, 55,
                                 1,  1,  1,  1,  1,  1,  1,  1,  1,
                                10, 10, 10, 10, 10, 10, 10, 10, 10
                            };


/* bit pattern to sweep single bit to bottom of columns

   * * * * * * * * *
   * * * * * * * * *
   * * * * * * * * *
   * * * * * * * * *
   * * * * * * * * *
   * * * * * * * * *
   * * * * * * * * *
   * * * * * * * * *
   . . . . . . . . .

           +
   . . . . . . . . .
   . . . . . . . . .
   . . . . . . . . .
   . . . . . . . . .
   . . . . . . . . .  
   . . * . . . . . .
   * * . * * * * * * | sweep
   . . . . . . . . . V
   . . . . . . . . .

      (mask RNK9)
   . . . . . . . . .
   . . . . . . . . .
   . . . . . . . . .
   . . . . . . . . .
   . . . . . . . . .  
   . . . . . . . . .  
   . . . . . . . . .  
   . . . . . . . . .
   * * * * * * * * *

   YaneuraOU gave me this excellent idea. Thanks.

 */
static constexpr Bitboard   _sweepPattern
                                (0x3fdfeff7fbfdfeffULL, 0x000000000001feffULL);

// bit mask for PEXT to gather the swept bits
static const uint64_t       _sweepPextMask = 0x4020100804030180ULL;

// column mask indexed by result of the PEXT 
static constexpr Bitboard   L000(0x7fffffffffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L001(0x7fffffffffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L002(0x7ffffffffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L003(0x7ffffffffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L004(0x7fffffffffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L005(0x7fffffffffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L006(0x7ffffffffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L007(0x7ffffffffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L008(0x7ffffffffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L009(0x7ffffffffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L010(0x7ffffffffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L011(0x7ffffffffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L012(0x7ffffffffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L013(0x7ffffffffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L014(0x7ffffffffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L015(0x7ffffffffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L016(0x7ffffffff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L017(0x7ffffffff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L018(0x7ffffffff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L019(0x7ffffffff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L020(0x7ffffffff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L021(0x7ffffffff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L022(0x7ffffffff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L023(0x7ffffffff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L024(0x7ffffffff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L025(0x7ffffffff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L026(0x7ffffffff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L027(0x7ffffffff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L028(0x7ffffffff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L029(0x7ffffffff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L030(0x7ffffffff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L031(0x7ffffffff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L032(0x7ffffff007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L033(0x7ffffff007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L034(0x7ffffff007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L035(0x7ffffff007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L036(0x7ffffff007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L037(0x7ffffff007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L038(0x7ffffff007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L039(0x7ffffff007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L040(0x7ffffff007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L041(0x7ffffff007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L042(0x7ffffff007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L043(0x7ffffff007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L044(0x7ffffff007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L045(0x7ffffff007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L046(0x7ffffff007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L047(0x7ffffff007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L048(0x7ffffff00003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L049(0x7ffffff00003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L050(0x7ffffff00003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L051(0x7ffffff00003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L052(0x7ffffff00003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L053(0x7ffffff00003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L054(0x7ffffff00003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L055(0x7ffffff00003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L056(0x7ffffff0000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L057(0x7ffffff0000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L058(0x7ffffff000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L059(0x7ffffff000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L060(0x7ffffff0000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L061(0x7ffffff0000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L062(0x7ffffff000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L063(0x7ffffff000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L064(0x7fffe00fffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L065(0x7fffe00fffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L066(0x7fffe00ffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L067(0x7fffe00ffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L068(0x7fffe00fffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L069(0x7fffe00fffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L070(0x7fffe00ffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L071(0x7fffe00ffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L072(0x7fffe00ffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L073(0x7fffe00ffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L074(0x7fffe00ffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L075(0x7fffe00ffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L076(0x7fffe00ffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L077(0x7fffe00ffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L078(0x7fffe00ffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L079(0x7fffe00ffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L080(0x7fffe00ff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L081(0x7fffe00ff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L082(0x7fffe00ff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L083(0x7fffe00ff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L084(0x7fffe00ff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L085(0x7fffe00ff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L086(0x7fffe00ff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L087(0x7fffe00ff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L088(0x7fffe00ff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L089(0x7fffe00ff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L090(0x7fffe00ff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L091(0x7fffe00ff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L092(0x7fffe00ff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L093(0x7fffe00ff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L094(0x7fffe00ff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L095(0x7fffe00ff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L096(0x7fffe00007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L097(0x7fffe00007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L098(0x7fffe00007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L099(0x7fffe00007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L100(0x7fffe00007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L101(0x7fffe00007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L102(0x7fffe00007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L103(0x7fffe00007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L104(0x7fffe00007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L105(0x7fffe00007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L106(0x7fffe00007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L107(0x7fffe00007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L108(0x7fffe00007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L109(0x7fffe00007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L110(0x7fffe00007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L111(0x7fffe00007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L112(0x7fffe0000003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L113(0x7fffe0000003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L114(0x7fffe0000003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L115(0x7fffe0000003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L116(0x7fffe0000003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L117(0x7fffe0000003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L118(0x7fffe0000003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L119(0x7fffe0000003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L120(0x7fffe000000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L121(0x7fffe000000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L122(0x7fffe00000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L123(0x7fffe00000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L124(0x7fffe000000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L125(0x7fffe000000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L126(0x7fffe00000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L127(0x7fffe00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L128(0x7fc01fffffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L129(0x7fc01fffffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L130(0x7fc01ffffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L131(0x7fc01ffffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L132(0x7fc01fffffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L133(0x7fc01fffffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L134(0x7fc01ffffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L135(0x7fc01ffffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L136(0x7fc01ffffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L137(0x7fc01ffffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L138(0x7fc01ffffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L139(0x7fc01ffffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L140(0x7fc01ffffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L141(0x7fc01ffffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L142(0x7fc01ffffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L143(0x7fc01ffffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L144(0x7fc01ffff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L145(0x7fc01ffff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L146(0x7fc01ffff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L147(0x7fc01ffff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L148(0x7fc01ffff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L149(0x7fc01ffff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L150(0x7fc01ffff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L151(0x7fc01ffff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L152(0x7fc01ffff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L153(0x7fc01ffff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L154(0x7fc01ffff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L155(0x7fc01ffff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L156(0x7fc01ffff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L157(0x7fc01ffff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L158(0x7fc01ffff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L159(0x7fc01ffff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L160(0x7fc01ff007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L161(0x7fc01ff007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L162(0x7fc01ff007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L163(0x7fc01ff007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L164(0x7fc01ff007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L165(0x7fc01ff007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L166(0x7fc01ff007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L167(0x7fc01ff007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L168(0x7fc01ff007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L169(0x7fc01ff007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L170(0x7fc01ff007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L171(0x7fc01ff007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L172(0x7fc01ff007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L173(0x7fc01ff007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L174(0x7fc01ff007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L175(0x7fc01ff007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L176(0x7fc01ff00003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L177(0x7fc01ff00003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L178(0x7fc01ff00003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L179(0x7fc01ff00003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L180(0x7fc01ff00003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L181(0x7fc01ff00003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L182(0x7fc01ff00003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L183(0x7fc01ff00003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L184(0x7fc01ff0000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L185(0x7fc01ff0000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L186(0x7fc01ff000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L187(0x7fc01ff000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L188(0x7fc01ff0000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L189(0x7fc01ff0000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L190(0x7fc01ff000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L191(0x7fc01ff000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L192(0x7fc0000fffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L193(0x7fc0000fffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L194(0x7fc0000ffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L195(0x7fc0000ffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L196(0x7fc0000fffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L197(0x7fc0000fffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L198(0x7fc0000ffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L199(0x7fc0000ffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L200(0x7fc0000ffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L201(0x7fc0000ffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L202(0x7fc0000ffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L203(0x7fc0000ffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L204(0x7fc0000ffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L205(0x7fc0000ffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L206(0x7fc0000ffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L207(0x7fc0000ffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L208(0x7fc0000ff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L209(0x7fc0000ff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L210(0x7fc0000ff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L211(0x7fc0000ff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L212(0x7fc0000ff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L213(0x7fc0000ff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L214(0x7fc0000ff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L215(0x7fc0000ff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L216(0x7fc0000ff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L217(0x7fc0000ff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L218(0x7fc0000ff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L219(0x7fc0000ff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L220(0x7fc0000ff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L221(0x7fc0000ff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L222(0x7fc0000ff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L223(0x7fc0000ff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L224(0x7fc0000007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L225(0x7fc0000007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L226(0x7fc0000007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L227(0x7fc0000007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L228(0x7fc0000007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L229(0x7fc0000007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L230(0x7fc0000007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L231(0x7fc0000007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L232(0x7fc0000007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L233(0x7fc0000007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L234(0x7fc0000007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L235(0x7fc0000007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L236(0x7fc0000007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L237(0x7fc0000007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L238(0x7fc0000007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L239(0x7fc0000007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L240(0x7fc000000003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L241(0x7fc000000003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L242(0x7fc000000003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L243(0x7fc000000003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L244(0x7fc000000003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L245(0x7fc000000003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L246(0x7fc000000003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L247(0x7fc000000003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L248(0x7fc00000000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L249(0x7fc00000000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L250(0x7fc0000000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L251(0x7fc0000000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L252(0x7fc00000000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L253(0x7fc00000000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L254(0x7fc0000000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L255(0x7fc0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L256(0x003fffffffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L257(0x003fffffffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L258(0x003ffffffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L259(0x003ffffffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L260(0x003fffffffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L261(0x003fffffffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L262(0x003ffffffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L263(0x003ffffffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L264(0x003ffffffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L265(0x003ffffffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L266(0x003ffffffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L267(0x003ffffffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L268(0x003ffffffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L269(0x003ffffffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L270(0x003ffffffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L271(0x003ffffffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L272(0x003ffffff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L273(0x003ffffff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L274(0x003ffffff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L275(0x003ffffff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L276(0x003ffffff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L277(0x003ffffff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L278(0x003ffffff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L279(0x003ffffff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L280(0x003ffffff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L281(0x003ffffff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L282(0x003ffffff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L283(0x003ffffff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L284(0x003ffffff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L285(0x003ffffff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L286(0x003ffffff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L287(0x003ffffff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L288(0x003ffff007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L289(0x003ffff007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L290(0x003ffff007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L291(0x003ffff007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L292(0x003ffff007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L293(0x003ffff007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L294(0x003ffff007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L295(0x003ffff007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L296(0x003ffff007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L297(0x003ffff007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L298(0x003ffff007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L299(0x003ffff007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L300(0x003ffff007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L301(0x003ffff007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L302(0x003ffff007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L303(0x003ffff007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L304(0x003ffff00003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L305(0x003ffff00003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L306(0x003ffff00003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L307(0x003ffff00003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L308(0x003ffff00003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L309(0x003ffff00003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L310(0x003ffff00003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L311(0x003ffff00003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L312(0x003ffff0000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L313(0x003ffff0000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L314(0x003ffff000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L315(0x003ffff000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L316(0x003ffff0000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L317(0x003ffff0000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L318(0x003ffff000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L319(0x003ffff000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L320(0x003fe00fffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L321(0x003fe00fffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L322(0x003fe00ffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L323(0x003fe00ffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L324(0x003fe00fffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L325(0x003fe00fffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L326(0x003fe00ffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L327(0x003fe00ffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L328(0x003fe00ffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L329(0x003fe00ffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L330(0x003fe00ffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L331(0x003fe00ffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L332(0x003fe00ffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L333(0x003fe00ffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L334(0x003fe00ffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L335(0x003fe00ffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L336(0x003fe00ff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L337(0x003fe00ff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L338(0x003fe00ff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L339(0x003fe00ff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L340(0x003fe00ff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L341(0x003fe00ff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L342(0x003fe00ff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L343(0x003fe00ff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L344(0x003fe00ff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L345(0x003fe00ff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L346(0x003fe00ff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L347(0x003fe00ff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L348(0x003fe00ff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L349(0x003fe00ff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L350(0x003fe00ff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L351(0x003fe00ff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L352(0x003fe00007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L353(0x003fe00007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L354(0x003fe00007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L355(0x003fe00007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L356(0x003fe00007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L357(0x003fe00007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L358(0x003fe00007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L359(0x003fe00007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L360(0x003fe00007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L361(0x003fe00007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L362(0x003fe00007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L363(0x003fe00007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L364(0x003fe00007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L365(0x003fe00007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L366(0x003fe00007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L367(0x003fe00007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L368(0x003fe0000003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L369(0x003fe0000003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L370(0x003fe0000003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L371(0x003fe0000003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L372(0x003fe0000003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L373(0x003fe0000003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L374(0x003fe0000003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L375(0x003fe0000003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L376(0x003fe000000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L377(0x003fe000000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L378(0x003fe00000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L379(0x003fe00000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L380(0x003fe000000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L381(0x003fe000000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L382(0x003fe00000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L383(0x003fe00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L384(0x00001fffffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L385(0x00001fffffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L386(0x00001ffffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L387(0x00001ffffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L388(0x00001fffffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L389(0x00001fffffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L390(0x00001ffffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L391(0x00001ffffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L392(0x00001ffffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L393(0x00001ffffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L394(0x00001ffffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L395(0x00001ffffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L396(0x00001ffffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L397(0x00001ffffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L398(0x00001ffffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L399(0x00001ffffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L400(0x00001ffff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L401(0x00001ffff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L402(0x00001ffff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L403(0x00001ffff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L404(0x00001ffff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L405(0x00001ffff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L406(0x00001ffff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L407(0x00001ffff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L408(0x00001ffff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L409(0x00001ffff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L410(0x00001ffff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L411(0x00001ffff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L412(0x00001ffff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L413(0x00001ffff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L414(0x00001ffff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L415(0x00001ffff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L416(0x00001ff007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L417(0x00001ff007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L418(0x00001ff007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L419(0x00001ff007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L420(0x00001ff007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L421(0x00001ff007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L422(0x00001ff007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L423(0x00001ff007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L424(0x00001ff007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L425(0x00001ff007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L426(0x00001ff007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L427(0x00001ff007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L428(0x00001ff007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L429(0x00001ff007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L430(0x00001ff007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L431(0x00001ff007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L432(0x00001ff00003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L433(0x00001ff00003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L434(0x00001ff00003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L435(0x00001ff00003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L436(0x00001ff00003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L437(0x00001ff00003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L438(0x00001ff00003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L439(0x00001ff00003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L440(0x00001ff0000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L441(0x00001ff0000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L442(0x00001ff000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L443(0x00001ff000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L444(0x00001ff0000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L445(0x00001ff0000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L446(0x00001ff000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L447(0x00001ff000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L448(0x0000000fffffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L449(0x0000000fffffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L450(0x0000000ffffffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L451(0x0000000ffffffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L452(0x0000000fffffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L453(0x0000000fffffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L454(0x0000000ffffffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L455(0x0000000ffffffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L456(0x0000000ffffc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L457(0x0000000ffffc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L458(0x0000000ffffc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L459(0x0000000ffffc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L460(0x0000000ffffc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L461(0x0000000ffffc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L462(0x0000000ffffc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L463(0x0000000ffffc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L464(0x0000000ff803ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L465(0x0000000ff803ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L466(0x0000000ff803fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L467(0x0000000ff803fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L468(0x0000000ff803ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L469(0x0000000ff803ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L470(0x0000000ff803fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L471(0x0000000ff803fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L472(0x0000000ff80001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L473(0x0000000ff80001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L474(0x0000000ff8000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L475(0x0000000ff8000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L476(0x0000000ff80001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L477(0x0000000ff80001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L478(0x0000000ff8000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L479(0x0000000ff8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L480(0x0000000007ffffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L481(0x0000000007ffffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L482(0x0000000007fffe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L483(0x0000000007fffe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L484(0x0000000007ffffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L485(0x0000000007ffffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L486(0x0000000007fffe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L487(0x0000000007fffe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L488(0x0000000007fc01ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L489(0x0000000007fc01ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L490(0x0000000007fc0000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L491(0x0000000007fc0000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L492(0x0000000007fc01ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L493(0x0000000007fc01ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L494(0x0000000007fc0000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L495(0x0000000007fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L496(0x000000000003ffffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L497(0x000000000003ffffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L498(0x000000000003fe00ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L499(0x000000000003fe00ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L500(0x000000000003ffffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L501(0x000000000003ffffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L502(0x000000000003fe00ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L503(0x000000000003fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   L504(0x00000000000001ffULL, 0x000000000003ffffULL);
static constexpr Bitboard   L505(0x00000000000001ffULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L506(0x0000000000000000ULL, 0x000000000003ffffULL);
static constexpr Bitboard   L507(0x0000000000000000ULL, 0x000000000003fe00ULL);
static constexpr Bitboard   L508(0x00000000000001ffULL, 0x00000000000001ffULL);
static constexpr Bitboard   L509(0x00000000000001ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   L510(0x0000000000000000ULL, 0x00000000000001ffULL);
static constexpr Bitboard   L511(0x0000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   _sweepMask [2 << Square::FIL9] = { 
                                 L000,  L001,  L002,  L003,  L004,  L005,  L006,  L007,
                                 L008,  L009,  L010,  L011,  L012,  L013,  L014,  L015,
                                 L016,  L017,  L018,  L019,  L020,  L021,  L022,  L023,
                                 L024,  L025,  L026,  L027,  L028,  L029,  L030,  L031,
                                 L032,  L033,  L034,  L035,  L036,  L037,  L038,  L039,
                                 L040,  L041,  L042,  L043,  L044,  L045,  L046,  L047,
                                 L048,  L049,  L050,  L051,  L052,  L053,  L054,  L055,
                                 L056,  L057,  L058,  L059,  L060,  L061,  L062,  L063,
                                 L064,  L065,  L066,  L067,  L068,  L069,  L070,  L071,
                                 L072,  L073,  L074,  L075,  L076,  L077,  L078,  L079,
                                 L080,  L081,  L082,  L083,  L084,  L085,  L086,  L087,
                                 L088,  L089,  L090,  L091,  L092,  L093,  L094,  L095,
                                 L096,  L097,  L098,  L099,  L100,  L101,  L102,  L103,
                                 L104,  L105,  L106,  L107,  L108,  L109,  L110,  L111,
                                 L112,  L113,  L114,  L115,  L116,  L117,  L118,  L119,
                                 L120,  L121,  L122,  L123,  L124,  L125,  L126,  L127,
                                 L128,  L129,  L130,  L131,  L132,  L133,  L134,  L135,
                                 L136,  L137,  L138,  L139,  L140,  L141,  L142,  L143,
                                 L144,  L145,  L146,  L147,  L148,  L149,  L150,  L151,
                                 L152,  L153,  L154,  L155,  L156,  L157,  L158,  L159,
                                 L160,  L161,  L162,  L163,  L164,  L165,  L166,  L167,
                                 L168,  L169,  L170,  L171,  L172,  L173,  L174,  L175,
                                 L176,  L177,  L178,  L179,  L180,  L181,  L182,  L183,
                                 L184,  L185,  L186,  L187,  L188,  L189,  L190,  L191,
                                 L192,  L193,  L194,  L195,  L196,  L197,  L198,  L199,
                                 L200,  L201,  L202,  L203,  L204,  L205,  L206,  L207,
                                 L208,  L209,  L210,  L211,  L212,  L213,  L214,  L215,
                                 L216,  L217,  L218,  L219,  L220,  L221,  L222,  L223,
                                 L224,  L225,  L226,  L227,  L228,  L229,  L230,  L231,
                                 L232,  L233,  L234,  L235,  L236,  L237,  L238,  L239,
                                 L240,  L241,  L242,  L243,  L244,  L245,  L246,  L247,
                                 L248,  L249,  L250,  L251,  L252,  L253,  L254,  L255,
                                 L256,  L257,  L258,  L259,  L260,  L261,  L262,  L263,
                                 L264,  L265,  L266,  L267,  L268,  L269,  L270,  L271,
                                 L272,  L273,  L274,  L275,  L276,  L277,  L278,  L279,
                                 L280,  L281,  L282,  L283,  L284,  L285,  L286,  L287,
                                 L288,  L289,  L290,  L291,  L292,  L293,  L294,  L295,
                                 L296,  L297,  L298,  L299,  L300,  L301,  L302,  L303,
                                 L304,  L305,  L306,  L307,  L308,  L309,  L310,  L311,
                                 L312,  L313,  L314,  L315,  L316,  L317,  L318,  L319,
                                 L320,  L321,  L322,  L323,  L324,  L325,  L326,  L327,
                                 L328,  L329,  L330,  L331,  L332,  L333,  L334,  L335,
                                 L336,  L337,  L338,  L339,  L340,  L341,  L342,  L343,
                                 L344,  L345,  L346,  L347,  L348,  L349,  L350,  L351,
                                 L352,  L353,  L354,  L355,  L356,  L357,  L358,  L359,
                                 L360,  L361,  L362,  L363,  L364,  L365,  L366,  L367,
                                 L368,  L369,  L370,  L371,  L372,  L373,  L374,  L375,
                                 L376,  L377,  L378,  L379,  L380,  L381,  L382,  L383,
                                 L384,  L385,  L386,  L387,  L388,  L389,  L390,  L391,
                                 L392,  L393,  L394,  L395,  L396,  L397,  L398,  L399,
                                 L400,  L401,  L402,  L403,  L404,  L405,  L406,  L407,
                                 L408,  L409,  L410,  L411,  L412,  L413,  L414,  L415,
                                 L416,  L417,  L418,  L419,  L420,  L421,  L422,  L423,
                                 L424,  L425,  L426,  L427,  L428,  L429,  L430,  L431,
                                 L432,  L433,  L434,  L435,  L436,  L437,  L438,  L439,
                                 L440,  L441,  L442,  L443,  L444,  L445,  L446,  L447,
                                 L448,  L449,  L450,  L451,  L452,  L453,  L454,  L455,
                                 L456,  L457,  L458,  L459,  L460,  L461,  L462,  L463,
                                 L464,  L465,  L466,  L467,  L468,  L469,  L470,  L471,
                                 L472,  L473,  L474,  L475,  L476,  L477,  L478,  L479,
                                 L480,  L481,  L482,  L483,  L484,  L485,  L486,  L487,
                                 L488,  L489,  L490,  L491,  L492,  L493,  L494,  L495,
                                 L496,  L497,  L498,  L499,  L500,  L501,  L502,  L503,
                                 L504,  L505,  L506,  L507,  L508,  L509,  L510,  L511
                            };

/* ------------------------------------------------------------------------- */



/* ---------------------------- implementations ---------------------------- */

/**
 * Debug function to print out the bitboard combining p0 and p1
 * @param p0
 * @param p1
 */
void Bitboard::debug (uint64_t p0, uint64_t p1)
{

    std::cout << Bitboard(p0, p1);

}



/**
 * Conversion to boolean
 * @return result
 */
Bitboard::operator bool() const
{

    return (m != 0);

}



/**
 * Copy operation
 * @param rhs value to assign 
 * @return assigned value
 */
Bitboard & Bitboard::operator= (const Bitboard &rhs)
{

    m  = rhs.m;
    return *this;

}



/**
 * And operation and assign (board-wide And operation)
 * @param rhs value operated with
 * @return result
 */
Bitboard & Bitboard::operator&= (const Bitboard &rhs)
{

    m &= rhs.m;
    return *this;

}



/**
 * Or operation and assign (board-wide Or operation)
 * @param rhs value operated with
 * @return result
 */
Bitboard & Bitboard::operator|= (const Bitboard &rhs)
{

    m |= rhs.m;
    return *this;

}



/**
 * Xor operation and assign (board-wide Xor operation)
 * @param rhs value operated with
 * @return result
 */
Bitboard & Bitboard::operator^= (const Bitboard &rhs)
{

    m ^= rhs.m;
    return *this;

}



/**
 * Addition and assign (divided addition)
 * @param rhs value operated with
 * @return result
 */
Bitboard & Bitboard::operator+= (const Bitboard &rhs)
{

    p[0] += rhs.p[0];
    p[1] += rhs.p[1];
    return *this;

}



/**
 * Shift left and assign
 * @param shift number of bits to be shifted
 * @return result
 */
Bitboard & Bitboard::operator>>= (int shift)
{

    m >>= shift;
    return *this;

}



/**
 * Shift right and assign
 * @param shift number of bits to be shifted
 * @return result
 */
Bitboard & Bitboard::operator<<= (int shift)
{

    m <<= shift;
    return *this;

}



/**
 * And operation
 * @param rhs value operated with
 * @return result
 */
Bitboard Bitboard::operator& (const Bitboard &rhs) const
{

    return (Bitboard(*this) &= rhs);

}



/**
 * Or operation
 * @param rhs value operated with
 * @return result
 */
Bitboard Bitboard::operator| (const Bitboard &rhs) const
{

    return (Bitboard(*this) |= rhs);

}



/**
 * Xor operation
 * @param rhs value operated with
 * @return result
 */
Bitboard Bitboard::operator^ (const Bitboard &rhs) const
{

    return (Bitboard(*this) ^= rhs);

}



/**
 * Addition
 * @param rhs value operated with
 * @return result
 */
Bitboard Bitboard::operator+ (const Bitboard &rhs) const
{

    return (Bitboard(*this) += rhs);

}



/**
 * Shift left
 * @param shift number of bits to be shifted
 * @return result
 */
Bitboard Bitboard::operator>> (int shift) const
{

    return (Bitboard(*this) >>= shift);

}



/**
 * Shift right
 * @param shift number of bits to be shifted
 * @return result
 */
Bitboard Bitboard::operator<< (int shift) const
{

    return (Bitboard(*this) <<= shift);

}



/**
 * And operation with Square (test the bit at that square)
 * @param rhs Square
 * @return result
 */
Bitboard Bitboard::operator& (Square::Square rhs) const
{

    return (Bitboard(*this) &= Square[rhs]);

}



/**
 * Or operation with Square (set the 1 at that square)
 * @param rhs Square
 * @return result
 */
Bitboard Bitboard::operator| (Square::Square rhs) const
{

    return (Bitboard(*this) |= Square[rhs]);

}



/**
 * Xor operation with Square
 * @param rhs Square
 * @return result
 */
Bitboard Bitboard::operator^ (Square::Square rhs) const
{

    return (Bitboard(*this) ^= Square[rhs]);

}



/**
 * Or operation and assign with Square
 * @param rhs Square
 * @return result
 */
Bitboard & Bitboard::operator|= (Square::Square rhs)
{

    m |= Square[rhs].m;
    return *this;

}



/**
 * Xor operation and assign with Square
 * @param rhs Square
 * @return result
 */
Bitboard & Bitboard::operator^= (Square::Square rhs)
{

    m ^= Square[rhs].m;
    return *this;

}



/**
 * Print raw bits in the bitboard
 * @param os  output stream
 * @param rhs bitboard
 * @return output stream
 */
std::ostream & operator<< (std::ostream &os, const Bitboard &rhs)
{

    for (auto r : Square::rankAll) {
        for (auto f : Square::fileRev) {
            os << ((rhs & (f | r)) ? " *" : " .");
        }
        os << std::endl;
    }
    os << std::endl;

    return os;

}



/**
 * Bitbord to chose vacant columns
 *
 */
const Bitboard & Bitboard::column(void) const
{

    Bitboard swept   = *this + _sweepPattern;
    swept           &= Rank[Square::RNK9];
    swept.p[1]     >>= 1;
    uint64_t index   = swept.unite();
    return _sweepMask[foundation::Bitope::pext(index, _sweepPextMask)];

}



/**
 * Print raw bits in the bitboard
 *
 */
void Bitboard::show(void)
{

    std::cout << *this << std::endl;

}

/* ------------------------------------------------------------------------- */

// end namespace 'game'
}

