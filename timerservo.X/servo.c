#include <timers.h>
#include <p18f2550.h>


unsigned int AngleServo[8] = {0};
int CountServo = 0;
int CurrentServo = 0;

/*
 * Configure the Servo library.
 * count is the number of servo connected (maximum of 8), starting from pin RB0.
 */
void OpenServo(int count) {
    if (count < 0 || count > 8)
        return;

    CountServo = count;

    // Configure pins as outputs and initialize them to 0.
    TRISB &= 0xFF << count;
    PORTB &= 0xFF << count;

    INTCONbits.GIE = 1;
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT
            & T0_PS_1_1 /* Internal oscillator of 8MHz */);
}

void WriteServo(int n, int angle) {
    if (0 <= n && n < CountServo && 0 <= angle && angle < 180)
        AngleServo[n] = angle;
}

void InterruptServo() {
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        if (CurrentServo > 0) {
            // Set previous pin (CurrentServo - 1) to 0.
            PORTB &= ~(1 << (CurrentServo - 1));
        }

        if (CurrentServo < CountServo) {
            // Set current (CurrentServo) pin to 1.
            PORTB |= 1 << CurrentServo;

            WriteTimer0(65535 /* Max int */
                    /* Maximum width of 2.1ms. */
                    - AngleServo[CurrentServo] * 2 * ((2100 - 900) / 180)
                    /* Minimum width of 0.9ms. */
                    - 900 * 2 /* 2 = 8MHz / 4 / 1ms  */
                    );
            // Between 0.9ms and 2.1ms.
        } else {
            WriteTimer0(65535 - 20000 * 2); // 20ms
        }

        CurrentServo++;
        if (CurrentServo > CountServo)
            CurrentServo = 0;
        // When CurrentServo == CountServo, the next interrupt sets every pin to 0.

        INTCONbits.TMR0IF = 0;
    }
}
