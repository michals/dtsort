#pragma once
#include <utility>
#include <cassert>

// Used by dtsortX.hh files.
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

template<class T>
inline void X(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i)
{
  T tmp(std::move(i));
  i=std::move(h);
  h=std::move(g);
  g=std::move(f);
  f=std::move(e);
  e=std::move(d);
  d=std::move(c);
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}

template<class T>
inline void X(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j)
{
  T tmp(std::move(j));
  j=std::move(i);
  i=std::move(h);
  h=std::move(g);
  g=std::move(f);
  f=std::move(e);
  e=std::move(d);
  d=std::move(c);
  c=std::move(b);
  b=std::move(a);
  a=std::move(tmp);
}
