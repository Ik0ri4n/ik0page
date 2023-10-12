---
title: CyberSecurityRumble Quals & Finals 2023
excerpt: 'After the forced pause this years edition of the CyberSecurityRumble finally returned with an onsite finals event. '
---

<script lang="ts">
	import Challenge from '$lib/components/Challenge.svelte';

    let exterminate_downloads = ["/blog/2023/06_rumble-23/exterminate.apk"];
    let pcas_downloads = ["/blog/2023/06_rumble-23/pcas.zip"];

    import exterminate_main_activity from "$lib/assets/2023/06_rumble-23/exterminate_main_activity.png";
    import exterminate_isCodeCorrect1 from "$lib/assets/2023/06_rumble-23/exterminate_isCodeCorrect1.png";
    import exterminate_isCodeCorrect2 from "$lib/assets/2023/06_rumble-23/exterminate_isCodeCorrect2.png";
    //import pcas_ from "$lib/assets/2023/06_rumble-23/";
</script>

- Short description of competition? (Teams of 6, 24h qualifier, onsite finals)
- Exterminate: Easy android challenge from finals
- PCaS: Bonus, medium challenge from quals I really enjoyed

## Exterminate

<Challenge name="Exterminate" author="claudia|nviso" category="rev|mobile" solves={6} points={270} flag="CSR&lbrace;_the-def1n1te-article-fl4g_&rbrace;" downloads={exterminate_downloads}>
Daleks have invaded the earth and the Doctor is nowhere to be seen. Now it's up to you to find out how they are built and which code can stop the extermination of all humankind.
</Challenge>

The heavily Doctor Who-themed challenge Exterminate provides us with an apk file for some Android reversing.
Looking at the decompilation (most online tools or IDEs should suffice) result I found a few simple classes including two activities.
The main activity of the app is the `CountdownActivity` but is otherwise irrelevant
since it simply counts down from three until the extermination by the Daleks and effectively closes the app.
The other activity however, in the `CodeActivity` class, contains a code input field, a button and a message field like a plain login flow.
The button on-click listener is defined as an external synthetic lambda, suggesting a JNI call.
We find corresponding libraries at the path `./resources/lib/<arch>/libexterminate.so` which we can analyze with the decompiler of our choice (in my case ghidra).

<div class="h-96 relative [&>*]:flex [&>*]:h-full">

![Exterminate CountdownActivity screenshot]({exterminate_main_activity})

</div>

