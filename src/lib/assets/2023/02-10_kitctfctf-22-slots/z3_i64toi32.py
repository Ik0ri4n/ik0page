#!/ usr/bin/ python
from z3 import *


def cryptic(input: int) -> int:
    if input < 0xffff000000000000:
        return -1

    result = 0
    current = input
    for i in range(4):
        if (current & 0b0000101001001000) != 0:
            return -1
        bits = current & 0b00000111 | (current >> 1) & 0b00011000 | (
            current >> 2) & 0b01100000 | (current >> 3) & 0b10000000
        result = result | bits << ((i << 3) & 0b00011111)
        current = current >> 0xc
    return result


def symbolic(s: Solver, input: BitVec):
    res = BitVec("res", 64)
    s.add(input & 0xffff000000000000 == 0xffff000000000000)

    last = input
    lastRes = BitVec("rIn", 64)
    s.add(lastRes == 0b0)
    for i in range(4):
        s.add((last & 0b0000101001001000) == 0)

        bi = BitVec("b"+str(i), 64)
        ri = BitVec("r"+str(i), 64)

        s.add(bi == last & 0b00000111 | (last >> 1) & 0b00011000 | (
            last >> 2) & 0b01100000 | (last >> 3) & 0b10000000)

        s.add(ri == lastRes | bi << ((i << 3) & 0b00011111))
        lastRes = ri

        lTmp = BitVec("l"+str(i), 64)
        s.add(lTmp == last >> 0xc)
        last = lTmp
    s.add(res == lastRes)
    return res


print(cryptic(18446490111005696309))

x = BitVec('x', 64)
s = Solver()

res = symbolic(s, x)
s.add(res == 1761313373)

if s.check() == z3.sat:
    print(s.model()[x].as_long())
else:
    print("Solver error!")
