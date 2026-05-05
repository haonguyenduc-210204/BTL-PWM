#ifndef PWM_H_
#define PWM_H_

#include "usbstk5515.h"

/* ------------------------------------------------------------------------ *
 * PWM parameters
 * ------------------------------------------------------------------------ */
#define PWM_SAMPLES_PER_SECOND   48000

/* Carrier frequency in Hz */
#define PWM_CARRIER_FREQ         10000

/* PWM output amplitude */
#define PWM_HIGH_AMPLITUDE       20000
#define PWM_LOW_AMPLITUDE       -20000

/* Base duty cycle in percent
   50 means square wave when input signal is zero */
#define PWM_BASE_DUTY_PERCENT    50

/* Duty modulation depth in percent
   Higher value -> stronger duty cycle variation
   Suggested test range: 20, 30, 40, 45 */
#define PWM_DUTY_DEPTH_PERCENT   45

/* Safety clamp for duty cycle */
#define PWM_DUTY_MIN_PERCENT     5
#define PWM_DUTY_MAX_PERCENT     95

/* Fixed-point phase scale for PWM carrier */
#define PWM_PHASE_SCALE          65536L

/* ------------------------------------------------------------------------ *
 * Prototypes
 * ------------------------------------------------------------------------ */
void PWM_init(void);
Int16 PWM_process(Int16 msg_sample);

#endif /* PWM_H_ */
