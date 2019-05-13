// Copyright (C) 2018 Michał Šrajer <michal@post.pl>
// This software is licensed under (one of your choice):
// - MIT
// - Apache-2.0
// - LGPL-2.1
// See LICENSE-*.txt for details.

#pragma once
#include <utility>

// Used by dtsortX.h files.
// Provides ways to swap elements
// or to cycle (right shift by one element) them.

template<class T>
inline void X(T& a, T& b)
{
  T tmp(std::move(b));
  b=std::move(a);
  a=std::move(tmp);
}

template<class T>
inline void X(T& a, T& b, T& c)
{
  T tmp(std::move(c));
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}

template<class T>
inline void X(T& a, T& b, T& c, T& d)
{
  T tmp(std::move(d));
  d=std::move(c);
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}

template<class T>
inline void X(T& a, T& b, T& c, T& d, T& e)
{
  T tmp(std::move(e));
  e=std::move(d);
  d=std::move(c);
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}

template<class T>
inline void X(T& a, T& b, T& c, T& d, T& e, T& f)
{
  T tmp(std::move(f));
  f=std::move(e);
  e=std::move(d);
  d=std::move(c);
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}

template<class T>
inline void X(T& a, T& b, T& c, T& d, T& e, T& f, T& g)
{
  T tmp(std::move(g));
  g=std::move(f);
  f=std::move(e);
  e=std::move(d);
  d=std::move(c);
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}

template<class T>
inline void X(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h)
{
  T tmp(std::move(h));
  h=std::move(g);
  g=std::move(f);
  f=std::move(e);
  e=std::move(d);
  d=std::move(c);
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}
