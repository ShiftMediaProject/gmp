/* mpfrxx.h -- C++ class wrapper for MPFR.

Copyright 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#ifndef __GMPFR_PLUSPLUS__
#define __GMPFR_PLUSPLUS__

#include <iostream>
#include <string>
#include <gmp.h>
#include <mpfr.h>
#include "gmpxx.h"

template <class T, class U>
void __gmp_set_expr(mpfr_ptr, const __gmp_expr<T, U> &);

class __gmpfr_value { };

const int mpfr_default_base = 10;

// class wrapper for mpfr_t

#define __GMPFRR_DECLARE_COMPOUND_OPERATOR(fun)                             \
  template <class T, class U>                                               \
  __gmp_expr<__gmpfr_value, __gmpfr_value> & fun(const __gmp_expr<T, U> &);

#define __GMPFRN_DECLARE_COMPOUND_OPERATOR(fun) \
  __gmp_expr & fun(bool);                       \
  __gmp_expr & fun(signed char);                \
  __gmp_expr & fun(unsigned char);              \
  __gmp_expr & fun(signed int);                 \
  __gmp_expr & fun(unsigned int);               \
  __gmp_expr & fun(signed short int);           \
  __gmp_expr & fun(unsigned short int);         \
  __gmp_expr & fun(signed long int);            \
  __gmp_expr & fun(unsigned long int);          \
  __gmp_expr & fun(float);                      \
  __gmp_expr & fun(double);                     \
  __gmp_expr & fun(long double);

#define __GMPFR_DECLARE_COMPOUND_OPERATOR(fun) \
__GMPFRR_DECLARE_COMPOUND_OPERATOR(fun)        \
__GMPFRN_DECLARE_COMPOUND_OPERATOR(fun)

#define __GMPFR_DECLARE_COMPOUND_OPERATOR_UI(fun) \
  __gmp_expr & fun(unsigned long int);

#define __GMPFR_DECLARE_INCREMENT_OPERATOR(fun) \
  inline __gmp_expr & fun();                    \
  inline __gmp_expr fun(int);

template <>
class __gmp_expr<__gmpfr_value, __gmpfr_value>
{
private:
  mpfr_t mp;
public:
  // size information
  unsigned long int precision() const { return mpfr_get_prec(mp); }

  // constructors and destructor
  __gmp_expr() { mpfr_init(mp); }

  __gmp_expr(const __gmp_expr &f)
  {
    mpfr_init2(mp, f.precision());
    mpfr_set(mp, f.mp, __gmp_default_rounding_mode);
  }
  __gmp_expr(const __gmp_expr &f, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set(mp, f.mp, mode); }
  template <class T, class U>
  __gmp_expr(const __gmp_expr<T, U> &expr)
  { mpfr_init2(mp, expr.precision()); __gmp_set_expr(mp, expr); }
  template <class T, class U>
  __gmp_expr(const __gmp_expr<T, U> &expr, unsigned long int precision)
  { mpfr_init2(mp, precision); __gmp_set_expr(mp, expr); }

  __gmp_expr(bool b)
  { mpfr_init(mp); mpfr_set_ui(mp, b, __gmp_default_rounding_mode); }
  __gmp_expr(bool b, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_ui(mp, b, mode); }

  __gmp_expr(signed char c)
  { mpfr_init(mp); mpfr_set_si(mp, c, __gmp_default_rounding_mode); }
  __gmp_expr(signed char c, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_si(mp, c, mode); }
  __gmp_expr(unsigned char c)
  { mpfr_init(mp); mpfr_set_ui(mp, c, __gmp_default_rounding_mode); }
  __gmp_expr(unsigned char c, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_ui(mp, c, mode); }

  __gmp_expr(signed int i)
  { mpfr_init(mp); mpfr_set_si(mp, i, __gmp_default_rounding_mode); }
  __gmp_expr(signed int i, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_si(mp, i, mode); }
  __gmp_expr(unsigned int i)
  { mpfr_init(mp); mpfr_set_ui(mp, i, __gmp_default_rounding_mode); }
  __gmp_expr(unsigned int i, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_ui(mp, i, mode); }

  __gmp_expr(signed short int s)
  { mpfr_init(mp); mpfr_set_si(mp, s, __gmp_default_rounding_mode); }
  __gmp_expr(signed short int s, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_si(mp, s, mode); }
  __gmp_expr(unsigned short int s)
  { mpfr_init(mp); mpfr_set_ui(mp, s, __gmp_default_rounding_mode); }
  __gmp_expr(unsigned short int s, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_ui(mp, s, mode); }

  __gmp_expr(signed long int l)
  { mpfr_init(mp); mpfr_set_si(mp, l, __gmp_default_rounding_mode); }
  __gmp_expr(signed long int l, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_si(mp, l, mode); }
  __gmp_expr(unsigned long int l)
  { mpfr_init(mp); mpfr_set_ui(mp, l, __gmp_default_rounding_mode); }
  __gmp_expr(unsigned long int l, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_ui(mp, l, mode); }

  __gmp_expr(float f)
  { mpfr_init(mp); mpfr_set_d(mp, f, __gmp_default_rounding_mode); }
  __gmp_expr(float f, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_d(mp, f, mode); }
  __gmp_expr(double d)
  { mpfr_init(mp); mpfr_set_d(mp, d, __gmp_default_rounding_mode); }
  __gmp_expr(double d, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_d(mp, d, mode); }
  /*
  __gmp_expr(long double ld)
  { mpfr_init(mp); mpfr_set_d(mp, ld, __gmp_default_rounding_mode); }
  __gmp_expr(long double ld, unsigned long int precision,
	     char mode = __gmp_default_rounding_mode)
  { mpfr_init2(mp, precision); mpfr_set_d(mp, ld, mode); }
  */

  /*
  explicit __gmp_expr(const char *s)
  { mpfr_init_set_str(mp, s, mpfr_default_base); }
  __gmp_expr(const char *s, int base) { mpfr_init_set_str(mp, s, base); }
  explicit __gmp_expr(const string &s)
  { mpfr_init_set_str(mp, s.c_str(), mpfr_default_base); }
  __gmp_expr(const string &s, int base)
  { mpfr_init_set_str(mp, s.c_str(), base); }
  */

  explicit __gmp_expr(mpfr_srcptr f)
  {
    mpfr_init2(mp, mpfr_get_prec(f));
    mpfr_set(mp, f, __gmp_default_rounding_mode);
  }
  explicit __gmp_expr(mpfr_srcptr f, unsigned long int precision)
  {
    mpfr_init2(mp, precision);
    mpfr_set(mp, f, __gmp_default_rounding_mode);
  }

  ~__gmp_expr() { mpfr_clear(mp); }

  // assignment operators
  __gmp_expr & operator=(const __gmp_expr &f)
  { mpfr_set(mp, f.mp, __gmp_default_rounding_mode); return *this; }
  template <class T, class U>
  __gmp_expr<__gmpfr_value, __gmpfr_value> & operator=
  (const __gmp_expr<T, U> &expr)
  { __gmp_set_expr(mp, expr); return *this; }

  __gmp_expr & operator=(bool b)
  { mpfr_set_ui(mp, b, __gmp_default_rounding_mode); return *this; }

  __gmp_expr & operator=(signed char c)
  { mpfr_set_si(mp, c, __gmp_default_rounding_mode); return *this; }
  __gmp_expr & operator=(unsigned char c)
  { mpfr_set_ui(mp, c, __gmp_default_rounding_mode); return *this; }

  __gmp_expr & operator=(signed int i)
  { mpfr_set_si(mp, i, __gmp_default_rounding_mode); return *this; }
  __gmp_expr & operator=(unsigned int i)
  { mpfr_set_ui(mp, i, __gmp_default_rounding_mode); return *this; }

  __gmp_expr & operator=(signed short int s)
  { mpfr_set_si(mp, s, __gmp_default_rounding_mode); return *this; }
  __gmp_expr & operator=(unsigned short int s)
  { mpfr_set_ui(mp, s, __gmp_default_rounding_mode); return *this; }

  __gmp_expr & operator=(signed long int l)
  { mpfr_set_si(mp, l, __gmp_default_rounding_mode); return *this; }
  __gmp_expr & operator=(unsigned long int l)
  { mpfr_set_ui(mp, l, __gmp_default_rounding_mode); return *this; }

  __gmp_expr & operator=(float f)
  { mpfr_set_d(mp, f, __gmp_default_rounding_mode); return *this; }
  __gmp_expr & operator=(double d)
  { mpfr_set_d(mp, d, __gmp_default_rounding_mode); return *this; }
  /*
  __gmp_expr & operator=(long double ld)
  { mpfr_set_d(mp, ld, __gmp_default_rounding_mode); return *this; }
  */

  /*
  __gmp_expr & operator=(const char *s)
  { mpfr_set_str(mp, s, mpfr_default_base); return *this; }
  __gmp_expr & operator=(const string &s)
  { mpfr_set_str(mp, s.c_str(), mpfr_default_base); return *this; }

  // string input/output functions
  void set_str_base(const string &s, int base)
  { mpfr_set_str(mp, s.c_str(), base); }
  */
  string get_str_base(int base) const
  {
    mp_exp_t expo;
    __gmp_alloc_cstring temp(mpfr_get_str(0, &expo, base, 0, mp,
					  __gmp_default_rounding_mode));

    // cancel terminating zeros
    for (size_t i=strlen(temp.str)-1; temp.str[i]=='0' && i>0; --i)
      temp.str[i] = '\0';

    if (*temp.str == '-')
      return "-0." + string(temp.str+1) + "e"; // the exponent?
    else
      return "0." + string(temp.str) + "e"; // the exponent?
  }

  // conversion functions
  mpfr_srcptr get_mpfr_t() const { return mp; }
  mpfr_ptr get_mpfr_t() { return mp; }

  // signed long get_si() const { return mpfr_get_si(mp); }
  // unsigned long get_ui() const { return mpfr_get_ui(mp); }
  double get_d() const { return mpfr_get_d(mp); } // should be long double

  // compound assignments
  __GMPFR_DECLARE_COMPOUND_OPERATOR(operator+=)
  __GMPFR_DECLARE_COMPOUND_OPERATOR(operator-=)
  __GMPFR_DECLARE_COMPOUND_OPERATOR(operator*=)
  __GMPFR_DECLARE_COMPOUND_OPERATOR(operator/=)

  __GMPFR_DECLARE_COMPOUND_OPERATOR_UI(operator<<=)
  __GMPFR_DECLARE_COMPOUND_OPERATOR_UI(operator>>=)

  __GMPFR_DECLARE_INCREMENT_OPERATOR(operator++)
  __GMPFR_DECLARE_INCREMENT_OPERATOR(operator--)
};

