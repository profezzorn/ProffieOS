#ifndef SOUND_TALKIE_H
#define SOUND_TALKIE_H

// This is adapted from the Talkie Library
#include "audiostream.h"
#include "../common/command_parser.h"
#include "voice_data.h"

#define MAX_K                   10
#define MAX_SCALE_BITS          6
#define MAX_SCALE               (1<<MAX_SCALE_BITS)
#define MAX_CHIRP_SIZE          52

struct tms5100_coeffs
{
  int             num_k;
  int             energy_bits;
  int             pitch_bits;
  int             kbits[MAX_K];
  unsigned short  energytable[MAX_SCALE];
  unsigned short  pitchtable[MAX_SCALE];
  int             ktable[MAX_K][MAX_SCALE];
  int16_t        chirptable[MAX_CHIRP_SIZE];
};

/* common, shared coefficients */
/* energy */
#define TI_0280_PATENT_ENERGY \
    /* E  */\
    {   0,  0,  1,  1,  2,  3,  5,  7, \
      10, 15, 21, 30, 43, 61, 86, 0 },

#define TI_028X_LATER_ENERGY \
    /* E  */\
    {   0,  1,  2,  3,  4,  6,  8, 11, \
      16, 23, 33, 47, 63, 85,114, 0 },

/* pitch */
#define TI_0280_2801_PATENT_PITCH \
  /* P  */\
  {  0,   41,  43,  45,  47,  49,  51,  53,  \
    55,  58,  60,  63,  66,  70,  73,  76,  \
    79,  83,  87,  90,  94,  99,  103, 107,  \
    112, 118, 123, 129, 134, 140, 147, 153 },

#define TI_2802_PITCH \
  /* P */\
  {   0,  16,  18,  19,  21,  24,  26,  28,  \
    31,  35,  37,  42,  44,  47,  50,  53,  \
    56,  59,  63,  67,  71,  75,  79,  84,  \
    89,  94, 100, 106, 112, 126, 141, 150},

#define TI_5110_PITCH \
  /* P */\
  {   0,  15,  16,  17,  19,  21,  22,  25,  \
    26,  29,  32,  36,  40,  42,  46,  50,  \
    55,  60,  64,  68,  72,  76,  80,  84,  \
    86,  93, 101, 110, 120, 132, 144, 159},

#define TI_2501E_PITCH \
  /* P */\
  {   0,  14,  15,  16,  17,  18,  19,  20,  \
    21,  22,  23,  24,  25,  26,  27,  28,  \
    29,  30,  31,  32,  34,  36,  38,  40,  \
    41,  43,  45,  48,  49,  51,  54,  55,  \
    57,  60,  62,  64,  68,  72,  74,  76,  \
    81,  85,  87,  90,  96,  99, 103, 107, \
    112, 117, 122, 127, 133, 139, 145, 151, \
    157, 164, 171, 178, 186, 194, 202, 211},

#define TI_5220_PITCH \
  /* P */\
  {   0,  15,  16,  17,  18,  19,  20,  21,  \
    22,  23,  24,  25,  26,  27,  28,  29,  \
    30,  31,  32,  33,  34,  35,  36,  37,  \
    38,  39,  40,  41,  42,  44,  46,  48,  \
    50,  52,  53,  56,  58,  60,  62,  65,  \
    68,  70,  72,  76,  78,  80,  84,  86,  \
    91,  94,  98, 101, 105, 109, 114, 118, \
    122, 127, 132, 137, 142, 148, 153, 159},

/* LPC */
#define TI_0280_PATENT_LPC \
    /* K1  */\
    { -501, -497, -493, -488, -480, -471, -460, -446,\
      -427, -405, -378, -344, -305, -259, -206, -148,\
      -86,  -21,   45,  110,  171,  227,  277,  320,\
      357,  388,  413,  434,  451,  464,  474,  498 },\
    /* K2  */\
    { -349, -328, -305, -280, -252, -223, -192, -158,\
      -124,  -88,  -51,  -14,  23,    60,   97,  133,\
      167,  199,  230,  259,  286,  310,  333,  354,\
      372,  389,  404,  417,  429,  439,  449,  506 },\
    /* K3  */\
    { -397, -365, -327, -282, -229, -170, -104, -36,\
      35,  104,  169,  228,  281,  326,  364, 396 },\
    /* K4  */\
    { -369, -334, -293, -245, -191, -131, -67,  -1,\
      64,  128,  188,  243,  291,  332, 367, 397 },\
    /* K5  */\
    { -319, -286, -250, -211, -168, -122, -74, -25,\
      24,   73,  121,  167,  210,  249, 285, 318 },\
    /* K6  */\
    { -290, -252, -209, -163, -114,  -62,  -9,  44,\
      97,  147,  194,  238,  278,  313, 344, 371 },\
    /* K7  */\
    { -291, -256, -216, -174, -128, -80, -31,  19,\
      69,  117,  163,  206,  246, 283, 316, 345 },\
    /* K8  */\
    { -218, -133,  -38,  59,  152,  235, 305, 361 },\
    /* K9  */\
    { -226, -157,  -82,  -3,   76,  151, 220, 280 },\
    /* K10 */\
    { -179, -122,  -61,    1,   62,  123, 179, 231 },

