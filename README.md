# Pattern-Avoiding Permutation Enumeration

Software to enumerate pattern avoiding permutations by an algorithm based on RotPiDD.

## How to use

1. Copy [SAPPOROBDD](https://github.com/Shin-ichi-Minato/SAPPOROBDD) under PatternAvoiding directory and follow the install instructions in the linked GitHub repository.
   - This program assumes you run `INSTALL`, not `INSTALL32` in the instructions.

2. Move to `PatternAvoidingCount` directory and run `make` on command line.
   - You can `make clean` to remove all the compiled files.

3. Run `./avoid.exe [N] [perm]`.
   Where `[N]` is the length of pattern avoiding permutations you want to know their count.
   `[perm]` is a pattern permutation must be avoided. The format is `p_1+p_2+...+p_k`.
   - Example command: `./avoid.exe 16 3+1+2+4+5`
     - This command outputs the number of permutations with length 16 which avoid permutation 31245.
   - This program also accepts adjacent patterns:
      - Use `_` instead of `+`. If you put `_` between the i-th element and (i+1)-th element, they must be adjacent in the pattern occurrences. For example, 1423 avoids `1_2+3` because 1 and 2 are not adjacent, but 1423 contains `1+2_3` because 2 and 3 are adjacent.
