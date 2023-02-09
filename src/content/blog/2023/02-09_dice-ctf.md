---
title: Writeups from DiceCTF 2023
date: '2023-02-08'
excerpt: "As it is my main field of interest currently, I mainly worked on reversing challenges this DiceCTF. I'll go over the challenges parallelism and not-baby-parallelism, two challenges I really enjoyed. So let's dive right in!"
---

<script lang="ts">
	import Challenge from '$lib/components/Challenge.svelte';

    let parallelism_downloads = ["/src/lib/assets/2023/02-09_dice_ctf/parallelism"];
    let nbparallelism_downloads = ["/src/lib/assets/2023/02-09_dice_ctf/not-baby-parallelism", "/src/lib/assets/2023/02-09_dice_ctf/flag.out"];

    import nbparallel_accesses_first from "$lib/assets/2023/02-09_dice_ctf/nbparallel_accesses_first.png"
    import nbparallel_accesses_second from "$lib/assets/2023/02-09_dice_ctf/nbparallel_accesses_second.png"
    import nbparallel_barrier from "$lib/assets/2023/02-09_dice_ctf/nbparallel_barrier.png"
    import nbparallel_encryption_first from "$lib/assets/2023/02-09_dice_ctf/nbparallel_encryption_first.png"
    import nbparallel_encryption_second from "$lib/assets/2023/02-09_dice_ctf/nbparallel_encryption_second.png"
    import nbparallel_main from "$lib/assets/2023/02-09_dice_ctf/nbparallel_main.png"
    import nbparallel_run from "$lib/assets/2023/02-09_dice_ctf/nbparallel_run.png"
    import nbparallel_signature from "$lib/assets/2023/02-09_dice_ctf/nbparallel_signature.png"
    import nbparallel_thread_constructor from "$lib/assets/2023/02-09_dice_ctf/nbparallel_thread_constructor.png"
    import parallel_gather from "$lib/assets/2023/02-09_dice_ctf/parallel_gather.png"
    import parallel_main from "$lib/assets/2023/02-09_dice_ctf/parallel_main.png"
    import parallel_scatter from "$lib/assets/2023/02-09_dice_ctf/parallel_scatter.png"
    import parallel_shuffle from "$lib/assets/2023/02-09_dice_ctf/parallel_shuffle.png"
</script>