#define TI_2801_2501E_LPC \
    /* K1  */\
    { -501, -498, -495, -490, -485, -478, -469, -459,\
      -446, -431, -412, -389, -362, -331, -295, -253,\
      -207, -156, -102,  -45,   13,   70,  126,  179,\
      228,  272,  311,  345,  374,  399,  420,  437 },\
    /* K2  */\
    { -376, -357, -335, -312, -286, -258, -227, -195,\
      -161, -124,  -87,  -49,  -10,   29,   68,  106,\
      143,  178,  212,  243,  272,  299,  324,  346,\
      366,  384,  400,  414,  427,  438,  448,  506 },\
    /* K3  */\
    { -407, -381, -349, -311, -268, -218, -162, -102,\
      -39,   25,   89,  149,  206,  257,  302,  341 },\
    /* K4  */\
    { -290, -252, -209, -163, -114,  -62,   -9,   44,\
      97,  147,  194,  238,  278,  313,  344,  371 },\
    /* K5  */\
    { -318, -283, -245, -202, -156, -107,  -56,   -3,\
      49,  101,  150,  196,  239,  278,  313,  344 },\
    /* K6  */\
    { -193, -152, -109,  -65,  -20,   26,   71,  115,\
      158,  198,  235,  270,  301,  330,  355,  377 },\
    /* K7  */\
    { -254, -218, -180, -140,  -97,  -53,   -8,   36,\
      81,  124,  165,  204,  240,  274,  304,  332 },\
    /* K8  */\
    { -205, -112,  -10,   92,  187,  269,  336,  387 },\
    /* K9  */\
    { -249, -183, -110,  -32,   48,  126,  198,  261 }, /* on patents 4,403,965 and 4,946,391 the 4th entry is 0x3ED (-19) which is a typo of the correct value of 0x3E0 (-32)*/\
    /* K10 */\
    { -190, -133,  -73,  -10,   53,  115,  173,  227 },

// below is the same as 2801/2501E above EXCEPT for K4 which is completely different.
#define TI_2802_LPC \
    /* K1 */\
    { -501, -498, -495, -490, -485, -478, -469, -459,\
      -446, -431, -412, -389, -362, -331, -295, -253,\
      -207, -156, -102,  -45,   13,   70,  126,  179,\
      228,  272,  311,  345,  374,  399,  420,  437},\
    /* K2 */\
    { -376, -357, -335, -312, -286, -258, -227, -195,\
      -161, -124,  -87,  -49,  -10,   29,   68,  106,\
      143,  178,  212,  243,  272,  299,  324,  346,\
      366,  384,  400,  414,  427,  438,  448,  506},\
    /* K3 */\
    { -407, -381, -349, -311, -268, -218, -162, -102,\
      -39,   25,   89,  149,  206,  257,  302,  341},\
    /* K4 */\
    { -289, -248, -202, -152,  -98,  -43,   14,   71,\
      125,  177,  225,  269,  307,  341,  371,  506},\
    /* K5 */\
    { -318, -283, -245, -202, -156, -107,  -56,   -3,\
      49,  101,  150,  196,  239,  278,  313,  344},\
    /* K6 */\
    { -193, -152, -109,  -65,  -20,   26,   71,  115,\
      158,  198,  235,  270,  301,  330,  355,  377},\
    /* K7 */\
    { -254, -218, -180, -140,  -97,  -53,   -8,   36,\
      81,  124,  165,  204,  240,  274,  304,  332},\
    /* K8 */\
    { -205, -112,  -10,   92,  187,  269,  336,  387},\
    /* K9 */\
    { -249, -183, -110,  -32,   48,  126,  198,  261},\
    /* K10 */\
    { -190, -133,  -73,  -10,   53,  115,  173,  227},

#define TI_5110_5220_LPC \
    /* K1  */\
    { -501, -498, -497, -495, -493, -491, -488, -482,\
      -478, -474, -469, -464, -459, -452, -445, -437,\
      -412, -380, -339, -288, -227, -158,  -81,   -1,\
      80,  157,  226,  287,  337,  379,  411,  436 },\
    /* K2  */\
    { -328, -303, -274, -244, -211, -175, -138,  -99,\
      -59,  -18,   24,   64,  105,  143,  180,  215,\
      248,  278,  306,  331,  354,  374,  392,  408,\
      422,  435,  445,  455,  463,  470,  476,  506 },\
    /* K3  */\
    { -441, -387, -333, -279, -225, -171, -117,  -63,\
      -9,   45,   98,  152,  206,  260,  314,  368  },\
    /* K4  */\
    { -328, -273, -217, -161, -106,  -50,    5,   61,\
      116,  172,  228,  283,  339,  394,  450,  506  },\
    /* K5  */\
    { -328, -282, -235, -189, -142,  -96,  -50,   -3,\
      43,   90,  136,  182,  229,  275,  322,  368  },\
    /* K6  */\
    { -256, -212, -168, -123,  -79,  -35,   10,   54,\
      98,  143,  187,  232,  276,  320,  365,  409  },\
    /* K7  */\
    { -308, -260, -212, -164, -117,  -69,  -21,   27,\
      75,  122,  170,  218,  266,  314,  361,  409  },\
    /* K8  */\
    { -256, -161,  -66,   29,  124,  219,  314,  409  },\
    /* K9  */\
    { -256, -176,  -96,  -15,   65,  146,  226,  307  },\
    /* K10 */\
    { -205, -132,  -59,   14,   87,  160,  234,  307  },

