# GMP perl module tests

# Copyright 2001 Free Software Foundation, Inc.
#
# This file is part of the GNU MP Library.
#
# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2.1 of the License, or (at
# your option) any later version.
#
# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.


# These tests aim to exercise the many possible combinations of operands
# etc, and to run all functions at least once, which if nothing else will
# check everything intended is in the :all list.
#
# Use the following in .emacs to match test failure messages.
#
# ;; perl "Test" module error messages
# (eval-after-load "compile"
#   '(add-to-list
#     'compilation-error-regexp-alist
#     '("^.*Failed test [0-9]+ in \\([^ ]+\\) at line \\([0-9]+\\)" 1 2)))


use strict;
use Test;

BEGIN {
  plan tests => 123,
  onfail => sub { print "there were failures\n" },
}

use GMP qw(:all);
use GMP::Mpz qw(:all);
use GMP::Mpq qw(:all);
use GMP::Mpf qw(:all);
use GMP::Rand qw(:all);

use GMP::Mpz qw(:constants);
use GMP::Mpz qw(:noconstants);
use GMP::Mpq qw(:constants);
use GMP::Mpq qw(:noconstants);
use GMP::Mpf qw(:constants);
use GMP::Mpf qw(:noconstants);

use GMP qw(version);
print '$GMP::VERSION ',$GMP::VERSION,' GMP::version() ',version(),"\n";

# force to a string, not a PVIV
sub str {
  my $s = "@_[0]" . "";
  return $s;
}

