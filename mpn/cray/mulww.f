      subroutine gmpn_mulww(p1,p0,a,n,s)
      integer*8 p1(0:*),p0(0:*),a(0:*),s
      integer n

      integer*8 a0,a1,a2,s0,s1,s2,cy
      integer*8 ai,t0,t1,t2,t3,t4,r0,r1,r2

      s0 = shiftl(and(s,4194303),24)
      s1 = shiftl(and(shiftr(s,22),4194303),24)
      s2 = shiftl(and(shiftr(s,44),4194303),24)

      do i = 0,n-1
         ai = a(i)
         a0 = shiftl(and(ai,4194303),24)
         a1 = shiftl(and(shiftr(ai,22),4194303),24)
         a2 = shiftl(and(shiftr(ai,44),4194303),24)

         t0 = i24mult(a0,s0)
         t1 = i24mult(a0,s1) + i24mult(a1,s0)
         t2 = i24mult(a0,s2) + i24mult(a1,s1) + i24mult(a2,s0)
         t3 = i24mult(a1,s2) + i24mult(a2,s1)
         t4 = i24mult(a2,s2)

         r0 = t0 + shiftl(t2,44) + shiftl(t1,22)

         cy = shiftr(shiftr(t0,22)+and(t1,4398046511103)
     $        +shiftl(and(t2,1048575),22),42)

         p0(i) = r0
         r2 = shiftl(t4,24)+shiftl(t3,2)+shiftr(t2,20)+shiftr(t1,42)
         p1(i) = r2 + cy
      end do
      end
