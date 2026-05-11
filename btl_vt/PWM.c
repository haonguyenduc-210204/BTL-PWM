/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   PWM.c                                                                   */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Pulse Width Modulation (PWM) tren TMS320C5535 eZdsp.                   */
/*                                                                           */
/*   Cong thuc:                                                               */
/*     D[n] = 0.5 + kpwm * (m[n] / 32768)    (duty cycle, 0..1)            */
/*     y[n] = PWM_HIGH  neu saw[n] < D_count                                */
/*            PWM_LOW   nguoc lai                                             */
/*                                                                           */
/*   Trien khai so (sawtooth comparator):                                    */
/*     saw_counter: dem tu 0 den PWM_PERIOD-1, tang 1 moi mau               */
/*     D_count    : so mau muc cao trong mot chu ky                          */
/*     y[n]       : muc cao hoac muc thap tuy theo saw_counter vs D_count   */
/*                                                                           */
/*   Khac voi PM / FM:                                                       */
/*     PM/FM: bien doi pha/tan so cua song mang sin                         */
/*     PWM  : bien doi do rong xung cua song vuong                           */
/*                                                                           */
/*****************************************************************************/

#include "PWM.h"

/* ------------------------------------------------------------------------ *
 * Bien noi bo
 * saw_counter: dem vi tri trong chu ky PWM hien tai (0 .. PWM_PERIOD-1)
 * ------------------------------------------------------------------------ */
static Uint16 saw_counter = 0;

/* ------------------------------------------------------------------------ *
 * PWM_init()
 * Reset sawtooth counter ve 0
 * ------------------------------------------------------------------------ */
void PWM_init(void)
{
    saw_counter = 0;
}

/* ------------------------------------------------------------------------ *
 * PWM_process( msg_sample )
 *
 * Xu ly mot mau PWM.
 *
 * Thuat toan:
 *   1. Tinh duty count tu tin hieu tin tuc:
 *        scaled_msg = msg_sample >> PWM_MSG_SHIFT   (~[-128, 127])
 *        deviation  = (scaled_msg * PWM_KPWM) / 128
 *        D_count    = CLAMP( PWM_PERIOD/2 + deviation, 0, PWM_PERIOD )
 *
 *   2. So sanh sawtooth voi D_count:
 *        y = (saw_counter < D_count) ? PWM_HIGH : PWM_LOW
 *
 *   3. Tang sawtooth counter (quay ve 0 sau PWM_PERIOD):
 *        saw_counter = (saw_counter + 1) % PWM_PERIOD
 *
 * Tham so:
 *   msg_sample: mau tin tuc, Int16 [-32768, 32767]
 *
 * Tra ve:
 *   mau PWM dua ra codec, Int16 (PWM_HIGH hoac PWM_LOW)
 * ------------------------------------------------------------------------ */
Int16 PWM_process(Int16 msg_sample)
{
    Int16  scaled_msg;
    Int32  deviation;
    Int32  d_count;
    Int16  output;

    /* ---- Buoc 1: Tinh duty count ----
     * Scale m[n] xuong truoc roi nhan voi PWM_KPWM
     * scaled_msg: [-128, 127]
     * deviation : [-24, 23.8] (voi PWM_KPWM=24)
     * D_count   : [0, 48] (tong so mau muc cao trong chu ky)
     */
    scaled_msg = (Int16)(msg_sample >> PWM_MSG_SHIFT);

    /* deviation = scaled_msg * PWM_KPWM / 128
     * Nhan truoc roi chia: tranh mat do chinh xac
     * Chia cho 128 (= 2^7) bang shift phai 7 bit
     */
    deviation = ((Int32)scaled_msg * (Int32)PWM_KPWM) >> 7;

    /* D_count = PWM_PERIOD/2 + deviation (trung tinh la duty 50%) */
    d_count = (Int32)(PWM_PERIOD / 2) + deviation;

    /* Clamp ve [0, PWM_PERIOD] de tranh tran */
    if (d_count < 0)          d_count = 0;
    if (d_count > PWM_PERIOD) d_count = PWM_PERIOD;

    /* ---- Buoc 2: So sanh sawtooth voi duty count ---- */
    if ((Int32)saw_counter < d_count)
        output = (Int16)PWM_HIGH;
    else
        output = (Int16)PWM_LOW;

    /* ---- Buoc 3: Tang sawtooth counter (vong lap 0..PWM_PERIOD-1) ---- */
    saw_counter++;
    if (saw_counter >= (Uint16)PWM_PERIOD)
        saw_counter = 0;

    return output;
}

/*****************************************************************************/
/*   End of PWM.c                                                            */
/*****************************************************************************/