foreach my $name ('mpz', 'mpq', 'mpf') {
  print "$name\n";
  my $mpx = eval("\\&$name");

  ok (&$mpx() == 0);

  foreach my $x (0, 1, -1, 123, -123) {
    foreach my $y ($x, ""."$x", substr("$x",0), 0.0+$x,
		   mpz($x), mpq($x), mpf($x)) {
      ok (&$mpx($x) == $x);
    }
  }
  
  ok (&$mpx(substr("11",0,1)) == 1);

  ok (&$mpx(0) < 1);
  ok (&$mpx(0) > -1);

  ok (&$mpx(0) != 1);
  ok (&$mpx(0) != -1);
  ok (&$mpx(1) != 0);
  ok (&$mpx(1) != -1);
  ok (&$mpx(-1) != 0);
  ok (&$mpx(-1) != 1);

  ok (&$mpx(0) + 1 == 1);
  ok (&$mpx(-1) + 1 == 0);
  ok (1 + &$mpx(0) == 1);
  ok (1 + &$mpx(-1) == 0);

  ok (&$mpx(0) - 1 == -1);
  ok (&$mpx(1) - 1 == 0);
  ok (1 - &$mpx(0) == 1);
  ok (1 - &$mpx(1) == 0);

  ok (&$mpx(2) * 3 == 6);

  ok (&$mpx(6) / 2 == 3);
  ok (&$mpx(-6) / 2 == -3);
  ok (&$mpx(6) / -2 == -3);
  ok (&$mpx(-6) / -2 == 3);

  ok (abs(&$mpx(0)) == 0);
  ok (abs(&$mpx(1)) == 1);
  ok (abs(&$mpx(-1)) == 1);
  { my $x = &$mpx(-123); $x = abs($x); ok ($x == 123); }
  { my $x = &$mpx(-1);   $x = abs($x); ok ($x == 1);   }
  { my $x = &$mpx(0);    $x = abs($x); ok ($x == 0);   }
  { my $x = &$mpx(1);    $x = abs($x); ok ($x == 1);   }
  { my $x = &$mpx(123);  $x = abs($x); ok ($x == 123); }


  foreach my $x (-123, -1, 0, 1, 123) {
    foreach my $xv ($x, "$x", substr("$x",0), $x+0.0,
		    mpz($x), mpq($x), mpf($x)) {
      my $xm = &$mpx($xv);
      if ($xm) {
	ok ($x);
      } else {
	ok (not $x);
      }
      if (not $xm) {
	ok (not $x);
      } else {
	ok ($x);
      }
      if (! $xm) {
	ok (not $x);
      } else {
	ok ($x);
      }
    }
  }

  ok (- &$mpx(0) == 0);
  ok (- &$mpx(1) == -1);
  ok (- &$mpx(-1) == 1);

  {
    my $a = &$mpx(0); my $b = $a;
    $a = &$mpx(1);
    ok ($a == 1);
    ok ($b == 0);
  }
  {
    my $a = &$mpx(0); my $b = $a;
    $a++;
    ok ($a == 1);
    ok ($b == 0);
  }
  {
    my $a = &$mpx(0); my $b = $a;
    $a--;
    ok ($a == -1);
    ok ($b == 0);
  }
  {
    my $a = &$mpx(0); my $b = $a;
    $a++;
    ok ($a == 1);
    ok ($b == 0);
  }

  { my $a = &$mpx(0); ok ($a++ == 0); }
  { my $a = &$mpx(0); ok ($a-- == 0); }
  { my $a = &$mpx(0); ok (++$a == 1); }
  { my $a = &$mpx(0); ok (--$a == -1); }

  { my $x = &$mpx(0);    ok("$x" eq "0"); }
  { my $x = &$mpx(123);  ok("$x" eq "123"); }
  { my $x = &$mpx(-123); ok("$x" eq "-123"); }

  ok (&$mpx(0) ** 1 == 0);
  ok (&$mpx(1) ** 1 == 1);
  ok (&$mpx(2) ** 0 == 1);
  ok (&$mpx(2) ** 1 == 2);
  ok (&$mpx(2) ** 2 == 4);
  ok (&$mpx(2) ** 3 == 8);
  ok (&$mpx(2) ** 4 == 16);

  ok (&$mpx(0) ** &$mpx(1) == 0);
  ok (&$mpx(1) ** &$mpx(1) == 1);
  ok (&$mpx(2) ** &$mpx(0) == 1);
  ok (&$mpx(2) ** &$mpx(1) == 2);
  ok (&$mpx(2) ** &$mpx(2) == 4);
  ok (&$mpx(2) ** &$mpx(3) == 8);
  ok (&$mpx(2) ** &$mpx(4) == 16);

  { my $a = &$mpx(7);  $a += 1;  ok ($a == 8); }
  { my $a = &$mpx(7);  $a -= 1;  ok ($a == 6); }
  { my $a = &$mpx(7);  $a *= 3;  ok ($a == 21); }
  { my $a = &$mpx(21); $a /= 3;  ok ($a == 7); }
  { my $a = &$mpx(7);  $a <<= 1; ok ($a == 14); }
  { my $a = &$mpx(32); $a >>= 1; ok ($a == 16); }
  { my $a = &$mpx(3);  $a **= 4; ok ($a == 81); }

  ok (! &$mpx("-9999999999999999999999999999999999999999999")->fits_slong_p());
  ok (  &$mpx(-123)->fits_slong_p());
  ok (  &$mpx(0)->fits_slong_p());
  ok (  &$mpx(123)->fits_slong_p());
  ok (! &$mpx("9999999999999999999999999999999999999999999")->fits_slong_p());

  ok (&$mpx(-123)->sgn() == -1);
  ok (&$mpx(-1)  ->sgn() == -1);
  ok (&$mpx(0)   ->sgn() == 0);
  ok (&$mpx(1)   ->sgn() == 1);
  ok (&$mpx(123) ->sgn() == 1);
}

foreach my $x (-123, -1, 0, 1, 123) {
  foreach my $xv ($x, "$x", substr("$x",0), $x+0.0,
		  mpz($x), mpq($x), mpf($x)) {
    # print "x is $x\n";

    ok (GMP::get_d($xv) == $x);
    ok (GMP::get_si($xv) == $x);
    ok (GMP::integer_p($xv) == 1);
    ok (GMP::sgn($xv) == ($x <=> 0));
    
    foreach my $y (-123, -1, 0, 1, 123) {
      foreach my $yv ($y, "$y", 0.0+$y, mpz($y), mpq($y), mpf($y)) {

	# print "$x cmp $y\n";
	ok (($xv <=> $yv) == ($x <=> $y));
      }
    }
  }
}

