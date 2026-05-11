#ifndef PWM_H_
#define PWM_H_

#include "ezdsp5535.h"

/* ======================================================================== *
 * PWM.h  -  Pulse Width Modulation parameters & prototypes
 *
 * Mo hinh PWM chuan:
 *   Tin hieu PWM la song vuong co do rong xung (duty cycle) thay doi
 *   ty le voi bien do cua tin hieu tin tuc m(t).
 *
 *   Duty cycle:
 *     D[n] = 0.5 + kpwm * m[n]    (0.0 ~ 1.0)
 *
 *   Trien khai so (sawtooth comparator):
 *     saw[n]  = sawtooth counter (tang tuyen tinh tu 0 den PWM_PERIOD-1)
 *     D_count = round( D[n] * PWM_PERIOD )
 *     y[n]    = (saw[n] < D_count) ?  PWM_HIGH : PWM_LOW
 *
 * Tan so PWM:
 *   f_pwm = fs / PWM_PERIOD
 *   Vi du: fs=48000, PWM_PERIOD=48 => f_pwm = 1000 Hz
 * ======================================================================== */

/* Sampling frequency (Hz) */
#define PWM_SAMPLES_PER_SECOND   48000

/* ADC input gain (dB) */
#define PWM_GAIN_IN_DB           5

/* ---- PWM period (samples) ----
 * So mau trong mot chu ky PWM.
 * f_pwm = 48000 / PWM_PERIOD
 *   PWM_PERIOD = 48  => f_pwm = 1000 Hz
 *   PWM_PERIOD = 96  => f_pwm =  500 Hz
 *   PWM_PERIOD = 480 => f_pwm =  100 Hz
 */
#define PWM_PERIOD              48          /* samples per PWM cycle */

/* ---- PWM output levels ----
 * Muc cao va muc thap cua xung PWM (Q0, Int16)
 * PWM_HIGH toi da 32767
 */
#define PWM_HIGH                20000       /* Q0, muc cao  */
#define PWM_LOW                 0           /* Q0, muc thap */

/* ---- PWM sensitivity kpwm ----
 *
 * m(t) dang Int16 ~ [-32768, 32767]
 * Duty cycle D[n] = 0.5 + kpwm * (m[n] / 32768)
 *
 * De tranh tran, trien khai bang fixed-point:
 *   D_count = PWM_PERIOD/2 + (msg_sample >> PWM_MSG_SHIFT) * PWM_KPWM / PWM_PERIOD
 *
 * Thay the don gian hon:
 *   scaled_msg = msg_sample >> PWM_MSG_SHIFT    ~ [-128, 127]
 *   D_count    = PWM_PERIOD/2 + (scaled_msg * PWM_KPWM) / 128
 *
 * PWM_KPWM = 24 => max deviation = 24 mau (tren tong so 48 mau)
 *   => duty cycle tu 0% den 100% khi m(t) max
 * Tang PWM_KPWM de tang do bien thien duty cycle.
 * Khong de qua PWM_PERIOD/2 de tranh bao hoa.
 */
#define PWM_KPWM                24          /* PWM sensitivity */
#define PWM_MSG_SHIFT           8           /* right-shift truoc khi nhan KPWM */

/* ======================================================================== *
 * Prototypes
 * ======================================================================== */
void  PWM_init(void);
Int16 PWM_process(Int16 msg_sample);

#endif /* PWM_H_ */
