/* mpz_fib_ui(result, n) -- Set RESULT to the Nth Fibonacci number.

Copyright (C) 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */


/* The lookup table makes small n instantaneous.  After that either a simple
   addition loop or a powering algorithm is used.

   Times vary a bit between odd and even sizes, due to the powering using a
   different last step for odd or even.  The even case does one multiply,
   the odd case two squares.

   Future:

   If the Lucas numbers L[n] were used as an auxiliary sequence, the
   "doubling" operation would be two squares rather than two multiplies.
   The formulas are a little more complicated, something like the following
   (not checked).

       F[2n] = ((F[n]+L[n])^2 - 6*F[n]^2 - 4*(-1)^n) / 2
       L[2n] = 5*F[n]^2 + 2*(-1)^n

       F[2n+1] = (F[2n] + L[2n]) / 2
       L[2n+1] = (5*F[2n] + L[2n]) / 2

   Lookup tables for L[] like the current F[] ones would be wanted.
   The L[] could even be optionally returned.

   Are there formulas with two squares using just F[n]?  Suspect not.  */


#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* change this to "#define TRACE(x) x" for diagnostics */
#define TRACE(x)


/* It seems the bigcase code usually cuts in almost immediately (ie. only a
   few sizes want the simple addition based code), so set the defaults like
   that.  */
#ifndef FIB_THRESHOLD
#if BITS_PER_MP_LIMB == 32
#define FIB_THRESHOLD  100
#endif
#if BITS_PER_MP_LIMB == 64
#define FIB_THRESHOLD  200
#endif
#endif


/* table1[i] is F[i], table2[i] is F[i+numberof(table1)].
   table2[i][0] is the low limb, table2[i][1] the high limb.  */

