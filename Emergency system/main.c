/******************************************************************************
 *
 * File Name: emergency_system.c
 *
 * Description: Emergency system for notifying important authorities.
 *
 * Author: Mohamed Sarhan AL-Hussieny
 *
 ******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned flag = 0; // Variable to track timer interrupts
unsigned i; // Loop counter variable

// Timer1 compare match interrupt service routine
ISR(TIMER1_COMPA_vect)
{
    flag++; // Increment flag variable on each timer interrupt
}

/*
  * Description:
  * For System Clock = 1MHz and prescaler F_CPU/1024.
  * Timer frequency will be around 1kHz, Timer = 1ms
  * So we just need 1000 counts to get 1s period.
 */

// Initialize Timer1 for 1ms resolution
void Timer1_Int(void)
{
    TCNT1 = 0; // Initialize Timer1 counter
    OCR1A = 1000; // Set compare value for 1ms resolution
    TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12); // Set timer mode and prescaler for 1MHz clock
    TIMSK |= (1 << OCIE1A); // Enable Timer1 compare match interrupt
}

// External Interrupt 0 service routine to STOP ALARM
ISR(INT0_vect)
{
    PORTC &= 0x00; // Turn off all LEDs on PC0-PC2
    PORTB &= 0x00; // Turn off all LEDs on PB0-PB7
    flag = 0; // Reset flag variable
}

// Initialize External Interrupt 0
void INT0_Init(void)
{
    GICR |= (1 << INT0); // Enable External Interrupt 0 (INT0)
    MCUCR |= (1 << ISC01); // Trigger INT0 on falling edge
    DDRB &= ~(1 << PD2); // Set PD2 (INT0 pin) as input (BUTTON)
    SREG |= (1 << 7); // Enable global interrupts
}

int main()
{
    DDRA &= ~(1 << PA0); // Set first two pins of Port A as inputs
    DDRB = 0x33; // Set pins 0, 1, 5, and 6 of Port B as outputs
    DDRC = 0x0F; // Set pins 0, 1, 2, and 3 of Port C as outputs

    INT0_Init(); // Initialize external interrupt 0
    Timer1_Int(); // Initialize Timer1

    while (1)
    {
        SREG &= ~(1 << 7); // Disable global interrupts
        if (PINA & (1 << PA0)) // Check if emergency button is pressed
        {
            SREG |= (1 << 7); // Enable global interrupts

            while (flag != 0)
            {
                if (flag == 1)
                {
                    PORTB |= (1 << PB1);
                    PORTB &= ~(1 << PB0);
                    PORTB |= (1 << PB5);
                    PORTB &= ~(1 << PB4);
                    PORTC |= (1 << PD2);
                }
                else if (flag == 10)
                {
                    PORTC |= (1 << PD1);
                    PORTB = 0; // Turn off all LEDs on Port B
                }
                else if (flag == 20)
                {
                    PORTC |= (1 << PC0);
                    PORTC |= (1 << PC3);
                    PORTB |= (1 << PB0);
                    PORTB &= ~(1 << PB1);
                    PORTB |= (1 << PB4);
                    PORTB &= ~(1 << PB5);
                }
                else if (flag == 30)
                {
                    // Blink LEDs on PC0-PC2 five times
                    for (i = 0; i < 5; i++)
                    {
                        PORTC |= 0x07; // Turn on all LEDs on PC0-PC2
                        _delay_ms(1000); // Delay for 1 second
                        PORTC &= 0xF8; // Turn off all LEDs on PC0-PC2
                        _delay_ms(1000); // Delay for 1 second
                    }
                    PORTC = 0x00; // Turn off all LEDs on Port C
                    PORTB = 0x00; // Turn off all LEDs on Port B
                    flag = 0; // Reset flag variable
                }
            }
        }
    }

}
