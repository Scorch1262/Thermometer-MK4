#include "mcc_generated_files/mcc.h"
#include "dht22.h"

// variables declaration
char Temperature[] = " 00.0C";
char Humidity[]    = " 00.0%";
unsigned char T_Byte1, T_Byte2, RH_Byte1, RH_Byte2, CheckSum ;
unsigned int Temp, RH;

void Start_Signal(void){                        // Start_Signal
    Data_SetDigitalOutput();                    // setzt "Data" auf Output
    Data_SetLow();                              // setzt "Data" auf 0
    __delay_ms(25);                             // warte 25ms
    Data_SetHigh();                             // setzt "Data" auf 1
    __delay_us(30);                             // warte 30ms
    Data_SetDigitalInput();                     // setzt "Data" auf Input
}                                               // 

bool Check_Response(){                          // Check_Response
    TMR1_Reload();                              // setzt "TMR1" zur?ck
    TMR1_StartTimer();                          // startet "TMR1" 
    while(!Data_GetValue() && TMR1L < 100);     // solange "Data" = 0 und "TMR1L" kleiner als 100
    if(TMR1L > 99){                             // wenn "TMR1L" gr??er als 99
        return 0;                               // return 0
    }else{                                      // sonst
        TMR1_Reload();                          // setzt "TMR1" zur?ck
        while(Data_GetValue() && TMR1L < 100);  // solange "Data" = 1 und "TMR1L" kleiner als 100
        if(TMR1L > 99){                         // wenn "TMR1L" gr??er als 99
            return 0;                           // return 0
        }else{                                  // sonst
            return 1;                           // return 1
        }                                       //
    }                                           //
}                                               //

bool Read_Data(unsigned char* dht_data){        // Read_Data
    *dht_data = 0;                              // "dht_data" gleich 0
    for(char i = 0; i < 8; i++){                // soange bis "i" gleich 8 ist
        TMR1_Reload();                          // setzt "TMR1" zur?ck
        while(!Data_GetValue()){                // solange "Data_GetValue" = 0
            if(TMR1L > 100) {                   // wenn "TMR1L" gr??er als 100
                return 1;                       // return1
            }                                   //
        }                                       //
        TMR1_Reload();                          // setzt "TMR1" zur?ck
        while(Data_GetValue()){                 // solange "Data_GetValue" = 1
            if(TMR1L > 100) {                   // wenn "TMR1L" gr??er als 100
                return 1;                       // return 1
            }                                   //
        }                                       //
        if(TMR1L > 50){                         // wenn "TMR1L" gr??er als 50
            *dht_data |= (1 << (7 - i));        // "dht_data" ???????
        }                                       //
    }                                           //
    return 0;                                   // return 0 (data read OK)
}                                               // 