typedef __gmp_expr<__gmpfr_value, __gmpfr_value> mpfr_class;

// unary expressions

template <class Op>
class __gmp_expr<__gmpfr_value, __gmp_unary_expr<mpfr_class, Op> >
{
private:
  __gmp_unary_expr<mpfr_class, Op> expr;
public:
  __gmp_expr(const mpfr_class &val) : expr(val) { }
  void eval(mpfr_ptr f, unsigned long int) const
  { Op::eval(f, expr.val.get_mpfr_t(), __gmp_default_rounding_mode); }
  unsigned long int precision() const
  { return mpfr_get_prec(expr.val.get_mpfr_t()); }
};

template <class T, class U, class Op>
class __gmp_expr<__gmpfr_value, __gmp_unary_expr<__gmp_expr<T, U>, Op> >
{
private:
  __gmp_unary_expr<__gmp_expr<T, U>, Op> expr;
public:
  __gmp_expr(const __gmp_expr<T, U> &val) : expr(val) { }
  void eval(mpfr_ptr f, unsigned long int precision) const
  {
    mpfr_class temp(expr.val, precision);
    Op::eval(f, temp.get_mpfr_t(), __gmp_default_rounding_mode);
  }
  unsigned long int precision() const { return expr.val.precision(); }
};

// binary expressions

