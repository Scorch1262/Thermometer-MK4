/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.80.0
        Device            :  PIC18F46K20
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "oled.h"
#include "peri.h"
#include "bmp.h"
#include "dht22.h"

extern char Temperature[];
extern char Humidity[];
extern unsigned char T_Byte1, T_Byte2, RH_Byte1, RH_Byte2, CheckSum ;
extern unsigned int Temp, RH;

/*
                         Main application
 */

void main(void){
    // Initialize the device
    SYSTEM_Initialize();                                                            //
    initI2C();                                                                      //
    oled_init();                                                                     // 

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    T1CON  = 0x20;                                                                  // set Timer1 clock source to internal with 1:4 prescaler (Timer1 clock = 1MHz)
    TMR1_Reload();                                                                  // setzt "TMR1" zur?ck// 

    fb_draw_string_big(0,0," Thermometer ");                                        // schreibt " Thermometer " an OLED
    fb_draw_string_big(44,2,"MK4");                                                 // schreibt "MK4" an OLED
    fb_draw_string(230,6,"V1.1");                                                   // schreibt "V1.0" an OLED
    fb_show();                                                                      // Den Framebuffer zur Anzeige bringen
    __delay_ms(1000);                                                               // wartet 1000ms
    fb_clear();                                                                     // 
    
    while (1){                                                                      // Endlosschleife
        Start_Signal();                                                             // ruft "Start_Signal" (DHT22) auf
        if(Check_Response()){                                                       // wenn "Check_Response" gleich "true"
            if(Read_Data(&RH_Byte1) || Read_Data(&RH_Byte2) || Read_Data(&T_Byte1) || Read_Data(&T_Byte2) || Read_Data(&CheckSum)){ // 
                printf("Time out!\r\n");                                            // schreibt "Time out!\r\n" an UART
                fb_draw_string_big(10,3,"Time");                                    // schreibt inhalte von "Time" an OLED
                fb_draw_string_big(10,5,"Out!");                                    // schreibt inhalte von "Out!" an OLED
                fb_show();                                                          // Den Framebuffer zur Anzeige bringen
            }else{                                                                  // sonst
                if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF)){ // ?berpr?fung der "CheckSum"
                    RH = RH_Byte1;                                                  // "RH" gleich "RH_Byte1"
                    RH = (RH << 8) | RH_Byte2;                                      //
                    Temp = T_Byte1;                                                 // "Temp" gleich "T_Byte1"
                    Temp = (Temp << 8) | T_Byte2;                                   // 

                    if(Temp > 0X8000){                                              // wenn "Temp" gr??er als "0X8000"
                        Temperature[0] = '-';                                       // "Temperature" stelle 6 gleich "-"
                        Temp = Temp & 0X7FFF;                                       // "Temp" gleich "Temp" UND 0X7FFF
                    }else{                                                          // sonst
                        Temperature[0]  = ' ';                                      // "Temperature" stelle 3 gleich " "
                        Temperature[1]  = (Temp / 100) % 10  + '0';                 // "Temperature" stelle 4
                        Temperature[2]  = (Temp / 10) % 10   + '0';                 // "Temperature" stelle 5
                        Temperature[4]  =  Temp % 10  + '0';                        // "Temperature" stelle 7
                    }                                                               //
                    if(RH == 1000){                                                 // wenn "RH" gleich 1000 ist
                        Humidity[0]  = 1 + '0';                                     // "Humidity" stelle 3 gleich 1 + "0" 
                    }else{                                                          // sonst
                        Humidity[0]  = ' ';                                         // "Humidity" stelle 3 gleich " "
                        Humidity[1]  = (RH / 100) % 10 + '0';                       // "Humidity" stelle 4
                        Humidity[2]  = (RH / 10) % 10  + '0';                       // "Humidity" stelle 5
                        Humidity[4]  = RH % 10 + '0';                               // "Humidity" stelle 7
                    }                                                               //
                    
                    printf("Temperatur: ");                                         // schreibt "Temperatur: " an UART
                    printf(Temperature);                                            // schreibt inhalte von "Temperature" an UART
                    printf("\t");                                                   // schreibt Tab an UART
                    printf("Feuchtigkeit: ");                                       // schreibt "Feuchtigkeit: " an UART
                    printf(Humidity);                                               // schreibt inhalte von "Humidity" an UART
                    printf("\r");                                                   // setzt die Ziel zurück
                    
                    fb_draw_string_big(0,0," Thermometer ");                        // schreibt inhalte von "Temperature" an OLED
                    fb_draw_string_big(15,3,Temperature);                           // schreibt inhalte von "Temperature" an OLED
                    fb_draw_string_big(15,6,Humidity);                              // schreibt inhalte von "Humidity" an OLED
                    fb_draw_string(10,2,"Temperatur:");                             // schreibt "Temperatur" an OLED
                    fb_draw_string(10,5,"Feuchtigkeit:");                           // schreibt "Feuchtigkeit" an OLED
                    fb_show();                                                      // Den Framebuffer zur Anzeige bringen

                }else{                                                              // sonst
                    printf("Checksum Error!\r\n");                                  // schreibt "Checksum Error!\r\n" an UART
                    fb_draw_string_big(10,3,"Checksum");                            // schreibt inhalte von "Checksum" an OLED
                    fb_draw_string_big(10,5,"Error!");                              // schreibt inhalte von "Error!" an OLED
                    fb_show();                                                      // Den Framebuffer zur Anzeige bringen
                }                                                                   //
            }                                                                       //
        }else{                                                                      // sonst
            printf("No response from the sensor\r\n");                              // schreibt "No response from the sensor\r\n" an UART
            fb_draw_string_big(10,3,"No response");                                 // schreibt inhalte von "No response" an OLED
                    fb_draw_string_big(10,5,"from Sensor");                         // schreibt inhalte von "from Sensor" an OLED
                    fb_show();                                                      // Den Framebuffer zur Anzeige bringen
        }                                                                           //
        TMR1_StopTimer();                                                           // stoppt "TMR1" 
        __delay_ms(1000);                                                           // warte 1000ms
    }                                                                               //
}                                                                                   //
/**
 End of File
*/
