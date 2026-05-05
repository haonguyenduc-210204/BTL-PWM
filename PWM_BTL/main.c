/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   main.c                                                                  */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Phase Modulation (PM) on TMS320C5515 USB Stick                          */
/*                                                                           */
/*****************************************************************************/

#include "stdio.h"
#include "usbstk5515.h"
#include "aic3204.h"
#include "PLL.h"
#include "PM.h"

#define SAMPLES_PER_SECOND   PM_SAMPLES_PER_SECOND
#define GAIN_IN_dB           PM_GAIN_IN_DB

Int16 left_input;
Int16 right_input;
Int16 left_output;
Int16 right_output;

/* ------------------------------------------------------------------------ *
 *  main( )
 * ------------------------------------------------------------------------ */
void main(void)
{
    /* Initialize board support */
    USBSTK5515_init();

    /* Initialize PLL */
    pll_frequency_setup(100);

    /* Initialize codec hardware and I2C */
    aic3204_hardware_init();

    /* Initialize codec */
    aic3204_init();

    /* Setup sampling frequency and input gain */
    set_sampling_frequency_and_gain(SAMPLES_PER_SECOND, GAIN_IN_dB);

    /* Initialize PM engine */
    PM_init();

    puts("\n Phase Modulation (PM) - carrier 10kHz");

    while (1)
    {
        /* Read stereo audio input */
        aic3204_codec_read(&left_input, &right_input);

        /* Use right channel as message signal */
        left_output = PM_process(right_input);

        /* Send original input to right channel for comparison */
        right_output = right_input;

        /* Write output */
        aic3204_codec_write(left_output, right_output);
    }
}