/* chirp */
#define TI_0280_PATENT_CHIRP \
  /* Chirp table */\
  {   0x00, 0x2a, 0xd4, 0x32, 0xb2, 0x12, 0x25, 0x14,\
    0x02, 0xe1, 0xc5, 0x02, 0x5f, 0x5a, 0x05, 0x0f,\
    0x26, 0xfc, 0xa5, 0xa5, 0xd6, 0xdd, 0xdc, 0xfc,\
    0x25, 0x2b, 0x22, 0x21, 0x0f, 0xff, 0xf8, 0xee,\
    0xed, 0xef, 0xf7, 0xf6, 0xfa, 0x00, 0x03, 0x02,\
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00 },

// almost, but not exactly the same as the patent chirp above (25 bits differ)
#define TI_2801_CHIRP \
  /* Chirp table */\
  {   0x00, 0x2b, 0xd4, 0x33, 0xb3, 0x12, 0x25, 0x14,\
    0x02, 0xe2, 0xc6, 0x03, 0x60, 0x5b, 0x05, 0x0f,\
    0x26, 0xfc, 0xa6, 0xa5, 0xd6, 0xdd, 0xdd, 0xfd,\
    0x25, 0x2b, 0x23, 0x22, 0x0f, 0xff, 0xf8, 0xef,\
    0xed, 0xef, 0xf7, 0xf7, 0xfa, 0x01, 0x04, 0x03,\
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00 },

#define TI_2802_CHIRP \
  /* Chirp table */\
  {   0x00, 0xa5, 0xbd, 0xee, 0x34, 0x73, 0x7e, 0x3d,\
    0xe8, 0xea, 0x34, 0x24, 0xd1, 0x01, 0x13, 0xc3,\
    0x0c, 0xd2, 0xe7, 0xdd, 0xd9, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00 },

#define TI_LATER_CHIRP \
  /* Chirp table */\
  {   0x00, 0x03, 0x0f, 0x28, 0x4c, 0x6c, 0x71, 0x50,\
    0x25, 0x26, 0x4c, 0x44, 0x1a, 0x32, 0x3b, 0x13,\
    0x37, 0x1a, 0x25, 0x1f, 0x1d, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
    0x00, 0x00, 0x00, 0x00 },

/* Interpolation Table */
#define TI_INTERP \
  /* interpolation shift coefficients */\
  { 0, 3, 3, 3, 2, 2, 1, 1 }

/* TMS5100/TMC0281:
   (Die revs A, B; 1977?-1981?)
   The TMS5100NL was decapped and imaged by digshadow in April, 2013.
    The LPC table is verified to match the decap.
    It also matches the intended contents of US Patent 4,209,836 and several others.
    The chirp table is verified to match the decap, and also matches the patents.
   In April, 2013, digshadow decapped a TMS5100 chip from 1980: http://siliconpr0n.org/map/ti/tms5100nl/
    The TMS5100 had the die markings: "0281 B  281A"
   In December 2014, Sean riddle decapped a TMC0281 chip from 1978 from an early speak
   and spell.
    The TMC0281 die had the die markings "0281 A  281"
    The chirp table matches what digshadow had decapped earlier.
    The LPC table hasn't been fully typed yet.
   Digitally dumped via PROMOUT by PlgDavid in 2014 for verification.
*/
static const struct tms5100_coeffs T0280B_0281A_coeff =
{
  10,
  4,
  5,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_0280_PATENT_ENERGY
  TI_0280_2801_PATENT_PITCH
  {
  TI_0280_PATENT_LPC
  },
  TI_0280_PATENT_CHIRP
};

/* TMS5110A/TMC0281D:
   This chip is used on the later speak & spell, and speak & spell compact;
   The energy table differs from the original tmc0281/tms5100, as does the interpolation behavior,
   which is the 'alternate' behavior.
   The chips have datecodes in the 1983-1984 range, probably 1982 also.
   Digitally dumped via PROMOUT by PlgDavid in 2014
   */
static const struct tms5100_coeffs T0280D_0281D_coeff =
{
  10,
  4,
  5,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_0280_2801_PATENT_PITCH
  {
  TI_0280_PATENT_LPC
  },
  TI_0280_PATENT_CHIRP
};

/* TMC0280/CD2801:
   Used in the Speak & Math, Speak & Read, and Language Translator
   Decapped by Digshadow in 2014 http://siliconpr0n.org/map/ti/tmc0280fnl/
   Digitally dumped via PROMOUT by PlgDavid in 2014
   The coefficients are exactly the same as the TMS5200.
   The coefficients also come from US Patents 4,403,965, 4,631,748 and
   4,946,391 (with one typo in all 3 patents: K9(3) is 0x3E0, not 0x3ED).
   The chirp table is very slightly different from the 4,209,836 patent one,
   but matches the table in the 4,403,965 and 4,946,391 patents.
   The Mitsubishi M58817 also seems to work best with these coefficients, so
   it is possible the engineers of that chip copied them from the TI patents.
   ***TODO: there are 2 versions of this chip, and the interpolation
      behavior between the two differs slightly:
   * TMC0280NLP // CD2801 with datecodes around 1980 has the same
     interpolation inhibit behavior as 5100/TMC0281 on unvoiced->silent
     transition.
   * CD2801A-NL with datecodes around 1982 have the 'alternate behavior',
     which seems to be a somewhat crude workaround for a bug where when
     an unvoiced frame follows a silent one (which in turn followed a
     voiced frame), the k5-k10 parameters are not zeroed as they should be,
     producing a loud noise.
     This bug is fixed correctly on the tms52xx chips.
   */
