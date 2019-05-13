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
void dtsort2(const RandomAccessIterator& first, const RandomAccessIterator& last, const Compare& compare) {
assert(last-first==2);
auto &a = *(first+0);
auto &b = *(first+1);
Q(q1,b,a,s2,s3)
s2:X(a,b);/*perm=10*/R
s3:/*perm=01*/R
}
#undef Q
#undef R
