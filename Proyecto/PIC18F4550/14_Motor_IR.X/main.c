/*
 * File:   main.c
 * Author: Microside Technology
 *
 * Created on 27 de noviembre de 2020, 01:54 AM
 */

// CONFIG1L
#pragma config PLLDIV = 1         // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2 // System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1         // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HSPLL_HS // Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))
#pragma config FCMEN = OFF     // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF      // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
// CONFIG2L
#pragma config PWRT = ON    // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOR = OFF    // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3     // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF // USB Voltage Regulator Enable bit (USB voltage regulator disabled)
// CONFIG2H
#pragma config WDT = OFF     // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768 // Watchdog Timer Postscale Select bits (1:32768)
// CONFIG3H
#pragma config CCP2MX = ON   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF  // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON    // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
// CONFIG4L
#pragma config STVREN = ON // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF   // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
// CONFIG5L
#pragma config CP0 = OFF // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)
// CONFIG5H
#pragma config CPB = OFF // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)
// CONFIG6L
#pragma config WRT0 = OFF // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)
// CONFIG6H
#pragma config WRTC = OFF // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)
// CONFIG7L
#pragma config EBTR0 = OFF // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)
// CONFIG7H
#pragma config EBTRB = OFF // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 48000000
#include <pic18f4550.h>
#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

void InitPorts( void );
void PWMinit( void );
void PWM2_LoadDutyValue( uint16_t dutyValue );

void ADCInit( void );
uint16_t ReadADC( void );

void main( void )
{
    ADCInit();                               // Configura el m�dulo ADC en el canal 0
    InitPorts();                             // Configura el puerto A0 como entrada anal�gica y C2 como salida digiral
    PWMinit();                               // Configura el m�dulo PWM
    while ( 1 ) {
        uint16_t PWM_Set = ReadADC() * 1.53; // Escala el valor anal�gico a el ciclo PWM
        if ( PWM_Set < 1024 )
            PWM2_LoadDutyValue( PWM_Set );   // Genera la se�al PWM
    }
    return;
}

void InitPorts( void )
{
    ADCON1bits.PCFG = 0x0B;
    LATC = 0;
    LATA = 0;
    TRISCbits.RC2 = 0;
}

void PWMinit( void )
{
    PR2 = 0xFE;
    TMR2 = 0x00;
    T2CON = 0x06;
    CCP1CON = 0x0C;
    T2CONbits.TMR2ON = 1;
}

void PWM2_LoadDutyValue( uint16_t dutyValue )
{
    CCPR1L = ( ( dutyValue & 0x03FC ) >> 2 );
    CCP1CON = ( (uint8_t)( CCP1CON & 0xCF ) | ( ( dutyValue & 0x0003 ) << 4 ) );
}

void ADCInit( void )
{
    ADCON0bits.CHS = 0b0011;
    ADCON0bits.ADON = 1;
    ADCON2 = 0xBE;
}

uint16_t ReadADC( void )
{
    uint16_t result;
    ADCON0bits.GO_DONE = 1;                  //  Inicia la conversi�n AD.
    while ( ADCON0bits.GO_DONE )
        ;                                    //  Espera a que termine la conversi�n AD.
    result = ( ( ADRESH << 8 ) + ADRESL );
    return ( result );
}