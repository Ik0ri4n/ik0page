# Solving hints (no full solution)

## General hints

- Try to get an idea of what the binary does first (try it out, basic examinations)
- Focus on the relevant bits of information (C++ binaries contain a lot of bloated code from templates)
- Generate data structures (easier to understand and track than pointers)
- Double-check method signatures (some decompilers have problems with C++ code here)
- Trace input and ouput to find the relevant values and checks
- Don't give up! The binary grew larger than intended, just analyze it step by step 😉
- Test with the provided setup please!

I added some more specific hints in case you get stuck. Use them as you like.

## Initialization part

1. The method that uses your input is used for initialization if three checks apply.
   The third is most important for reversing.
   - If you use the version we deployed you can completely ignore the checks (see the description in the source file).
2. There is some custom conversion that might cause headaches. Sorry in advance! Maybe it helps to know that it uses specific bits...

## Backtracking part

1. Try to find the checks that are required for the flag case
2. Trace the required value references to find it's initialization and usage
