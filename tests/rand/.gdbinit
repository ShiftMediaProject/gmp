define pz
set __gmpz_dump ($)
end

define pq
set __gmpz_dump ($->_mp_num)
echo /
set __gmpz_dump ($->_mp_den)
end

define pf
set __gmpf_dump ($)
end