static const struct tms5100_coeffs T0280F_2801A_coeff =
{
  10,
  4,
  5,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_0280_2801_PATENT_PITCH
  {
  TI_2801_2501E_LPC
  },
  TI_2801_CHIRP
};

/* Mitsubishi M58817
The Mitsubishi M58817 seems to have (partly?) copied the coefficients from the
TMC0280/CD2801 above, but has some slight differences to it within the chip:
the main accumulator seems to have 1 extra bit and the digital values are
tapped 1 bit higher than on the TI chips. This is emulated within tms5110.c
   */
static const struct tms5100_coeffs M58817_coeff =
{
  10,
  4,
  5,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_0280_2801_PATENT_PITCH
  {
  TI_2801_2501E_LPC
  },
  TI_2801_CHIRP
};

/* CD2802:
   (1984 era?)
   Used in Touch and Tell only (and Vocaid), this chip has a unique pitch, LPC and chirp table.
   Has the 'alternate' interpolation behavior.
   Digitally dumped via PROMOUT by PlgDavid in 2014
   Decapped by Sean Riddle in 2015
   */
static const struct tms5100_coeffs T0280F_2802_coeff =
{
  10,
  4,
  5,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_2802_PITCH
  {
  TI_2802_LPC
  },
  TI_2802_CHIRP
};

/* TMS5110A:
   (1984-90 era? early chips may be called TMS5110C; later chips past 1988 or so may be called TSP5110A)
   The TMS5110A LPC coefficients were originally read from an actual TMS5110A
   chip by Jarek Burczynski using the PROMOUT pin, later verified/redumped
   by PlgDavid.
   NullMoogleCable decapped a TMS5110AN2L in 2015: http://wtfmoogle.com/wp-content/uploads/2015/03/0317_1.jpg
   which was used to verify the chirp table.
   The slightly older but otherwise identical TMS5111NLL was decapped and imaged by digshadow in April, 2013,
   its die is marked "TMS5110AJ"
   The LPC table is verified from decap to match the values from Jarek and PlgDavid's PROMOUT dumps of the TMS5110.
   The LPC table matches that of the TMS5220.
   It uses the 'newer' 5200-style chirp table.
   It has the 'alternate' interpolation behavor (tested on 5110a; 5111 behavior is unknown)
*/
static const struct tms5100_coeffs tms5110a_coeff =
{
  10,
  4,
  5,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_5110_PITCH
  {
  TI_5110_5220_LPC
  },
  TI_LATER_CHIRP
};

/* The following coefficients come from US Patent 4,335,277 and 4,581,757.
However, the K10 row of coefficients are entirely missing from both of those
patents.
The K values don't match the values read from any of the TI chips so far, but
might match some other undiscovered chip? Or may be complete garbage put as a red
herring in the patent?
*/
  // k* is followed by d if done transcription, c if checked for derivative aberrations
static const struct tms5100_coeffs pat4335277_coeff =
{
  10,
  4,
  6,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_2501E_PITCH
  {
    /* K1dc  */
    { -507, -505, -503, -501, -497, -493, -488, -481,
      -473, -463, -450, -434, -414, -390, -362, -328,
      -288, -242, -191, -135,  -75,  -13,   49,  110,
      168,  221,  269,  311,  348,  379,  404,  426 },
    /* K2dc  */
    { -294, -266, -235, -202, -167, -130,  -92,  -52,
      -12,   28,   68,  108,  145,  182,  216,  248,
      278,  305,  330,  352,  372,  390,  406,  420,
      432,  443,  453,  461,  468,  474,  479,  486 },
    /* K3dc  */
    { -449, -432, -411, -385, -354, -317, -273, -223,
      -167, -107,  -43,   22,   87,  148,  206,  258 },
    /* K4dc (first 4-5 values are probably wrong but close) */
    { -321, -270, -220, -157,  -97,  -40,   25,   89,
      150,  207,  259,  304,  343,  376,  403,  425 },
    /* K5dc  */
    { -373, -347, -318, -284, -247, -206, -162, -115,
      -65,  -15,   36,   86,  135,  181,  224,  263 },
    /* K6dc  */
    { -213, -176, -137,  -96,  -54,  -11,   33,   75,
      117,  157,  195,  231,  264,  294,  322,  347 },
    /* K7dc  */
    { -294, -264, -232, -198, -161, -122,  -82,  -41,
        1,   43,   84,  125,  163,  200,  234,  266 },
    /* K8dc  */
    { -195, -117,  -32,   54,  137,  213,  279,  335 },
    /* K9dc  */
    { -122,  -55,   15,   83, 149,  210,  264,  311  },
    /* K10  - this was entirely missing from the patent, and I've simply copied the real TMS5220 one, which is wrong */
    { -205, -132,  -59,   14,  87,  160,  234,  307  },
  },
  TI_0280_PATENT_CHIRP
};