template <class Op>
class __gmp_expr
<__gmpfr_value, __gmp_binary_expr<mpfr_class, mpfr_class, Op> >
{
private:
  __gmp_binary_expr<mpfr_class, mpfr_class, Op> expr;
public:
  __gmp_expr(const mpfr_class &val1, const mpfr_class &val2)
    : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int) const
  { Op::eval(f, expr.val1.get_mpfr_t(), expr.val2.get_mpfr_t(),
	     __gmp_default_rounding_mode); }
  unsigned long int precision() const
  {
    unsigned long int prec1 = expr.val1.precision(),
      prec2 = expr.val2.precision();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class Op>
class __gmp_expr<__gmpfr_value, __gmp_binary_expr<mpfr_class, T, Op> >
{
private:
  __gmp_binary_expr<mpfr_class, T, Op> expr;
public:
  __gmp_expr(const mpfr_class &val1, T val2) : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int) const
  { Op::eval(f, expr.val1.get_mpfr_t(), expr.val2,
	     __gmp_default_rounding_mode); }
  unsigned long int precision() const
  {
    unsigned long int prec1 = expr.val1.precision(),
      prec2 = mpf_get_default_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class Op>
class __gmp_expr<__gmpfr_value, __gmp_binary_expr<T, mpfr_class, Op> >
{
private:
  __gmp_binary_expr<T, mpfr_class, Op> expr;
public:
  __gmp_expr(T val1, const mpfr_class &val2) : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int) const
  { Op::eval(f, expr.val1, expr.val2.get_mpfr_t(),
	     __gmp_default_rounding_mode); }
  unsigned long int precision() const
  {
    unsigned long int prec1 = mpf_get_default_prec(),
      prec2 = expr.val2.precision();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class Op>
class __gmp_expr
<__gmpfr_value, __gmp_binary_expr<mpfr_class, __gmp_expr<T, U>, Op> >
{
private:
  __gmp_binary_expr<mpfr_class, __gmp_expr<T, U>, Op> expr;
public:
  __gmp_expr(const mpfr_class &val1, const __gmp_expr<T, U> &val2)
    : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int precision) const
  {
    mpfr_class temp(expr.val2, precision);
    Op::eval(f, expr.val1.get_mpfr_t(), temp.get_mpfr_t(),
	     __gmp_default_rounding_mode);
  }
  unsigned long int precision() const
  {
    unsigned long int prec1 = expr.val1.precision(),
      prec2 = expr.val2.precision();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class Op>
class __gmp_expr
<__gmpfr_value, __gmp_binary_expr<__gmp_expr<T, U>, mpfr_class, Op> >
{
private:
  __gmp_binary_expr<__gmp_expr<T, U>, mpfr_class, Op> expr;
public:
  __gmp_expr(const __gmp_expr<T, U> &val1, const mpfr_class &val2)
    : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int precision) const
  {
    mpfr_class temp(expr.val1, precision);
    Op::eval(f, temp.get_mpfr_t(), expr.val2.get_mpfr_t(),
	     __gmp_default_rounding_mode);
  }
  unsigned long int precision() const
  {
    unsigned long int prec1 = expr.val1.precision(),
      prec2 = expr.val2.precision();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class V, class Op>
class __gmp_expr<__gmpfr_value, __gmp_binary_expr<__gmp_expr<T, U>, V, Op> >
{
private:
  __gmp_binary_expr<__gmp_expr<T, U>, V, Op> expr;
public:
  __gmp_expr(const __gmp_expr<T, U> &val1, V val2) : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int precision) const
  {
    mpfr_class temp(expr.val1, precision);
    Op::eval(f, temp.get_mpfr_t(), expr.val2, __gmp_default_rounding_mode);
  }
  unsigned long int precision() const
  {
    unsigned long int prec1 = expr.val1.precision(),
      prec2 = mpf_get_default_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class V, class Op>
class __gmp_expr<__gmpfr_value, __gmp_binary_expr<T, __gmp_expr<U, V>, Op> >
{
private:
  __gmp_binary_expr<T, __gmp_expr<U, V>, Op> expr;
public:
  __gmp_expr(T val1, const __gmp_expr<U, V> &val2) : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int precision) const
  {
    mpfr_class temp(expr.val2, precision);
    Op::eval(f, expr.val1, temp.get_mpfr_t(), __gmp_default_rounding_mode);
  }
  unsigned long int precision() const
  {
    unsigned long int prec1 = mpf_get_default_prec(),
      prec2 = expr.val2.precision();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class V, class W, class Op>
class __gmp_expr
<__gmpfr_value, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op> >
{
private:
  __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op> expr;
public:
  __gmp_expr(const __gmp_expr<T, U> &val1, const __gmp_expr<V, W> &val2)
    : expr(val1, val2) { }
  void eval(mpfr_ptr f, unsigned long int precision) const
  {
    mpfr_class temp1(expr.val1, precision), temp2(expr.val2, precision);
    Op::eval(f, temp1.get_mpfr_t(), temp2.get_mpfr_t(),
	     __gmp_default_rounding_mode);
  }
  unsigned long int precision() const
  {
    unsigned long int prec1 = expr.val1.precision(),
      prec2 = expr.val2.precision();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

// type conversions

class __gmpfr_temp
{
private:
  mpfr_srcptr mp;
  bool is_temp;
  mpfr_t temp;

  __gmpfr_temp();
  __gmpfr_temp(const __gmpfr_temp &);
  void operator=(const __gmpfr_temp &);
public:
  __gmpfr_temp(const mpfr_class &f) : mp(f.get_mpfr_t()), is_temp(false) { }
  __gmpfr_temp(const mpfr_class &f, unsigned long int)
    : mp(f.get_mpfr_t()), is_temp(false) { }
  template <class T, class U>
  __gmpfr_temp(const __gmp_expr<T, U> &expr)
  {
    mpfr_init2(temp, expr.precision());
    __gmp_set_expr(temp, expr);
    mp = temp;
    is_temp = true;
  }
  template <class T, class U>
  __gmpfr_temp(const __gmp_expr<T, U> &expr, unsigned long int precision)
  {
    mpfr_init2(temp, precision);
    __gmp_set_expr(temp, expr);
    mp = temp;
    is_temp = true;
  }
  ~__gmpfr_temp() { if (is_temp) mpfr_clear(temp); }

  mpfr_srcptr get_mp() const { return mp; }
};

template <>
struct __gmp_resolve_expr<__gmpfr_value, __gmpfr_value>
{
  typedef __gmpfr_value value_type;
  typedef __gmpfr_temp temp_type;
};

template <>
inline void __gmp_set_expr(mpfr_ptr f, const mpfr_class &g)
{
  mpfr_set(f, g.get_mpfr_t(), __gmp_default_rounding_mode);
}

template <class T>
inline void __gmp_set_expr
(mpfr_ptr f, const __gmp_expr<__gmpfr_value, T> &expr)
{
  expr.eval(f, mpfr_get_prec(f));
}

// functions

inline ostream & operator<<(ostream &o, const mpfr_class &f)
{
  // handle the case f == 0
  if (mpfr_cmp_ui(f.get_mpfr_t(), 0) == 0)
    return o << "0";

  // else
  mp_exp_t expo;
  size_t o_prec = o.precision();
  __gmp_alloc_cstring temp
    (mpfr_get_str(0, &expo, mpfr_default_base, o_prec,
		  f.get_mpfr_t(), __gmp_default_rounding_mode));
  char *temp2 = temp.str;
  size_t length = strlen(temp2)+1;
  --expo;

  // cancel terminating zeros
  for (int i=length-2; temp2[i]=='0' && i>=0; --i)
    temp2[i] = '\0';

  // print trailing minus if number is negative
  if (temp2[0] == '-')
    {
      o << "-";
      ++temp2;
    }

  if (expo < -4 || expo > 5)
    {
      // scientific notation
      o << temp2[0] << "." << temp2+1 << "e";
      if (expo >= 10)
	o << "+" << expo;
      else if (expo > 0)
	o << "+0" << expo;
      else if (expo > -10)
	o << "-0" << -expo;
      else
	o << expo;
    }
  else if (expo < 0)
    {
      // number less than 1
      o << "0.";
      for (int i=-1; i>expo; --i)
	o << "0";
      o << temp2;
    }
  else
    {
      // number greater or equal to 1
      int i;
      for (i=0; i<=expo && *temp2!='\0'; ++i, ++temp2)
	o << *temp2;
      for (; i<=expo; ++i)
	o << "0";
      if (strlen(temp2) > 0)
	o << "." << temp2;
    }

  return o;
}

/*
inline ostream & operator<<(ostream &o, const mpfr_class &f)
{
  mp_exp_t expo;
  __gmp_alloc_cstring temp
    (mpfr_get_str(0, &expo, mpfr_default_base, o.precision(),
		  f.get_mpfr_t(), __gmp_default_rounding_mode));

  // cancel terminating zeros
  for (size_t i=strlen(temp.str)-1; temp.str[i]=='0' && i>0; --i)
    temp.str[i] = '\0';

  if (*temp.str == '-')
    return o << "-0." << temp.str+1 << "e" << expo;
  else
    return o << "0." << temp.str << "e" << expo;
}
*/

template <class T>
inline ostream & operator<<
(ostream &o, const __gmp_expr<__gmpfr_value, T> &expr)
{
  mpfr_class temp(expr);
  return o << temp;
}

inline istream & operator>>(istream &i, mpfr_class &f)
{
  mpf_t temp;
  mpf_init2(temp, f.precision());
  i >> temp;
  mpfr_set_f(f.get_mpfr_t(), temp, __gmp_default_rounding_mode);
  mpf_clear(temp);
  return i;
}

#define __GMPFRR_DEFINE_COMPOUND_OPERATOR(fun, eval_fun)              \
                                                                      \
template <class T, class U>                                           \
inline mpfr_class & mpfr_class::fun(const __gmp_expr<T, U> &expr)     \
{                                                                     \
  __gmpfr_temp temp(expr, precision());                               \
  eval_fun::eval(mp, mp, temp.get_mp(), __gmp_default_rounding_mode); \
  return *this;                                                       \
}

#define __GMPFRN_DEFINE_COMPOUND_OPERATOR(fun, eval_fun)           \
                                                                   \
inline mpfr_class & mpfr_class::fun(bool b)                        \
{                                                                  \
  eval_fun::eval(mp, mp, (unsigned long int) b,                    \
                 __gmp_default_rounding_mode);                     \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(signed char c)                 \
{                                                                  \
  eval_fun::eval(mp, mp, (signed long int) c,                      \
                 __gmp_default_rounding_mode);                     \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(unsigned char c)               \
{                                                                  \
  eval_fun::eval(mp, mp, (unsigned long int) c,                    \
                 __gmp_default_rounding_mode);                     \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(signed int i)                  \
{                                                                  \
  eval_fun::eval(mp, mp, (signed long int) i,                      \
                 __gmp_default_rounding_mode);                     \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(unsigned int i)                \
{                                                                  \
  eval_fun::eval(mp, mp, (unsigned long int) i,                    \
                 __gmp_default_rounding_mode);                     \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(signed short int s)            \
{                                                                  \
  eval_fun::eval(mp, mp, (signed long int) s,                      \
                 __gmp_default_rounding_mode);                     \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(unsigned short int s)          \
{                                                                  \
  eval_fun::eval(mp, mp, (unsigned long int) s,                    \
                 __gmp_default_rounding_mode);                     \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(signed long int l)             \
{                                                                  \
  eval_fun::eval(mp, mp, l, __gmp_default_rounding_mode);          \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(unsigned long int l)           \
{                                                                  \
  eval_fun::eval(mp, mp, l, __gmp_default_rounding_mode);          \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(float f)                       \
{                                                                  \
  eval_fun::eval(mp, mp, (double) f, __gmp_default_rounding_mode); \
  return *this;                                                    \
}                                                                  \
                                                                   \
inline mpfr_class & mpfr_class::fun(double d)                      \
{                                                                  \
  eval_fun::eval(mp, mp, d, __gmp_default_rounding_mode);          \
  return *this;                                                    \
}                                                                  \
                                                                   \
/*                                                                 \
inline mpfr_class & mpfr_class::fun(long double ld)                \
{                                                                  \
  eval_fun::eval(mp, mp, ld, __gmp_default_rounding_mode);         \
  return *this;                                                    \
} */

#define __GMPFR_DEFINE_COMPOUND_OPERATOR(fun, eval_fun) \
__GMPFRR_DEFINE_COMPOUND_OPERATOR(fun, eval_fun)        \
__GMPFRN_DEFINE_COMPOUND_OPERATOR(fun, eval_fun)

#define __GMPFR_DEFINE_COMPOUND_OPERATOR_UI(fun, eval_fun) \
                                                           \
inline mpfr_class & mpfr_class::fun(unsigned long int l)   \
{                                                          \
  eval_fun::eval(mp, mp, l, __gmp_default_rounding_mode);  \
  return *this;                                            \
}

#define __GMPFR_DEFINE_INCREMENT_OPERATOR(fun, eval_fun) \
                                                         \
inline mpfr_class & mpfr_class::fun()                    \
{                                                        \
  eval_fun::eval(mp, mp, __gmp_default_rounding_mode);   \
  return *this;                                          \
}                                                        \
                                                         \
inline mpfr_class mpfr_class::fun(int)                   \
{                                                        \
  mpfr_class temp(*this);                                \
  eval_fun::eval(mp, mp, __gmp_default_rounding_mode);   \
  return temp;                                           \
}

// define operators and functions

__GMPFR_DEFINE_COMPOUND_OPERATOR(operator+=, __gmp_binary_plus)
__GMPFR_DEFINE_COMPOUND_OPERATOR(operator-=, __gmp_binary_minus)
__GMPFR_DEFINE_COMPOUND_OPERATOR(operator*=, __gmp_binary_multiplies)
__GMPFR_DEFINE_COMPOUND_OPERATOR(operator/=, __gmp_binary_divides)

__GMPFR_DEFINE_COMPOUND_OPERATOR_UI(operator<<=, __gmp_binary_lshift)
__GMPFR_DEFINE_COMPOUND_OPERATOR_UI(operator>>=, __gmp_binary_rshift)

__GMPFR_DEFINE_INCREMENT_OPERATOR(operator++, __gmp_unary_increment)
__GMPFR_DEFINE_INCREMENT_OPERATOR(operator--, __gmp_unary_decrement)

// type conversions

template <>
struct __gmp_resolve_expr<__gmpz_value, __gmpfr_value>
{
  typedef __gmpfr_value value_type;
  typedef __gmpfr_temp temp_type;
};

template <>
struct __gmp_resolve_expr<__gmpfr_value, __gmpz_value>
{
  typedef __gmpfr_value value_type;
  typedef __gmpfr_temp temp_type;
};

template <>
struct __gmp_resolve_expr<__gmpq_value, __gmpfr_value>
{
  typedef __gmpfr_value value_type;
  typedef __gmpfr_temp temp_type;
};

template <>
struct __gmp_resolve_expr<__gmpfr_value, __gmpq_value>
{
  typedef __gmpfr_value value_type;
  typedef __gmpfr_temp temp_type;
};

template <>
struct __gmp_resolve_expr<__gmpf_value, __gmpfr_value>
{
  typedef __gmpfr_value value_type;
  typedef __gmpfr_temp temp_type;
};

template <>
struct __gmp_resolve_expr<__gmpfr_value, __gmpf_value>
{
  typedef __gmpfr_value value_type;
  typedef __gmpfr_temp temp_type;
};

/*
template <class T>
inline void __gmp_set_expr(mpz_ptr z, const mpfr_class &f)
{
  mpz_set_fr(z, f.get_mpfr_t());
}

template <class T>
inline void __gmp_set_expr
(mpz_ptr z, const __gmp_expr<__gmpfr_value, T> &expr)
{
  mpfr_class temp(expr);
  mpz_set_fr(z, temp.get_mpfr_t());
}

template <class T>
inline void __gmp_set_expr(mpq_ptr q, const mpfr_class &f)
{
  mpq_set_fr(q, f.get_mpfr_t());
}

template <class T>
inline void __gmp_set_expr
(mpq_ptr q, const __gmp_expr<__gmpfr_value, T> &expr)
{
  mpfr_class temp(expr);
  mpq_set_fr(q, temp.get_mpfr_t());
}

template <class T>
inline void __gmp_set_expr(mpf_ptr f, const mpfr_class &g)
{
  mpf_set_fr(f, g.get_mpfr_t());
}

template <class T>
inline void __gmp_set_expr
(mpf_ptr f, const __gmp_expr<__gmpfr_value, T> &expr)
{
  mpfr_class temp(expr);
  mpf_set_fr(f, temp.get_mpfr_t());
}
*/

template <class T>
inline void __gmp_set_expr(mpfr_ptr f, const mpz_class &z)
{
  mpfr_set_z(f, z.get_mpz_t(), __gmp_default_rounding_mode);
}

template <class T>
inline void __gmp_set_expr(mpfr_ptr f, const mpz_classref &z)
{
  mpfr_set_z(f, z.get_mpz_t(), __gmp_default_rounding_mode);
}

template <class T>
inline void __gmp_set_expr
(mpfr_ptr f, const __gmp_expr<__gmpz_value, T> &expr)
{
  mpz_class temp(expr);
  mpfr_set_z(f, temp.get_mpz_t(), __gmp_default_rounding_mode);
}

template <class T>
inline void __gmp_set_expr(mpfr_ptr f, const mpq_class &q)
{
  mpfr_set_q(f, q.get_mpq_t(), __gmp_default_rounding_mode);
}

template <class T>
inline void __gmp_set_expr
(mpfr_ptr f, const __gmp_expr<__gmpq_value, T> &expr)
{
  mpq_class temp(expr);
  mpfr_set_q(f, temp.get_mpq_t(), __gmp_default_rounding_mode);
}

template <class T>
inline void __gmp_set_expr(mpfr_ptr f, const mpf_class &g)
{
  mpfr_set_f(f, g.get_mpf_t(), __gmp_default_rounding_mode);
}

template <class T>
inline void __gmp_set_expr
(mpfr_ptr f, const __gmp_expr<__gmpf_value, T> &expr)
{
  mpq_class temp(expr);
  mpfr_set_f(f, temp.get_mpf_t(), __gmp_default_rounding_mode);
}

#endif
