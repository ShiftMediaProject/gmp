dnl  SPARC T3/T4/T5 mpn_invert_limb.

dnl  Contributed to the GNU project by Torbjörn Granlund.

dnl  Copyright 2013 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C                  cycles/limb
C UltraSPARC T3:	 ?
C UltraSPARC T4/T5:	 ?

C INPUT PARAMETERS
define(`d',  `%o0')

ASM_START()
	REGISTER(%g2,#scratch)
	REGISTER(%g3,#scratch)
PROLOGUE(mpn_invert_limb)
	srlx	d, 55, %g1
	add	%g1, %g1, %g1

ifdef(`PIC',`
	rd	%pc, %g3
	sethi	%hi(_GLOBAL_OFFSET_TABLE_+4), %g4
	add	%g4, %lo(_GLOBAL_OFFSET_TABLE_+8), %g4
	add	%g3, %g4, %g4
	sethi	%hi(approx_tab-512), %g2
	or	%g2, %lo(approx_tab-512), %g2
	ldx	[%g4+%g2], %g2
',`
	sethi	%hh(approx_tab-512), %g3
	or	%g3, %hm(approx_tab-512), %g3
	sllx	%g3, 32, %g3
	sethi	%lm(approx_tab-512), %g2
	add	%g3, %g2, %g3
	or	%g3, %lo(approx_tab-512), %g2
')
	lduh	[%g2+%g1], %g3
	srlx	d, 24, %g4
	add	%g4, 1, %g4
	sllx	%g3, 11, %g2
	add	%g2, -1, %g2
	mulx	%g3, %g3, %g3
	mulx	%g3, %g4, %g3
	srlx	%g3, 40, %g3
	sub	%g2, %g3, %g2
	sllx	%g2, 60, %g1
	mulx	%g2, %g2, %g3
	mulx	%g3, %g4, %g4
	sub	%g1, %g4, %g1
	srlx	%g1, 47, %g1
	sllx	%g2, 13, %g2
	add	%g1, %g2, %g1
	and	d, 1, %g2
	srlx	%g1, 1, %g4
	sub	%g0, %g2, %g3
	and	%g4, %g3, %g3
	srlx	d, 1, %g4
	add	%g4, %g2, %g2
	mulx	%g1, %g2, %g2
	sub	%g3, %g2, %g2
	umulxhi(%g1, %g2, %g2)
	srlx	%g2, 1, %g2
	sllx	%g1, 31, %g1
	add	%g2, %g1, %g1
	mulx	%g1, d, %g3
	umulxhi(d, %g1, %g4)
	addcc	%g3, d, %g0
	addxc(	%g4, d, %o0)
	jmp	%o7+8
	 sub	%g1, %o0, %o0
EPILOGUE()

	RODATA
	ALIGN(2)
	TYPE(	approx_tab, object)
	SIZE(	approx_tab, 512)
approx_tab:
	.half	2045,2037,2029,2021,2013,2005,1998,1990
	.half	1983,1975,1968,1960,1953,1946,1938,1931
	.half	1924,1917,1910,1903,1896,1889,1883,1876
	.half	1869,1863,1856,1849,1843,1836,1830,1824
	.half	1817,1811,1805,1799,1792,1786,1780,1774
	.half	1768,1762,1756,1750,1745,1739,1733,1727
	.half	1722,1716,1710,1705,1699,1694,1688,1683
	.half	1677,1672,1667,1661,1656,1651,1646,1641
	.half	1636,1630,1625,1620,1615,1610,1605,1600
	.half	1596,1591,1586,1581,1576,1572,1567,1562
	.half	1558,1553,1548,1544,1539,1535,1530,1526
	.half	1521,1517,1513,1508,1504,1500,1495,1491
	.half	1487,1483,1478,1474,1470,1466,1462,1458
	.half	1454,1450,1446,1442,1438,1434,1430,1426
	.half	1422,1418,1414,1411,1407,1403,1399,1396
	.half	1392,1388,1384,1381,1377,1374,1370,1366
	.half	1363,1359,1356,1352,1349,1345,1342,1338
	.half	1335,1332,1328,1325,1322,1318,1315,1312
	.half	1308,1305,1302,1299,1295,1292,1289,1286
	.half	1283,1280,1276,1273,1270,1267,1264,1261
	.half	1258,1255,1252,1249,1246,1243,1240,1237
	.half	1234,1231,1228,1226,1223,1220,1217,1214
	.half	1211,1209,1206,1203,1200,1197,1195,1192
	.half	1189,1187,1184,1181,1179,1176,1173,1171
	.half	1168,1165,1163,1160,1158,1155,1153,1150
	.half	1148,1145,1143,1140,1138,1135,1133,1130
	.half	1128,1125,1123,1121,1118,1116,1113,1111
	.half	1109,1106,1104,1102,1099,1097,1095,1092
	.half	1090,1088,1086,1083,1081,1079,1077,1074
	.half	1072,1070,1068,1066,1064,1061,1059,1057
	.half	1055,1053,1051,1049,1047,1044,1042,1040
	.half	1038,1036,1034,1032,1030,1028,1026,1024
