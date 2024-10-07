/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <math.h>
#endif

#include "user.h"
#include "adcc.h"

extern double e_total;
extern uint8_t Imax;
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */
    TRISA = 0x35;
    PORTA = 0x00;
    LATA = 0x00;
    
    TRISB = 0x01;
    PORTB = 0x00;
    LATB = 0x00;    
      
    TRISC = 0x7C;
    PORTC = 0x00;
    LATC = 0x00;
    
    TRISD = 0xCF;
    PORTD = 0x00;
    LATD = 0x00;
    
    TRISE = 0x03;
    PORTE = 0x00;
    LATE = 0x00;
    
    TRISF = 0xEE;
    PORTF = 0x00;
    LATF = 0x00;
    
    /* Initialize peripherals */

    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */

    /* Enable interrupts */
}


void PMD_Initialize(void)
{
    // CLKRMD CLKR enabled; SYSCMD SYSCLK enabled; SCANMD SCANNER enabled; FVRMD FVR enabled; IOCMD IOC enabled; CRCMD CRC enabled; HLVDMD HLVD enabled; 
    PMD0 = 0x00;
    // TMR0MD TMR0 enabled; TMR1MD TMR1 enabled; TMR4MD TMR4 enabled; SMT1MD SMT1 enabled; TMR5MD TMR5 enabled; TMR2MD TMR2 enabled; TMR3MD TMR3 enabled; TMR6MD TMR6 enabled; 
    PMD1 = 0x00;
    // ZCDMD ZCD enabled; ADCMD ADC enabled; ACTMD ACT enabled; CM2MD CM2 enabled; CM1MD CM1 enabled; DAC1MD DAC1 enabled; 
    PMD3 = 0x00;
    // NCO1MD NCO1 enabled; NCO2MD NCO2 enabled; DSM1MD DSM1 enabled; CWG3MD CWG3 enabled; CWG2MD CWG2 enabled; CWG1MD CWG1 enabled; NCO3MD NCO3 enabled; 
    PMD4 = 0x00;
    // CCP2MD CCP2 enabled; CCP1MD CCP1 enabled; PWM2MD PWM2 enabled; CCP3MD CCP3 enabled; PWM1MD PWM1 enabled; PWM3MD PWM3 enabled; 
    PMD5 = 0x00;
    // U5MD UART5 enabled; U4MD UART4 enabled; U3MD UART3 enabled; U2MD UART2 enabled; U1MD UART1 enabled; SPI2MD SPI2 enabled; SPI1MD SPI1 enabled; I2C1MD I2C1 enabled; 
    PMD6 = 0x00;
    // CLC5MD CLC5 enabled; CLC6MD CLC6 enabled; CLC3MD CLC3 enabled; CLC4MD CLC4 enabled; CLC7MD CLC7 enabled; CLC8MD CLC8 enabled; CLC1MD CLC1 enabled; CLC2MD CLC2 enabled; 
    PMD7 = 0x00;
    // DMA5MD DMA5 enabled; DMA6MD DMA6 enabled; DMA1MD DMA1 enabled; DMA2MD DMA2 enabled; DMA3MD DMA3 enabled; DMA4MD DMA4 enabled; 
    PMD8 = 0x00;
}

/*  Initialize ADB4 
    Start ADC conversion with AN B4 ADPCH value
 */
void InitADB4(void)
{
    ADCC_Initialize();
    ADCC_StartConversion(channel_ANB4);
}

/*  Initialize ADF5 
    Start ADC conversion with AN F5 ADPCH value
 */
void InitADF5(void)
{
    ADCC_Initialize();
    ADCC_StartConversion(channel_ANF5);
}

/*  Get the squared total value till the end of the cycle
    Calculate the mean square of the captured values 
 */
long double mean_sq(long double value, uint16_t n)
{
    /* calculate the average total of squared current values */
    return (long double)sqrt(value/n);
}

/* Calculate Perror constant and I error constant
   KAP * error 
   KAI * sum of errors
 */
uint8_t measure_PI(long double val)
{
    /* Calculate the errors for P and I constant
       Change PWM_Val: duty cycle 
       I/P : double KAP, double KAI, double e_val, long double val, double e_total
       O/P : double e_total(updated), uint16_t mod
     */
    double e_val;
    double P = KAP;
    double I = KAI;
    long double Iset = ISET;
    
    if (val >= Iset)
    {
        e_val = (double)(val - Iset);
        e_total += e_val;
        return TMR0H - (uint8_t)((long double)((e_val * KAP) + (e_total * KAI))*255/val);
    }
    
    else
    {
        e_val = (double)(Iset - val);
        e_total += e_val;
        return TMR0H + (uint8_t)((long double)((e_val * KAP) + (e_total * KAI))*255/val);
    }
    
}

/* Display the value in LCD display 
   Read bitwise and send data checking all 4 bits
 */

void LCD_Display(uint16_t value)
{   
    LCD_Initialize();
    
    uint8_t num = 0;
    char str[3];
    char* c = str;
    uint8_t div = 100;
    uint8_t iter;
    
    //convert the hexa into decimal store in char* addresses 
    for(iter=0; iter < ITOA_LENGTH; iter++)
    {
        num = value / div;
        *c = (num % 10) + '0';
        div /= 10;
        c++;
    }
    
    //Display it to LCD Display
    for (iter = 0; iter < ITOA_LENGTH; iter++)
    {   
        LCD_EN_ON();
        CHAR_MODE();
        WRITE_MODE();
        LCD_EN_OFF();
        
        CLEAR_D0();
        CLEAR_D1();
        CLEAR_D2();
        CLEAR_D3();
        if(str[iter] & 0x01)
            SEND_D0();
        if(str[iter] & 0x02)
            SEND_D1();
        if(str[iter] & 0x04)
            SEND_D2();
        if(str[iter] & 0x08)
            SEND_D3();
    }
}

void LCD_Initialize(void)
{
    COMMAND_MODE();
    RETURN_CURSHOME();
}