foreach my $xpair ([-123,"-7b"], [-1,"-1"], [0,"0"], [1,"1"], [123,"7b"]) {
  my $x = $$xpair[0];
  my $xhex = $$xpair[1];

  foreach my $xv ($x, "$x", substr("$x",0), $x+0.0,
		  mpz($x), mpq($x), mpf($x)) {

    ok (GMP::get_str($xv) eq "$x");
    ok (GMP::get_str($xv,10) eq "$x");

    ok (GMP::get_str($xv,16) == $xhex);
    $xhex =~ tr [a-z] [A-Z];
    ok (GMP::get_str($xv,-16) == $xhex);
  }
}

ok (GMP::get_str(mpq(5/8)) eq "5/8");
ok (GMP::get_str(mpq(-5/8)) eq "-5/8");
ok (GMP::get_str(mpq(255/256),16) eq "ff/100");
ok (GMP::get_str(mpq(255/256),-16) eq "FF/100");
ok (GMP::get_str(mpq(-255/256),16) eq "-ff/100");
ok (GMP::get_str(mpq(-255/256),-16) eq "-FF/100");

ok (GMP::get_str(1.5) eq "1.5");
ok (GMP::get_str(-1.5) eq "-1.5");
ok (GMP::get_str(1.5,16) eq "1.8");
ok (GMP::get_str(-1.5,16) eq "-1.8");

ok (GMP::get_str(mpf(1.5)) eq "1.5");
ok (GMP::get_str(mpf(-1.5)) eq "-1.5");
ok (GMP::get_str(mpf(1.5),16) eq "1.8");
ok (GMP::get_str(mpf(-1.5),16) eq "-1.8");

ok (GMP::get_str(1.625,-16) eq "1.A");
ok (GMP::get_str(mpf(0x1000),16) eq '1@3');

ok (  GMP::integer_p (mpq(1)));
ok (! GMP::integer_p (mpq(1,2)));
ok (  GMP::integer_p (mpf(1.0)));
ok (! GMP::integer_p (mpf(1.5)));


ok (mpz(-8) % 3 == -2);
ok (mpz(-7) % 3 == -1);
ok (mpz(-6) % 3 == 0);
ok (mpz(6) % 3 == 0);
ok (mpz(7) % 3 == 1);
ok (mpz(8) % 3 == 2);

{ my $a = mpz(24); $a %= 7; ok ($a == 3); }

ok ((mpz(3) & 1) == 1);
ok ((mpz(3) & 4) == 0);
ok ((mpz(3) | 1) == 3);
ok ((mpz(3) | 4) == 7);
ok ((mpz(3) ^ 1) == 2);
ok ((mpz(3) ^ 4) == 7);
ok ((mpz("0xAA") | mpz("0x55")) == mpz("0xFF"));

{ my $a = mpz(3); $a &= 1; ok ($a == 1); }
{ my $a = mpz(3); $a &= 4; ok ($a == 0); }
{ my $a = mpz(3); $a |= 1; ok ($a == 3); }
{ my $a = mpz(3); $a |= 4; ok ($a == 7); }
{ my $a = mpz(3); $a ^= 1; ok ($a == 2); }
{ my $a = mpz(3); $a ^= 4; ok ($a == 7); }

ok (~ mpz(0) == -1);
ok (~ mpz(1) == -2);
ok (~ mpz(-2) == 1);
ok (~ mpz(0xFF) == -0x100);
ok (~ mpz(-0x100) == 0xFF);


# mpz functions

ok (bin(2,0) == 1);
ok (bin(2,1) == 2);
ok (bin(2,2) == 1);

ok (bin(3,0) == 1);
ok (bin(3,1) == 3);
ok (bin(3,2) == 3);
ok (bin(3,3) == 1);

{ my ($q, $r);
  ($q, $r) = cdiv (16, 3);
  ok ($q == 6);
  ok ($r == -2);
  ($q, $r) = cdiv (16, -3);
  ok ($q == -5);
  ok ($r == 1);
  ($q, $r) = cdiv (-16, 3);
  ok ($q == -5);
  ok ($r == -1);
  ($q, $r) = cdiv (-16, -3);
  ok ($q == 6);
  ok ($r == 2);
}
{ my ($q, $r);
  ($q, $r) = fdiv (16, 3);
  ok ($q == 5);
  ok ($r == 1);
  ($q, $r) = fdiv (16, -3);
  ok ($q == -6);
  ok ($r == -2);
  ($q, $r) = fdiv (-16, 3);
  ok ($q == -6);
  ok ($r == 2);
  ($q, $r) = fdiv (-16, -3);
  ok ($q == 5);
  ok ($r == -1);
}