Last weekend we participated in [DiceCTF](https://ctftime.org/event/1838) with [KITCTF](https://kitctf.de/) and achieved a solid 16th place together.
I worked full-time on the reversing challenges and actually managed to claim my first-ever first-blood on [parallelism](#parallelism).
As such, I wanted to provide a writeup for the challenge.
Additionally, I'll include a writeup for the slightly more difficult [not-baby-parallelism](#not-baby-parallelism), another challenge I enjoyed a lot.

Judging by the number of solves, the challenges have an easy or medium difficulty.
Still, having started with CTFs about half a year ago, I wanted to provide these writeups for others starting out or trying to learn more.
Hopefully, you'll find my perspective and the approach I took interesting.
Also, the theme of the two challenges just fits too well 😉

## parallelism

<Challenge name="parallelism" author="infuzion" category="rev" solves={68} points={144} flag="dice&lbrace;P4ral1isM_m4kEs_eV3ryt4InG_sUp3r_f4ST_aND_s3CuRE_a17m4k9l4&rbrace;" downloads={parallelism_downloads}>
High performance computing is cool and all, but high performance flag checking is where it's at.

Run with `mpirun --oversubscribe -np 8 ./parallelism`
</Challenge>

**TL;DR:** parallelism was a reversing challenge using cooperative data shuffling with eight processes.
I solved it by analyzing the program and implementing its inverse in Python to reconstruct the flag.

I started out the CTF with this challenge because I just recently worked with MPI for my studies and was intrigued to find out what the author had prepared for us.
For those not familiar with MPI is short for message passing interface.
It defines protocols and methods for synchronization and data distribution over multiple processes working cooperatively on a problem in parallel.
The challenge uses a common implementation, OpenMPI, which supports C and uses the `mpirun` command we were given.
Thus, I expected a C program and quickly imported it into ghidra for analysis.
(If you're starting out with ghidra reversing, I can recommend [stacksmashing](https://www.youtube.com/@stacksmashing) for an introduction.)

Starting out, I clicked through the few functions to find the entry an main method with their common structure.
In the main method, the program initializes and MPI and gets the number of processes and the rank (the number identifying a process), which is the number of the current process.
We can see that the program always runs with 8 processes and uses three steps for probably checking the flag.

![parallelism main method]({parallel_main})

The first method I called scatter reads our input when run on the main process and expects a line with a 64-character string.
It then uses an array of 32 indices to shuffle this string by swapping the character at the given index with the one offset by 31.
Finally, it uses MPI_Scatter, a method for distributing a block of data equally between processes, to split the input in blocks of eight characters.

![parallelism scatter method]({parallel_scatter})

Next, the program implements 10.000 rounds of another shuffle.
Each round, each process sends the character at a given offset (`i % 8`) to the process before it, by the same offset to its own rank.
It then receives its character and waits for the operations to complete since the program uses the non-blocking method variants.
Then, it updates its buffer and synchronizes with the other processes at a barrier before starting the next round.

![parallelism shuffle method]({parallel_shuffle})

The last method of the program gathers the message parts from the processes which is the inverse of scatter.
Then, it compares the result to a string constant on the main process to determine whether it is the flag.

![parallelism gather method]({parallel_gather})

After looking at the program shortly, I decided I understood it enough to implement the reverse process in python (I'm pretty used to that now).
I managed to forget a zero for the round number but fixed that soon after to produce the following program:

```python
COMM_SIZE = 8

POS = [0] * 32
POS[0] = 0x1a
POS[1] = 0x20
POS[2] = 0xe
POS[3] = 0xb
POS[4] = 3
POS[5] = 1
POS[6] = 0x20
POS[7] = 0x18
POS[8] = 0xd
POS[9] = 0x11
POS[10] = 3
POS[11] = 0x11
POS[12] = 2
POS[13] = 0xd
POS[14] = 0x13
POS[15] = 6
POS[16] = 0xc
POS[17] = 0x16
POS[18] = 3
POS[19] = 0x1e
POS[20] = 10
POS[21] = 6
POS[22] = 8
POS[23] = 0x1a
POS[24] = 6
POS[25] = 0x16
POS[26] = 0xd
POS[27] = 1
POS[28] = 0x13
POS[29] = 1
POS[30] = 1
POS[31] = 0x1d
EXP = "m_ERpmfrNkekU4_4asI_Tra1e_4l_c4_GCDlryidS3{Ptsu9i}13Es4V73M4_ans"


BUFFER = [0]*8
# Inverse gather
for i in range(8):
    BUFFER[i] = list(EXP[i*8:i*8+8])

# Inverse of parallel shuffle
for i in range(10000):
    i = 10000 - i - 1

    tmp = [0] * 8
    for COMM_RANK in range(8):
        offset = i % 8
        dest = (COMM_SIZE + (COMM_RANK - i) % COMM_SIZE) % COMM_SIZE
        tmp[COMM_RANK] = BUFFER[dest][offset]
    # Splitting here to avoid loosing values
    for COMM_RANK in range(8):
        BUFFER[COMM_RANK][offset] = tmp[COMM_RANK]

# Inverse of scatter and start shuffle
input = []
for i in range(COMM_SIZE):
    input += BUFFER[i]

for i in range(0x20):
    i = 0x20 - i - 1
    current = input[i]
    input[i] = input[POS[i] + 31]
    input[POS[i] + 31] = current

print("".join(input))
```

Of course, the challenge was pretty simple and I managed that primarily because people start with different challenges depending on their preferences.
I was happy nonetheless to be able to claim the first-blood for my team.

## not-baby-parallelism

<Challenge name="not-baby-parallelism" author="jyu" category="rev" solves={19} points={205} flag="dice&lbrace;p4r411el_pref1x_sc4ns_w0rk_efficient_but_sl0w&rbrace;" downloads={nbparallelism_downloads}>
High performance computing is cool and all, but high performance flag checking is where it's at.

Run with `mpirun --oversubscribe -np 8 ./parallelism`
</Challenge>

**TL;DR:** The challenge requires us to reverse an encryption with three operations applied in rounds in a structure that reminded me a bit of BCD and Hamming code.
We solved it by deducing the order of the operations (they're ordered randomly) and implementing a decryption from that.

As works best for me, I quickly loaded the binary into ghidra to look at the program and find it's C++.
Many people in our team dislike C++ reversing since it often complicates the process with structs, classes, template methods, optimizations etc.
I agree that it is challenging but I started looking into it since my first own challenge (writeup coming soon!) and enjoying taking my time to look at the types and methods.
If you're mainly interested in the part reversing the encoding, you can [skip to that block](#encryption-logic) directly.

### The program structure

The program was fully stripped, but as is often the case, we could jump to the main function from the entry point.
Here, the program did some parameter parsing with `getopt` to get the options.
The challenge already explained what each option is for and we can confirm that easily.
The number of threads is stored in a global constant and the input and output paths are used for opening file streams.
Here, C++ methods often require us to fix their signature by hand in ghidra but I could produce readable results quickly with the [docs](https://en.cppreference.com/w/).

![not-baby-parallelism adjusting signature]({nbparallel_signature})

The core part of the main method reads a size from the input file and initializes `srand` with this size XOR the number of threads.
The it reads `size` numbers into a vector, executes some timed method and finally writes the resulting number vector to the output file.

![not-baby-parallelism main method]({nbparallel_main})

The `run`-method sets up a lot of structs and starts the encryption threads.
First, it initializes some kind of sync struct, shuffles the operations using `rand` and starts the threads with the vector data, the operations, a global position pointer and the sync struct.
The rest of the method waits for completion and checks for exceptions.

![not-baby-parallelism run method]({nbparallel_run})

The thread construction method contains the real thread constructor with a reference to a vtable.
Additionally, it copies the arguments mentioned earlier to a new struct.
Creating this struct was really useful to understand the thread `run` method, the last method in the vtable.

![not-baby-parallelism thread constructor]({nbparallel_thread_constructor})

The logic of the thread `run` method is hidden six layers deep in a tree of nested calls copying the parameters.
It does however contain the encryption logic so it's worth the search.

### Encryption logic

After analyzing the program structure and fixing signatures and types, I looked at the algorithm itself together with others of my team, [KITCTF](https://kitctf.de/).
The encryption method uses the `thread_args`-struct I created from the constructor and all the other structs and methods I analyzed.
Since I already named them pretty specifically, I will only show you the most important methods.
The encryption contains two phases.

![not-baby-parallelism first part of encryption method]({nbparallel_encryption_first})

During the first one, it iterates over the vector using the global position pointer to get unique positions for each thread.
The round counter is used as the distance between elements and also determines the operation to be used.#
Every time, the upper index is updated wit the result of the operation.
We visualized the element accesses of the algorithm like this:

![not-baby-parallelism first part of access structure]({nbparallel_accesses_first})

We actually have 51 characters in the flag, but the pattern still applies.
After each round, the threads are synchronized with an interesting barrier implementation of a barrier using the sync struct:

![not-baby-parallelism barrier method]({nbparallel_barrier})

It first counts down using the first semaphore and a mutex and then up again using a second semaphore.

Now, after the first encryption block, the same structure is repeated, this time counting down the counter and adding the counter instead of subtracting it.

![not-baby-parallelism second part of encryption method]({nbparallel_encryption_second})

This results in this access structure, from the bottom upwards:

![not-baby-parallelism second part of access structure]({nbparallel_accesses_second})

Now, as you might have noticed, the first number is never updated.
We could use this to confirm that it is the flag as ASCII codes since `chr(100) == 'd'`.
Knowing that the flag starts with `dice{` we can deduce that the first operation in the list is `XOR` from the second character: `chr(100 ^ 13) == 'i'` (also the possible option, with such a low number).
Next, we can do the same with the fourth character (after reconstructing those before it) to deduce that the next operation is `add`: `19 = 13 + (99 ^ 101)`.

Finally, I implemented the inverse of the encryption knowing the order of the operations:

```python
import math

values = []
with open("flag.out", "r") as file:
    line = file.readline()
    while line != "":
        values.append(int(line))
        line = file.readline()


def xor(a, b):
    return a ^ b


def add(a, b):
    return a - b


def mult(a, b):
    return a // b


ops = [xor, add, mult]


SIZE = len(values)

counter = 1

while counter < 32:
    glob = 1
    while True:
        if SIZE <= glob * counter * 2 - 1:
            break

        lower = glob * counter * 2 - 1
        upper = lower + counter

        if upper < SIZE:
            iter_count = round(math.log2(counter))

            values[upper] = ops[iter_count % 3](values[upper], values[lower])

        glob += 1

    counter <<= 1


while counter > 0:
    glob = 1
    while True:
        if SIZE <= glob * counter * 2 - 1:
            break

        upper = glob * counter * 2 - 1
        lower = upper - counter

        if upper < SIZE:
            iter_count = round(math.log2(counter))

            values[upper] = ops[iter_count % 3](values[upper], values[lower])

        glob += 1

    counter >>= 1

print("".join([chr(i) for i in values]))
```