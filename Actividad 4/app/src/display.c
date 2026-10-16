//=====[Libraries]=============================================================
//#include "mbed.h"
//#include "arm_book_lib.h"

#include "display.h"
#include "main.h"
#include <stdbool.h>

/* Demo includes */
#include "logger.h"
#include "dwt.h"
#include "systick.h"

/********************** arm_book Defines *******************************/
//#include "arm_book_lib.h"
// Functional states
#ifndef OFF
#define OFF    0
#endif
#ifndef ON
#define ON     ( !OFF )
#endif

// Electrical states
#ifndef LOW
#define LOW    0
#endif
#ifndef HIGH
#define HIGH   ( !LOW )
#endif

//=====[Declaration of private defines]========================================
#define DISPLAY_IR_CLEAR_DISPLAY   0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL 0b00001000
#define DISPLAY_IR_FUNCTION_SET    0b00100000
#define DISPLAY_IR_SET_DDRAM_ADDR  0b10000000

#define DISPLAY_IR_ENTRY_MODE_SET_INCREMENT 0b00000010
#define DISPLAY_IR_ENTRY_MODE_SET_DECREMENT 0b00000000
#define DISPLAY_IR_ENTRY_MODE_SET_SHIFT     0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT  0b00000000

#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF 0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_ON   0b00000010
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF  0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_ON    0b00000001
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF   0b00000000

#define DISPLAY_IR_FUNCTION_SET_8BITS    0b00010000
#define DISPLAY_IR_FUNCTION_SET_4BITS    0b00000000
#define DISPLAY_IR_FUNCTION_SET_2LINES   0b00001000
#define DISPLAY_IR_FUNCTION_SET_1LINE    0b00000000
#define DISPLAY_IR_FUNCTION_SET_5x10DOTS 0b00000100
#define DISPLAY_IR_FUNCTION_SET_5x8DOTS  0b00000000

#define DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS 0
#define DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS 64
#define DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS 20
#define DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS 84

#define DISPLAY_RS_INSTRUCTION 0
#define DISPLAY_RS_DATA        1

#define DISPLAY_RW_WRITE 0
#define DISPLAY_RW_READ  1

#define DISPLAY_PIN_RS  4
#define DISPLAY_PIN_RW  5
#define DISPLAY_PIN_EN  6
#define DISPLAY_PIN_D0  7
#define DISPLAY_PIN_D1  8
#define DISPLAY_PIN_D2  9
#define DISPLAY_PIN_D3 10
#define DISPLAY_PIN_D4 11
#define DISPLAY_PIN_D5 12
#define DISPLAY_PIN_D6 13
#define DISPLAY_PIN_D7 14

#define DISPLAY_DEL_37US	37ul
#define DISPLAY_DEL_01US	01ul

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

/*
DigitalOut displayD0( D0 );
DigitalOut displayD1( D1 );
DigitalOut displayD2( D2 );
DigitalOut displayD3( D3 );
DigitalOut displayD4( D4 );
DigitalOut displayD5( D5 );
DigitalOut displayD6( D6 );
DigitalOut displayD7( D7 );
DigitalOut displayRs( D8 );
DigitalOut displayEn( D9 );
*/

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of private global variables]============
static display_t display;
static bool initial8BitCommunicationIsCompleted;

//=====[Declarations (prototypes) of private functions]========================
static void displayPinWrite( uint8_t pinName, int value );
static void displayDataBusWrite( uint8_t dataByte );
static void displayCodeWrite( bool type, uint8_t dataBus );

//=====[Implementations of public functions]===================================
void displayInit( displayConnection_t connection )
{
    display.connection = connection;

    initial8BitCommunicationIsCompleted = false;

    HAL_Delay( 50 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                      DISPLAY_IR_FUNCTION_SET |
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    HAL_Delay( 5 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                      DISPLAY_IR_FUNCTION_SET |
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    HAL_Delay( 1 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                      DISPLAY_IR_FUNCTION_SET |
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    HAL_Delay( 1 );

    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                              DISPLAY_IR_FUNCTION_SET |
                              DISPLAY_IR_FUNCTION_SET_8BITS |
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            HAL_Delay( 1 );
        break;

        case DISPLAY_CONNECTION_GPIO_4BITS:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                              DISPLAY_IR_FUNCTION_SET |
                              DISPLAY_IR_FUNCTION_SET_4BITS );
            HAL_Delay( 1 );

            initial8BitCommunicationIsCompleted = true;

            displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                              DISPLAY_IR_FUNCTION_SET |
                              DISPLAY_IR_FUNCTION_SET_4BITS |
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            HAL_Delay( 1 );
        break;
    }

    displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                      DISPLAY_IR_DISPLAY_CONTROL |
                      DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF |
                      DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |
                      DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );
    HAL_Delay( 1 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                      DISPLAY_IR_CLEAR_DISPLAY );
    HAL_Delay( 1 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                      DISPLAY_IR_ENTRY_MODE_SET |
                      DISPLAY_IR_ENTRY_MODE_SET_INCREMENT |
                      DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT );
    HAL_Delay( 1 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                      DISPLAY_IR_DISPLAY_CONTROL |
                      DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON |
                      DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |
                      DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );
    HAL_Delay( 1 );
}

