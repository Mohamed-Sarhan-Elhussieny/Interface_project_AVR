/*
 ============================================================================
 Name        : Mini_Project2.c
 Author      : Mohamed Sarhan EL-Hussenei
 Description : STop Watching Project
 Date        : 01/29/2024
 ============================================================================
 */
#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>


unsigned char num_flag=0;
unsigned char second = 0;
unsigned char minute = 0;
unsigned char hour = 0;


    ISR(INT2_vect)
  {
/******************************* Timer work agian ************************************************************/

    	TCCR1B|=(1<<CS10);     // the Stop Watch time should be resumed.
    	TCCR1B|=(1<<CS12);

  }


     ISR(INT1_vect)
  {
/*********************************  Stop Timer1 **********************************************************/

  TCCR1B &= ~(1<<CS10);
  TCCR1B &= ~(1<<CS11);       //  No clock source (Timer/Counter stopped).
  TCCR1B &= ~(1<<CS12);
  }




    ISR(INT0_vect)
   {
/********************************  MAKE ALL COUNT ZEROS ***********************************************/

    	hour=0;
        minute =0;        //  Stop Watch time should be reset
		second=0;

   }



// Initialize INT2 interrupt
void INT2_Init(void) {
    MCUCSR &= ~(1 << ISC2); // Falling edge of INT1
    GICR |= (1 << INT2);    // Enable INT1
    DDRB &= ~(1 << PB2);    // Enable pin 2-B for button interrupt input
    PORTB |= (1 << PB2);
}

// Initialize INT1 interrupt
void INT1_Init(void) {
    MCUCR |= (1 << ISC11) | (1 << ISC10); // Rising edge of INT1
    GICR |= (1 << INT1);                  // Enable INT1
    DDRD &= ~(1 << PD3);                  // Enable pin 3-D for button interrupt input
}

// Initialize INT0 interrupt
void INT0_Init(void) {
    MCUCR |= (1 << ISC01); // Falling edge of INT0
    MCUCR &= ~(1 << ISC00);
    GICR |= (1 << INT0);   // Enable INT0
    DDRD &= ~(1 << PD2);   // Enable pin 2D for button interrupt input
    PORTD |= (1 << PD2);
}

// Timer1 Compare Match A interrupt service routine
ISR(TIMER1_COMPA_vect) {
    num_flag = 1; // Set flag indicating 1 second has passed
}

// Initialize Timer1
void Timer1_Init(void) {
    TCNT1 = 0;            // Start count
    OCR1A = 1000;         // Set compare match value for 1 second
    TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12); // Set prescaler to 1024, CTC mode
    TIMSK |= (1 << OCIE1A); // Enable Timer1 compare match interrupt
}

int main() {
    // Initialize interrupts
    INT0_Init();
    INT1_Init();
    INT2_Init();

    // Initialize Timer1
    Timer1_Init();

    // Enable global interrupts
    sei();

    // Setup 7-segment display pins
    DDRC |= 0x0F;  // Activate 4 pins for decoder
    PORTC &= 0xF0; // Define 4-pin input for decoder

    DDRA |= 0x3F;  // Activate 6 pins in PORTA as enable/disable pins for 7-segment display
    PORTA |= 0x3F; // Define 6-pin output for enable/disable pins

    while (1) {
        // If one second has passed
        if (num_flag == 1) {
            second++; // Increment seconds
            if (second == 60) {
                second = 0;
                minute++; // Increment minutes every 60 seconds
            }
            if (minute == 60) {
                second = 0;
                minute = 0;
                hour++; // Increment hours every 60 minutes
            }
            if (hour == 24) {
                second = 0;
                minute = 0;
                hour = 0; // Reset hours every 24 hours
            }
            num_flag = 0; // Reset flag
        }

        // Display current time on 7-segment display
        PORTA |= (1 << 0);     //enable first 7-seg
        PORTC &= (0xF0);
        PORTC = second % 10; // Write first digit for seconds
        _delay_ms(5);         // Delay to see the update

        PORTA &= ~(1 << 0);   //enable first 7-seg
        PORTA |= (1 << 1);    //enable second 7-seg
        PORTC &= (0xF0);     
        PORTC = second / 10;  // Write second digit for seconds
        _delay_ms(5);

        PORTA &= ~(1 << 1);
        PORTA |= (1 << 2);
        PORTC &= (0xF0);
        PORTC = minute % 10;
        _delay_ms(5);

        PORTA &= ~(1 << 2);
        PORTA |= (1 << 3);
        PORTC &= (0xF0);
        PORTC = minute / 10;
        _delay_ms(5);

        PORTA &= ~(1 << 3);
        PORTA |= (1 << 4);
        PORTC &= (0xF0);
        PORTC = hour % 10;
        _delay_ms(5);

        PORTA &= ~(1 << 4);
        PORTA |= (1 << 5);
        PORTC &= (0xF0);
        PORTC = hour / 10;
        _delay_ms(5);
    }

    return 0;
}
