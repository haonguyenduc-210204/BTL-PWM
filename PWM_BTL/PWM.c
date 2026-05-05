#include "PWM.h"

/* ------------------------------------------------------------------------ *
 * Internal PWM state
 * ------------------------------------------------------------------------ */
static Int32 pwm_phase;
static Int16 pwm_duty_percent;

/* ------------------------------------------------------------------------ *
 * clamp_duty()
 * ------------------------------------------------------------------------ */
static Int16 clamp_duty(Int32 duty)
{
    if (duty < PWM_DUTY_MIN_PERCENT)
        return PWM_DUTY_MIN_PERCENT;
    if (duty > PWM_DUTY_MAX_PERCENT)
        return PWM_DUTY_MAX_PERCENT;
    return (Int16)duty;
}

/* ------------------------------------------------------------------------ *
 * PWM_init()
 * ------------------------------------------------------------------------ */
void PWM_init(void)
{
    pwm_phase = 0;
    pwm_duty_percent = PWM_BASE_DUTY_PERCENT;
}

/* ------------------------------------------------------------------------ *
 * PWM_process(msg_sample)
 *
 * PWM principle:
 *
 * duty[n] = base_duty + k*m[n]
 *
 * A fixed-point phase accumulator creates the carrier ramp. The output is
 * HIGH when the carrier phase is smaller than the duty threshold, otherwise
 * LOW.
 * ------------------------------------------------------------------------ */
Int16 PWM_process(Int16 msg_sample)
{
    Int32 duty;
    Int32 duty_threshold;
    Int16 y;

    /* Convert input sample to duty cycle variation.
       Input sample is Int16, roughly in range [-32768, 32767].
       The right shift by 15 maps it approximately to [-1, 1]. */
    duty = PWM_BASE_DUTY_PERCENT +
           (((Int32)msg_sample * PWM_DUTY_DEPTH_PERCENT) >> 15);

    /* Clamp duty cycle for stable PWM output */
    pwm_duty_percent = clamp_duty(duty);

    /* Convert duty percent to fixed-point threshold */
    duty_threshold = ((Int32)pwm_duty_percent * PWM_PHASE_SCALE) / 100;

    /* Generate PWM output sample */
    if (pwm_phase < duty_threshold)
        y = PWM_HIGH_AMPLITUDE;
    else
        y = PWM_LOW_AMPLITUDE;

    /* Advance carrier phase */
    pwm_phase += ((Int32)PWM_CARRIER_FREQ * PWM_PHASE_SCALE) /
                 PWM_SAMPLES_PER_SECOND;

    while (pwm_phase >= PWM_PHASE_SCALE)
        pwm_phase -= PWM_PHASE_SCALE;

    return y;
}
