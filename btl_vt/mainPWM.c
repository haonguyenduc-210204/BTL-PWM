/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   main.c                                                                  */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Pulse Width Modulation (PWM) tren TMS320C5535 eZdsp.                   */
/*                                                                           */
/*   Luong xu ly chinh:                                                      */
/*     1. Doc mau tu codec (line-in / mic)  -> left_input, right_input      */
/*     2. Xu ly PWM: left_output = PWM_process(left_input)                  */
/*     3. Ghi mau ra codec (headphone/line-out)                              */
/*     4. Luu vao buffer de xem Graph tren CCS                               */
/*                                                                           */
/*   Xem Graph tren CCS:                                                     */
/*     View -> Graph -> Dual Time                                            */
/*     - Signal 1: &graph_input[0],  length = GRAPH_N, Q0 Int16             */
/*     - Signal 2: &graph_pwm[0],    length = GRAPH_N, Q0 Int16             */
/*                                                                           */
/*   Dau ra PWM:                                                             */
/*     - Song vuong (PWM_HIGH / PWM_LOW) co duty cycle bien doi theo m[n]   */
/*     - Kenh trai: tin hieu PWM da bien doi                                 */
/*     - Kenh phai: tin hieu goc m[n] de so sanh                            */
/*                                                                           */
/*****************************************************************************/

#include "stdio.h"
#include "ezdsp5535.h"
#include "aic3204.h"
#include "PLL.h"
#include "PWM.h"
#include "sinewaves.h"

/* ---- Sampling & gain ---- */
#define SAMPLES_PER_SECOND   PWM_SAMPLES_PER_SECOND   /* 48000 Hz */
#define GAIN_IN_dB           PWM_GAIN_IN_DB            /* 5 dB     */

/* ---- Bien codec ---- */
Int16 left_input;
Int16 right_input;
Int16 left_output;
Int16 right_output;

/* ======================================================================== *
 * Buffer Graph cho CCS
 *
 * graph_input[]: tin hieu tin tuc m[n] dua vao PWM
 * graph_pwm[]  : tin hieu PWM dau ra s[n] (song vuong)
 *
 * Cach xem tren CCS:
 *   View -> Expressions -> them bien graph_input va graph_pwm
 *   hoac
 *   View -> Graph -> Dual Time:
 *     Start Address : graph_input
 *     Acquisition   : GRAPH_N
 *     Data Size     : 16-bit signed integer
 * ======================================================================== */
#define GRAPH_N  512
/* Message signal for PM demo */
#define MESSAGE_FREQ_HZ  1000     /* 1000 Hz de de nhin PM tren graph */
#define MESSAGE_AMP      8000

#pragma DATA_SECTION(graph_input, ".bss")
volatile Int16 graph_input[GRAPH_N];

#pragma DATA_SECTION(graph_pwm, ".bss")
volatile Int16 graph_pwm[GRAPH_N];

volatile Uint16 graph_index = 0;
volatile Uint16 graph_ready = 0;   /* =1 khi da day 1 frame, dung de set breakpoint */

/* ======================================================================== *
 * main()

 * ======================================================================== */
 Int16 sine_msg;
 Int16 pwm_output;
void main(void)

{
    /* ---- Khoi tao board ---- */
    EZDSP5535_init();

    /* ---- Cau hinh PLL 100 MHz ---- */
    pll_frequency_setup(100);


    /* ---- Cau hinh tan so lay mau va gain ADC ---- */
    set_sampling_frequency_and_gain(SAMPLES_PER_SECOND, GAIN_IN_dB);

    /* ---- Khoi tao PWM engine ---- */
    PWM_init();

    puts("\n=== Pulse Width Modulation (PWM) Demo ===");
    puts("fs: 48 kHz | PWM period: 48 mau | f_pwm: 1000 Hz");
    puts("Dua tin hieu vao LINE-IN, xem dau ra PWM qua HEADPHONE.\n");

    /* ======================================================================
     * Vong lap xu ly chinh - dong bo voi I2S interrupt cua codec
     *
     * aic3204_codec_read():  cho den khi co mau ADC moi (RcvR flag)
     * aic3204_codec_write(): cho den khi I2S san sang nhan mau (XmitR flag)
     *
     * Thoi gian 1 vong lap = 1/48000 s ~ 20.8 us
     *
     * Voi PWM_PERIOD = 48:
     *   Moi 48 mau tao 1 chu ky PWM (1 ms)
     *   Duty cycle bien doi theo bien do cua left_input
     * ====================================================================== */
    while (1)
    {
        /*
         * Generate internal message sine wave.
         * Use generate_sinewave_2() for message signal.
         * generate_sinewave_1() is reserved for carrier inside PM_process().
         */
        sine_msg = (Int16)generate_sinewave_2(MESSAGE_FREQ_HZ, MESSAGE_AMP);

        /*
         * Phase Modulation processing:
         * graph_input = message signal
         * graph_pwm    = PWM signal
         */
        
    pwm_output = PWM_process(sine_msg);

   graph_input[graph_index] = sine_msg;

    graph_pwm[graph_index] = pwm_output; //
        graph_index++;

        if (graph_index >= GRAPH_N)
        {
            graph_index = 0;
            graph_ready = 1;

            /*
             * Stop here after one complete buffer.
             * Run program -> wait a moment -> Suspend/Pause -> open Graph.
             */
            while (1);
        }
    }
}

/*****************************************************************************/
/*   End of main.c                                                           */
/*****************************************************************************/