By default, ghidra doesn't resolve JNI types and signatures well.
I often used the [JNIAnalyzer plugin](https://github.com/Ayrx/JNIAnalyzer) by Ayrx which extracts function definitions from an apk file, includes a JNI header
and automatically updates the relevant function signatures.
In this case it helps a little in identifying the JNI string parameter and return value.
The `CodeActivity.getResponseCode()` function is fairly simple though, 
first checking whether the code is correct via `isCodeCorrect` and if so calling `getFlag(out, code)`.

The check method `isCodeCorrect` first checks that the length of the code is 8.
In case your not familiar with the C++ basic_string struct ([the libc++ version](https://joellaity.com/2020/01/31/string.html)), 
it has a small version for 22 or less bytes and a version for longer strings.
For the small version, the first bit of the first byte is set and additionally contains the length shifted left by one.
The data pointer is 
The version for long strings has the length at offset 8 and the string data at offset `0x10`.
While otherwise irrelevant for the check logic, it does clobber the decompilation result and makes it harder to read without the correct struct header.

![isCodeCorrect main logic]({exterminate_isCodeCorrect1})

The main logic of the check validates that the characters at index 1 and 2 are equal.
Then it looks for the substring "on" (by checking every index that contains 'o', with `memchr`) 
and checks that index 6 contains '-'.
Finally, the check rejects characters below or equal to '9'.
With 4 valid positions for the substring "on" and assuming the other four characters are in limited printable range, 
this limits our search space to `4 * (128 - 0x3A)**4 = 96040000` valid codes but does not give us a definite result.

![isCodeCorrect final check]({exterminate_isCodeCorrect2})

The `getFlag` function is very simple: it concatenates the code with "geronimo" and then calls `decrypt` 
which decrypts two blocks with AES ECB mode and combines them to get the flag.
Since we can check a key by checking the flag format, we can brute-force the code in the limited search space relatively easily.
Depending on the available hardware, we can execute the brute force in under 20 minutes (full runtime around 30 minutes on my old laptop).
I simply asked my teammates and we crowd sourced the problem, running one of the four cases on separate laptops in the background.

```py
from Crypto.Cipher import AES
import rich.progress

FIRST = bytes.fromhex("d66d203aadabea4f8e583745eff2b871")[::-1]
SECOND = bytes.fromhex("3f97be164d0323d7c7e98af11a35c8a8")[::-1]
SUFFIX = bytes.fromhex("6f6d696e6f726567")[::-1]


def test(key: bytes) -> bool:
    cipher = AES.new(key, AES.MODE_ECB)
    out = cipher.decrypt(SECOND)

    return b"CSR{" in out


def by(val: int):
    return int.to_bytes(val, 1, "big")


# b"ABBCEF-G" with b"on" somewhere?!
def brute_force() -> bytes:
    for a in rich.progress.track(range(0x3A, 128)):
        for b in range(0x3A, 128):
            for c in range(0x3A, 128):
                for d in range(0x3A, 128):
                    key = b"onn" + by(a) + by(b) + by(c) + b"-" + by(d) + SUFFIX

                    if test(key):
                        return key

    for a in rich.progress.track(range(0x3A, 128)):
        for b in range(0x3A, 128):
            for c in range(0x3A, 128):
                for d in range(0x3A, 128):
                    key = by(a) + b"oon" + by(b) + by(c) + b"-" + by(d) + SUFFIX
                    
                    if test(key):
                        return key

    for a in rich.progress.track(range(0x3A, 128)):
        for b in range(0x3A, 128):
            for c in range(0x3A, 128):
                for d in range(0x3A, 128):
                    key = by(a) + by(b) + by(b) + b"on" + by(c) + b"-" + by(d) + SUFFIX
                    
                    if test(key):
                        return key

    for a in rich.progress.track(range(0x3A, 128)):
        for b in range(0x3A, 128):
            for c in range(0x3A, 128):
                for d in range(0x3A, 128):
                    key = by(a) + by(b) + by(b) + by(c) + b"on" + b"-" + by(d) + SUFFIX
                    
                    if test(key):
                        return key

KEY = brute_force()
assert KEY == b"allons-ygeronimo"

cipher = AES.new(KEY, AES.MODE_ECB)
out = cipher.decrypt(SECOND)
out += cipher.decrypt(FIRST)

print(out)

```

The resulting key "allons-ygeronimo" is a combination of two popular exclamations of the doctor, hurray!

Funny enough, I had a pretty much complete solve script lying around for a few hours before finally solving this challenge.
Turns out I simply mixed up the two decrypted blocks and accidentally checked the second block for the flag start, ouch!
It did help to look at the problem again with fresh energy later though and we got flag 😉

## PCaS

<Challenge name="PCaS" author="lukas" category="rev" solves={1} points={600} flag="CSR&lbrace;&rbrace;" downloads={pcas_downloads}>
I've heard critical aircraft performance data computations need to be done in the cloud nowadays using coding and algorithms. What could possibly go wrong?
</Challenge>

- Dropped a few hours before end
  - Some (including me) already had worked through the night
  - Managed to get first and only solve with team work
- .net C# binary
  - Contains dotnet information etc.
  - Can sometimes be decompiled with dotPeek, ILSpy etc.
  - 23 got us good result from dotPeek with C# files
- Airport controller
  - Processing of planes from loading to takeoff
  - Can generate a session ticket and than connect with ticket to same airport later
- Airport session
  - Goal to get flag, sadly as apology for inconvenience if all three runways are blocked with crashed airplanes
  - Gets free runway from airport
  - Session is big state machine after first/welcome message (draw as diagram)
    - WaitingForPlane:
      - Provide callsign (format; SPIN reference)
      - Get plane data (runway, number of containers, max takeoff weight) & reserve runway
    - PlaneAssigned:
      - Provide cargo data (weight and value separated by space, "END" after last line)
      - Minimum number of containers `NumberOfContainers / 2`
      - Loading plane (in class `Airport`; look at it in detail below)
      - Timeout if loading takes too long
      - Loaded, requesting taxi clearance (can be granted or denied, depends on runway availability)
    - WaitingForTaxi
      - State to retry requesting taxi clearance
      - Can cancel flight too (resets runway to state `Free`)
    - Taxiing
      - Attempts takeoff
      - Crash if loaded more than max takeoff weight
    - Takeoff/Crashed:
      - Final states
      - Simply urges user on before closing connection
  - Important points
    - Loading
    - Crash check at takeoff
    - Important stuff happens in `Airport`
- Loading plane
  - Code snippets for methods?
  - Starts thread with `DoWork` and waits for signal to retrieve result
  - Thread tries solving knapsack problem with `Solver` in `DoWork`, sets result and then signal
  - Solver guarantees result below max weight
- Race condition
  - Not made for simultaneous processing of multiple planes
  - Can connect multiple times with ticket and to same runway though, runway only reserved in `GetPlane`
  - Can start load at same time, first result set for all planes
  - Abuse this for our exploit
- Exploit
  - Spawn several connections including reserve connection
  - Send callsign for all but reserve (format; SPIN reference)
  - Get plane data
  - Send problem depending on max weight
    - More difficult problem for connections with small max weight (fraction of max weight, full number of containers)
    - Simple one for large max weight (minimum possible number, all max weight already)
  - Start simultaneously
  - Collect load configurations to find overloaded plane
  - If found check clearance
    - Possibly finish takeoff of wrong plane and cancel rest (resets to runway state free)
    - Set runway state to reserved again with reserve connection (sending callsign now)
    - Request clearance for overloaded plane
  - Try takeoff and crash overloaded plane at runway
  - Retry until all runways are blocked and we get the flag
  - Solve script
