/* qstack.c */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

mp_size_t
qstack_itch (mp_size_t size)
{
  /* Limit on the recursion depth */

  unsigned k = hgcd_max_recursion (size);

  ASSERT (2 * k < QSTACK_MAX_QUOTIENTS);

  return 2 * size + 12 * (k+1);
}

void
qstack_reset (struct qstack *stack,
	      mp_size_t asize)
{
  /* Limit on the recursion depth */
  unsigned k = hgcd_max_recursion (asize);

  stack->size_next = 0;
  stack->limb_next= 0;
  stack->nkeep = 2 * (k + 1);
  ASSERT (stack->nkeep < QSTACK_MAX_QUOTIENTS);

  qstack_sanity (stack);
}

void
qstack_init (struct qstack *stack,
	     mp_size_t asize,
	     mp_limb_t *limbs, mp_size_t alloc)
{
  stack->limb = limbs;
  stack->limb_alloc = alloc;
  /* Should depend on input size, we need 2 * recursion depth */
  stack->nkeep = QSTACK_MAX_QUOTIENTS - 1;

  qstack_reset (stack, asize);
}

/* Drop all but the nkeep latest quotients. Drop additional quotients
   if needed to reclain at least SIZE limbs of storage. */
void
qstack_rotate (struct qstack *stack,
	       mp_size_t size)
{
  unsigned dropped_sizes;
  unsigned kept;
  unsigned i;

  mp_size_t dropped_limbs;

  qstack_sanity (stack);

  if (stack->size_next > stack->nkeep)
    dropped_sizes = stack->size_next - stack->nkeep;
  else
    dropped_sizes = 0;

  for (i = 0, dropped_limbs = 0; i < dropped_sizes; i++)
    dropped_limbs += stack->size[i];

  for (; dropped_limbs < size; dropped_sizes++)
    {
      ASSERT (dropped_sizes < stack->size_next);
      dropped_limbs += stack->size[dropped_sizes];
    }

  ASSERT (dropped_limbs <= stack->limb_next);

  kept = stack->size_next - dropped_sizes;

  if (dropped_sizes)
    /* memmove isn't portable */
    for (i = 0; i < kept; i++)
      stack->size[i] = stack->size[i + dropped_sizes];

  stack->size_next = kept;

  if (dropped_limbs)
    {
      if (dropped_limbs < stack->limb_next)
	{
	  MPN_COPY_INCR (stack->limb, stack->limb + dropped_limbs,
			stack->limb_next - dropped_limbs);
	  ASSERT (dropped_limbs <= stack->limb_next);
	  stack->limb_next -= dropped_limbs;
	}
      else
	stack->limb_next = 0;
    }
  qstack_sanity (stack);
}

#if WANT_ASSERT
void
qstack_sanity (struct qstack *stack)
{
  mp_size_t next;
  unsigned i;

  for (i = 0, next = 0; i < stack->size_next; i++)
    {
      mp_size_t qsize = stack->size[i];
      ASSERT (next <= stack->limb_alloc);

      ASSERT (qsize == 0 || stack->limb[next+qsize - 1] != 0);
      next += qsize;
    }

  ASSERT (next == stack->limb_next);
}
#endif

/* What's the right place for this function? */
unsigned
hgcd_max_recursion (mp_size_t n)
{
  int count;

  count_leading_zeros (count, (mp_limb_t)
		       (1 + n / (HGCD_SCHOENHAGE_THRESHOLD  - 5)));

  return GMP_LIMB_BITS - count;
}