{ my ($q, $r);
  ($q, $r) = tdiv (16, 3);
  ok ($q == 5);
  ok ($r == 1);
  ($q, $r) = tdiv (16, -3);
  ok ($q == -5);
  ok ($r == 1);
  ($q, $r) = tdiv (-16, 3);
  ok ($q == -5);
  ok ($r == -1);
  ($q, $r) = tdiv (-16, -3);
  ok ($q == 5);
  ok ($r == -1);
}

{ my ($q, $r);
  ($q, $r) = cdiv_2exp (23, 2);
  ok ($q == 6);
  ok ($r == -1);
  ($q, $r) = cdiv_2exp (-23, 2);
  ok ($q == -5);
  ok ($r == -3);
}
{ my ($q, $r);
  ($q, $r) = fdiv_2exp (23, 2);
  ok ($q == 5);
  ok ($r == 3);
  ($q, $r) = fdiv_2exp (-23, 2);
  ok ($q == -6);
  ok ($r == 1);
}

{ my ($q, $r);
  ($q, $r) = tdiv_2exp (23, 2);
  ok ($q == 5);
  ok ($r == 3);
  ($q, $r) = tdiv_2exp (-23, 2);
  ok ($q == -5);
  ok ($r == -3);
}

{
  my $a = 3;
  my $b = $a;
  ok ($b == 3);
  clrbit ($a, 0);
  print "a==2\n";
  ok ($a == 2);
  print "b==3\n";
  ok ($b == 3);
  print "b=a\n";
  $b = $a;
  print "b==2\n";
  ok ($b == 2);
  print "done\n";
}
{
  my $a = 0;
  my $b = $a;
  ok ($b == 0);
  setbit ($a, 0);
  ok ($a == 1);
  ok ($b == 0);
  $b = $a;
  ok ($b == 1);
}
{
  # compiled constants unchanged
  foreach (0, 1, 2) {
    use GMP::Mpz qw(:constants);
    my $a = 15;
    my $b = 6;
    use GMP::Mpz qw(:noconstants);
    clrbit ($a, 0);
    ok ($a == 14);
    setbit ($b, 0);
    ok ($b == 7);
  }
}

ok (  congruent_p (21, 0, 7));
ok (! congruent_p (21, 1, 7));
ok (  congruent_p (21, 5, 8));
ok (! congruent_p (21, 6, 8));

ok (  congruent_2exp_p (20, 0, 2));
ok (! congruent_2exp_p (21, 0, 2));
ok (! congruent_2exp_p (20, 1, 2));

ok (divexact(27,3) == 9);
ok (divexact(27,-3) == -9);
ok (divexact(-27,3) == -9);
ok (divexact(-27,-3) == 9);

ok (  divisible_p (21, 7));
ok (! divisible_p (21, 8));

ok (  divisible_2exp_p (20, 2));
ok (! divisible_2exp_p (21, 2));

ok (! even_p(mpz(-3)));
ok (  even_p(mpz(-2)));
ok (! even_p(mpz(-1)));
ok (  even_p(mpz(0)));
ok (! even_p(mpz(1)));
ok (  even_p(mpz(2)));
ok (! even_p(mpz(3)));

ok (fac(0) == 1);
ok (fac(1) == 1);
ok (fac(2) == 2);
ok (fac(3) == 6);
ok (fac(4) == 24);
ok (fac(5) == 120);

ok (fib(0) == 0);
ok (fib(1) == 1);
ok (fib(2) == 1);
ok (fib(3) == 2);
ok (fib(4) == 3);
ok (fib(5) == 5);
ok (fib(6) == 8);

ok (gcd (21) == 21);
ok (gcd (21,15) == 3);
ok (gcd (21,15,30,57) == 3);
ok (gcd (21,-15) == 3);
ok (gcd (-21,15) == 3);
ok (gcd (-21,-15) == 3);