#if BITS_PER_MP_LIMB == 32
const mp_limb_t table1[] = {
 CNST_LIMB (0),  /* 0 */
 CNST_LIMB (1),  /* 1 */
 CNST_LIMB (1),  /* 2 */
 CNST_LIMB (2),  /* 3 */
 CNST_LIMB (3),  /* 4 */
 CNST_LIMB (5),  /* 5 */
 CNST_LIMB (8),  /* 6 */
 CNST_LIMB (13),  /* 7 */
 CNST_LIMB (21),  /* 8 */
 CNST_LIMB (34),  /* 9 */
 CNST_LIMB (55),  /* 10 */
 CNST_LIMB (89),  /* 11 */
 CNST_LIMB (144),  /* 12 */
 CNST_LIMB (233),  /* 13 */
 CNST_LIMB (377),  /* 14 */
 CNST_LIMB (610),  /* 15 */
 CNST_LIMB (987),  /* 16 */
 CNST_LIMB (1597),  /* 17 */
 CNST_LIMB (2584),  /* 18 */
 CNST_LIMB (4181),  /* 19 */
 CNST_LIMB (6765),  /* 20 */
 CNST_LIMB (10946),  /* 21 */
 CNST_LIMB (17711),  /* 22 */
 CNST_LIMB (28657),  /* 23 */
 CNST_LIMB (46368),  /* 24 */
 CNST_LIMB (75025),  /* 25 */
 CNST_LIMB (121393),  /* 26 */
 CNST_LIMB (196418),  /* 27 */
 CNST_LIMB (317811),  /* 28 */
 CNST_LIMB (514229),  /* 29 */
 CNST_LIMB (832040),  /* 30 */
 CNST_LIMB (1346269),  /* 31 */
 CNST_LIMB (2178309),  /* 32 */
 CNST_LIMB (3524578),  /* 33 */
 CNST_LIMB (5702887),  /* 34 */
 CNST_LIMB (9227465),  /* 35 */
 CNST_LIMB (14930352),  /* 36 */
 CNST_LIMB (24157817),  /* 37 */
 CNST_LIMB (39088169),  /* 38 */
 CNST_LIMB (63245986),  /* 39 */
 CNST_LIMB (102334155),  /* 40 */
 CNST_LIMB (165580141),  /* 41 */
 CNST_LIMB (267914296),  /* 42 */
 CNST_LIMB (433494437),  /* 43 */
 CNST_LIMB (701408733),  /* 44 */
 CNST_LIMB (1134903170),  /* 45 */
 CNST_LIMB (1836311903),  /* 46 */
 CNST_LIMB (2971215073),  /* 47 */
};
const mp_limb_t table2[][2] = {
 {CNST_LIMB(512559680), CNST_LIMB(1)}, /* 48 */
 {CNST_LIMB(3483774753), CNST_LIMB(1)}, /* 49 */
 {CNST_LIMB(3996334433), CNST_LIMB(2)}, /* 50 */
 {CNST_LIMB(3185141890), CNST_LIMB(4)}, /* 51 */
 {CNST_LIMB(2886509027), CNST_LIMB(7)}, /* 52 */
 {CNST_LIMB(1776683621), CNST_LIMB(12)}, /* 53 */
 {CNST_LIMB(368225352), CNST_LIMB(20)}, /* 54 */
 {CNST_LIMB(2144908973), CNST_LIMB(32)}, /* 55 */
 {CNST_LIMB(2513134325), CNST_LIMB(52)}, /* 56 */
 {CNST_LIMB(363076002), CNST_LIMB(85)}, /* 57 */
 {CNST_LIMB(2876210327), CNST_LIMB(137)}, /* 58 */
 {CNST_LIMB(3239286329), CNST_LIMB(222)}, /* 59 */
 {CNST_LIMB(1820529360), CNST_LIMB(360)}, /* 60 */
 {CNST_LIMB(764848393), CNST_LIMB(583)}, /* 61 */
 {CNST_LIMB(2585377753), CNST_LIMB(943)}, /* 62 */
 {CNST_LIMB(3350226146), CNST_LIMB(1526)}, /* 63 */
 {CNST_LIMB(1640636603), CNST_LIMB(2470)}, /* 64 */
 {CNST_LIMB(695895453), CNST_LIMB(3997)}, /* 65 */
 {CNST_LIMB(2336532056), CNST_LIMB(6467)}, /* 66 */
 {CNST_LIMB(3032427509), CNST_LIMB(10464)}, /* 67 */
 {CNST_LIMB(1073992269), CNST_LIMB(16932)}, /* 68 */
 {CNST_LIMB(4106419778), CNST_LIMB(27396)}, /* 69 */
 {CNST_LIMB(885444751), CNST_LIMB(44329)}, /* 70 */
 {CNST_LIMB(696897233), CNST_LIMB(71726)}, /* 71 */
 {CNST_LIMB(1582341984), CNST_LIMB(116055)}, /* 72 */
 {CNST_LIMB(2279239217), CNST_LIMB(187781)}, /* 73 */
 {CNST_LIMB(3861581201), CNST_LIMB(303836)}, /* 74 */
 {CNST_LIMB(1845853122), CNST_LIMB(491618)}, /* 75 */
 {CNST_LIMB(1412467027), CNST_LIMB(795455)}, /* 76 */
 {CNST_LIMB(3258320149), CNST_LIMB(1287073)}, /* 77 */
 {CNST_LIMB(375819880), CNST_LIMB(2082529)}, /* 78 */
 {CNST_LIMB(3634140029), CNST_LIMB(3369602)}, /* 79 */
 {CNST_LIMB(4009959909), CNST_LIMB(5452131)}, /* 80 */
 {CNST_LIMB(3349132642), CNST_LIMB(8821734)}, /* 81 */
 {CNST_LIMB(3064125255), CNST_LIMB(14273866)}, /* 82 */
 {CNST_LIMB(2118290601), CNST_LIMB(23095601)}, /* 83 */
 {CNST_LIMB(887448560), CNST_LIMB(37369468)}, /* 84 */
 {CNST_LIMB(3005739161), CNST_LIMB(60465069)}, /* 85 */
 {CNST_LIMB(3893187721), CNST_LIMB(97834537)}, /* 86 */
 {CNST_LIMB(2603959586), CNST_LIMB(158299607)}, /* 87 */
 {CNST_LIMB(2202180011), CNST_LIMB(256134145)}, /* 88 */
 {CNST_LIMB(511172301), CNST_LIMB(414433753)}, /* 89 */
 {CNST_LIMB(2713352312), CNST_LIMB(670567898)}, /* 90 */
 {CNST_LIMB(3224524613), CNST_LIMB(1085001651)}, /* 91 */
 {CNST_LIMB(1642909629), CNST_LIMB(1755569550)}, /* 92 */
 {CNST_LIMB(572466946), CNST_LIMB(2840571202)}, /* 93 */
};
#endif