void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY )
{
    switch( charPositionY ) {
        case 0:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            //HAL_Delay(1);
            systick_delay_us(DISPLAY_DEL_01US);

        break;

        case 1:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            //HAL_Delay( 1 );
            systick_delay_us(DISPLAY_DEL_01US);

        break;

        case 2:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            //HAL_Delay( 1 );
            systick_delay_us(DISPLAY_DEL_01US);

        break;

        case 3:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            //HAL_Delay( 1 );
            
        break;
    }
}

void displayStringWrite( const char * str )
{
    while (*str) {
    	displayCodeWrite(DISPLAY_RS_DATA, *str++);
    }
}

void displayDataWrite( const char data )
{
    	displayCodeWrite(DISPLAY_RS_DATA, data);
}

//=====[Implementations of private functions]==================================
static void displayCodeWrite( bool type, uint8_t dataBus )
{
	if ( type == DISPLAY_RS_INSTRUCTION )
		displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_INSTRUCTION );
	else
		displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_DATA );

	displayPinWrite( DISPLAY_PIN_RW, DISPLAY_RW_WRITE );
	displayDataBusWrite( dataBus );
    systick_delay_us(DISPLAY_DEL_37US);
}

static void displayPinWrite( uint8_t pinName, int value )
{
    switch( display.connection ) {
    	case DISPLAY_CONNECTION_GPIO_8BITS:
            switch( pinName ) {
            	/*
                case DISPLAY_PIN_D0: displayD0 = value;   break;
                case DISPLAY_PIN_D1: displayD1 = value;   break;
                case DISPLAY_PIN_D2: displayD2 = value;   break;
                case DISPLAY_PIN_D3: displayD3 = value;   break;
                case DISPLAY_PIN_D4: displayD4 = value;   break;
                case DISPLAY_PIN_D5: displayD5 = value;   break;
                case DISPLAY_PIN_D6: displayD6 = value;   break;
                case DISPLAY_PIN_D7: displayD7 = value;   break;
                case DISPLAY_PIN_RS: displayRs = value;   break;
                case DISPLAY_PIN_EN: displayEn = value;   break;
                case DISPLAY_PIN_RW: break;
                */
                default: break;
            }
            break;

        case DISPLAY_CONNECTION_GPIO_4BITS:
            switch( pinName ) {
            	case DISPLAY_PIN_D4: HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, value);   break;
				case DISPLAY_PIN_D5: HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, value);   break;
				case DISPLAY_PIN_D6: HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, value);   break;
				case DISPLAY_PIN_D7: HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, value);   break;
				case DISPLAY_PIN_RS: HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, value);   break;
				case DISPLAY_PIN_EN: HAL_GPIO_WritePin(D9_GPIO_Port, D9_Pin, value);   break;
                case DISPLAY_PIN_RW: break;
                default: break;
            }
            break;
    }
}

static void displayDataBusWrite( uint8_t dataBus )
{
    displayPinWrite( DISPLAY_PIN_EN, OFF );
    displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b10000000 );
    displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b01000000 );
    displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00100000 );
    displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00010000 );

    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayPinWrite( DISPLAY_PIN_D3, dataBus & 0b00001000 );
            displayPinWrite( DISPLAY_PIN_D2, dataBus & 0b00000100 );
            displayPinWrite( DISPLAY_PIN_D1, dataBus & 0b00000010 );
            displayPinWrite( DISPLAY_PIN_D0, dataBus & 0b00000001 );
        break;

        case DISPLAY_CONNECTION_GPIO_4BITS:
            if ( initial8BitCommunicationIsCompleted == true) {
                displayPinWrite( DISPLAY_PIN_EN, ON );
                //HAL_Delay( 1 );
                systick_delay_us(DISPLAY_DEL_01US);

                displayPinWrite( DISPLAY_PIN_EN, OFF );
                //HAL_Delay( 1 );
                systick_delay_us(DISPLAY_DEL_01US);

                displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b00001000 );
                displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b00000100 );
                displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00000010 );
                displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00000001 );
            }
        break;

    }
    displayPinWrite( DISPLAY_PIN_EN, ON );
    //HAL_Delay( 1 );
    systick_delay_us(DISPLAY_DEL_01US);

    displayPinWrite( DISPLAY_PIN_EN, OFF );
    //HAL_Delay( 1 );
    systick_delay_us(DISPLAY_DEL_01US);
}

/********************** end of file ******************************************/