{
  my ($g, $x, $y) = gcdext (3,5);
  ok ($g == 1);
  ok ($x == 2);
  ok ($y == -1);
}

ok (hamdist(5,7) == 1);

ok (invert(1,123) == 1);
ok (invert(6,7) == 6);
ok (! defined invert(2,8));

foreach my $i ([  1, 19,  1 ],
	       [  4, 19,  1 ],
	       [  5, 19,  1 ],
	       [  6, 19,  1 ],
	       [  7, 19,  1 ],
	       [  9, 19,  1 ],
	       [ 11, 19,  1 ],
	       [ 16, 19,  1 ],
	       [ 17, 19,  1 ],
	       [  2, 19, -1 ],
	       [  3, 19, -1 ],
	       [  8, 19, -1 ],
	       [ 10, 19, -1 ],
	       [ 12, 19, -1 ],
	       [ 13, 19, -1 ],
	       [ 14, 19, -1 ],
	       [ 15, 19, -1 ],
	       [ 18, 19, -1 ]) {
  foreach my $fun (\&jacobi, \&kronecker) {
    ok (&$fun ($$i[0], $$i[1]) == $$i[2]);

    ok (&$fun ($$i[0],      str($$i[1])) == $$i[2]);
    ok (&$fun (str($$i[0]),     $$i[1])  == $$i[2]);
    ok (&$fun (str($$i[0]), str($$i[1])) == $$i[2]);

    ok (&$fun ($$i[0],      mpz($$i[1])) == $$i[2]);
    ok (&$fun (mpz($$i[0]), $$i[1]) == $$i[2]);
    ok (&$fun (mpz($$i[0]), mpz($$i[1])) == $$i[2]);
  }
}	       

ok (lcm (2) == 2);
ok (lcm (0) == 0);
ok (lcm (0,0) == 0);
ok (lcm (0,0,0) == 0);
ok (lcm (0,0,0,0) == 0);
ok (lcm (2,0) == 0);
ok (lcm (-2,0) == 0);
ok (lcm (2,3) == 6);
ok (lcm (2,3,4) == 12);
ok (lcm (2,-3) == 6);
ok (lcm (-2,3) == 6);
ok (lcm (-2,-3) == 6);
ok (lcm (mpz(2)**512,1) == mpz(2)**512);
ok (lcm (mpz(2)**512,-1) == mpz(2)**512);
ok (lcm (-mpz(2)**512,1) == mpz(2)**512);
ok (lcm (-mpz(2)**512,-1) == mpz(2)**512);
ok (lcm (mpz(2)**512,mpz(2)**512) == mpz(2)**512);
ok (lcm (mpz(2)**512,-mpz(2)**512) == mpz(2)**512);
ok (lcm (-mpz(2)**512,mpz(2)**512) == mpz(2)**512);
ok (lcm (-mpz(2)**512,-mpz(2)**512) == mpz(2)**512);

ok (nextprime(2) == 3);
ok (nextprime(3) == 5);
ok (nextprime(5) == 7);
ok (nextprime(7) == 11);
ok (nextprime(11) == 13);

ok (  odd_p(mpz(-3)));
ok (! odd_p(mpz(-2)));
ok (  odd_p(mpz(-1)));
ok (! odd_p(mpz(0)));
ok (  odd_p(mpz(1)));
ok (! odd_p(mpz(2)));
ok (  odd_p(mpz(3)));

# ok (  perfect_power_p(mpz(-27)));
# ok (! perfect_power_p(mpz(-9)));
# ok (! perfect_power_p(mpz(-1)));
ok (! perfect_power_p(mpz(0)));
ok (! perfect_power_p(mpz(1)));
ok (! perfect_power_p(mpz(2)));
ok (! perfect_power_p(mpz(3)));
ok (  perfect_power_p(mpz(4)));
ok (  perfect_power_p(mpz(9)));
ok (  perfect_power_p(mpz(27)));
ok (  perfect_power_p(mpz(81)));

ok (! perfect_square_p(mpz(-9)));
ok (! perfect_square_p(mpz(-1)));
ok (  perfect_square_p(mpz(0)));
ok (  perfect_square_p(mpz(1)));
ok (! perfect_square_p(mpz(2)));
ok (! perfect_square_p(mpz(3)));
ok (  perfect_square_p(mpz(4)));
ok (  perfect_square_p(mpz(9)));
ok (! perfect_square_p(mpz(27)));
ok (  perfect_square_p(mpz(81)));

