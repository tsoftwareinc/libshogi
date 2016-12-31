/**
 *****************************************************************************

 @file       Effect.cpp

 @brief      Effect implementation
  
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

#include <Piece.h>
#include <Effect.h>
#include <Region.h>
#include <Convert.h>

// begin namespace 'game::Effect'
namespace game { namespace Effect {

/* ------------------------------ parameters ------------------------------- */

// adjacent bitboard for 12 directions
static constexpr Bitboard   DU11(0x0000000000000e02ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU12(0x0000000000001e05ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU13(0x0000000000003e0aULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU14(0x0000000000007c14ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU15(0x000000000000f828ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU16(0x000000000001f050ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU17(0x000000000003e0a0ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU18(0x000000000003c140ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU19(0x0000000000038080ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU21(0x00000000001c0407ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU22(0x00000000003c0a0fULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU23(0x00000000007c141fULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU24(0x0000000000f8283eULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU25(0x0000000001f0507cULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU26(0x0000000003e0a0f8ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU27(0x0000000007c141f0ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU28(0x00000000078281e0ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU29(0x00000000070101c0ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU31(0x0000000038080e00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU32(0x0000000078141e00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU33(0x00000000f8283e00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU34(0x00000001f0507c00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU35(0x00000003e0a0f800ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU36(0x00000007c141f000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU37(0x0000000f8283e000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU38(0x0000000f0503c000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU39(0x0000000e02038000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU41(0x00000070101c0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU42(0x000000f0283c0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU43(0x000001f0507c0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU44(0x000003e0a0f80000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU45(0x000007c141f00000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU46(0x00000f8283e00000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU47(0x00001f0507c00000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU48(0x00001e0a07800000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU49(0x00001c0407000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU51(0x0000e02038000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU52(0x0001e05078000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU53(0x0003e0a0f8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU54(0x0007c141f0000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU55(0x000f8283e0000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU56(0x001f0507c0000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU57(0x003e0a0f80000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU58(0x003c140f00000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU59(0x0038080e00000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU61(0x01c0407000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU62(0x03c0a0f000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU63(0x07c141f000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU64(0x0f8283e000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU65(0x1f0507c000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU66(0x3e0a0f8000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU67(0x7c141f0000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU68(0x78281e0000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU69(0x70101c0000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   DU71(0x0080e00000000000ULL, 0x0000000000000007ULL);
static constexpr Bitboard   DU72(0x0141e00000000000ULL, 0x000000000000000fULL);
static constexpr Bitboard   DU73(0x0283e00000000000ULL, 0x000000000000001fULL);
static constexpr Bitboard   DU74(0x0507c00000000000ULL, 0x000000000000003eULL);
static constexpr Bitboard   DU75(0x0a0f800000000000ULL, 0x000000000000007cULL);
static constexpr Bitboard   DU76(0x141f000000000000ULL, 0x00000000000000f8ULL);
static constexpr Bitboard   DU77(0x283e000000000000ULL, 0x00000000000001f0ULL);
static constexpr Bitboard   DU78(0x503c000000000000ULL, 0x00000000000001e0ULL);
static constexpr Bitboard   DU79(0x2038000000000000ULL, 0x00000000000001c0ULL);
static constexpr Bitboard   DU81(0x01c0000000000000ULL, 0x0000000000000e02ULL);
static constexpr Bitboard   DU82(0x03c0000000000000ULL, 0x0000000000001e05ULL);
static constexpr Bitboard   DU83(0x07c0000000000000ULL, 0x0000000000003e0aULL);
static constexpr Bitboard   DU84(0x0f80000000000000ULL, 0x0000000000007c14ULL);
static constexpr Bitboard   DU85(0x1f00000000000000ULL, 0x000000000000f828ULL);
static constexpr Bitboard   DU86(0x3e00000000000000ULL, 0x000000000001f050ULL);
static constexpr Bitboard   DU87(0x7c00000000000000ULL, 0x000000000003e0a0ULL);
static constexpr Bitboard   DU88(0x7800000000000000ULL, 0x000000000003c140ULL);
static constexpr Bitboard   DU89(0x7000000000000000ULL, 0x0000000000038080ULL);
static constexpr Bitboard   DU91(0x0000000000000000ULL, 0x0000000000000407ULL);
static constexpr Bitboard   DU92(0x0000000000000000ULL, 0x0000000000000a0fULL);
static constexpr Bitboard   DU93(0x0000000000000000ULL, 0x000000000000141fULL);
static constexpr Bitboard   DU94(0x0000000000000000ULL, 0x000000000000283eULL);
static constexpr Bitboard   DU95(0x0000000000000000ULL, 0x000000000000507cULL);
static constexpr Bitboard   DU96(0x0000000000000000ULL, 0x000000000000a0f8ULL);
static constexpr Bitboard   DU97(0x0000000000000000ULL, 0x00000000000141f0ULL);
static constexpr Bitboard   DU98(0x0000000000000000ULL, 0x00000000000281e0ULL);
static constexpr Bitboard   DU99(0x0000000000000000ULL, 0x00000000000101c0ULL);
static constexpr Bitboard   _duodecimal [Square::Squares] = { 
                                 DU11,  DU12,  DU13,  DU14,  DU15,
                                 DU16,  DU17,  DU18,  DU19,
                                 DU21,  DU22,  DU23,  DU24,  DU25,
                                 DU26,  DU27,  DU28,  DU29,
                                 DU31,  DU32,  DU33,  DU34,  DU35,
                                 DU36,  DU37,  DU38,  DU39,
                                 DU41,  DU42,  DU43,  DU44,  DU45,
                                 DU46,  DU47,  DU48,  DU49,
                                 DU51,  DU52,  DU53,  DU54,  DU55,
                                 DU56,  DU57,  DU58,  DU59,
                                 DU61,  DU62,  DU63,  DU64,  DU65,
                                 DU66,  DU67,  DU68,  DU69,
                                 DU71,  DU72,  DU73,  DU74,  DU75,
                                 DU76,  DU77,  DU78,  DU79,
                                 DU81,  DU82,  DU83,  DU84,  DU85,
                                 DU86,  DU87,  DU88,  DU89,
                                 DU91,  DU92,  DU93,  DU94,  DU95,
                                 DU96,  DU97,  DU98,  DU99
                                 // should be poisoning here?
                            };

// adjacent bitboard for 8 directions
static constexpr Bitboard   OC11(0x0000000000000602ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC12(0x0000000000000e05ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC13(0x0000000000001c0aULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC14(0x0000000000003814ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC15(0x0000000000007028ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC16(0x000000000000e050ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC17(0x000000000001c0a0ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC18(0x0000000000038140ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC19(0x0000000000030080ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC21(0x00000000000c0403ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC22(0x00000000001c0a07ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC23(0x000000000038140eULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC24(0x000000000070281cULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC25(0x0000000000e05038ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC26(0x0000000001c0a070ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC27(0x00000000038140e0ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC28(0x00000000070281c0ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC29(0x0000000006010180ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC31(0x0000000018080600ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC32(0x0000000038140e00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC33(0x0000000070281c00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC34(0x00000000e0503800ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC35(0x00000001c0a07000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC36(0x000000038140e000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC37(0x000000070281c000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC38(0x0000000e05038000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC39(0x0000000c02030000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC41(0x00000030100c0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC42(0x00000070281c0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC43(0x000000e050380000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC44(0x000001c0a0700000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC45(0x0000038140e00000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC46(0x0000070281c00000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC47(0x00000e0503800000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC48(0x00001c0a07000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC49(0x0000180406000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC51(0x0000602018000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC52(0x0000e05038000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC53(0x0001c0a070000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC54(0x00038140e0000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC55(0x00070281c0000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC56(0x000e050380000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC57(0x001c0a0700000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC58(0x0038140e00000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC59(0x0030080c00000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC61(0x00c0403000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC62(0x01c0a07000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC63(0x038140e000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC64(0x070281c000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC65(0x0e05038000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC66(0x1c0a070000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC67(0x38140e0000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC68(0x70281c0000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC69(0x6010180000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   OC71(0x0080600000000000ULL, 0x0000000000000003ULL);
static constexpr Bitboard   OC72(0x0140e00000000000ULL, 0x0000000000000007ULL);
static constexpr Bitboard   OC73(0x0281c00000000000ULL, 0x000000000000000eULL);
static constexpr Bitboard   OC74(0x0503800000000000ULL, 0x000000000000001cULL);
static constexpr Bitboard   OC75(0x0a07000000000000ULL, 0x0000000000000038ULL);
static constexpr Bitboard   OC76(0x140e000000000000ULL, 0x0000000000000070ULL);
static constexpr Bitboard   OC77(0x281c000000000000ULL, 0x00000000000000e0ULL);
static constexpr Bitboard   OC78(0x5038000000000000ULL, 0x00000000000001c0ULL);
static constexpr Bitboard   OC79(0x2030000000000000ULL, 0x0000000000000180ULL);
static constexpr Bitboard   OC81(0x00c0000000000000ULL, 0x0000000000000602ULL);
static constexpr Bitboard   OC82(0x01c0000000000000ULL, 0x0000000000000e05ULL);
static constexpr Bitboard   OC83(0x0380000000000000ULL, 0x0000000000001c0aULL);
static constexpr Bitboard   OC84(0x0700000000000000ULL, 0x0000000000003814ULL);
static constexpr Bitboard   OC85(0x0e00000000000000ULL, 0x0000000000007028ULL);
static constexpr Bitboard   OC86(0x1c00000000000000ULL, 0x000000000000e050ULL);
static constexpr Bitboard   OC87(0x3800000000000000ULL, 0x000000000001c0a0ULL);
static constexpr Bitboard   OC88(0x7000000000000000ULL, 0x0000000000038140ULL);
static constexpr Bitboard   OC89(0x6000000000000000ULL, 0x0000000000030080ULL);
static constexpr Bitboard   OC91(0x0000000000000000ULL, 0x0000000000000403ULL);
static constexpr Bitboard   OC92(0x0000000000000000ULL, 0x0000000000000a07ULL);
static constexpr Bitboard   OC93(0x0000000000000000ULL, 0x000000000000140eULL);
static constexpr Bitboard   OC94(0x0000000000000000ULL, 0x000000000000281cULL);
static constexpr Bitboard   OC95(0x0000000000000000ULL, 0x0000000000005038ULL);
static constexpr Bitboard   OC96(0x0000000000000000ULL, 0x000000000000a070ULL);
static constexpr Bitboard   OC97(0x0000000000000000ULL, 0x00000000000140e0ULL);
static constexpr Bitboard   OC98(0x0000000000000000ULL, 0x00000000000281c0ULL);
static constexpr Bitboard   OC99(0x0000000000000000ULL, 0x0000000000010180ULL);
static constexpr Bitboard   _octant    [Square::Squares] = { 
                                 OC11,  OC12,  OC13,  OC14,  OC15,
                                 OC16,  OC17,  OC18,  OC19,
                                 OC21,  OC22,  OC23,  OC24,  OC25,
                                 OC26,  OC27,  OC28,  OC29,
                                 OC31,  OC32,  OC33,  OC34,  OC35,
                                 OC36,  OC37,  OC38,  OC39,
                                 OC41,  OC42,  OC43,  OC44,  OC45,
                                 OC46,  OC47,  OC48,  OC49,
                                 OC51,  OC52,  OC53,  OC54,  OC55,
                                 OC56,  OC57,  OC58,  OC59,
                                 OC61,  OC62,  OC63,  OC64,  OC65,
                                 OC66,  OC67,  OC68,  OC69,
                                 OC71,  OC72,  OC73,  OC74,  OC75,
                                 OC76,  OC77,  OC78,  OC79,
                                 OC81,  OC82,  OC83,  OC84,  OC85,
                                 OC86,  OC87,  OC88,  OC89,
                                 OC91,  OC92,  OC93,  OC94,  OC95,
                                 OC96,  OC97,  OC98,  OC99
                                 // should be poisoning here?
                            };

// distant bitboard
static constexpr Bitboard   LD11(0x10442110481407feULL, 0x0000000000020281ULL);
static constexpr Bitboard   LD12(0x2088422090280ffdULL, 0x0000000000000502ULL);
static constexpr Bitboard   LD13(0x4110844120541dfbULL, 0x0000000000000804ULL);
static constexpr Bitboard   LD14(0x0221088248a839f7ULL, 0x0000000000001008ULL);
static constexpr Bitboard   LD15(0x04021114915071efULL, 0x0000000000002010ULL);
static constexpr Bitboard   LD16(0x0804222922a0e1dfULL, 0x0000000000004020ULL);
static constexpr Bitboard   LD17(0x104844424541c1bfULL, 0x0000000000008040ULL);
static constexpr Bitboard   LD18(0x209088848283817fULL, 0x0000000000010081ULL);
static constexpr Bitboard   LD19(0x41211109050300ffULL, 0x0000000000020302ULL);
static constexpr Bitboard   LD21(0x08422090280ffc03ULL, 0x0000000000010241ULL);
static constexpr Bitboard   LD22(0x10844120501ffa07ULL, 0x0000000000020482ULL);
static constexpr Bitboard   LD23(0x21088240a83bf60eULL, 0x0000000000000904ULL);
static constexpr Bitboard   LD24(0x421104915073ee1cULL, 0x0000000000001008ULL);
static constexpr Bitboard   LD25(0x04222922a0e3de38ULL, 0x0000000000002010ULL);
static constexpr Bitboard   LD26(0x0844524541c3be70ULL, 0x0000000000004020ULL);
static constexpr Bitboard   LD27(0x1088848a83837ee0ULL, 0x0000000000008041ULL);
static constexpr Bitboard   LD28(0x211109050702ffc0ULL, 0x0000000000010282ULL);
static constexpr Bitboard   LD29(0x4222120a0601ff80ULL, 0x0000000000020504ULL);
static constexpr Bitboard   LD31(0x044120501ff80605ULL, 0x0000000000008221ULL);
static constexpr Bitboard   LD32(0x088240a03ff40e0aULL, 0x0000000000010442ULL);
static constexpr Bitboard   LD33(0x1104815077ec1c15ULL, 0x0000000000020884ULL);
static constexpr Bitboard   LD34(0x220922a0e7dc382aULL, 0x0000000000001108ULL);
static constexpr Bitboard   LD35(0x44524541c7bc7054ULL, 0x0000000000002010ULL);
static constexpr Bitboard   LD36(0x08a48a83877ce0a8ULL, 0x0000000000004021ULL);
static constexpr Bitboard   LD37(0x1109150706fdc150ULL, 0x0000000000008242ULL);
static constexpr Bitboard   LD38(0x22120a0e05ff80a0ULL, 0x0000000000010484ULL);
static constexpr Bitboard   LD39(0x4424140c03ff0140ULL, 0x0000000000020908ULL);
static constexpr Bitboard   LD41(0x0240a03ff00c0a09ULL, 0x0000000000004211ULL);
static constexpr Bitboard   LD42(0x0481407fe81c1412ULL, 0x0000000000008422ULL);
static constexpr Bitboard   LD43(0x0902a0efd8382a24ULL, 0x0000000000010844ULL);
static constexpr Bitboard   LD44(0x124541cfb8705449ULL, 0x0000000000021088ULL);
static constexpr Bitboard   LD45(0x248a838f78e0a892ULL, 0x0000000000002111ULL);
static constexpr Bitboard   LD46(0x4915070ef9c15124ULL, 0x0000000000004222ULL);
static constexpr Bitboard   LD47(0x122a0e0dfb82a048ULL, 0x0000000000008444ULL);
static constexpr Bitboard   LD48(0x24141c0bff014090ULL, 0x0000000000010888ULL);
static constexpr Bitboard   LD49(0x48281807fe028120ULL, 0x0000000000021110ULL);
static constexpr Bitboard   LD51(0x01407fe018141211ULL, 0x0000000000002209ULL);
static constexpr Bitboard   LD52(0x0280ffd038282422ULL, 0x0000000000004412ULL);
static constexpr Bitboard   LD53(0x0541dfb070544844ULL, 0x0000000000008824ULL);
static constexpr Bitboard   LD54(0x0a839f70e0a89288ULL, 0x0000000000011049ULL);
static constexpr Bitboard   LD55(0x15071ef1c1512511ULL, 0x0000000000022292ULL);
static constexpr Bitboard   LD56(0x2a0e1df382a24822ULL, 0x0000000000004524ULL);
static constexpr Bitboard   LD57(0x541c1bf705409044ULL, 0x0000000000008848ULL);
static constexpr Bitboard   LD58(0x283817fe02812088ULL, 0x0000000000011090ULL);
static constexpr Bitboard   LD59(0x50300ffc05024110ULL, 0x0000000000022120ULL);
static constexpr Bitboard   LD61(0x00ffc03028242221ULL, 0x0000000000001205ULL);
static constexpr Bitboard   LD62(0x01ffa07050484442ULL, 0x000000000000240aULL);
static constexpr Bitboard   LD63(0x03bf60e0a8908884ULL, 0x0000000000004815ULL);
static constexpr Bitboard   LD64(0x073ee1c151251108ULL, 0x000000000000922aULL);
static constexpr Bitboard   LD65(0x0e3de382a24a2210ULL, 0x0000000000012454ULL);
static constexpr Bitboard   LD66(0x1c3be70544904421ULL, 0x00000000000248a8ULL);
static constexpr Bitboard   LD67(0x3837ee0a81208842ULL, 0x0000000000009150ULL);
static constexpr Bitboard   LD68(0x702ffc0502411084ULL, 0x00000000000120a0ULL);
static constexpr Bitboard   LD69(0x601ff80a04822108ULL, 0x0000000000024140ULL);
static constexpr Bitboard   LD71(0x7f80605048444241ULL, 0x0000000000000a03ULL);
static constexpr Bitboard   LD72(0x7f40e0a090888482ULL, 0x0000000000001407ULL);
static constexpr Bitboard   LD73(0x7ec1c15121110904ULL, 0x0000000000002a0eULL);
static constexpr Bitboard   LD74(0x7dc382a24a221008ULL, 0x000000000000541cULL);
static constexpr Bitboard   LD75(0x7bc7054494442010ULL, 0x000000000000a838ULL);
static constexpr Bitboard   LD76(0x77ce0a8920884220ULL, 0x0000000000015070ULL);
static constexpr Bitboard   LD77(0x6fdc150241108441ULL, 0x000000000002a0e0ULL);
static constexpr Bitboard   LD78(0x5ff80a0482210882ULL, 0x00000000000141c0ULL);
static constexpr Bitboard   LD79(0x3ff0140904421104ULL, 0x0000000000028180ULL);
static constexpr Bitboard   LD81(0x00c0a09088848281ULL, 0x00000000000007feULL);
static constexpr Bitboard   LD82(0x01c1412111090502ULL, 0x0000000000000ffdULL);
static constexpr Bitboard   LD83(0x0382a24222120804ULL, 0x0000000000001dfbULL);
static constexpr Bitboard   LD84(0x0705449444201008ULL, 0x00000000000039f7ULL);
static constexpr Bitboard   LD85(0x0e0a892888402010ULL, 0x00000000000071efULL);
static constexpr Bitboard   LD86(0x1c15124110844020ULL, 0x000000000000e1dfULL);
static constexpr Bitboard   LD87(0x382a048221088240ULL, 0x000000000001c1bfULL);
static constexpr Bitboard   LD88(0x7014090442110481ULL, 0x000000000003817fULL);
static constexpr Bitboard   LD89(0x6028120884220902ULL, 0x00000000000300ffULL);
static constexpr Bitboard   LD91(0x0141211109050301ULL, 0x000000000003fc03ULL);
static constexpr Bitboard   LD92(0x02824222120a0402ULL, 0x000000000003fa07ULL);
static constexpr Bitboard   LD93(0x0544844424100804ULL, 0x000000000003f60eULL);
static constexpr Bitboard   LD94(0x0a89288840201008ULL, 0x000000000003ee1cULL);
static constexpr Bitboard   LD95(0x1512511080402010ULL, 0x000000000003de38ULL);
static constexpr Bitboard   LD96(0x2a24822108804020ULL, 0x000000000003be70ULL);
static constexpr Bitboard   LD97(0x5409044211048040ULL, 0x0000000000037ee0ULL);
static constexpr Bitboard   LD98(0x2812088422090280ULL, 0x000000000002ffc0ULL);
static constexpr Bitboard   LD99(0x5024110844120501ULL, 0x000000000001ff80ULL);
static constexpr Bitboard   _distant   [Square::Squares] = { 
                                 LD11,  LD12,  LD13,  LD14,  LD15,
                                 LD16,  LD17,  LD18,  LD19,
                                 LD21,  LD22,  LD23,  LD24,  LD25,
                                 LD26,  LD27,  LD28,  LD29,
                                 LD31,  LD32,  LD33,  LD34,  LD35,
                                 LD36,  LD37,  LD38,  LD39,
                                 LD41,  LD42,  LD43,  LD44,  LD45,
                                 LD46,  LD47,  LD48,  LD49,
                                 LD51,  LD52,  LD53,  LD54,  LD55,
                                 LD56,  LD57,  LD58,  LD59,
                                 LD61,  LD62,  LD63,  LD64,  LD65,
                                 LD66,  LD67,  LD68,  LD69,
                                 LD71,  LD72,  LD73,  LD74,  LD75,
                                 LD76,  LD77,  LD78,  LD79,
                                 LD81,  LD82,  LD83,  LD84,  LD85,
                                 LD86,  LD87,  LD88,  LD89,
                                 LD91,  LD92,  LD93,  LD94,  LD95,
                                 LD96,  LD97,  LD98,  LD99
                                 // should be poisoning here?
                            };

// right shoulder up
static constexpr Bitboard   RS11(0x1004010040100400ULL, 0x0000000000020080ULL);
static constexpr Bitboard   RS12(0x2008020080200800ULL, 0x0000000000000100ULL);
static constexpr Bitboard   RS13(0x4010040100401000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS14(0x0020080200802000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS15(0x0000100401004000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS16(0x0000000802008000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS17(0x0000000004010000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS18(0x0000000000020000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS19(0x0000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS21(0x0802008020080000ULL, 0x0000000000010040ULL);
static constexpr Bitboard   RS22(0x1004010040100001ULL, 0x0000000000020080ULL);
static constexpr Bitboard   RS23(0x2008020080200002ULL, 0x0000000000000100ULL);
static constexpr Bitboard   RS24(0x4010040100400004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS25(0x0020080200800008ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS26(0x0000100401000010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS27(0x0000000802000020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS28(0x0000000004000040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS29(0x0000000000000080ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS31(0x0401004010000000ULL, 0x0000000000008020ULL);
static constexpr Bitboard   RS32(0x0802008020000200ULL, 0x0000000000010040ULL);
static constexpr Bitboard   RS33(0x1004010040000401ULL, 0x0000000000020080ULL);
static constexpr Bitboard   RS34(0x2008020080000802ULL, 0x0000000000000100ULL);
static constexpr Bitboard   RS35(0x4010040100001004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS36(0x0020080200002008ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS37(0x0000100400004010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS38(0x0000000800008020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS39(0x0000000000010040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS41(0x0200802000000000ULL, 0x0000000000004010ULL);
static constexpr Bitboard   RS42(0x0401004000040000ULL, 0x0000000000008020ULL);
static constexpr Bitboard   RS43(0x0802008000080200ULL, 0x0000000000010040ULL);
static constexpr Bitboard   RS44(0x1004010000100401ULL, 0x0000000000020080ULL);
static constexpr Bitboard   RS45(0x2008020000200802ULL, 0x0000000000000100ULL);
static constexpr Bitboard   RS46(0x4010040000401004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS47(0x0020080000802008ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS48(0x0000100001004010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS49(0x0000000002008020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS51(0x0100400000000000ULL, 0x0000000000002008ULL);
static constexpr Bitboard   RS52(0x0200800008000000ULL, 0x0000000000004010ULL);
static constexpr Bitboard   RS53(0x0401000010040000ULL, 0x0000000000008020ULL);
static constexpr Bitboard   RS54(0x0802000020080200ULL, 0x0000000000010040ULL);
static constexpr Bitboard   RS55(0x1004000040100401ULL, 0x0000000000020080ULL);
static constexpr Bitboard   RS56(0x2008000080200802ULL, 0x0000000000000100ULL);
static constexpr Bitboard   RS57(0x4010000100401004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS58(0x0020000200802008ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS59(0x0000000401004010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS61(0x0080000000000000ULL, 0x0000000000001004ULL);
static constexpr Bitboard   RS62(0x0100001000000000ULL, 0x0000000000002008ULL);
static constexpr Bitboard   RS63(0x0200002008000000ULL, 0x0000000000004010ULL);
static constexpr Bitboard   RS64(0x0400004010040000ULL, 0x0000000000008020ULL);
static constexpr Bitboard   RS65(0x0800008020080200ULL, 0x0000000000010040ULL);
static constexpr Bitboard   RS66(0x1000010040100401ULL, 0x0000000000020080ULL);
static constexpr Bitboard   RS67(0x2000020080200802ULL, 0x0000000000000100ULL);
static constexpr Bitboard   RS68(0x4000040100401004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS69(0x0000080200802008ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS71(0x0000000000000000ULL, 0x0000000000000802ULL);
static constexpr Bitboard   RS72(0x0000200000000000ULL, 0x0000000000001004ULL);
static constexpr Bitboard   RS73(0x0000401000000000ULL, 0x0000000000002008ULL);
static constexpr Bitboard   RS74(0x0000802008000000ULL, 0x0000000000004010ULL);
static constexpr Bitboard   RS75(0x0001004010040000ULL, 0x0000000000008020ULL);
static constexpr Bitboard   RS76(0x0002008020080200ULL, 0x0000000000010040ULL);
static constexpr Bitboard   RS77(0x0004010040100401ULL, 0x0000000000020080ULL);
static constexpr Bitboard   RS78(0x0008020080200802ULL, 0x0000000000000100ULL);
static constexpr Bitboard   RS79(0x0010040100401004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS81(0x0000000000000000ULL, 0x0000000000000400ULL);
static constexpr Bitboard   RS82(0x0040000000000000ULL, 0x0000000000000800ULL);
static constexpr Bitboard   RS83(0x0080200000000000ULL, 0x0000000000001000ULL);
static constexpr Bitboard   RS84(0x0100401000000000ULL, 0x0000000000002000ULL);
static constexpr Bitboard   RS85(0x0200802008000000ULL, 0x0000000000004000ULL);
static constexpr Bitboard   RS86(0x0401004010040000ULL, 0x0000000000008000ULL);
static constexpr Bitboard   RS87(0x0802008020080200ULL, 0x0000000000010000ULL);
static constexpr Bitboard   RS88(0x1004010040100401ULL, 0x0000000000020000ULL);
static constexpr Bitboard   RS89(0x2008020080200802ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS91(0x0000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   RS92(0x0000000000000000ULL, 0x0000000000000001ULL);
static constexpr Bitboard   RS93(0x0040000000000000ULL, 0x0000000000000002ULL);
static constexpr Bitboard   RS94(0x0080200000000000ULL, 0x0000000000000004ULL);
static constexpr Bitboard   RS95(0x0100401000000000ULL, 0x0000000000000008ULL);
static constexpr Bitboard   RS96(0x0200802008000000ULL, 0x0000000000000010ULL);
static constexpr Bitboard   RS97(0x0401004010040000ULL, 0x0000000000000020ULL);
static constexpr Bitboard   RS98(0x0802008020080200ULL, 0x0000000000000040ULL);
static constexpr Bitboard   RS99(0x1004010040100401ULL, 0x0000000000000080ULL);
static constexpr Bitboard   _rsup      [Square::Squares] = { 
                                 RS11,  RS12,  RS13,  RS14,  RS15,
                                 RS16,  RS17,  RS18,  RS19,
                                 RS21,  RS22,  RS23,  RS24,  RS25,
                                 RS26,  RS27,  RS28,  RS29,
                                 RS31,  RS32,  RS33,  RS34,  RS35,
                                 RS36,  RS37,  RS38,  RS39,
                                 RS41,  RS42,  RS43,  RS44,  RS45,
                                 RS46,  RS47,  RS48,  RS49,
                                 RS51,  RS52,  RS53,  RS54,  RS55,
                                 RS56,  RS57,  RS58,  RS59,
                                 RS61,  RS62,  RS63,  RS64,  RS65,
                                 RS66,  RS67,  RS68,  RS69,
                                 RS71,  RS72,  RS73,  RS74,  RS75,
                                 RS76,  RS77,  RS78,  RS79,
                                 RS81,  RS82,  RS83,  RS84,  RS85,
                                 RS86,  RS87,  RS88,  RS89,
                                 RS91,  RS92,  RS93,  RS94,  RS95,
                                 RS96,  RS97,  RS98,  RS99
                                 // should be poisoning here?
                            };

// falling to the right
static constexpr Bitboard   FR11(0x0000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR12(0x0000000000000200ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR13(0x0000000000040400ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR14(0x0000000008080800ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR15(0x0000001010101000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR16(0x0000202020202000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR17(0x0040404040404000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR18(0x0080808080808000ULL, 0x0000000000000001ULL);
static constexpr Bitboard   FR19(0x0101010101010000ULL, 0x0000000000000202ULL);
static constexpr Bitboard   FR21(0x0000000000000002ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR22(0x0000000000040004ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR23(0x0000000008080008ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR24(0x0000001010100010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR25(0x0000202020200020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR26(0x0040404040400040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR27(0x0080808080800080ULL, 0x0000000000000001ULL);
static constexpr Bitboard   FR28(0x0101010101000100ULL, 0x0000000000000202ULL);
static constexpr Bitboard   FR29(0x0202020202000000ULL, 0x0000000000000404ULL);
static constexpr Bitboard   FR31(0x0000000000000404ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR32(0x0000000008000808ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR33(0x0000001010001010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR34(0x0000202020002020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR35(0x0040404040004040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR36(0x0080808080008080ULL, 0x0000000000000001ULL);
static constexpr Bitboard   FR37(0x0101010100010100ULL, 0x0000000000000202ULL);
static constexpr Bitboard   FR38(0x0202020200020000ULL, 0x0000000000000404ULL);
static constexpr Bitboard   FR39(0x0404040400000000ULL, 0x0000000000000808ULL);
static constexpr Bitboard   FR41(0x0000000000080808ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR42(0x0000001000101010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR43(0x0000202000202020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR44(0x0040404000404040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR45(0x0080808000808080ULL, 0x0000000000000001ULL);
static constexpr Bitboard   FR46(0x0101010001010100ULL, 0x0000000000000202ULL);
static constexpr Bitboard   FR47(0x0202020002020000ULL, 0x0000000000000404ULL);
static constexpr Bitboard   FR48(0x0404040004000000ULL, 0x0000000000000808ULL);
static constexpr Bitboard   FR49(0x0808080000000000ULL, 0x0000000000001010ULL);
static constexpr Bitboard   FR51(0x0000000010101010ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR52(0x0000200020202020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR53(0x0040400040404040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR54(0x0080800080808080ULL, 0x0000000000000001ULL);
static constexpr Bitboard   FR55(0x0101000101010100ULL, 0x0000000000000202ULL);
static constexpr Bitboard   FR56(0x0202000202020000ULL, 0x0000000000000404ULL);
static constexpr Bitboard   FR57(0x0404000404000000ULL, 0x0000000000000808ULL);
static constexpr Bitboard   FR58(0x0808000800000000ULL, 0x0000000000001010ULL);
static constexpr Bitboard   FR59(0x1010000000000000ULL, 0x0000000000002020ULL);
static constexpr Bitboard   FR61(0x0000002020202020ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR62(0x0040004040404040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR63(0x0080008080808080ULL, 0x0000000000000001ULL);
static constexpr Bitboard   FR64(0x0100010101010100ULL, 0x0000000000000202ULL);
static constexpr Bitboard   FR65(0x0200020202020000ULL, 0x0000000000000404ULL);
static constexpr Bitboard   FR66(0x0400040404000000ULL, 0x0000000000000808ULL);
static constexpr Bitboard   FR67(0x0800080800000000ULL, 0x0000000000001010ULL);
static constexpr Bitboard   FR68(0x1000100000000000ULL, 0x0000000000002020ULL);
static constexpr Bitboard   FR69(0x2000000000000000ULL, 0x0000000000004040ULL);
static constexpr Bitboard   FR71(0x0000404040404040ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR72(0x0000808080808080ULL, 0x0000000000000001ULL);
static constexpr Bitboard   FR73(0x0001010101010100ULL, 0x0000000000000202ULL);
static constexpr Bitboard   FR74(0x0002020202020000ULL, 0x0000000000000404ULL);
static constexpr Bitboard   FR75(0x0004040404000000ULL, 0x0000000000000808ULL);
static constexpr Bitboard   FR76(0x0008080800000000ULL, 0x0000000000001010ULL);
static constexpr Bitboard   FR77(0x0010100000000000ULL, 0x0000000000002020ULL);
static constexpr Bitboard   FR78(0x0020000000000000ULL, 0x0000000000004040ULL);
static constexpr Bitboard   FR79(0x0000000000000000ULL, 0x0000000000008080ULL);
static constexpr Bitboard   FR81(0x0080808080808080ULL, 0x0000000000000000ULL);
static constexpr Bitboard   FR82(0x0101010101010100ULL, 0x0000000000000200ULL);
static constexpr Bitboard   FR83(0x0202020202020000ULL, 0x0000000000000400ULL);
static constexpr Bitboard   FR84(0x0404040404000000ULL, 0x0000000000000800ULL);
static constexpr Bitboard   FR85(0x0808080800000000ULL, 0x0000000000001000ULL);
static constexpr Bitboard   FR86(0x1010100000000000ULL, 0x0000000000002000ULL);
static constexpr Bitboard   FR87(0x2020000000000000ULL, 0x0000000000004000ULL);
static constexpr Bitboard   FR88(0x4000000000000000ULL, 0x0000000000008000ULL);
static constexpr Bitboard   FR89(0x0000000000000000ULL, 0x0000000000010000ULL);
static constexpr Bitboard   FR91(0x0101010101010100ULL, 0x0000000000000002ULL);
static constexpr Bitboard   FR92(0x0202020202020000ULL, 0x0000000000000004ULL);
static constexpr Bitboard   FR93(0x0404040404000000ULL, 0x0000000000000008ULL);
static constexpr Bitboard   FR94(0x0808080800000000ULL, 0x0000000000000010ULL);
static constexpr Bitboard   FR95(0x1010100000000000ULL, 0x0000000000000020ULL);
static constexpr Bitboard   FR96(0x2020000000000000ULL, 0x0000000000000040ULL);
static constexpr Bitboard   FR97(0x4000000000000000ULL, 0x0000000000000080ULL);
static constexpr Bitboard   FR98(0x0000000000000000ULL, 0x0000000000000100ULL);
static constexpr Bitboard   FR99(0x0000000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   _failtor   [Square::Squares] = { 
                                 FR11,  FR12,  FR13,  FR14,  FR15,
                                 FR16,  FR17,  FR18,  FR19,
                                 FR21,  FR22,  FR23,  FR24,  FR25,
                                 FR26,  FR27,  FR28,  FR29,
                                 FR31,  FR32,  FR33,  FR34,  FR35,
                                 FR36,  FR37,  FR38,  FR39,
                                 FR41,  FR42,  FR43,  FR44,  FR45,
                                 FR46,  FR47,  FR48,  FR49,
                                 FR51,  FR52,  FR53,  FR54,  FR55,
                                 FR56,  FR57,  FR58,  FR59,
                                 FR61,  FR62,  FR63,  FR64,  FR65,
                                 FR66,  FR67,  FR68,  FR69,
                                 FR71,  FR72,  FR73,  FR74,  FR75,
                                 FR76,  FR77,  FR78,  FR79,
                                 FR81,  FR82,  FR83,  FR84,  FR85,
                                 FR86,  FR87,  FR88,  FR89,
                                 FR91,  FR92,  FR93,  FR94,  FR95,
                                 FR96,  FR97,  FR98,  FR99
                                 // should be poisoning here?
                            };

// horizontal
static constexpr Bitboard   HH11(0x0040201008040200ULL, 0x0000000000000201ULL);
static constexpr Bitboard   HH12(0x0080402010080400ULL, 0x0000000000000402ULL);
static constexpr Bitboard   HH13(0x0100804020100800ULL, 0x0000000000000804ULL);
static constexpr Bitboard   HH14(0x0201008040201000ULL, 0x0000000000001008ULL);
static constexpr Bitboard   HH15(0x0402010080402000ULL, 0x0000000000002010ULL);
static constexpr Bitboard   HH16(0x0804020100804000ULL, 0x0000000000004020ULL);
static constexpr Bitboard   HH17(0x1008040201008000ULL, 0x0000000000008040ULL);
static constexpr Bitboard   HH18(0x2010080402010000ULL, 0x0000000000010080ULL);
static constexpr Bitboard   HH19(0x4020100804020000ULL, 0x0000000000020100ULL);
static constexpr Bitboard   HH21(0x0040201008040001ULL, 0x0000000000000201ULL);
static constexpr Bitboard   HH22(0x0080402010080002ULL, 0x0000000000000402ULL);
static constexpr Bitboard   HH23(0x0100804020100004ULL, 0x0000000000000804ULL);
static constexpr Bitboard   HH24(0x0201008040200008ULL, 0x0000000000001008ULL);
static constexpr Bitboard   HH25(0x0402010080400010ULL, 0x0000000000002010ULL);
static constexpr Bitboard   HH26(0x0804020100800020ULL, 0x0000000000004020ULL);
static constexpr Bitboard   HH27(0x1008040201000040ULL, 0x0000000000008040ULL);
static constexpr Bitboard   HH28(0x2010080402000080ULL, 0x0000000000010080ULL);
static constexpr Bitboard   HH29(0x4020100804000100ULL, 0x0000000000020100ULL);
static constexpr Bitboard   HH31(0x0040201008000201ULL, 0x0000000000000201ULL);
static constexpr Bitboard   HH32(0x0080402010000402ULL, 0x0000000000000402ULL);
static constexpr Bitboard   HH33(0x0100804020000804ULL, 0x0000000000000804ULL);
static constexpr Bitboard   HH34(0x0201008040001008ULL, 0x0000000000001008ULL);
static constexpr Bitboard   HH35(0x0402010080002010ULL, 0x0000000000002010ULL);
static constexpr Bitboard   HH36(0x0804020100004020ULL, 0x0000000000004020ULL);
static constexpr Bitboard   HH37(0x1008040200008040ULL, 0x0000000000008040ULL);
static constexpr Bitboard   HH38(0x2010080400010080ULL, 0x0000000000010080ULL);
static constexpr Bitboard   HH39(0x4020100800020100ULL, 0x0000000000020100ULL);
static constexpr Bitboard   HH41(0x0040201000040201ULL, 0x0000000000000201ULL);
static constexpr Bitboard   HH42(0x0080402000080402ULL, 0x0000000000000402ULL);
static constexpr Bitboard   HH43(0x0100804000100804ULL, 0x0000000000000804ULL);
static constexpr Bitboard   HH44(0x0201008000201008ULL, 0x0000000000001008ULL);
static constexpr Bitboard   HH45(0x0402010000402010ULL, 0x0000000000002010ULL);
static constexpr Bitboard   HH46(0x0804020000804020ULL, 0x0000000000004020ULL);
static constexpr Bitboard   HH47(0x1008040001008040ULL, 0x0000000000008040ULL);
static constexpr Bitboard   HH48(0x2010080002010080ULL, 0x0000000000010080ULL);
static constexpr Bitboard   HH49(0x4020100004020100ULL, 0x0000000000020100ULL);
static constexpr Bitboard   HH51(0x0040200008040201ULL, 0x0000000000000201ULL);
static constexpr Bitboard   HH52(0x0080400010080402ULL, 0x0000000000000402ULL);
static constexpr Bitboard   HH53(0x0100800020100804ULL, 0x0000000000000804ULL);
static constexpr Bitboard   HH54(0x0201000040201008ULL, 0x0000000000001008ULL);
static constexpr Bitboard   HH55(0x0402000080402010ULL, 0x0000000000002010ULL);
static constexpr Bitboard   HH56(0x0804000100804020ULL, 0x0000000000004020ULL);
static constexpr Bitboard   HH57(0x1008000201008040ULL, 0x0000000000008040ULL);
static constexpr Bitboard   HH58(0x2010000402010080ULL, 0x0000000000010080ULL);
static constexpr Bitboard   HH59(0x4020000804020100ULL, 0x0000000000020100ULL);
static constexpr Bitboard   HH61(0x0040001008040201ULL, 0x0000000000000201ULL);
static constexpr Bitboard   HH62(0x0080002010080402ULL, 0x0000000000000402ULL);
static constexpr Bitboard   HH63(0x0100004020100804ULL, 0x0000000000000804ULL);
static constexpr Bitboard   HH64(0x0200008040201008ULL, 0x0000000000001008ULL);
static constexpr Bitboard   HH65(0x0400010080402010ULL, 0x0000000000002010ULL);
static constexpr Bitboard   HH66(0x0800020100804020ULL, 0x0000000000004020ULL);
static constexpr Bitboard   HH67(0x1000040201008040ULL, 0x0000000000008040ULL);
static constexpr Bitboard   HH68(0x2000080402010080ULL, 0x0000000000010080ULL);
static constexpr Bitboard   HH69(0x4000100804020100ULL, 0x0000000000020100ULL);
static constexpr Bitboard   HH71(0x0000201008040201ULL, 0x0000000000000201ULL);
static constexpr Bitboard   HH72(0x0000402010080402ULL, 0x0000000000000402ULL);
static constexpr Bitboard   HH73(0x0000804020100804ULL, 0x0000000000000804ULL);
static constexpr Bitboard   HH74(0x0001008040201008ULL, 0x0000000000001008ULL);
static constexpr Bitboard   HH75(0x0002010080402010ULL, 0x0000000000002010ULL);
static constexpr Bitboard   HH76(0x0004020100804020ULL, 0x0000000000004020ULL);
static constexpr Bitboard   HH77(0x0008040201008040ULL, 0x0000000000008040ULL);
static constexpr Bitboard   HH78(0x0010080402010080ULL, 0x0000000000010080ULL);
static constexpr Bitboard   HH79(0x0020100804020100ULL, 0x0000000000020100ULL);
static constexpr Bitboard   HH81(0x0040201008040201ULL, 0x0000000000000200ULL);
static constexpr Bitboard   HH82(0x0080402010080402ULL, 0x0000000000000400ULL);
static constexpr Bitboard   HH83(0x0100804020100804ULL, 0x0000000000000800ULL);
static constexpr Bitboard   HH84(0x0201008040201008ULL, 0x0000000000001000ULL);
static constexpr Bitboard   HH85(0x0402010080402010ULL, 0x0000000000002000ULL);
static constexpr Bitboard   HH86(0x0804020100804020ULL, 0x0000000000004000ULL);
static constexpr Bitboard   HH87(0x1008040201008040ULL, 0x0000000000008000ULL);
static constexpr Bitboard   HH88(0x2010080402010080ULL, 0x0000000000010000ULL);
static constexpr Bitboard   HH89(0x4020100804020100ULL, 0x0000000000020000ULL);
static constexpr Bitboard   HH91(0x0040201008040201ULL, 0x0000000000000001ULL);
static constexpr Bitboard   HH92(0x0080402010080402ULL, 0x0000000000000002ULL);
static constexpr Bitboard   HH93(0x0100804020100804ULL, 0x0000000000000004ULL);
static constexpr Bitboard   HH94(0x0201008040201008ULL, 0x0000000000000008ULL);
static constexpr Bitboard   HH95(0x0402010080402010ULL, 0x0000000000000010ULL);
static constexpr Bitboard   HH96(0x0804020100804020ULL, 0x0000000000000020ULL);
static constexpr Bitboard   HH97(0x1008040201008040ULL, 0x0000000000000040ULL);
static constexpr Bitboard   HH98(0x2010080402010080ULL, 0x0000000000000080ULL);
static constexpr Bitboard   HH99(0x4020100804020100ULL, 0x0000000000000100ULL);
static constexpr Bitboard   _hhorizont [Square::Squares] = { 
                                 HH11,  HH12,  HH13,  HH14,  HH15,
                                 HH16,  HH17,  HH18,  HH19,
                                 HH21,  HH22,  HH23,  HH24,  HH25,
                                 HH26,  HH27,  HH28,  HH29,
                                 HH31,  HH32,  HH33,  HH34,  HH35,
                                 HH36,  HH37,  HH38,  HH39,
                                 HH41,  HH42,  HH43,  HH44,  HH45,
                                 HH46,  HH47,  HH48,  HH49,
                                 HH51,  HH52,  HH53,  HH54,  HH55,
                                 HH56,  HH57,  HH58,  HH59,
                                 HH61,  HH62,  HH63,  HH64,  HH65,
                                 HH66,  HH67,  HH68,  HH69,
                                 HH71,  HH72,  HH73,  HH74,  HH75,
                                 HH76,  HH77,  HH78,  HH79,
                                 HH81,  HH82,  HH83,  HH84,  HH85,
                                 HH86,  HH87,  HH88,  HH89,
                                 HH91,  HH92,  HH93,  HH94,  HH95,
                                 HH96,  HH97,  HH98,  HH99
                                 // should be poisoning here?
                            };

// vertical
static constexpr Bitboard   HV11(0x00000000000001feULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV12(0x00000000000001fdULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV13(0x00000000000001fbULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV14(0x00000000000001f7ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV15(0x00000000000001efULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV16(0x00000000000001dfULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV17(0x00000000000001bfULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV18(0x000000000000017fULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV19(0x00000000000000ffULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV21(0x000000000003fc00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV22(0x000000000003fa00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV23(0x000000000003f600ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV24(0x000000000003ee00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV25(0x000000000003de00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV26(0x000000000003be00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV27(0x0000000000037e00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV28(0x000000000002fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV29(0x000000000001fe00ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV31(0x0000000007f80000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV32(0x0000000007f40000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV33(0x0000000007ec0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV34(0x0000000007dc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV35(0x0000000007bc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV36(0x00000000077c0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV37(0x0000000006fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV38(0x0000000005fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV39(0x0000000003fc0000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV41(0x0000000ff0000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV42(0x0000000fe8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV43(0x0000000fd8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV44(0x0000000fb8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV45(0x0000000f78000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV46(0x0000000ef8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV47(0x0000000df8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV48(0x0000000bf8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV49(0x00000007f8000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV51(0x00001fe000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV52(0x00001fd000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV53(0x00001fb000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV54(0x00001f7000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV55(0x00001ef000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV56(0x00001df000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV57(0x00001bf000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV58(0x000017f000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV59(0x00000ff000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV61(0x003fc00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV62(0x003fa00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV63(0x003f600000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV64(0x003ee00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV65(0x003de00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV66(0x003be00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV67(0x0037e00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV68(0x002fe00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV69(0x001fe00000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV71(0x7f80000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV72(0x7f40000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV73(0x7ec0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV74(0x7dc0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV75(0x7bc0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV76(0x77c0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV77(0x6fc0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV78(0x5fc0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV79(0x3fc0000000000000ULL, 0x0000000000000000ULL);
static constexpr Bitboard   HV81(0x0000000000000000ULL, 0x00000000000001feULL);
static constexpr Bitboard   HV82(0x0000000000000000ULL, 0x00000000000001fdULL);
static constexpr Bitboard   HV83(0x0000000000000000ULL, 0x00000000000001fbULL);
static constexpr Bitboard   HV84(0x0000000000000000ULL, 0x00000000000001f7ULL);
static constexpr Bitboard   HV85(0x0000000000000000ULL, 0x00000000000001efULL);
static constexpr Bitboard   HV86(0x0000000000000000ULL, 0x00000000000001dfULL);
static constexpr Bitboard   HV87(0x0000000000000000ULL, 0x00000000000001bfULL);
static constexpr Bitboard   HV88(0x0000000000000000ULL, 0x000000000000017fULL);
static constexpr Bitboard   HV89(0x0000000000000000ULL, 0x00000000000000ffULL);
static constexpr Bitboard   HV91(0x0000000000000000ULL, 0x000000000003fc00ULL);
static constexpr Bitboard   HV92(0x0000000000000000ULL, 0x000000000003fa00ULL);
static constexpr Bitboard   HV93(0x0000000000000000ULL, 0x000000000003f600ULL);
static constexpr Bitboard   HV94(0x0000000000000000ULL, 0x000000000003ee00ULL);
static constexpr Bitboard   HV95(0x0000000000000000ULL, 0x000000000003de00ULL);
static constexpr Bitboard   HV96(0x0000000000000000ULL, 0x000000000003be00ULL);
static constexpr Bitboard   HV97(0x0000000000000000ULL, 0x0000000000037e00ULL);
static constexpr Bitboard   HV98(0x0000000000000000ULL, 0x000000000002fe00ULL);
static constexpr Bitboard   HV99(0x0000000000000000ULL, 0x000000000001fe00ULL);
static constexpr Bitboard   _hvertical [Square::Squares] = { 
                                 HV11,  HV12,  HV13,  HV14,  HV15,
                                 HV16,  HV17,  HV18,  HV19,
                                 HV21,  HV22,  HV23,  HV24,  HV25,
                                 HV26,  HV27,  HV28,  HV29,
                                 HV31,  HV32,  HV33,  HV34,  HV35,
                                 HV36,  HV37,  HV38,  HV39,
                                 HV41,  HV42,  HV43,  HV44,  HV45,
                                 HV46,  HV47,  HV48,  HV49,
                                 HV51,  HV52,  HV53,  HV54,  HV55,
                                 HV56,  HV57,  HV58,  HV59,
                                 HV61,  HV62,  HV63,  HV64,  HV65,
                                 HV66,  HV67,  HV68,  HV69,
                                 HV71,  HV72,  HV73,  HV74,  HV75,
                                 HV76,  HV77,  HV78,  HV79,
                                 HV81,  HV82,  HV83,  HV84,  HV85,
                                 HV86,  HV87,  HV88,  HV89,
                                 HV91,  HV92,  HV93,  HV94,  HV95,
                                 HV96,  HV97,  HV98,  HV99
                                 // should be poisoning here?
                            };

// bitboard with a filled rank
static constexpr Bitboard   RNK1B(0x0040201008040201ULL << 0,
                                  0x0000000000000201ULL << 0        );
static constexpr Bitboard   RNK9B(0x0040201008040201ULL << 8,
                                  0x0000000000000201ULL << 8        );

// bitboards with a filled column
static constexpr Bitboard   FIL1B(0x00000000000001ffULL << (9 * 0),
                                  0x0000000000000000ULL             );
static constexpr Bitboard   FIL9B(0x0000000000000000ULL,
                                  0x00000000000001ffULL << (9 * 1)  );

/* ------------------------------------------------------------------------- */


/* ---------------------------- global variables --------------------------- */

// Array of bitboard representing upper part of the board
static Bitboard             _forward[Color::Colors][Square::Ranks];

// KA Effect tables
static Bitboard             _KAEffectT[20224  + 1];
static Bitboard             _KAEffectM[Square::Squares];
static int                  _KAEffectI[Square::Squares];

// HI Effect tables
static Bitboard             _HIEffectT[495616 + 1];
static Bitboard             _HIEffectM[Square::Squares];
static int                  _HIEffectI[Square::Squares];

// KY Effect table (the variation is 2^8)
static Bitboard             _KYEffectT[Square::Squares][Color::Colors][128];
static Bitboard             _KBEffectT[Square::Squares][128];
static Bitboard             _KWEffectT[Square::Squares][128];

// Adjacent Effect table
static Bitboard             _ADEffectT[Square::Squares][Piece::Pieces];

// Direction mask for escaping from distant effect
static Bitboard             _direction[Square::Squares][Direction::Directions];

/* ------------------------------------------------------------------------- */

/* ---------------------------- function protos ---------------------------- */

static void     _initForward     (void);
static void     _initKAEffect    (void);
static void     _initHIEffect    (void);
static void     _initKYEffect    (void);
static void     _initADEffect    (void);
static void     _initDirection   (void);

static Bitboard _indexToOccupied (int, int, Bitboard);
static Bitboard _KAEffectCalc    (Square::Square, const Bitboard &);
static Bitboard _KAEffectMask    (Square::Square);
static Bitboard _HIEffectCalc    (Square::Square, const Bitboard &);
static Bitboard _HIEffectMask    (Square::Square);
static Bitboard _KEEffectCalc    (Color::Color, Square::Square);
static Bitboard _KIEffectCalc    (Color::Color, Square::Square);

/* ------------------------------------------------------------------------- */


/* ---------------------------- implementations ---------------------------- */

/**
 * Initialize correspoinding bits of the forward part.
 * This bitboards instantly seem to be fundamental and should be contained
 * in Bitboard implementaion. However, Bitboard resuses any semantics other
 * than geometries. _foraward includes Color::White and Color::Black indices.
 * 
 *  ex) _forward[Color::Black][Square::RNK5]
 *
 *      * * * * * * * * *
 *      * * * * * * * * *
 *      * * * * * * * * *  ^
 *      * * * * * * * * *  |
 *      . . . . . . . . . RNK5
 *      . . . . . . . . .
 *      . . . . . . . . .
 *      . . . . . . . . .
 *      . . . . . . . . .
 * 
 */
static void _initForward (void)
{

    // for Black player
    for (auto r : Square::rankAll) {
        Bitboard b(Bitboard::Zero);
        for (auto i = Square::RNK1; i < r; ++i) {
            b |= Bitboard::Rank[i];
        }
        _forward[Color::Black][r] = b;
    }

    // for White player
    for (auto r : Square::rankAll) {
        Bitboard b(Bitboard::Zero);
        for (auto i = Square::RNK9; i > r; --i) {
            b |= Bitboard::Rank[i];
        }
        _forward[Color::White][r] = b;
    }

}



/**
 * initialize effect tables for KA
 * 
 */
static void _initKAEffect (void)
{

    Bitboard *              effect = _KAEffectT;
    Bitboard *              mask   = _KAEffectM;
    int *                   eindex = _KAEffectI;

    int                     index  = 0;

    for (auto sq : Square::all) {

        eindex[sq] = index;
        mask  [sq] = _KAEffectMask(sq);

        // how many bits are associated with effect of KA at the square
        // determins a number of elements in the effect table.
        const int bits = mask[sq].popcnt();
        const int num  = 1 << bits;

        for (int i = 0; i < num; ++i) {
            Bitboard occupied = _indexToOccupied(i, bits, mask[sq]);
            size_t idx        = index + occupied.index(mask[sq]);
            effect[idx]       = _KAEffectCalc(sq, occupied);
        }

        index += num;

    }

    // give zero bitboard for piece at out of the board.
    effect[index]        = Bitboard::Zero;
    eindex[Square::SQVD] = index;

}



/**
 * initialize effect tables for HI
 * 
 */
static void _initHIEffect (void)
{

    Bitboard *              effect = _HIEffectT;
    Bitboard *              mask   = _HIEffectM;
    int *                   eindex = _HIEffectI;

    int                     index  = 0;

    for (auto sq : Square::all) {

        eindex[sq] = index;
        mask  [sq] = _HIEffectMask(sq);

        // how many bits are associated with effect of HI at the square
        // determins a number of elements in the effect table.
        const int bits = mask[sq].popcnt();
        const int num  = 1 << bits;

        for (int i = 0; i < num; ++i) {
            Bitboard occupied = _indexToOccupied(i, bits, mask[sq]);
            size_t idx        = index + occupied.index(mask[sq]);
            effect[idx]       = _HIEffectCalc(sq, occupied);
        }

        index += num;

    }

    // give zero bitboard for piece at out of the board.
    effect[index]        = Bitboard::Zero;
    eindex[Square::SQVD] = index;

}



/**
 * initialize effect tables for KY
 * 
 */
static void _initKYEffect (void)
{

    const int               var = 7; // variation of the occupation

    for (auto c : Color::all) {
        for (auto sq : Square::all) {
            Bitboard mask = Bitboard::File[Square::toFile(sq)] & (~(RNK1B | RNK9B));
            for (int i = 0; i < (1 << var); ++i) {
                Bitboard occupied    = _indexToOccupied(i, var, mask);
                _KYEffectT[sq][c][i] = HI(sq, occupied)
                                        & _forward[c][Square::toRank(sq)];
                if (c == Color::Black) {
                    _KBEffectT[sq][i] = _KYEffectT[sq][c][i];
                } else {
                    _KWEffectT[sq][i] = _KYEffectT[sq][c][i];
                }
                    
            }
        }
    }
}



/**
 * initialize the tables for adjacent effect
 * 
 */
static void _initADEffect (void)
{

    using namespace Piece;

    for (auto c : Color::all) {
        for (auto sq : Square::all) {

            // FU
            _ADEffectT[sq][polar(Piece::FU, c)] = KY(c, sq, Bitboard::Fill);

            // KY has none of adjacent effect but long-distance effect
            _ADEffectT[sq][polar(Piece::KY, c)] = Bitboard::Zero;

            // KE 
            _ADEffectT[sq][polar(Piece::KE, c)] = _KEEffectCalc(c, sq);

            // GI
            _ADEffectT[sq][polar(Piece::GI, c)] = KY(c, sq, Bitboard::Fill) |
                                                  KA(   sq, Bitboard::Fill)   ;

            // KI, TO, NY, NK, NG
            _ADEffectT[sq][polar(Piece::KI, c)] = _KIEffectCalc(c, sq);
            _ADEffectT[sq][polar(Piece::TO, c)] = _KIEffectCalc(c, sq);
            _ADEffectT[sq][polar(Piece::NY, c)] = _KIEffectCalc(c, sq);
            _ADEffectT[sq][polar(Piece::NK, c)] = _KIEffectCalc(c, sq);
            _ADEffectT[sq][polar(Piece::NG, c)] = _KIEffectCalc(c, sq);

            // KA has none of adjacent effect but long-distance effect
            _ADEffectT[sq][polar(Piece::KA, c)] = Bitboard::Zero;

            // HI has none of adjacent effect but long-distance effect
            _ADEffectT[sq][polar(Piece::HI, c)] = Bitboard::Zero;

            // OU
            _ADEffectT[sq][polar(Piece::OU, c)] = HI(sq, Bitboard::Fill) |
                                                  KA(sq, Bitboard::Fill)   ;

            // UM
            _ADEffectT[sq][polar(Piece::UM, c)] = HI(sq, Bitboard::Fill);

            // RY
            _ADEffectT[sq][polar(Piece::RY, c)] = KA(sq, Bitboard::Fill);
        }
    }

}



/**
 * initialize direction mask
 * 
 */
static void _initDirection (void)
{

    // initialize whole table
    for (auto s1 : Square::all) {
        Bitboard oct = _octant[s1];
        while (oct) {
            auto s2    = oct.pick();
            int  index = Direction::adjacntDirection(s1, s2);
            _direction[s1][index]                       = Bitboard::Fill;
            _direction[s1][index ^ Direction::Reverse]  = Bitboard::Fill;
        }
    }

    // make mask
    for (auto s1 : Square::all) {
        Bitboard oct = _octant[s1];
        while (oct) {
            auto s2    = oct.pick();
            int  index = Direction::adjacntDirection(s1, s2);
            _direction[s1][index                     ] ^= Bitboard::Square[s2];
            _direction[s1][index ^ Direction::Reverse] ^= Bitboard::Square[s2];
        }
    }

}



/**
 * Make an occupied bitboard from the given index and mask
 * @param index an unique number corresponding the occupation
 * @param bits  number of the correspongin bits
 * @param mask  effect mask
 * @return a bitboard representing the occupation
 */
static Bitboard _indexToOccupied (int index, int bits, Bitboard mask)
{

    Bitboard                result = Bitboard::Zero;

    for (int i = 0; i < bits; ++i) {
        Square::Square p = mask.pick();
        if (index & (1 << i)) {
            result ^= p;
        }
    }

    return result;

}


/**
 * Make an effect bitboard for KA
 * @param sq square of KA
 * @param ocp bitboard representing occupied square
 * @return masking Bitboard
 */
static Bitboard _KAEffectCalc (Square::Square sq, const Bitboard &ocp)
{

    const int               delta[] = { Region::UWRDR, // Upward right
                                        Region::UWRDL, // Upward left
                                        Region::DWRDL, // Downward left
                                        Region::DWRDR  // Downward right
                                      };
    Bitboard                result  = Bitboard::Zero;

    using namespace Region;

    for (auto dir : delta) {
        for (auto r = Convert::toRegion(sq); ! isWall(r); r += dir) {

            const auto p  = Convert::toSquare(r);
            result       ^= p;

            if (ocp & p) {
                break;
            }

        }
    }

    return result;

}



/**
 * Make a simple effect mask around the square for KA
 * @param sq square of KA
 * @return masking Bitboard
 */
static Bitboard _KAEffectMask (Square::Square sq)
{

    const Square::Rank      rank   = Square::toRank(sq);
    const Square::File      file   = Square::toFile(sq);
    Bitboard                result = Bitboard::Zero;

    for (auto r : Square::rankInt) {
        for (auto c : Square::fileInt) {
            if (abs(rank - r) == abs(file - c)) { // abs(tangent) == 1
                result ^= (r | c);
            }
        }
    }

    result &= (Bitboard::Fill ^ sq);
    return result;

}



/**
 * Make an effect bitboard for HI
 * @param sq square of HI
 * @param ocp bitboard representing occupied square
 * @return masking Bitboard
 */
static Bitboard _HIEffectCalc (Square::Square sq, const Bitboard &ocp)
{

    const int               delta[] = { Region::RWARD, // Rightward
                                        Region::UWARD, // Upward
                                        Region::LWARD, // Leftward
                                        Region::DWARD  // Downward
                                      };
    Bitboard                result  = Bitboard::Zero;

    using namespace Region;

    for (auto dir : delta) {
        for (auto r = Convert::toRegion(sq); ! isWall(r); r += dir) {

            const auto p  = Convert::toSquare(r);
            result       ^= p;

            if (ocp & p) {
                break;
            }

        }
    }

    return result;

}



/**
 * Make a simple effect mask around the square for HI
 * @param sq square of HI
 * @return masking Bitboard
 */
static Bitboard _HIEffectMask (Square::Square sq)
{

    const Square::Rank      rank   = Square::toRank(sq);
    const Square::File      file   = Square::toFile(sq);
    Bitboard                result = Bitboard::Rank[rank] ^ Bitboard::File[file];

    if (file != Square::FIL1) { result &= (~FIL1B); }
    if (file != Square::FIL9) { result &= (~FIL9B); }
    if (rank != Square::RNK1) { result &= (~RNK1B); }
    if (rank != Square::RNK9) { result &= (~RNK9B); }

    return result;

}



/**
 * Calculate KE effect
 * @param c color of piece
 * @param sq square of KE
 * @return Bitboard
 */
static Bitboard _KEEffectCalc (Color::Color c, Square::Square sq)
{

    const Square::Rank      rank   = Square::toRank(sq);
    const Square::File      file   = Square::toFile(sq);
    Bitboard                result = Bitboard::Zero;

    if (c == Color::Black) {
        if (rank < Square::RNK3) {
            return result;
        }
        if (file > Square::FIL1) {
            result |= (sq + Square::UWARD + Square::UWRDR);
        }
        if (file < Square::FIL9) {
            result |= (sq + Square::UWARD + Square::UWRDL);
        }
    } else {
        if (rank > Square::RNK7) {
            return result;
        }
        if (file > Square::FIL1) {
            result |= (sq + Square::DWARD + Square::DWRDR);
        }
        if (file < Square::FIL9) {
            result |= (sq + Square::DWARD + Square::DWRDL);
        }
    }

    return result;

}



/**
 * Calculate KI effect
 * @param c color of piece
 * @param sq square of KE
 * @return Bitboard
 */
static Bitboard _KIEffectCalc (Color::Color c, Square::Square sq)
{

    const Square::Rank      rank   = Square::toRank(sq);
    const Square::File      file   = Square::toFile(sq);
    Bitboard                result = KA(sq, Bitboard::Fill) |
                                     HI(sq, Bitboard::Fill)   ;


    if (c == Color::Black) {
        if (rank < Square::RNK9) {
            if (file > Square::FIL1) {
                result ^= (sq + Square::DWRDR);
            }
            if (file < Square::FIL9) {
                result ^= (sq + Square::DWRDL);
            }
        }
    } else {
        if (rank > Square::RNK1) {
            if (file > Square::FIL1) {
                result ^= (sq + Square::UWRDR);
            }
            if (file < Square::FIL9) {
                result ^= (sq + Square::UWRDL);
            }
        }
    }

    return result;

}



/**
 * HI effect
 * @param sq square of the piece
 * @param ocp occupation
 * @return effect in Bitboard
 */
const Bitboard & HI (Square::Square sq, const Bitboard &ocp)
{

    size_t                  index;

    index = _HIEffectI[sq] + ocp.index(_HIEffectM[sq]);

    return _HIEffectT[index];

}



/**
 * KA effect
 * @param sq square of the piece
 * @param ocp occupation
 * @return effect in Bitboard
 */
const Bitboard & KA (Square::Square sq, const Bitboard &ocp)
{

    size_t                  index;

    index = _KAEffectI[sq] + ocp.index(_KAEffectM[sq]);

    return _KAEffectT[index];

}



/**
 * KY effect
 * @param c color of the piece
 * @param sq square of the piece
 * @param ocp occupation
 * @return effect in Bitboard
 */
const Bitboard & KY (Color::Color c, Square::Square sq, const Bitboard &ocp)
{

    int                     index;

    index = (ocp.part(sq) >> Bitboard::VShift[sq]) & 0x7f;

    return _KYEffectT[sq][c][index];

}



/**
 * BKY effect
 * @param sq square of the piece
 * @param ocp occupation
 * @return effect in Bitboard
 */
const Bitboard & KB (Square::Square sq, const Bitboard &ocp)
{

    int                     index;

    index = (ocp.part(sq) >> Bitboard::VShift[sq]) & 0x7f;

    return _KBEffectT[sq][index];

}



/**
 * WKY effect
 * @param sq square of the piece
 * @param ocp occupation
 * @return effect in Bitboard
 */
const Bitboard & KW (Square::Square sq, const Bitboard &ocp)
{

    int                     index;

    index = (ocp.part(sq) >> Bitboard::VShift[sq]) & 0x7f;

    return _KWEffectT[sq][index];

}



/**
 * Adjacent effect
 * @param sq square of the piece
 * @param p piece with polarity
 * @return effect in bitboard
 */
const Bitboard & AD (Square::Square sq, Piece::Piece p)
{

    return _ADEffectT[sq][p];

}



/**
 * KA effect on empty board
 * @param sq square of the piece
 * @return effect in bitboard
 */
const Bitboard & FK (Square::Square sq)
{

    return _ADEffectT[sq][Piece::BKA];

}



/**
 * Half KA effect on empty board (right shoulder up)
 * @param sq square of the piece
 * @return effect in bitboard
 */
const Bitboard & RS (Square::Square sq)
{

    return _rsup[sq];

}



/**
 * Half KA effect on empty board (falling to the right)
 * @param sq square of the piece
 * @return effect in bitboard
 */
const Bitboard & FR (Square::Square sq)
{

    return _failtor[sq];

}



/**
 * Half HI effect on empty board (horizontal)
 * @param sq square of the piece
 * @return effect in bitboard
 */
const Bitboard & HH (Square::Square sq)
{

    return _hhorizont[sq];

}



/**
 * Half HI effect on empty board (vertical)
 * @param sq square of the piece
 * @return effect in bitboard
 */
const Bitboard & HV (Square::Square sq)
{

    return _hvertical[sq];

}



/**
 * HI effect on empty board
 * @param sq square of the piece
 * @return effect in bitboard
 */
const Bitboard & FH (Square::Square sq)
{

    return _ADEffectT[sq][Piece::BHI];

}



/**
 * Direction mask for escaping from distant effect
 * @param sq square of pole
 * @param dr direction
 * @return direction mask
 */
const Bitboard & ED (Square::Square sq, Direction::Direction dr)
{

    return _direction[sq][dr];

}



/**
 * Masking bitboard adjacent squares for duodecimal directions
 * @param sq square
 * @return the masking bitboard
 */
const Bitboard & DU (Square::Square sq)
{

    return _duodecimal[sq];

}



/**
 * Masking bitboard adjacent squares for octant directions
 * @param sq square
 * @return the masking bitboard
 */
const Bitboard & OC (Square::Square sq)
{

    return _octant[sq];

}



/**
 * Masking bitboard distant squares for quadrant directions
 * @param sq square
 * @return the masking bitboard
 */
const Bitboard & LD (Square::Square sq)
{

    return _distant[sq];

}



/**
 * Initializer
 * 
 */
void initialize ()
{

    // initialize _forward
    _initForward();

    // initialize KA Effect
    _initKAEffect();

    // initialize HI Effect
    _initHIEffect();

    // initialize KY Effect
    _initKYEffect();

    // initialize adjacent Effect
    _initADEffect();

    // initialize direction mask
    _initDirection();

}

/* ------------------------------------------------------------------------- */

// end namespace 'game::Effect'
} }
