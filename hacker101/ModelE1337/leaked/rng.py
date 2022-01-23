import random
def setup(seed):
        global state
        state = 0
        for i in range(16):
                cur = seed & 3
                seed >>= 2
                state = (state << 4) | ((state & 3) ^ cur)
                state |= cur << 2

def next(bits):
        global state

        ret = 0
        for i in range(bits):
                ret <<= 1
                ret |= state & 1
                state = (state << 1) ^ (state >> 61)
                state &= 0xFFFFFFFFFFFFFFFF
                state ^= 0xFFFFFFFFFFFFFFFF

                for j in range(0, 64, 4):
                        cur = (state >> j) & 0xF
                        cur = (cur >> 3) | ((cur >> 2) & 2) | ((cur << 3) & 8) | ((cur << 2) & 4)
                        state ^= cur << j

        return ret

setup((random.randrange(0x10000) << 16) | random.randrange(0x10000))