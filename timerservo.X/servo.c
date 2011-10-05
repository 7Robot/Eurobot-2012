#include <timers.h>
#include <p18f2550.h>


unsigned int AngleServo[4] = {0};
int CountServo = 0;
int CurrentServo = 0;

void OpenServo(int count) {
    CountServo = count;

    switch (count) {
        case 4:
            TRISBbits.RB3 = 0;
            PORTBbits.RB3 = 0;
        case 3:
            TRISBbits.RB2 = 0;
            PORTBbits.RB2 = 0;
        case 2:
            TRISBbits.RB1 = 0;
            PORTBbits.RB1 = 0;
        case 1:
            TRISBbits.RB0 = 0;
            PORTBbits.RB0 = 0;
            break; // One break to rule them all.
    }
    //  TRISC = TRISC & (0xFF << count);

    RCONbits.IPEN = 1; // TODO
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IF = 0;
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT
            & T0_PS_1_1 /* Internal oscillator of 8MHz */);
}

void WriteServo(int n, int angle) {
    AngleServo[n] = angle;
}

void SwitchServo(int n) {
    if (n == 0)
        PORTBbits.RB0 = PORTBbits.RB0 ^ 1;
    else if (n == 1)
        PORTBbits.RB1 = PORTBbits.RB1 ^ 1;
    else if (n == 2)
        PORTBbits.RB2 = PORTBbits.RB2 ^ 1;
    else if (n == 3)
        PORTBbits.RB3 = PORTBbits.RB3 ^ 1;
}

void InterruptServo() {
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        if (CurrentServo > 0)
            SwitchServo(CurrentServo - 1); // Reset previous pin to 0.

        if (CurrentServo < CountServo) {
            SwitchServo(CurrentServo); // Set current pin to 1.

            WriteTimer0(65535
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

         INTCONbits.TMR0IF = 0;
    }
}