#if BITS_PER_MP_LIMB == 64
const mp_limb_t table1[] = {
 CNST_LIMB (0),  /* 0 */
 CNST_LIMB (1),  /* 1 */
 CNST_LIMB (1),  /* 2 */
 CNST_LIMB (2),  /* 3 */
 CNST_LIMB (3),  /* 4 */
 CNST_LIMB (5),  /* 5 */
 CNST_LIMB (8),  /* 6 */
 CNST_LIMB (13),  /* 7 */
 CNST_LIMB (21),  /* 8 */
 CNST_LIMB (34),  /* 9 */
 CNST_LIMB (55),  /* 10 */
 CNST_LIMB (89),  /* 11 */
 CNST_LIMB (144),  /* 12 */
 CNST_LIMB (233),  /* 13 */
 CNST_LIMB (377),  /* 14 */
 CNST_LIMB (610),  /* 15 */
 CNST_LIMB (987),  /* 16 */
 CNST_LIMB (1597),  /* 17 */
 CNST_LIMB (2584),  /* 18 */
 CNST_LIMB (4181),  /* 19 */
 CNST_LIMB (6765),  /* 20 */
 CNST_LIMB (10946),  /* 21 */
 CNST_LIMB (17711),  /* 22 */
 CNST_LIMB (28657),  /* 23 */
 CNST_LIMB (46368),  /* 24 */
 CNST_LIMB (75025),  /* 25 */
 CNST_LIMB (121393),  /* 26 */
 CNST_LIMB (196418),  /* 27 */
 CNST_LIMB (317811),  /* 28 */
 CNST_LIMB (514229),  /* 29 */
 CNST_LIMB (832040),  /* 30 */
 CNST_LIMB (1346269),  /* 31 */
 CNST_LIMB (2178309),  /* 32 */
 CNST_LIMB (3524578),  /* 33 */
 CNST_LIMB (5702887),  /* 34 */
 CNST_LIMB (9227465),  /* 35 */
 CNST_LIMB (14930352),  /* 36 */
 CNST_LIMB (24157817),  /* 37 */
 CNST_LIMB (39088169),  /* 38 */
 CNST_LIMB (63245986),  /* 39 */
 CNST_LIMB (102334155),  /* 40 */
 CNST_LIMB (165580141),  /* 41 */
 CNST_LIMB (267914296),  /* 42 */
 CNST_LIMB (433494437),  /* 43 */
 CNST_LIMB (701408733),  /* 44 */
 CNST_LIMB (1134903170),  /* 45 */
 CNST_LIMB (1836311903),  /* 46 */
 CNST_LIMB (2971215073),  /* 47 */
 CNST_LIMB (4807526976),  /* 48 */
 CNST_LIMB (7778742049),  /* 49 */
 CNST_LIMB (12586269025),  /* 50 */
 CNST_LIMB (20365011074),  /* 51 */
 CNST_LIMB (32951280099),  /* 52 */
 CNST_LIMB (53316291173),  /* 53 */
 CNST_LIMB (86267571272),  /* 54 */
 CNST_LIMB (139583862445),  /* 55 */
 CNST_LIMB (225851433717),  /* 56 */
 CNST_LIMB (365435296162),  /* 57 */
 CNST_LIMB (591286729879),  /* 58 */
 CNST_LIMB (956722026041),  /* 59 */
 CNST_LIMB (1548008755920),  /* 60 */
 CNST_LIMB (2504730781961),  /* 61 */
 CNST_LIMB (4052739537881),  /* 62 */
 CNST_LIMB (6557470319842),  /* 63 */
 CNST_LIMB (10610209857723),  /* 64 */
 CNST_LIMB (17167680177565),  /* 65 */
 CNST_LIMB (27777890035288),  /* 66 */
 CNST_LIMB (44945570212853),  /* 67 */
 CNST_LIMB (72723460248141),  /* 68 */
 CNST_LIMB (117669030460994),  /* 69 */
 CNST_LIMB (190392490709135),  /* 70 */
 CNST_LIMB (308061521170129),  /* 71 */
 CNST_LIMB (498454011879264),  /* 72 */
 CNST_LIMB (806515533049393),  /* 73 */
 CNST_LIMB (1304969544928657),  /* 74 */
 CNST_LIMB (2111485077978050),  /* 75 */
 CNST_LIMB (3416454622906707),  /* 76 */
 CNST_LIMB (5527939700884757),  /* 77 */
 CNST_LIMB (8944394323791464),  /* 78 */
 CNST_LIMB (14472334024676221),  /* 79 */
 CNST_LIMB (23416728348467685),  /* 80 */
 CNST_LIMB (37889062373143906),  /* 81 */
 CNST_LIMB (61305790721611591),  /* 82 */
 CNST_LIMB (99194853094755497),  /* 83 */
 CNST_LIMB (160500643816367088),  /* 84 */
 CNST_LIMB (259695496911122585),  /* 85 */
 CNST_LIMB (420196140727489673),  /* 86 */
 CNST_LIMB (679891637638612258),  /* 87 */
 CNST_LIMB (1100087778366101931),  /* 88 */
 CNST_LIMB (1779979416004714189),  /* 89 */
 CNST_LIMB (2880067194370816120),  /* 90 */
 CNST_LIMB (4660046610375530309),  /* 91 */
 CNST_LIMB (7540113804746346429),  /* 92 */
 CNST_LIMB (12200160415121876738),  /* 93 */
};
const mp_limb_t table2[][2] = {
 {CNST_LIMB(1293530146158671551), CNST_LIMB(1)}, /* 94 */
 {CNST_LIMB(13493690561280548289), CNST_LIMB(1)}, /* 95 */
 {CNST_LIMB(14787220707439219840), CNST_LIMB(2)}, /* 96 */
 {CNST_LIMB(9834167195010216513), CNST_LIMB(4)}, /* 97 */
 {CNST_LIMB(6174643828739884737), CNST_LIMB(7)}, /* 98 */
 {CNST_LIMB(16008811023750101250), CNST_LIMB(11)}, /* 99 */
 {CNST_LIMB(3736710778780434371), CNST_LIMB(19)}, /* 100 */
 {CNST_LIMB(1298777728820984005), CNST_LIMB(31)}, /* 101 */
 {CNST_LIMB(5035488507601418376), CNST_LIMB(50)}, /* 102 */
 {CNST_LIMB(6334266236422402381), CNST_LIMB(81)}, /* 103 */
 {CNST_LIMB(11369754744023820757), CNST_LIMB(131)}, /* 104 */
 {CNST_LIMB(17704020980446223138), CNST_LIMB(212)}, /* 105 */
 {CNST_LIMB(10627031650760492279), CNST_LIMB(344)}, /* 106 */
 {CNST_LIMB(9884308557497163801), CNST_LIMB(557)}, /* 107 */
 {CNST_LIMB(2064596134548104464), CNST_LIMB(902)}, /* 108 */
 {CNST_LIMB(11948904692045268265), CNST_LIMB(1459)}, /* 109 */
 {CNST_LIMB(14013500826593372729), CNST_LIMB(2361)}, /* 110 */
 {CNST_LIMB(7515661444929089378), CNST_LIMB(3821)}, /* 111 */
 {CNST_LIMB(3082418197812910491), CNST_LIMB(6183)}, /* 112 */
 {CNST_LIMB(10598079642741999869), CNST_LIMB(10004)}, /* 113 */
 {CNST_LIMB(13680497840554910360), CNST_LIMB(16187)}, /* 114 */
 {CNST_LIMB(5831833409587358613), CNST_LIMB(26192)}, /* 115 */
 {CNST_LIMB(1065587176432717357), CNST_LIMB(42380)}, /* 116 */
 {CNST_LIMB(6897420586020075970), CNST_LIMB(68572)}, /* 117 */
 {CNST_LIMB(7963007762452793327), CNST_LIMB(110952)}, /* 118 */
 {CNST_LIMB(14860428348472869297), CNST_LIMB(179524)}, /* 119 */
 {CNST_LIMB(4376692037216111008), CNST_LIMB(290477)}, /* 120 */
 {CNST_LIMB(790376311979428689), CNST_LIMB(470002)}, /* 121 */
 {CNST_LIMB(5167068349195539697), CNST_LIMB(760479)}, /* 122 */
 {CNST_LIMB(5957444661174968386), CNST_LIMB(1230481)}, /* 123 */
 {CNST_LIMB(11124513010370508083), CNST_LIMB(1990960)}, /* 124 */
 {CNST_LIMB(17081957671545476469), CNST_LIMB(3221441)}, /* 125 */
 {CNST_LIMB(9759726608206432936), CNST_LIMB(5212402)}, /* 126 */
 {CNST_LIMB(8394940206042357789), CNST_LIMB(8433844)}, /* 127 */
 {CNST_LIMB(18154666814248790725), CNST_LIMB(13646246)}, /* 128 */
 {CNST_LIMB(8102862946581596898), CNST_LIMB(22080091)}, /* 129 */
 {CNST_LIMB(7810785687120836007), CNST_LIMB(35726338)}, /* 130 */
 {CNST_LIMB(15913648633702432905), CNST_LIMB(57806429)}, /* 131 */
 {CNST_LIMB(5277690247113717296), CNST_LIMB(93532768)}, /* 132 */
 {CNST_LIMB(2744594807106598585), CNST_LIMB(151339198)}, /* 133 */
 {CNST_LIMB(8022285054220315881), CNST_LIMB(244871966)}, /* 134 */
 {CNST_LIMB(10766879861326914466), CNST_LIMB(396211164)}, /* 135 */
 {CNST_LIMB(342420841837678731), CNST_LIMB(641083131)}, /* 136 */
 {CNST_LIMB(11109300703164593197), CNST_LIMB(1037294295)}, /* 137 */
 {CNST_LIMB(11451721545002271928), CNST_LIMB(1678377426)}, /* 138 */
 {CNST_LIMB(4114278174457313509), CNST_LIMB(2715671722)}, /* 139 */
 {CNST_LIMB(15565999719459585437), CNST_LIMB(4394049148)}, /* 140 */
 {CNST_LIMB(1233533820207347330), CNST_LIMB(7109720871)}, /* 141 */
 {CNST_LIMB(16799533539666932767), CNST_LIMB(11503770019)}, /* 142 */
 {CNST_LIMB(18033067359874280097), CNST_LIMB(18613490890)}, /* 143 */
 {CNST_LIMB(16385856825831661248), CNST_LIMB(30117260910)}, /* 144 */
 {CNST_LIMB(15972180111996389729), CNST_LIMB(48730751801)}, /* 145 */
 {CNST_LIMB(13911292864118499361), CNST_LIMB(78848012712)}, /* 146 */
 {CNST_LIMB(11436728902405337474), CNST_LIMB(127578764514)}, /* 147 */
 {CNST_LIMB(6901277692814285219), CNST_LIMB(206426777227)}, /* 148 */
 {CNST_LIMB(18338006595219622693), CNST_LIMB(334005541741)}, /* 149 */
 {CNST_LIMB(6792540214324356296), CNST_LIMB(540432318969)}, /* 150 */
 {CNST_LIMB(6683802735834427373), CNST_LIMB(874437860711)}, /* 151 */
 {CNST_LIMB(13476342950158783669), CNST_LIMB(1414870179680)}, /* 152 */
 {CNST_LIMB(1713401612283659426), CNST_LIMB(2289308040392)}, /* 153 */
 {CNST_LIMB(15189744562442443095), CNST_LIMB(3704178220072)}, /* 154 */
 {CNST_LIMB(16903146174726102521), CNST_LIMB(5993486260464)}, /* 155 */
 {CNST_LIMB(13646146663458994000), CNST_LIMB(9697664480537)}, /* 156 */
 {CNST_LIMB(12102548764475544905), CNST_LIMB(15691150741002)}, /* 157 */
 {CNST_LIMB(7301951354224987289), CNST_LIMB(25388815221540)}, /* 158 */
 {CNST_LIMB(957756044990980578), CNST_LIMB(41079965962543)}, /* 159 */
 {CNST_LIMB(8259707399215967867), CNST_LIMB(66468781184083)}, /* 160 */
 {CNST_LIMB(9217463444206948445), CNST_LIMB(107548747146626)}, /* 161 */
 {CNST_LIMB(17477170843422916312), CNST_LIMB(174017528330709)}, /* 162 */
 {CNST_LIMB(8247890213920313141), CNST_LIMB(281566275477336)}, /* 163 */
 {CNST_LIMB(7278316983633677837), CNST_LIMB(455583803808046)}, /* 164 */
 {CNST_LIMB(15526207197553990978), CNST_LIMB(737150079285382)}, /* 165 */
 {CNST_LIMB(4357780107478117199), CNST_LIMB(1192733883093429)}, /* 166 */
 {CNST_LIMB(1437243231322556561), CNST_LIMB(1929883962378812)}, /* 167 */
 {CNST_LIMB(5795023338800673760), CNST_LIMB(3122617845472241)}, /* 168 */
 {CNST_LIMB(7232266570123230321), CNST_LIMB(5052501807851053)}, /* 169 */
 {CNST_LIMB(13027289908923904081), CNST_LIMB(8175119653323294)}, /* 170 */
 {CNST_LIMB(1812812405337582786), CNST_LIMB(13227621461174348)}, /* 171 */
 {CNST_LIMB(14840102314261486867), CNST_LIMB(21402741114497642)}, /* 172 */
 {CNST_LIMB(16652914719599069653), CNST_LIMB(34630362575671990)}, /* 173 */
 {CNST_LIMB(13046272960151004904), CNST_LIMB(56033103690169633)}, /* 174 */
 {CNST_LIMB(11252443606040522941), CNST_LIMB(90663466265841624)}, /* 175 */
 {CNST_LIMB(5851972492481976229), CNST_LIMB(146696569956011258)}, /* 176 */
 {CNST_LIMB(17104416098522499170), CNST_LIMB(237360036221852882)}, /* 177 */
 {CNST_LIMB(4509644517294923783), CNST_LIMB(384056606177864141)}, /* 178 */
 {CNST_LIMB(3167316542107871337), CNST_LIMB(621416642399717024)}, /* 179 */
 {CNST_LIMB(7676961059402795120), CNST_LIMB(1005473248577581165)}, /* 180 */
 {CNST_LIMB(10844277601510666457), CNST_LIMB(1626889890977298189)}, /* 181 */
 {CNST_LIMB(74494587203909961), CNST_LIMB(2632363139554879355)}, /* 182 */
 {CNST_LIMB(10918772188714576418), CNST_LIMB(4259253030532177544)}, /* 183 */
 {CNST_LIMB(10993266775918486379), CNST_LIMB(6891616170087056899)}, /* 184 */
 {CNST_LIMB(3465294890923511181), CNST_LIMB(11150869200619234444)}, /* 185 */
 {CNST_LIMB(14458561666841997560), CNST_LIMB(18042485370706291343)}, /* 186 */
};
#endif


