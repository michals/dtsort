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
void dtsort3(const RandomAccessIterator& first, const RandomAccessIterator& last, const Compare& compare) {
assert(last-first==3);
auto &a = *(first+0);
auto &b = *(first+1);
auto &c = *(first+2);
Q(q1,b,a,q2,q3)
Q(q2,c,a,q4,s5)
Q(q3,c,b,q6,s7)
Q(q4,c,b,s8,s9)
s5:X(a,b);/*perm=102*/R
Q(q6,c,a,s12,s13)
s7:/*perm=012*/R
s8:X(a,c);/*perm=210*/R
s9:X(a,c,b);/*perm=201*/R
s12:X(a,b,c);/*perm=120*/R
s13:X(b,c);/*perm=021*/R
}
#undef Q
#undef R
