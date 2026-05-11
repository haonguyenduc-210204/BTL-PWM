/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   sinewaves.c                                                             */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Self-contained sinewave generator for TMS320C5535/eZdsp5535 PM demo.    */
/*                                                                           */
/* NOTE                                                                      */
/*   This version does NOT call DSPLIB sine(), so the project does not need   */
/*   55xdsp.lib just to generate the carrier sinewave.                       */
/*                                                                           */
/*****************************************************************************/

#include "sinewaves.h"

#define SINE_FS_HZ       48000L
#define PHASE_BITS       16
#define PHASE_FULL_SCALE 65536UL
#define LUT_BITS         6
#define LUT_SIZE         64
#define LUT_SHIFT        (PHASE_BITS - LUT_BITS)

/* 64-point Q15 sine lookup table: round(32767*sin(2*pi*n/64)) */
static const signed short sine_lut[LUT_SIZE] = {
       0,   3212,   6393,   9512,  12539,  15446,  18204,  20787,
   23170,  25329,  27245,  28898,  30274,  31357,  32138,  32609,
   32767,  32609,  32138,  31357,  30274,  28898,  27245,  25329,
   23170,  20787,  18204,  15446,  12539,   9512,   6393,   3212,
       0,  -3212,  -6393,  -9512, -12539, -15446, -18204, -20787,
  -23170, -25329, -27245, -28898, -30274, -31357, -32138, -32609,
  -32767, -32609, -32138, -31357, -30274, -28898, -27245, -25329,
  -23170, -20787, -18204, -15446, -12539,  -9512,  -6393,  -3212
};

static signed int clamp_amplitude(signed short int amplitude)
{
    if (amplitude < 0)
        amplitude = (signed short int)(-amplitude);
    if (amplitude > 32767)
        amplitude = 32767;
    return amplitude;
}

static unsigned int frequency_to_phase_step(signed short int frequency)
{
    long f = frequency;
    unsigned long step;

    if (f < 0)
        f = -f;
    if (f < 1)
        f = 1;
    if (f > 16000)
        f = 16000;

    step = ((unsigned long)f * PHASE_FULL_SCALE + (SINE_FS_HZ / 2)) / SINE_FS_HZ;
    if (step == 0)
        step = 1;
    if (step > 65535UL)
        step = 65535UL;

    return (unsigned int)step;
}

static signed int generate_sine_common(signed short int frequency,
                                       signed short int amplitude,
                                       unsigned int *phase_acc)
{
    unsigned int idx;
    signed long result;
    signed int amp;

    *phase_acc = (unsigned int)(*phase_acc + frequency_to_phase_step(frequency));
    idx = (unsigned int)((*phase_acc >> LUT_SHIFT) & (LUT_SIZE - 1));

    amp = clamp_amplitude(amplitude);
    result = ((signed long)sine_lut[idx] * (signed long)amp) >> 15;

    return (signed int)result;
}

signed int generate_sinewave_1(signed short int frequency,
                               signed short int amplitude)
{
    static unsigned int phase1 = 0;
    return generate_sine_common(frequency, amplitude, &phase1);
}

signed int generate_sinewave_2(signed short int frequency,
                               signed short int amplitude)
{
    static unsigned int phase2 = 0;
    return generate_sine_common(frequency, amplitude, &phase2);
}