void
mpz_fib_ui (mpz_t r, unsigned long int n)
{
  mp_ptr         rp, rp_orig, tp, xp, yp;
  mp_size_t      ralloc, rsize, xsize;
  unsigned long  i, nfull;
  int            ni;
  TMP_DECL (marker);
  
  TRACE (printf ("\n", n);
         printf ("mpz_fib_ui n=%lu\n", n));

  if (n < numberof (table1))
    {
      mp_limb_t  l = table1[n];
      PTR(r)[0] = l;
      SIZ(r) = (l != 0);
      return;
    }

  if (n < numberof (table1) + numberof (table2))
    {
      mp_limb_t  hi, lo;
      MPZ_REALLOC (r, 2);
      lo = table2[n-numberof(table1)][0];
      hi = table2[n-numberof(table1)][1];
      PTR(r)[0] = lo;
      PTR(r)[1] = hi;
      SIZ(r) = 1 + (hi != 0);
      return;
    }

  TMP_MARK (marker);

  /* Variables are sized with +2 limbs because in bigcase, k limbs are
     doubled into a 2k+1 region, whereas the actual fib size may be only
     2k-1. */
  ralloc = MPZ_FIB_SIZE (n) + 2;
  MPZ_REALLOC (r, ralloc);

  rp = PTR(r);
  tp = TMP_ALLOC_LIMBS (ralloc);
  rp_orig = rp;

  nfull = n;
  ni = 0;
  if (n >= FIB_THRESHOLD)
    {
      /* find the base case to calculate */
      do {
        ni++, n /= 2;
      } while (n >= FIB_THRESHOLD);

      TRACE (printf ("new n=%lu ni=%d\n", n, ni));
      ASSERT (n >= FIB_THRESHOLD/2);

      /* Starting point for base case might come from just table lookups.
         The tests on FIB_THRESHOLD let the compiler eliminate some of this
         if FIB_THRESHOLD is big enough that one or both tables might never
         be in range. */

      if (FIB_THRESHOLD/2 < numberof (table1)
          && n < numberof (table1))
        {
          tp[0] = table1 [n-1];
          rp[0] = table1 [n];
          rsize = 1;
          goto bigcase;
        }
      if (FIB_THRESHOLD/2 <= numberof (table1)
          && n == numberof (table1))
        {
          tp[0] = table1 [n-1];
          tp[1] = 0;
          rp[0] = table2 [0][0];
          rp[1] = table2 [0][1];
          rsize = 2;
          goto bigcase;
        }
      if (FIB_THRESHOLD/2 < numberof (table1) + numberof (table2)
          && n < numberof (table1) + numberof (table2))
        {
          rp[0] = table2 [n - numberof (table1)][0];
          rp[1] = table2 [n - numberof (table1)][1];
          tp[0] = table2 [n-1 - numberof (table1)][0];
          tp[1] = table2 [n-1 - numberof (table1)][1];
          rsize = 2;
          goto bigcase;
        }
    }

  /* Simple loop starting from last two entries of table2[].
  
  rsize it maintained so that F[n] is normalized, meaning rp[rsize-1]!=0.
  rsize is also the size of tp=F[n-1], but it can have a high zero limb
  (sometimes).  rp and tp alternate between holding F[n] and F[n-1]. */

  ASSERT (n >= numberof (table1) + numberof (table2));
  i = n - (numberof (table1) + numberof (table2) - 1);
  
  rp[0] = table2 [numberof (table2) - 2][0];
  rp[1] = table2 [numberof (table2) - 2][1];
  tp[0] = table2 [numberof (table2) - 1][0];
  tp[1] = table2 [numberof (table2) - 1][1];
  rsize = 2;
  
  do
    {
      mp_limb_t  c;

      ASSERT (rsize < ralloc);
      c = mpn_add_n (rp, rp, tp, rsize);
      rp[rsize] = c;
      tp[rsize] = 0;
      rsize += c;

      if (--i == 0)
        goto no_swap;

      ASSERT (rsize < ralloc);
      c = mpn_add_n (tp, tp, rp, rsize);
      tp[rsize] = c;
      rp[rsize] = 0;
      rsize += c;
    }
  while (--i);

  MP_PTR_SWAP (rp, tp);

 no_swap:
  if (ni == 0)
    goto done;


 bigcase:

  /* Powering code for big N. 

  rp=F[n] and tp=F[n-1], and rsize is maintained so F[n] is normalized,
  meaning rp[rsize-1]!=0.  rsize is also the size of tp=F[n-1], but it
  can have a high zero limb.  */

  xp = TMP_ALLOC_LIMBS (ralloc);
  yp = TMP_ALLOC_LIMBS (ralloc);

  for (;;)
    {
      mp_limb_t  c;

      TRACE (printf ("n=%lu ni=%d rsize=%ld ralloc=%ld\n", 
                     nfull >> ni, ni, rsize, ralloc);
             mpn_trace ("rp", rp, rsize);
             mpn_trace ("tp", tp, rsize));

      ni--;
      
      /* if we want to stop on F[2n+1], break out for special last step code */
      if (ni == 0 && (nfull&1))
        break;

      /* xp,xsize = F[n]*(2F[n-1]+F[n]) = F[2n] */
      c = mpn_lshift (yp, tp, rsize, 1);    /* 2F[n-1] */
      c += mpn_add_n (yp, yp, rp, rsize);   /* (2F[n-1]+F[n]) */
      ASSERT (ralloc >= 2*rsize);
      mpn_mul_n (xp, yp, rp, rsize);
      TRACE (printf ("  c=%lu\n", c));
      if (c == 0)
        {
          /* this case is usual, the sum usually doesn't overflow */
          xsize = 2*rsize - (xp[2*rsize-1] == 0);
        }
      else 
        {
          ASSERT (ralloc >= 2*rsize+1);
          if (c == 1)
            {
              c = mpn_add_n (xp+rsize, xp+rsize, rp, rsize);
              xp[2*rsize] = c;
              xsize = 2*rsize + c;
            }
          else /* c == 2 */
            {
              c = mpn_lshift (yp, rp, rsize, 1);
              c += mpn_add_n (xp+rsize, xp+rsize, yp, rsize);
              xp[2*rsize] = c;
              xsize = 2*rsize + (c != 0);
            }
        }
      TRACE (printf ("xsize=%lu ", xsize);
             mpn_trace ("xp", xp, xsize));

      if (ni == 0)
        {
          /* stop now, wanting to finish on F[2n] */
          rp = xp;
          rsize = xsize;
          goto done;
        }

      /* yp = F[n-1]*(2F[n]-F[n-1]) = F[2n-2], in 2*rsize+1 limbs */
      ASSERT (ralloc >= 2*rsize+1);
      c = mpn_lshift (rp, rp, rsize, 1);    /* 2F[n] */
      c -= mpn_sub_n (rp, rp, tp, rsize);   /* 2F[n]-F[n-1] */
      ASSERT (c==0 || c==1);
      mpn_mul_n (yp, rp, tp, rsize);
      if (c)
        c = mpn_add_n (yp+rsize, yp+rsize, tp, rsize);
      yp[2*rsize] = c;

      TRACE (printf ("2*rsize+1=%lu yp[2*rsize]=%lu yp[2*rsize-1]=%lu\n",
                     2*rsize+1, yp[2*rsize], yp[2*rsize-1]);
             mpn_trace ("yp", yp, 2*rsize+1));


      /* When xsize < 2*rsize+1, yp[] should have zeros in the locations
         above xsize because F[2n-2] < F[2n]. */
      ASSERT (xsize > 2*rsize   || yp[2*rsize]   == 0);
      ASSERT (xsize > 2*rsize-1 || yp[2*rsize-1] == 0);
      rsize = xsize;

      /* At this point xp=F[2n], yp=F[2n-2], both rsize limbs */

      if (nfull & (1L << ni))
        {
          TRACE (printf ("  1 bit\n"));

          /* F[2n+1] = 2*F[2n]-F[2n-2] */
          c = mpn_lshift (rp, xp, rsize, 1);
          c -= mpn_sub_n (rp, rp, yp, rsize);
          ASSERT (c==0 || c==1);
          ASSERT (ralloc >= rsize+1);
          rp[rsize] = c;
          xp[rsize] = 0;
          rsize += (c != 0);
          MP_PTR_SWAP (tp, xp);   /* F[2n] */
        }
      else
        {
          TRACE (printf ("  0 bit\n"));

          ASSERT_NOCARRY (mpn_sub_n (yp, xp, yp, rsize));  /* F[2n-1] */
          MP_PTR_SWAP (rp, xp);    /* F[2n] */
          MP_PTR_SWAP (tp, yp);
        }
    }

  /* Here rp=F[n], tp=F[n-1] in rsize limbs.
     Want rp = F[n]^2 + (F[n]+F[n-1])^2 = F[2n+1] */
  {
    mp_size_t  ysize, tsize;
    mp_limb_t  c;

    ASSERT (ralloc >= 2*rsize);
    mpn_sqr_n (yp, rp, rsize);           /* F[n]^2 */
    ysize = 2*rsize - (yp[2*rsize-1] == 0);

    c = mpn_add_n (tp, tp, rp, rsize);   /* F[n]+F[n-1] */
    tp[rsize] = c;
    tsize = rsize + (c != 0);

    ASSERT (ralloc >= 2*tsize);
    mpn_sqr_n (rp, tp, tsize);           /* (F[n]+F[n-1])^2 */
    rsize = 2*tsize - (rp[2*tsize-1] == 0);

    ASSERT (ralloc >= rsize+1);
    c = mpn_add (rp, rp, rsize, yp, ysize);
    rp[rsize] = c;
    rsize += (c != 0);          
  }

 done:
  TRACE (printf ("done\n");
         mpn_trace ("rp", rp, rsize));
     
  if (rp != rp_orig)
    MPN_COPY (rp_orig, rp, rsize);
  SIZ(r) = rsize;

  ASSERT (rp_orig[rsize-1] != 0);
  ASSERT (rsize <= ralloc);

  TMP_FREE (marker);
}




