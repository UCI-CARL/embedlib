#ifndef PWM_H
#define PWM_H

void pwm_init(void);
void pwm_start(void);
void pwm_set_dutycycle(unsigned int dutycycle);

#endif //PWM_H