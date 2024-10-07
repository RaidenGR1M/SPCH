
#ifndef USER_H
#define USER_H

#include <xc.h>
/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define MAXVOLT         5
#define MINVOLT         1

#define MAX_CURR        150

#define SAMPLE_SIZE     1024

#define FALSE           1
#define TRUE            0

#define KAP             1
#define KAI             1

#define ISET            150
#define ZERO_CURRENT    75

#define ITOA_LENGTH     3

#define COMMAND_MODE()  (RD3 = 0)
#define CHAR_MODE()     (RD3 = 1)
#define RETURN_CURSHOME() (RD1 = 1)

#define LCD_EN_ON()     (RC5 = 1)
#define LCD_EN_OFF()    (RC5 = 0)
#define WRITE_MODE()    (RC4 = 0)

#define CLEAR_D0()      (RD0 = 0)
#define CLEAR_D1()      (RD1 = 0)
#define CLEAR_D2()      (RD2 = 0)
#define CLEAR_D3()      (RD3 = 0)

#define SEND_D0()       (RD0 = 1)
#define SEND_D1()       (RD1 = 1)
#define SEND_D2()       (RD2 = 1)
#define SEND_D3()       (RD3 = 1)

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);
void PMD_Initialize(void);
void InitADB4(void);
void InitADF5(void);
long double mean_sq(long double value, uint16_t n);
uint8_t measure_PI(long double val);
void LCD_Display(uint16_t value);
void LCD_Initialize(void);


#endif