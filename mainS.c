
/*El Sensor ultrasonico HC-SR04 trabaja a una frecuencia de 40KHz, mientras que el oido humano esta entre el rango de 20Hz a 20KHz.
 Rango de medici?n de 2 cm a 400 cm.
 Velocidad del sonido 340 m/s >>> 0.034cm/us   >>>>> (20*C).
 Formula: d=v*t  >>> Distancia = Velocidad del sonido*Tiempo de ida
 Distancia = (0.034cm/us)*(Tiempo/2)
 Distancia = 0.017*Tiempo de ida. */

/*==========================================================================================================
 ===========================================================================================================*/
#include <xc.h>                       // Incluimos todos los registros del PIC18F4550.
#include <stdint.h>                   // Incluimos esta librer?a para trabajar con variables enteras.
#include <stdio.h>                    // Incluimos esta librer?a para trabajar con perifericos de entrada/salida.
#include <string.h>                   // Incluimos esta librer?a para trabajar con cadenas de caracteres.
#include "Bits_Configuracion.h"       // Incluimos el archivo de cabecera para los fusibles.
#include "LCD_16x2_Library.h"         // Incluimos la librer?a LCD 16x2.

#define Pin_Trig    LATAbits.LATA0    // Asignamos el nombre "Pin_Trig" al pin RA0 (salida).
#define Pin_Echo    PORTAbits.RA1     // Asignamos el nombre "Pin_Echo" al pin RA1 (entrada).

/*==========================================================================================================
 ===========================================================================================================*/
uint16_t LCD_Buffer [16];              // Arreglo para mostrar las variabes en la pantalla LCD 2x16.
                                       // Variable de tipo flotante 32 bits para el tiempo.
/*==========================================================================================================
 ===========================================================================================================*/
void Configuracion_Registros (void);  // Funci?n para configurar registros de inicio.
void Timer1_Init (void);              // Funci?n para inicializar el Timer 1.
uint16_t Obtener_Distancia (void);    // Funci?n para obtener la distancia.

/*==========================================================================================================
 ===========================================================================================================*/
void main(void)                       // Funci?n Principal.
{
    uint16_t Distancia;               // Variable Distancia.
  
    Configuracion_Registros();        // Llamamos a la funci?n de configuraci?n de registros.
    Timer1_Init();                    // Inicializamos la configuraci?n del Timer1.
    lcd_init();                       // Inicializamos la pantalla LCD 2x16.
    
    while(1)
    {
        Distancia=Obtener_Distancia();// Cargamos la variable "Distancia" con el valor de distancia capturado por el sensor HC-SR04.
        lcd_gotoxy(1,1);              // Posicionamos el cursor en fila 1, columna 1.
        lcd_putc(" SENSOR HC-SR04 "); // Mostramos el mensaje en la pantalla LCD.
        sprintf(LCD_Buffer,"Distancia: %03dcm", Distancia);//Cargamos variable "Distancia" con formato en "LCD_Buffer".
        lcd_gotoxy(2,1);              //Ubicamos el cursor en fila 2, columna 1
        lcd_putc(LCD_Buffer);         //Mostramos el valor de buffer_lcd
        __delay_ms(200);    
    }
    return;
}

/*==========================================================================================================
 ===========================================================================================================*/
void Configuracion_Registros (void)   //Funci?n para configurar registros de inicio. 
{
    ADCON1bits.PCFG=0b1111;           // Deshabilitamos las entradas anal?gicas de los puerto A y B
    TRISA&=~(1<<0);                   // Configuramos el pin RA0 como salida digital.
    TRISA|=(1<<1);                    // Configuramos el pin RA1 como entrada digital.
}

/*==========================================================================================================
 ===========================================================================================================*/
void Timer1_Init (void)               // Funci?n de configuraci?n del Timer 1.
{
    T1CONbits.RD16=1;                 // Timer1 trabajando a 16 bits.
    T1CONbits.T1CKPS=0b00;            // Timer1  Pre-escaler=0.
    T1CONbits.TMR1CS=0;               // Internal clock (Fosc/4).
    TMR1=0;                           // Cargamos el registro TMR1 con el valor de 0.
    TMR1ON=0;                         // Temporizador Timer1 detenido.
}

/*==========================================================================================================
 ===========================================================================================================*/
uint16_t Obtener_Distancia (void) // Funci?n para obtener la distancia.
{
    uint16_t Duracion;         
    uint16_t Distancia;
    uint16_t Timer_1;
    
    Pin_Trig=1;                       // Ponemos a nivel alto el pin RA0.
    __delay_us(10);                   // Retardo de 10 us.
    Pin_Trig=0;                       // Ponemos a nivel bajo el pin RA0.
    while(Pin_Echo==0);               // Esperamos mientras el pin RA1 sea igual a 0.
    T1CONbits.TMR1ON=1;               // Activamos el temporizador del Timer1.
    while(Pin_Echo==1);               // Esperamos mientras el pin RA1 sea igual a 1.
    T1CONbits.TMR1ON=0;               // Detenemos el temporizador del Timer1.
    Timer_1=TMR1;                     // Cargamos la variable "Timer_1" con el valor del registro TMR1. (Tiempo en us)
    Duracion=Timer_1/2;             // Cargamos el valor de la variable "Timer_1"/2 (debido que con 8MHz se generan 0.5us) en la variable "Duracion".
    if(Duracion<=23200)               // 23529.4 us equivalen a 400cm.
    {
        Distancia=Duracion/58;     // Valor de la distancia en cm. (formula aplicada para us)
    }
    else if(Duracion<116)           // 117.6 us equivalen a 2 cm.
    {
        Distancia=0;                // Valor de distancia en cm.
    }
    else
    {
        Distancia=0;                // Valor de distancia en cm.
    }
    Duracion=0;                     // Reiniciamos el valor de la variable "Duracion".
    TMR1=0;                           // Reiniciamos el valor del registro TMR1.
    
    return Distancia;                 // Retornamos el valor de distancia.
}
