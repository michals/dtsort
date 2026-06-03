#pragma once
#include <utility>
#include <cassert>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-label"
#elif defined(_MSC_VER)
#pragma warning(disable: 4102)
#endif


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

// ---------------------------------------------------------
// Common decision tree macro definitions
// ---------------------------------------------------------

#ifndef DT_ADAPTIVE
#define DT_ADAPTIVE 1
#endif

#ifndef DT_UNLIKELY
#if defined(__GNUC__) || defined(__clang__)
#define DT_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define DT_UNLIKELY(x) (x)
#endif
#endif

#ifndef DT_ALWAYS_INLINE
#if defined(__GNUC__) || defined(__clang__)
#define DT_ALWAYS_INLINE __attribute__((always_inline))
#else
#define DT_ALWAYS_INLINE
#endif
#endif

#if DT_ADAPTIVE
#define DT_RETURN_TYPE bool
#define DT_RETURN return false;
#define DT_RETURN_SORTED return true;
#else
#define DT_RETURN_TYPE void
#define DT_RETURN return;
#define DT_RETURN_SORTED return;
#endif
