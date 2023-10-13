---
title: CyberSecurityRumble Quals & Finals 2023
excerpt: "After the forced pause this years edition of the CyberSecurityRumble finally returned with an onsite finals event. Let's look back at two interesting challenges!"
---

<script lang="ts">
	import Challenge from '$lib/components/Challenge.svelte';

    let exterminate_downloads = ["/blog/2023/06_rumble-23/exterminate.apk"];
    let pcas_downloads = ["/blog/2023/06_rumble-23/pcas", "/blog/2023/06_rumble-23/pcas_source.zip"];

    import exterminate_main_activity from "$lib/assets/2023/06_rumble-23/exterminate_main_activity.png";
    import exterminate_isCodeCorrect1 from "$lib/assets/2023/06_rumble-23/exterminate_isCodeCorrect1.png";
    import exterminate_isCodeCorrect2 from "$lib/assets/2023/06_rumble-23/exterminate_isCodeCorrect2.png";
    import pcas_airport_session from "$lib/assets/2023/06_rumble-23/pcas_airport_session.svg";
    import pcas_load_plane from "$lib/assets/2023/06_rumble-23/pcas_load_plane.png";
    import pcas_do_work from "$lib/assets/2023/06_rumble-23/pcas_do_work.png";
</script>

The CyberSecurityChallenge is a high quality CTF, specifically meant for the DACH community with a separate scoreboard and finals for the best DACH teams.
Interestingly, the CSR is intended for teams of 6 which is difficult to enforce for an online competition but perfect for onsite finals.
The qualifiers were a 24-hour competition with a great mix of challenges,
keeping the tension up with an additional batch of challenges released later into the competition.
Finally, the onsite finals had a competition with challenges made ba the sponsors and another great jeopardy CTF.

I participated with an active team from [KITCTF](https://kitctf.de) and we managed to score a close second place in the qualifiers 
and got the fourth place at the finals.
As last year, I really enjoyed the competition and meeting the other teams in Bonn for the finals made for a wonderful atmosphere.
For this post, I included a writeup for [Exterminate](#exterminate), a relatively easy android reversing challenge from the finals.
As a bonus, I finished a full writeup for [PCaS](#pcas) I had lying around for a few months now.
I really enjoyed the challenges and I hope you can share my excitement for reversing with these two writeups!

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

<div class="h-96 relative [&>*]:flex [&>*]:h-full [&>*>img]:my-0">

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

<Challenge name="PCaS" author="lukas" category="rev" solves={1} points={600} flag="CSR&lbrace;1_hop3_y0u_4lso_l1ke_alg0rithm5&rbrace;" downloads={pcas_downloads}>
I've heard critical aircraft performance data computations need to be done in the cloud nowadays using coding and algorithms. What could possibly go wrong?
</Challenge>

This challenge was released with the final batch of challenges of the qualifiers, 6 hours before end.
Some of our team (including, but not limited to, me) had already worked pretty much through the night, 
but we anticipated the challenges eagerly.
Thankfully, we managed to get the first and only solve for this challenge with great team work.

The challenge provides us with a .NET Core C# binary (for example see all the .NET symbols with `strings`).
With luck, you can decompile such binaries with tools like dotPeek or ILSpy.
[Liam](https://wachter-space.de) quickly realized this and exported an archive of C# files from dotPeek for us.

Interacting a bit with the program (you can connect to a local instance at port 3284) we can generate a session ticket and are then greeted by the system:

> Welcome to Campbell Airstrip, Anchorage.
>
> Runway 1 is available. Please provide a callsign:

With the challenge description I already found a reference to the name [PCaS](https://en.wikipedia.org/wiki/Portable_collision_avoidance_system), 
setting the theme for the challenge.
Specifically, the challenge is implemented as a kind of airport controller, processing planes from loading to takeoff.

Looking at `AirportSession.cs`, we find that we (sadly) get the flag as an apology if all runways are blocked with crashed airplanes.
The `AirportSession` is a big state machine handling the flow of processing a plane (see diagram).

![AirportSession processing state machine]({pcas_airport_session})

The processing contains some important information:

- Callsigns must match the regex `r"^[A-Z]{3}[A-Z0-9]{1,}$"`
- Plane data contains runway, number of containers & max takeoff weight
- We need to provide a minimum of `NumberOfContainers / 2`, to stay economical of course
- Loading can run into a timeout
- There is a crash check at takeoff

![LoadPlane function in Airport]({pcas_load_plane})

The most important logic is implemented in `Aiport.cs` though.
When loading a plane, the `LoadPlane` method starts a worker thread and weights for a signal before retrieving and returning the result.
The method `DoWork` tries to get the optimal loading configuration with a greedy knapsack solver and is cancelled after 15 seconds.
Sadly, the `Solver` does enforce the maximum takeoff weight.
When the worker thread gets a result, it sets a static `_result` variable and then sets the signal.

![DoWork function in Airport]({pcas_do_work})

Notably, all the `Airport` code is implemented with threads but is not designed to simultaneously process multiple planes at the same `Airport`.
We can however connect multiple times to the same aiport with our ticket, 
even to the same runway because it only reserved in `Airport.GetPlane`, after providing a callsign in the session.
Thus we can start loading multiple planes at nearly the same time, and the first completed result will be set for all planes.
We abuse this race condition for our exploit.

Our exploit strategy is as follows:

- Spawn several connections including reserve connection
- Send callsign for all but reserve (I had to use "SPIN", maybe you'll get the reference)
- Get plane data
- Send problem depending on max weight
  - More difficult problem for connections with small max weight (fraction of max weight, full number of containers; not too complex because of timeout)
  - Simple one for large max weight (minimum possible number, all max weight already; quick solve)
- Start simultaneously
- Collect load configurations to find overloaded plane
- If found check clearance
  - Possibly finish takeoff of wrong plane and cancel rest (resets to runway state `Free` 😬)
  - Set runway state to reserved again with reserve connection (sending callsign now)
  - Request clearance for overloaded plane
- Try takeoff and crash overloaded plane at runway
- Retry until all runways are blocked and we get the flag

I felt the need to write a well structured exploit for this problem to avoid implementation problems, 
but that is of course handy for sharing the solution with you.
You'll find it as my [PCaS exploit gist](https://gist.github.com/Ik0ri4n/8bea87b96cff96316ee857058695eee0), 
you'll need to replace `rumble.host` with `localhost` though.
Big thanks to Lukas, the author, I really enjoyed analyzing the challenge and implementing the exploit!
Also thanks to [Martin](https://blog.martinwagner.co/) for supporting me with the edge cases and helping me keep my sanity.
