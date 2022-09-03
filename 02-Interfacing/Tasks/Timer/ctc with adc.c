 /*
 * AVR.c
 *
 * Created: 7/4/2022 1:58:41 PM
 * Author : Osama
 */ 


#define  F_CPU    8000000
#include <util/delay.h>

#include "MemMap.h"
#include "StdTypes.h"
#include "Utils.h"
#include "DIO_Interface.h"
#include "Motor_Interface.h"
#include "LCD_Interface.h"
#include "KEYPAD_Interface.h"
#include "TIMER_Interface.h"
#include "ADC_Interface.h"



int main(void)
{
	sei();
	DIO_Init();
	LCD_Init();
	
	ADC_Init(ADC_VCC , divfact64);
	Timer0_Init(TIMER0_CTC_MODE , TIMER0_SCALER_8 ,  OCO_TOGGLE);
	OCR0 = 0; 
	LCD_WriteSentence("CTC   ADC:");
	LCD_GoTo(1,6);
	LCD_WriteSentence("OCR:");
	u16 adc=0;

	while (1)
	{ 
		
		adc = ADC_Read(ADC7);
		OCR0 = adc / 4;
		LCD_GoTo(0 , 10);
		LCD_WriteNumber_4D(adc);
		LCD_GoTo(1 , 10);
		LCD_WriteNumber_3D(OCR0);
		
	}
}



