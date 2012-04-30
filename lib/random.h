! ==============================================================================
! random.h -- a portable and predictable replacement for the standard system
!             'random' function.
!
! Version 1.3 (Feb06) by Roger Firth (roger@firthworks.com)
!                     based on an algorithm by George Marsaglia translated
!                     into Inform by Richard Bos (raltbos@xs4all.nl).
!
! Compatibility: for Inform 6.3 (Z-code and Glulx).
!
! Dependencies: none.
!
! License: The Creative Commons Attribution-ShareAlike License published at
! http://creativecommons.org/licenses/by-sa/2.5/ applies to this software.
! In summary: you must credit the original author(s); if you alter, transform,
! or build upon this software, you may distribute the SOURCE FORM of the
! resulting work only under a license identical to this one. Note that the
! ShareAlike clause does not affect the way in which you distribute the COMPILED
! FORM of works built upon this software.
! Copyright remains with the original author(s), from whom you must seek
! permission if you wish to vary any of the terms of this license.
! The author(s) would also welcome bug reports and enhancement suggestions.
!
! ------------------------------------------------------------------------------
! INSTALLATION
!
! Place this line before Parser.h:
!
!   Replace random;
!
! Place this line after Grammar.h:
!
!   Include "random";
!
! ------------------------------------------------------------------------------
! USAGE
!
! The library replaces the 'random' function which is built into the standard
! Inform system, and provides similar capabilities:
!
!   random(N)               returns a random number in the range 1..N.
!   random(a1,a2, ... aN)   returns a random choice from the given selection.
!
!   random(0)               initialises the seed unpredictably, and returns zero.
!
!   random(-S)              initialises the seed repeatably, and returns zero;
!                           this is needed only to avoid unpredictable random
!                           numbers (the default). As suggested in Section 2.4
!                           of the Z-machine Standard, the algorithm is:
!                           - if the absolute value of S is 1..999, the 'random'
!                           sequence is 1,2,3, ... S,1,2,3, ... S,1,2,3, ...
!                           and each random(N) returns the next entry in this
!                           sequence, modulo N.
!                           - if the absolute value of S is 1000..32767, each
!                           random(N) returns the next entry in the random
!                           sequence starting at S, modulo N.
!
! In DEBUG mode, the verbs RANDOM and RANDOM R[EPEATABLE] issue the call
! random(-1000) to seed a repeatable random sequence. The verb RANDOM S[ERIAL]
! issues the call random(-100) to seed a repeating rising sequence.
! The verb RANDOM U[NPREDICTABLE] reverts to unpredictable random numbers.
!
! ------------------------------------------------------------------------------
! COMPATIBILITY
!
! 1.    In the standard function, the RANDOM verb has only one option: to seed a
!       repeating rising sequence _or_ a repeatable random sequence (depending
!       on the interpreter being used). Here, RANDOM _always_ seeds a repeatable
!       random sequence, and the player must use RANDOM S[ERIAL] to obtain a
!       repeating rising sequence.
!
! 2.    In the standard function, the number of arguments to random(a1,a2, ... aN)
!       is unspecified. Here, the limit is seven.
!
! 3.    In the standard function, the arguments to random(a1,a2, ... aN) must
!       all be constants. Here, they can also be variables.
!
! 4.    In the standard function, the final argument to random(a1,a2, ... aN)
!       can be a zero. Here, the final argument must be non-zero. Thus, change
!       random(6,4,2,0) to random(6,4,0,2).
!
! 5.    In the standard function, the call random() is not permitted. Here, it
!       is identical to random(0), setting the random seed to an unpredictable
!       value.
!
! 6.    The use of the @log_shift instruction means that this code won't compile
!       to Z-code Version 3. To get round this, you could replace the calls to
!       shiftL() and shiftR() by multiplication and division statements.
!
! ------------------------------------------------------------------------------
! NOTES
!
! 1.    Normally, the system function performs adequately. You will probably
!       need this replacement only in exceptional circumstances, especially as
!       it is likely to be significantly slower than the original (though in
!       typical usage, you probably won't notice the difference).
!
! 2.    The generation of random numbers is a function provided by the
!       Z-machine and Glulx interpreters. If you discover an interpreter with
!       a poor generator, you may wish to replace it with this one, which
!       provides a good random spread of numbers.
!
! 3.    Because each interpreter's generator is different, the same game will
!       obtain different random numbers when played on different interpreters.
!       For test purposes, or to support a walkthrough of a demonstration game,
!       you may wish to obtain exactly the same random behaviour, irrespective
!       of the platform on which the interpreter is running (and also
!       irrespective of whether you're using the Z-machine or Glulx).
!
! 4.    Before Including this file, you can define:
!
!           Constant RANDOM_SERIAL_DEFAULT   = 100;
!           Constant RANDOM_SERIAL_THRESHOLD = 1000;
!
!       to override the standard values. If you set RANDOM_SERIAL_THRESHOLD
!       to 1, the RANDOM S[ERIAL] repeating rising sequence is disabled.
!
! ------------------------------------------------------------------------------
! HISTORY
!
! Version 1.3: (Feb06) Changed RANDOM default to be 'repeatable'.
! Version 1.2: (Feb06) Improved comments suggested by Richard Bos.
! Version 1.1: (Jan06) Incorporated 'rising sequence' feature.
! Version 1.0: (Jan06) Original by Roger Firth.
!
! ==============================================================================
! Replacement for system 'random' routine

System_file;

Array  randomArgs --> 8;                ! randomArgs-->0 not used

[ random a1 a2 a3 a4 a5 a6 a7
    n;                                  ! number of arguments to routine
    randomArgs-->1 = a1; randomArgs-->2 = a2; randomArgs-->3 = a3;
    randomArgs-->4 = a4; randomArgs-->5 = a5; randomArgs-->6 = a6;
    randomArgs-->7 = a7;
    for (n=7 : n>0 : n--) if (randomArgs-->n) break;
    if (n > 1) return randomArgs-->rnd(n);
    return rnd(a1);
];

! ------------------------------------------------------------------------------
! Random Number Generator

Default RANDOM_SERIAL_DEFAULT   100;
Default RANDOM_SERIAL_THRESHOLD 1000;

Global randomHiSeed;                    ! High 15 bits
Global randomMdSeed;                    ! Middle 2 bits
Global randomLoSeed;                    ! Low 15 bits
Global randomSerial;                    ! true: generate rising sequence

! The 32-bit seed (previous random number) is held as three Inform integers.
! There is nothing magical about the above division, except that it makes both
! the middle right-shift and the return statement simpler. This is a bit of
! serendipity, because the right-shift is by 17 bits, which just happens to be
! the 32-bit complement of the 15 bits needed for an Inform integer.
!
! George Marsaglia calls this a 3-shift shift register RNG, and says in
! message id WAEV7.5401$i8.940730@news1.rdc1.fl.home.com
! (look this up at, for example, http://groups.google.com/advanced_search)
!
! Given an initial 32-bit integer y, randomly selected from the set of
! unsigned values S={1,2,...,2**32-1}, the application of
!     y = y ^ (y<<13);  y = y ^ (y>>17);  y = y ^ (y<<5);
! (where '^' is the bitwise exclusive OR operator, '<<' is the logical left
! shift operator and '>>' is the logical right shift operator) will produce
! a sequence of 2**32-1 different random integers, each uniformly distributed
! over S. They will not be independent uniform choices from S, but will seem
! to behave as though they were, satisfying all but the most demanding tests
! of randomness, much better than do the more commonly used congruential RNGs.

[ rnd value
    tmp;

    if (value < 0) {                    ! seed a serial/predictable sequence
        value = -value;
        randomHiSeed = value; randomMdSeed = 0; randomLoSeed = value;
        if (value < RANDOM_SERIAL_THRESHOLD) randomSerial = true;
        else                                 randomSerial = false;
        rfalse;
    }

    if (value == 0) {                   ! seed an unpredictable sequence
        randomiseSeed();
        rfalse;
    }

    if (randomSerial) {                 ! generate next serial value
        if (++randomLoSeed >= randomHiSeed) randomLoSeed = 0;
    }
    else {                              ! generate next random value
        if (randomHiSeed|randomMdSeed|randomLoSeed == 0) randomiseSeed();

        ! Left-shift by 13 places
        ! Seed: 0ABCDEFGHIJKLMNO 00000000000000Pa 0bcdefghijklmnop
        ! Mask: 0NOPabcdefghijkl 00000000000000mn 0op0000000000000

        tmp = shiftL(randomHiSeed&$0003,13) | shiftL(randomMdSeed&$0003,11) | shiftR(randomLoSeed&$7FF0,4);
        randomHiSeed = xor(randomHiSeed, tmp);
        randomMdSeed = xor(randomMdSeed, shiftR(randomLoSeed&$000C,2));
        randomLoSeed = xor(randomLoSeed, shiftL(randomLoSeed&$0003,13));

        ! Right-shift by 17 places
        ! Seed: 0ABCDEFGHIJKLMNO 00000000000000Pa 0bcdefghijklmnop
        ! Mask: 0000000000000000 0000000000000000 0ABCDEFGHIJKLMNO

        !randomHiSeed = xor(randomHiSeed, 0);
        !randomMdSeed = xor(randomMdSeed, 0);
        randomLoSeed = xor(randomLoSeed, randomHiSeed);

        ! Left-shift by 5 places
        ! Seed: 0ABCDEFGHIJKLMNO 00000000000000Pa 0bcdefghijklmnop
        ! Mask: 0FGHIJKLMNOPabcd 00000000000000ef 0ghijklmnop00000

        tmp = shiftL(randomHiSeed&$03FF,5) | shiftL(randomMdSeed&$0003,3) | shiftR(randomLoSeed&$7000,12);
        randomHiSeed = xor(randomHiSeed, tmp);
        randomMdSeed = xor(randomMdSeed, shiftR(randomLoSeed&$0C00,10));
        randomLoSeed = xor(randomLoSeed, shiftL(randomLoSeed&$03FF,5));
    }

    ! Return 15-bit value based on randomLoSeed

    return (randomLoSeed % value) + 1;
];

! ------------------------------------------------------------------------------
! Low-level utility functions

#Ifdef TARGET_ZCODE;

[ randomiseSeed;
    @random 32767 randomHiSeed;
    @random 3     randomMdSeed;
    @random 32767 randomLoSeed;
    randomSerial = false;
];

[ shiftL a n r; @log_shift a n r; return r; ];

[ shiftR a n r; n = -n; @log_shift a n r; return r; ];

[ xor a b;      return (a|b) & (~(a&b)); ];

#Ifnot; ! TARGET_GLULX

[ randomiseSeed;
    @random 32767 randomHiSeed; randomHiSeed++;
    @random 3     randomMdSeed; randomMdSeed++;
    @random 32767 randomLoSeed; randomLoSeed++;
    randomSerial = false;
];

[ shiftL a n r; @shiftl a n r;  return r; ];

[ shiftR a n r; @ushiftr a n r; return r; ];

[ xor a b r;    @bitxor a b r;  return r; ];

#Endif; ! TARGET_

! ------------------------------------------------------------------------------
! Enhanced RANDOM verb

#Ifdef DEBUG;

[ RandomUnpredictableSub;
    noun = 0;
    RandomSeedSub();
];

[ RandomSerialSub;
    noun = RANDOM_SERIAL_DEFAULT;
    RandomSeedSub();
];

[ RandomRepeatableSub;
    noun = RANDOM_SERIAL_THRESHOLD;
    RandomSeedSub();
];

[ RandomSeedSub;
    if (noun < 0) noun = -noun;
    random(-noun);
    switch (noun) {
      0:
        "[Random number generator now unpredictable.]";
      1 to RANDOM_SERIAL_THRESHOLD-1:
        "[Random number generator now serial.]";
      default:
        "[Random number generator now repeatable.]";
    }
];

Extend 'random' replace
    *                                   -> RandomRepeatable
    * 'serial'/'s//'                    -> RandomSerial
    * 'repeatable'/'r//'                -> RandomRepeatable
    * 'unpredictable'/'u//'             -> RandomUnpredictable
    * number                            -> RandomSeed;

#Endif; ! DEBUG

! ==============================================================================
