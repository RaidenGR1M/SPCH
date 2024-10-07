/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif


#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

#include "adcc.h"
#include "tmr0.h"
#include "ext_int.h"
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
/* i.e. uint8_t <variable_name>; */
uint8_t Imax = MAX_CURR;
double e_total = 0;
uint16_t Voltage_In = MINVOLT;
uint16_t Iread = 0;
long double Isq = 0;
bool Neg_flag = FALSE;      // Current sign  TRUE if negative;FALSE if positive
long double Iact;
long double Irms;
uint16_t count = 0;
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void TMR0_DefaultInterruptHandler(void)
{
    RD5 = ~(RD5 & RD7);
    RC7 = (RD5 & RD6);
    RB0 = ~(RD5);
    TMR0_WriteTimer(0x00);
}

void INT1_DefaultInterruptHandler(void)
{
    Neg_flag = TRUE;
    
    Irms = mean_sq(Isq, count);
    
    //PWM(t) = PWM(t-1)+(I(t)*(P*e(t)+I*int(e(t))+D*d(e(t))/dt)/I(t-1))        
    TMR0_Reload(measure_PI(Irms)); 
    count = 0; Isq = 0;
}

int main(void)
{
    /* Configure the oscillator for the device */
    //ConfigureOscillator();
    
    /* Initialize I/O and Peripherals for application */
    InitApp();
    PMD_Initialize();
    TMR0_Initialize();
    TMR0_StartTimer();
    EXT_INT_Initialize();
    
    //uint16_t PWM_val = 0x77;
    
    /* TODO <INSERT USER APPLICATION CODE HERE> */
    
        
    while(1)
    {
        /* Measure Voltage using ANB4 channel*/
        InitADB4();
        while(!ADCC_IsConversionDone())
            Voltage_In = (uint16_t)((ADRESH << 8) + ADRESL);
        ADCC_StopConversion();
        
        
        /* Check If Voltage_In is > MAXVOLT or < MINVOLT */
        if(Voltage_In > MAXVOLT || Voltage_In < MINVOLT )
        {
            /* Reset system */
            RD5 = 0; RC7 = 0; RB0 = 0;
            TMR0_StopTimer();
            return 0;
        }
        
        
        /* Measure Actual Current using ANF5 channel */
        InitADF5();
        while(!ADCC_IsConversionDone())
            Iread = (uint16_t)((ADRESH << 8) + ADRESL);
        ADCC_StopConversion();
        count++;
        
        /* Calculate the actual current value from ADC conversion */
        Iact = (long double)(((long double)Iread / 4095) * Imax);
        if(Iact >= ZERO_CURRENT)
            Iact = Iact - ZERO_CURRENT;
        else
            Iact = ZERO_CURRENT - Iact;
        
        Isq += (Iact*Iact);
        
        /* Display the rms value in LCD 16x2 Display */
        LCD_Display((uint16_t)Irms);
        
        /* If current reading sign changes
            toggle sign flag (-)ve or (+)ve 
            Calculate Irms
            Estimate Error
            Perform PI control algorithm */ 
        
        
        /* If R7 gets a positive trigger */
        
        
        if(!Neg_flag && RD7)
        {
            Neg_flag = FALSE;
            
            Irms = mean_sq(Isq, count);
            
            TMR0_Reload(measure_PI(Irms)); //PWM(t) = PWM(t-1)*(P*e(t)+I*int(e(t))+D*d(e(t))/dt)
            count = 0; Isq = 0;
            
        }
                        
    }

}

