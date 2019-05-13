// Copyright (C) 2018 Michał Šrajer <michal@post.pl>
// This software is licensed under (one of your choice):
// - MIT
// - Apache-2.0
// - LGPL-2.1
// See LICENSE-*.txt for details.

#pragma once
#include <cassert>
#include "cycle.h"
#define Q(label, cmp1, cmp2, label1, label2) label: if (compare(cmp1, cmp2)) goto label1; else goto label2;
#define R return;
template<typename RandomAccessIterator, typename Compare>
void dtsort4(const RandomAccessIterator& first, const RandomAccessIterator& last, const Compare& compare) {
assert(last-first==4);
auto &a = *(first+0);
auto &b = *(first+1);
auto &c = *(first+2);
auto &d = *(first+3);
Q(q1,b,a,q2,q3)
Q(q2,d,c,q4,q5)
Q(q3,d,c,q6,q7)
Q(q4,c,a,q8,q9)
Q(q5,c,b,q10,q11)
Q(q6,c,b,q12,q13)
Q(q7,c,a,q14,q15)
Q(q8,d,b,q16,s17)
Q(q9,d,a,q18,s19)
Q(q10,d,a,q20,s21)
Q(q11,c,a,q22,s23)
Q(q12,c,a,s24,q25)
Q(q13,d,a,s26,q27)
Q(q14,d,a,s28,q29)
Q(q15,c,b,q30,s31)
Q(q16,c,b,s32,s33)
s17:X(a,d,b);/*perm=3021*/R
Q(q18,d,b,s36,s37)
s19:X(a,b);X(c,d);/*perm=1032*/R
Q(q20,d,b,s40,s41)
s21:X(a,c);/*perm=2103*/R
Q(q22,d,a,s44,s45)
s23:X(a,b);/*perm=1023*/R
s24:X(a,c,b,d);/*perm=2310*/R
Q(q25,d,a,s50,s51)
s26:X(a,b,c,d);/*perm=1230*/R
Q(q27,d,b,s54,s55)
s28:X(a,c);X(b,d);/*perm=2301*/R
Q(q29,d,b,s58,s59)
Q(q30,d,b,s60,s61)
s31:/*perm=0123*/R
s32:X(a,d);X(b,c);/*perm=3210*/R
s33:X(a,d);/*perm=3120*/R
s36:X(a,c,d);/*perm=2130*/R
s37:X(a,c,d,b);/*perm=2031*/R
s40:X(a,d,b,c);/*perm=3201*/R
s41:X(a,d,c);/*perm=3102*/R
s44:X(a,d,c,b);/*perm=3012*/R
s45:X(a,c,b);/*perm=2013*/R
s50:X(a,b,d);/*perm=1320*/R
s51:X(b,d);/*perm=0321*/R
s54:X(b,c,d);/*perm=0231*/R
s55:X(c,d);/*perm=0132*/R
s58:X(a,b,d,c);/*perm=1302*/R
s59:X(a,b,c);/*perm=1203*/R
s60:X(b,d,c);/*perm=0312*/R
s61:X(b,c);/*perm=0213*/R
}
#undef Q
#undef R