/* ------------------------------------------------------------------------- */

#if GENERATE_FIB_TABLE
/* Generate the tables of fibonacci data.  This doesn't depend on the limb
   size of the host, and doesn't need mpz_fib_ui working. */

void
generate (int bpml)
{
  int    n;
  mpz_t  fn, fn1, hi, lo;
  unsigned long  bytes = 0;

  mpz_init (hi);
  mpz_init (lo);

  printf ("#if BITS_PER_MP_LIMB == %d\n", bpml);
  printf ("const mp_limb_t table1[] = {\n");

  /* at n==0 */
  mpz_init_set_ui (fn1, 1);  /* F[n-1] */
  mpz_init_set_ui (fn,  0);  /* F[n]   */

  for (n = 0; mpz_sizeinbase(fn,2) <= 2*bpml; n++)
    {
      if (mpz_sizeinbase(fn,2) > bpml && mpz_sizeinbase(fn1,2) <= bpml)
        {
          printf ("};\n");
          printf ("const mp_limb_t table2[][2] = {\n");
        }

      if (mpz_sizeinbase(fn,2) <= bpml)
        {
          printf (" CNST_LIMB (");
          mpz_out_str (stdout, 10, fn);
          printf("),  /* %d */\n", n);
          bytes += bpml/8;
        }
      else
        {
          mpz_fdiv_q_2exp (hi, fn, bpml);
          mpz_fdiv_r_2exp (lo, fn, bpml);

          printf (" {CNST_LIMB(");
          mpz_out_str (stdout, 10, lo);
          printf("), CNST_LIMB(");
          mpz_out_str (stdout, 10, hi);
          printf(")}, /* %d */\n", n);
          bytes += 2 * bpml/8;
        }

      mpz_add (fn1, fn1, fn);  /* F[n+1] = F[n] + F[n-1] */
      mpz_swap (fn1, fn);
    }
  printf ("};\n");
  printf ("/* total %lu bytes if BITS_PER_LIMB==%d */\n", bytes, bpml);
  printf ("#endif\n");
  printf ("\n");
}

int
main (void)
{
  generate (32);
  generate (64);

  return 0;
}

#endif