ok (popcount(7) == 3);

ok (powm (3,2,8) == 1);

ok (  probab_prime_p(89,1));
ok (! probab_prime_p(81,1));

{
  my $z = mpz(123);
  realloc ($z, 512);
}

{
  my ($rem, $mult);
  ($rem, $mult) = remove(12,3);
  ok ($rem == 4);
  ok ($mult == 1);
  ($rem, $mult) = remove(12,2);
  ok ($rem == 3);
  ok ($mult == 2);
}

ok (root(0,2) == 0);
ok (root(8,3) == 2);
ok (root(-8,3) == -2);
ok (root(81,4) == 3);
ok (root(243,5) == 3);

{ my ($r,$e);
  ($r, $e) = roote(0,2);
  ok ($r == 0);
  ok ($e);
  ($r, $e) = roote(81,4);
  ok ($r == 3);
  ok ($e);
  ($r, $e) = roote(85,4);
  ok ($r == 3);
  ok (! $e);
}

{
  my $ulong_max = ~ 0;
  ok (scan0 (0, 0) == 0);
  ok (scan0 (1, 0) == 1);
  ok (scan0 (3, 0) == 2);
  ok (scan0 (-1, 0) == $ulong_max);
  ok (scan0 (-2, 1) == $ulong_max);

  ok (scan1 (1, 0) == 0);
  ok (scan1 (2, 0) == 1);
  ok (scan1 (4, 0) == 2);
  ok (scan1 (0, 0) == $ulong_max);
  ok (scan1 (3, 2) == $ulong_max);
}

ok (sizeinbase(1,10) == 1);
ok (sizeinbase(100,10) == 3);
ok (sizeinbase(9999,10) == 5);

foreach my $name ('mpz', 'mpf') {
  print "$name\n";
  my $mpx = eval("\\&$name");
  ok (sqrt(&$mpx(0)) == 0);
  ok (sqrt(&$mpx(1)) == 1);
  ok (sqrt(&$mpx(4)) == 2);
  ok (sqrt(&$mpx(81)) == 9);
}

{
  my ($root, $rem) = sqrtrem(mpz(0));
  ok ($root == 0);
  ok ($rem == 0);
}
{
  my ($root, $rem) = sqrtrem(mpz(1));
  ok ($root == 1);
  ok ($rem == 0);
}
{
  my ($root, $rem) = sqrtrem(mpz(2));
  ok ($root == 1);
  ok ($rem == 1);
}
{
  my ($root, $rem) = sqrtrem(mpz(9));
  ok ($root == 3);
  ok ($rem == 0);
}
{
  my ($root, $rem) = sqrtrem(mpz(35));
  ok ($root == 5);
  ok ($rem == 10);
}
{
  my ($root, $rem) = sqrtrem(mpz(0));
  ok ($root == 0);
  ok ($rem == 0);
}

ok (tstbit (6, 0) == 0);
ok (tstbit (6, 1) == 1);
ok (tstbit (6, 2) == 1);
ok (tstbit (6, 3) == 0);


# mpq functions

ok (mpq('3/2') == mpq(3,2));
ok (mpq('3/1') == mpq(3,1));
ok (mpq('-3/2') == mpq(-3,2));
ok (mpq('-3/1') == mpq(-3,1));
ok (mpq('0x3') == mpq(3,1));
ok (mpq('0b111') == mpq(7,1));
ok (mpq('0b0') == mpq(0,1));

ok (mpq(3,2) > 1);
ok (mpq(3,2) < 2);

ok (mpq(1,2)+mpq(1,3) == mpq(5,6));
ok (mpq(1,2)+mpq(-1,3) == mpq(1,6));
ok (mpq(-1,2)+mpq(1,3) == mpq(-1,6));
ok (mpq(-1,2)+mpq(-1,3) == mpq(-5,6));

ok (mpq(1,2)-mpq(1,3) == mpq(1,6));
ok (mpq(1,2)-mpq(-1,3) == mpq(5,6));
ok (mpq(-1,2)-mpq(1,3) == mpq(-5,6));
ok (mpq(-1,2)-mpq(-1,3) == mpq(-1,6));