/* TMS5200/CD2501E
 (1979-1983 era)
The TMS5200NL was decapped and imaged by digshadow in March, 2013.
It is equivalent to the CD2501E (internally: "TMC0285") chip used
 on the TI 99/4(A) speech module.
The LPC table is verified to match the decap.
 (It was previously dumped with PROMOUT which matches as well)
The chirp table is verified to match the decap. (sum = 0x3da)
Note that the K coefficients are VERY different from the coefficients given
 in the US 4,335,277 patent, which may have been for some sort of prototype or
 otherwise intentionally scrambled. The energy and pitch tables, however, are
 identical to that patent.
Also note, that the K coefficients are identical to the coefficients from the
 CD2801 (which itself is almost identical to the CD2802).
NOTE FROM DECAP: immediately to the left of each of the K1,2,3,4,5,and 6
 coefficients in the LPC rom are extra columns containing the constants
 -510, -502, 313, 318, or in hex 0x202, 0x20A, 0x139, 0x13E.
 Those EXACT constants DO appear (rather nonsensically) on the lpc table in US
 patent 4,335,277. They are likely related to the multiplicative interpolator
 described in us patent 4,419,540; whether the 5200/2501E and the 5220 or 5220C
 actually implement this interpolator or not is unclear. This interpolator
 seems intended for chips with variable frame rate, so if it exists at all,
 it may only exist on the TMS/TSP5220C and CD2501ECD.
*/

static const struct tms5100_coeffs T0285_2501E_coeff =
{
  10,
  4,
  6,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_2501E_PITCH
  {
  TI_2801_2501E_LPC
  },
  TI_LATER_CHIRP
};

