// Copyright (C) 2018 Michał Šrajer <michal@post.pl>
// This software is licensed under (one of your choice):
// - MIT
// - Apache-2.0
// - LGPL-2.1
// See LICENSE-*.txt for details.
#pragma once
#include <iostream>

#ifdef NDEBUG
#define Debug(x)
#define Debugn(x)
#else
#define Debug(x) (std::cerr << (x))
#define Debugn(x) (std::cerr << (x) << std::endl)
#endif 