{
  my $q = mpq(21,15); canonicalize($q);
  ok (num($q) == 7);
  ok (den($q) == 5);
}

{ my $q = mpq(5,7);  ok("$q" eq "5/7"); }
{ my $q = mpq(-5,7); ok("$q" eq "-5/7"); }


# mpf functions

ok (mpf(-1.5) == -1.5);
ok (mpf(-1.0) == -1.0);
ok (mpf(-0.5) == -0.5);
ok (mpf(0) == 0);
ok (mpf(0.5) == 0.5);
ok (mpf(1.0) == 1.0);
ok (mpf(1.5) == 1.5);

ok (mpf("-1.5") == -1.5);
ok (mpf("-1.0") == -1.0);
ok (mpf("-0.5") == -0.5);
ok (mpf("0") == 0);
ok (mpf("0.5") == 0.5);
ok (mpf("1.0") == 1.0);
ok (mpf("1.5") == 1.5);

{ my $f = mpf(0.25); ok("$f" eq "0.25"); }
{ my $f = mpf(-0.25); ok("$f" eq "-0.25"); }
{ my $f = mpf(1.25); ok("$f" eq "1.25"); }
{ my $f = mpf(-1.25); ok("$f" eq "-1.25"); }
{ my $f = mpf(1000000); ok("$f" eq "1e6"); }
{ my $f = mpf(-1000000); ok("$f" eq "-1e6"); }

ok (floor(mpf(-7.5)) == -8.0);
ok (ceil (mpf(-7.5)) == -7.0);
ok (trunc(mpf(-7.5)) == -7.0);
ok (floor(mpf(7.5)) == 7.0);
ok (ceil (mpf(7.5)) == 8.0);
ok (trunc(mpf(7.5)) == 7.0);

set_default_prec(128);
ok (  mpf_eq (mpz("0x10000000000000001"), mpz("0x10000000000000002"), 1));
ok (! mpf_eq (mpz("0x11"), mpz("0x12"), 128));

{
  set_default_prec(128);
  my $p = get_default_prec();
  set_default_prec($p);
  ok (get_default_prec() == $p);
}

foreach my $f (0.0, mpf(0.0)) {
  my ($s, $e);
  ($s, $e) = mpf_get_str ($f, 10, 0);
  ok ($s eq "");
  ok ($e == 0);
  ($s, $e) = mpf_get_str ($f, 16, 0);
  ok ($s eq "");
  ok ($e == 0);
}

foreach my $f (255.0, mpf(255)) {
  my ($s, $e) = mpf_get_str(255.0,16,0);
  ok ($s eq "ff");
  ok ($e == 2);
}
foreach my $f (255.0, mpf(255)) {
  my ($s, $e) = mpf_get_str(255.0,-16,0);
  ok ($s eq "FF");
  ok ($e == 2);
}
foreach my $f (-255.0, mpf(-255)) {
  my ($s, $e) = mpf_get_str(-255.0,16,0);
  ok ($s eq "-ff");
  ok ($e == 2);
}
foreach my $f (-255.0, mpf(-255)) {
  my ($s, $e) = mpf_get_str(-255.0,-16,0);
  ok ($s eq "-FF");
  ok ($e == 2);
}

ok (reldiff (2,4) == 1);
ok (reldiff (4,2) == 0.5);

{
  my $x = mpf(1.0, 512);
  my $y = $x;
  my $yprec = get_prec ($y);
  set_prec ($x, 1024);
  ok (get_prec ($y) == $yprec);

  my $xprec = get_prec ($x);
  set_prec ($x, $xprec);
  ok (get_prec ($x) == $xprec);
}


# random functions

{ my $r = randstate(); }
{ my $r = randstate('lc',64); }
{ my $r = randstate('lc2exp',1,2,3); }
{
  my $r = randstate();
  $r->seed(123);
  $r->seed(time());
  mpf_urandomb($r,1024);
  mpz_urandomb($r,1024);
  mpz_rrandomb($r,1024);
  mpz_urandomm($r,mpz(3)**100);
}



# Local variables:
# perl-indent-level: 2
# End:
