/*
 *  servo.c - C18 library to control up to eight servomotors on the PIC18F
 *  family from Microchip.
 *
 *  Tested with PIC18F2550 under MPLAB X.
 *
 *  2011-10-05 - First version by Ken Hasselmann and Martin d'Allens.
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <timers.h>

void OpenServo(int count);
void WriteServo(int n, int angle);
void InterruptServo();

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

/*
 * Change the angle of servo number n (pin RBn).
 */
void WriteServo(int n, int angle) {
    if (0 <= n && n < CountServo && 0 <= angle && angle < 180)
        AngleServo[n] = angle;
}

/*
 * Don't forget to call this function from the right interrupt vector.
 */
void InterruptServo() {
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        if (CurrentServo > 0) {
            // Set previous pin (CurrentServo - 1) to 0.
            PORTB &= ~(1 << (CurrentServo - 1));
        }

        if (CurrentServo < CountServo) {
            // Set current (CurrentServo) pin to 1.
            PORTB |= 1 << CurrentServo;

            WriteTimer0(65534 /* Max int */
                    /* Maximum width of 2.4ms. */
                    - AngleServo[CurrentServo] * 2 * ((2400 - 544) / 180)
                    /* Minimum width of 544µs. */
                    - 544 * 2 /* 2 = 8MHz / 4 (proc. freq) / 1µs  */
                    );
            // Between 0.9ms and 2.1ms.
        } else {
            WriteTimer0(65535 - 20000 * 2); // Minimum of 20ms between pulses.
        }

        CurrentServo++;
        if (CurrentServo > CountServo)
            CurrentServo = 0;
        // When CurrentServo == CountServo, the next interrupt sets every pin to 0.

        INTCONbits.TMR0IF = 0;
    }
}