/* TMS5220/5220C:
(1983 era for 5220, 1986-1992 era for 5220C; 5220C may also be called TSP5220C)
The TMS5220NL was decapped and imaged by digshadow in April, 2013.
The LPC table table is verified to match the decap.
The chirp table is verified to match the decap. (sum = 0x3da)
Note that all the LPC K* values match the TMS5110a table (as read via PROMOUT)
exactly.
The TMS5220CNL was decapped and imaged by digshadow in April, 2013.
The LPC table table is verified to match the decap and exactly matches TMS5220NL.
The chirp table is verified to match the decap. (sum = 0x3da)
*/
static const struct tms5100_coeffs tms5220_coeff =
{
  10,
  4,
  6,
  { 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
  TI_028X_LATER_ENERGY
  TI_5220_PITCH
  {
  TI_5110_5220_LPC
  },
  TI_LATER_CHIRP
};

/* The following Sanyo VLM5030 coefficients are derived from decaps of the chip
done by ogoun, plus image stitching done by John McMaster. The organization of
coefficients beyond k2 is derived from work by Tatsuyuki Satoh.
The actual coefficient rom on the chip die has 5 groups of bits only:
Address |   K1A   |   K1B   |   K2   | Energy | Pitch |
Decoder |   K1A   |   K1B   |   K2   | Energy | Pitch |
K1A, K1B and K2 are 10 bits wide, 32 bits long each.
Energy and pitch are both 7 bits wide, 32 bits long each.
K1A holds odd values of K1, K1B holds even values.
K2 holds values for K2 only
K3 and K4 are actually the table index values <<6
K5 thru K10 are actually the table index values <<7
This concept of only having non-binary weighted reflection coefficients for the
first two k stages is mentioned in Markel & Gray "Linear Prediction of Speech"
and in Thomas Parsons' "Voice and Speech Processing"
 */
static const struct tms5100_coeffs vlm5030_coeff =
{
  10,
  5,
  5,
  { 6, 5, 4, 4, 3, 3, 3, 3, 3, 3 },
  /* E   */
  { 0,  1,  2,  3,  5,  6,  7,  9,
    11, 13, 15, 17, 19, 22, 24, 27,
    31, 34, 38, 42, 47, 51, 57, 62,
    68, 75, 82, 89, 98,107,116,127},
  /* P   */
  {   0,  21,  22,  23,  24,  25,  26,  27,
    28,  29,  31,  33,  35,  37,  39,  41,
    43,  45,  49,  53,  57,  61,  65,  69,
    73,  77,  85,  93, 101, 109, 117, 125 },
  {
    /* K1  */
    /* (NOTE: the order of each table is correct, despite that the index MSb
    looks backwards) */
    {  390, 403, 414, 425, 434, 443, 450, 457,
      463, 469, 474, 478, 482, 485, 488, 491,
      494, 496, 498, 499, 501, 502, 503, 504,
      505, 506, 507, 507, 508, 508, 509, 509,
      -390,-376,-360,-344,-325,-305,-284,-261,
      -237,-211,-183,-155,-125, -95, -64, -32,
        0,  32,  64,  95, 125, 155, 183, 211,
      237, 261, 284, 305, 325, 344, 360, 376 },
    /* K2  */
    {    0,  50, 100, 149, 196, 241, 284, 325,
      362, 396, 426, 452, 473, 490, 502, 510,
        0,-510,-502,-490,-473,-452,-426,-396, /* entry 16(0x10) either has some special function, purpose unknown, or is a manufacturing error and should have been -512 */
      -362,-325,-284,-241,-196,-149,-100, -50 },
    /* K3  */
    {    0, 64, 128, 192, 256, 320, 384, 448,
      -512,-448,-384,-320,-256,-192,-128, -64 },
    /* K4  */
    {    0, 64, 128, 192, 256, 320, 384, 448,
      -512,-448,-384,-320,-256,-192,-128, -64 },
    /* K5  */
    {    0, 128, 256, 384,-512,-384,-256,-128 },
    /* K6  */
    {    0, 128, 256, 384,-512,-384,-256,-128 },
    /* K7  */
    {    0, 128, 256, 384,-512,-384,-256,-128 },
    /* K8  */
    {    0, 128, 256, 384,-512,-384,-256,-128 },
    /* K9  */
    {    0, 128, 256, 384,-512,-384,-256,-128 },
    /* K10 */
    {    0, 128, 256, 384,-512,-384,-256,-128 },
  },
  /* Chirp table */
  {   0,127,127,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0 },
};


const int32_t lanc2_11[] = {
  -36, -152, -349, -609, -901, -1173, -1366, -1409, -1233, -777,
  0,
  1120, 2572, 4315, 6273, 8347, 10416, 12347, 14013, 15297, 16107,
  16384,
  16107, 15297, 14013, 12347, 10416, 8347, 6273, 4315, 2572, 1120,
  0,
  -777, -1233, -1409, -1366, -1173, -901, -609, -349, -152, -36,
  0,
};

class Frame {
public:
  bool voiced() const { return period != 0; }
  void lerp(const Frame &A, const Frame &B,
      int count, int maxcount) {
    if (!A.inited) {
      inited = false;
      energy = period = 0;
      for (int i = 0; i < 10; i++) k[i] = 0;
      return;
    }
    if (A.voiced() != B.voiced() || !B.inited) {
      *this = A;
    } else {
      int b = count * 16384 / maxcount;
      int a = 16384 - b;
      inited = inited;
      energy = (A.energy * a + B.energy * b) >> 14;
      period = (A.period * a + B.period * b) >> 14;
      for (int i = 0; i < 10; i++) {
	k[i] = (A.k[i] * a + B.k[i] * b) >> 14;
      }
    }
  }
  int32_t energy;
  int32_t period;
  int32_t k[10];
  bool inited = false;
};

class Talkie : public AudioStream
#ifdef ENABLE_DEVELOPER_COMMANDS
   , CommandParser
#endif
{
public:
  struct Word {
    const uint8_t *ptr;
    uint32_t rate;
    const tms5100_coeffs* coeffs;
  };
  Word words[20];
  size_t num_words = 0;

  Talkie() {
    for (int i = 0; i < 10; i++) x[i] = 0;
  }

  bool Empty() { return num_words == 0; }

  void Say(const uint8_t* addr, uint32_t rate = 25,
	   const tms5100_coeffs* coeffs = &tms5220_coeff
//	   const tms5100_coeffs* coeffs = &tms5110a_coeff
    ) {
    rate *= 7;
    EnableAmplifier();
    noInterrupts();
    if (ptrAddr) {
      if (num_words < NELEM(words)) {
        words[num_words].ptr = addr;
        words[num_words].rate = rate;
        words[num_words].coeffs = coeffs;
        num_words++;
      }
    } else {
      rate_ = rate;
      coeffs_ = coeffs;
      count_ = rate_ - 1;
      ptrAddr = addr;
      ptrBit = 0;
    }
    interrupts();
  }

  void SayDigit(int digit) {
    switch (digit) {
      case 0: Say(spZERO); break;
      case 1: Say(spONE); break;
      case 2: Say(spTWO); break;
      case 3: Say(spTHREE); break;
      case 4: Say(spFOUR); break;
      case 5: Say(spFIVE); break;
      case 6: Say(spSIX); break;
      case 7: Say(spSEVEN); break;
      case 8: Say(spEIGHT); break;
      case 9: Say(spNINE); break;
    }
  }

  void SayNumber(int n) {
    if (n == 0) {
      Say(spZERO);
      return;
    }
    if (n / 1000000) {
      SayNumber(n/1000000);
      Say(spMILLION);
      n %= 1000000;
    }
    if (n / 1000) {
      SayNumber(n/1000);
      Say(spTHOUSAND);
      n %= 1000;
    }
    if (n / 100) {
      SayDigit(n / 100);
      Say(spHUNDRED);
      n %= 100;
    }
    switch (n / 10) {
      case 9: Say(spNINETY); break;
      case 8: Say(spEIGHTY); break;
      case 7: Say(spSEVENTY); break;
      case 6: Say(spSIXTY); break;
      case 5: Say(spFIFTY); break;
      case 4: Say(spFOURTY); break;
      case 3: Say(spTHIRTY); break;
      case 2: Say(spTWENTY); break;
      case 1:
	switch (n) {
	  case 19: Say(spNINETEEN); return;
	  case 18: Say(spEIGHTEEN); return;
	  case 17: Say(spSEVENTEEN); return;
	  case 16: Say(spSIXTEEN); return;
	  case 15: Say(spFIFTEEN); return;
	  case 14: Say(spFOURTEEN); return;
	  case 13: Say(spTHIRTEEN); return;
	  case 12: Say(spTWELVE); return;
	  case 11: Say(spELEVEN); return;
	  case 10: Say(spTEN); return;
	}
    }
    n %= 10;
    if (n) SayDigit(n);
  }

  void Say2Digits(int number) {
    int x = 1;
    while (x < number) x *= 10;
    x /= 100;
    if (x > 1) number -= number % x;
    SayNumber(number);
  }

  // The ROMs used with the TI speech were serial, not byte wide.
  // Here's a handy routine to flip ROM data which is usually reversed.
  static uint8_t rev(uint8_t a) {
    // 76543210
    a = (a>>4) | (a<<4); // Swap in groups of 4
    // 32107654
    a = ((a & 0xcc)>>2) | ((a & 0x33)<<2); // Swap in groups of 2
    // 10325476
    a = ((a & 0xaa)>>1) | ((a & 0x55)<<1); // Swap bit pairs
    // 01234567
    return a;
  }

  uint8_t getBits(uint8_t bits) {
    uint8_t value;
    uint16_t data;
    data = rev(pgm_read_byte(ptrAddr))<<8;
    if (ptrBit+bits > 8) {
      data |= rev(pgm_read_byte(ptrAddr+1));
    }
    data <<= ptrBit;
    value = data >> (16-bits);
    ptrBit += bits;
    if (ptrBit >= 8) {
      ptrBit -= 8;
      ptrAddr++;
    }
    return value;
  }

  void ReadFrame() {
    old_frame = new_frame;
    if (!ptrAddr) {
      new_frame.inited = false;
      return;
    }
    new_frame.inited = true;
    // Read speech data, processing the variable size frames.
    uint8_t energy = getBits(coeffs_->energy_bits);
    if (energy == 0) {
      // Energy = 0: rest frame
      new_frame.energy = 0;
    } else if (energy == ((1 << coeffs_->energy_bits) - 1)) {
      // Energy = 15: stop frame. Silence the synthesiser.
      new_frame.energy = 0;
      for (int i = 0; i < 10; i++) new_frame.k[i] = 0;
      if (num_words) {
        ptrAddr = words[0].ptr;
        rate_ = words[0].rate;
	coeffs_ = words[0].coeffs;
        num_words--;
        for (size_t i = 0; i < num_words; i++) words[i] = words[i + 1];
        ptrBit = 0;
      } else {
        ptrAddr = NULL;
      }
    } else {
      new_frame.energy = coeffs_->energytable[energy];
      uint8_t repeat = getBits(1);
      new_frame.period = coeffs_->pitchtable[getBits(coeffs_->pitch_bits)];
      // A repeat frame uses the last coefficients
      if (!repeat) {
	for (int i = 0; i < 4; i++)
	  new_frame.k[i] = coeffs_->ktable[i][getBits(coeffs_->kbits[i])];
	if (new_frame.period) {
	  for (int i = 4; i < 10; i++)
	    new_frame.k[i] = coeffs_->ktable[i][getBits(coeffs_->kbits[i])];
	} else {
	  for (int i = 4; i < 10; i++)
	    new_frame.k[i] = 0;
	}
      }
    }
  }
  
  int16_t Get8kHz() {
    if (count_++ >= rate_) {
      ReadFrame();
      count_ = 0;
    }

    Frame f;
    f.lerp(old_frame, new_frame, count_, rate_);

    
    int32_t u[11];

    if (f.period) {
      // Voiced source
      if (periodCounter < f.period) {
        periodCounter++;
      } else {
        periodCounter = 0;
      }
      if (periodCounter < MAX_CHIRP_SIZE) {
        u[10] = ((coeffs_->chirptable[periodCounter]) * f.energy) >> 3;
      } else {
        u[10] = 0;
      }
    } else {
      // Unvoiced source
      static uint16_t synthRand = 1;
      synthRand = (synthRand >> 1) ^ ((synthRand & 1) ? 0xB800 : 0);
      u[10] = ((synthRand & 1) ? f.energy : -f.energy) << 3;
    }

#define matrix_multiply(X, Y) (((X)*(Y)) >> 9)
    u[9] = u[10] - matrix_multiply(f.k[9], x[9]);
    u[8] = u[9] - matrix_multiply(f.k[8], x[8]);
    u[7] = u[8] - matrix_multiply(f.k[7], x[7]);
    u[6] = u[7] - matrix_multiply(f.k[6], x[6]);
    u[5] = u[6] - matrix_multiply(f.k[5], x[5]);
    u[4] = u[5] - matrix_multiply(f.k[4], x[4]);
    u[3] = u[4] - matrix_multiply(f.k[3], x[3]);
    u[2] = u[3] - matrix_multiply(f.k[2], x[2]);
    u[1] = u[2] - matrix_multiply(f.k[1], x[1]);
    u[0] = u[1] - matrix_multiply(f.k[0], x[0]);

    // Output clamp
    if (u[0] > 511) u[0] = 511;
    if (u[0] < -512) u[0] = -512;

    x[9] = x[8] + matrix_multiply(f.k[8], u[8]);
    x[8] = x[7] + matrix_multiply(f.k[7], u[7]);
    x[7] = x[6] + matrix_multiply(f.k[6], u[6]);
    x[6] = x[5] + matrix_multiply(f.k[5], u[5]);
    x[5] = x[4] + matrix_multiply(f.k[4], u[4]);
    x[4] = x[3] + matrix_multiply(f.k[3], u[3]);
    x[3] = x[2] + matrix_multiply(f.k[2], u[2]);
    x[2] = x[1] + matrix_multiply(f.k[1], u[1]);
    x[1] = x[0] + matrix_multiply(f.k[0], u[0]);
    x[0] = u[0];

    return u[0] << 5;
  }

#if 1
  // 2-lobe 5.5x upsampler
  uint32_t l_pos_ = 0;
  int16_t A = 0, B = 0, C = 0, D = 0;
  int16_t Get44kHz() {
    int32_t sum =
      A * lanc2_11[l_pos_] +
      B * lanc2_11[l_pos_ + 11] +
      C * lanc2_11[l_pos_ + 22] +
      D * lanc2_11[l_pos_ + 33];
    l_pos_ += 2;
    if (l_pos_ >= 11) {
      l_pos_ -= 11;
      D = C; C = B; B = A;
      A = Get8kHz();
    }
    return clamptoi16(sum >> 14);
  }
  bool eof() const override {
    return ptrAddr == NULL && !A && !B && !C && !D;
  }
#else
  // Very very stupid upsamler, slightly better than
  // just repeating samples.
  int16_t tmp = 0;
  int16_t last = 0;
  int32_t N = 0;
  int16_t Get44kHz() {
    N += rate_;
    if (N > 44100) {
      last = Get8kHz();
      N -= 44100;
    }
    tmp = (tmp * 3 + last) >> 2;
    return tmp;
  }
  bool eof() const override {
    return ptrAddr == NULL && tmp == 0;
  }
#endif
  
  int read(int16_t* data, int elements) override {
    if (eof()) return 0;
    for (int i = 0; i < elements; i++) {
      data[i] = Get44kHz();
    }
    return elements;
  }
  bool isPlaying() const {
    return !eof();
  }
  void Stop() override {}

#ifdef ENABLE_DEVELOPER_COMMANDS
  bool Parse(const char *cmd, const char* arg) override {
    uint32_t rate = 0;
    if (!strcmp(cmd, "say") && arg) {
      if (!strcmp(arg, "bof")) {
	Say(talkie_font_directory_15, 15);
	Say(talkie_not_found_15, 15);
	return true;
      }
      if (!strcmp(arg, "ftl")) {
	Say(talkie_font_directory_15, 15);
	Say(talkie_too_long_15, 15);
	return true;
      }
      if (!strcmp(arg, "sd")) {
	Say(talkie_sd_card_15, 15);
	Say(talkie_not_found_15, 15);
	return true;
      }
      if (!strcmp(arg, "bb")) {
	Say(talkie_error_in_15, 15);
	Say(talkie_blade_array_15, 15);
	return true;
      }
      if (!strcmp(arg, "bp")) {
	Say(talkie_error_in_15, 15);
	Say(talkie_preset_array_15, 15);
	return true;
      }
      if (!strcmp(arg, "lb")) {
	Say(talkie_low_battery_15, 15);
	return true;
      }
    }
    if (!strcmp(cmd, "talkie")) rate = 25;
    if (!strcmp(cmd, "talkie_slow")) rate = 25;
    if (!strcmp(cmd, "talkie12")) rate = 12;
    if (!strcmp(cmd, "talkie15")) rate = 15;
    if (rate && arg) {
      const char *start= strchr(arg, '{');
      const char *end = strchr(arg, '}');
      char *out = const_cast<char*>(arg);
      if (!start) start = arg;
      if (!end) end = arg + strlen(arg);
      int n = 0;
      int digits = 0;
      for (;start < end; start++) {
        if (*start >= '0' && *start <= '9') {
          n = (n << 4) + *start - '0';
          digits++;
        } else if (*start >= 'a' && *start <= 'f') {
          n = (n << 4) + *start - 'a' + 10;
          digits++;
        } else if (*start >= 'A' && *start <= 'F') {
          n = (n << 4) + *start - 'A' + 10;
          digits++;
        } else if (*start == 'x') {
          digits = 0;
        }
        if (digits == 2) {
          *(out++) = n;
          digits = n = 0;
        }
      }
      Say((uint8_t*)arg, rate);
      // As soon as we return "arg" will be freed, so let's wait for it.
      while (isPlaying());
    }

    return false;
  }

  void Help() override {
    STDOUT.println("say bof/sd/abort - test error messages");
    STDOUT.println("talkie HEXDATA - play talkie");
  }
#endif

private:
  const uint8_t * ptrAddr = NULL;
  uint32_t rate_ = 0;
  uint8_t ptrBit = 0;

  const tms5100_coeffs* coeffs_;
  Frame new_frame, old_frame;

  uint8_t count_ = 0;
  uint8_t pos_ = 0;
  uint8_t periodCounter;
  int32_t x[10];
};

#endif
