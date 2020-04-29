/*******************************************************************************
 * File:        peri.c
 * Project:     SP18 - I2C OLED Display
 * Author:      Nicolas Pannwitz
 * Version:     
 * Web:         http://pic-projekte.de
 ******************************************************************************/

#include <xc.h>
#include <stdint.h>

/*******************************************************************************
 * I2C
 */

void initI2C(void)
{
    /* Zugeh�rige Tris-Bits auf Input schalten */
    /* Master-Mode - Clock = Fosc / (4*(SSPxADD+1)) */  
    /* SSPxADD values of 0, 1 or 2 are not supported for I2C Mode */
        
    SSPCON1bits.SSPM3 = 1;
    SSPCON1bits.SSPM2 = 0;
    SSPCON1bits.SSPM1 = 0;
    SSPCON1bits.SSPM0 = 0;
    SSPCON1bits.SSPEN = 1;
    SSPADD = 9;                      // f�r 200kHz (Fosc = 8 MHz)
    SSPCON2bits.ACKDT = 0;
}

void waitI2C(void)
{
    while(!PIR1bits.SSPIF);          // Aktion* wurde beendet
    PIR1bits.SSPIF = 0;              // Flag zur�cksetzten
} 
 
// *Zum Beispiel: START-Sequenze, �bertragung von 8 Bit, ...

void idleI2C(void)
{
    while( SSPSTATbits.R_W );        // L�uft eine �bertragung?
    while( SSPCON2 & 0x1F );         // Ist irgendwas anderes aktiv?
}

void startI2C(void)
{
    idleI2C();                        // Ist der Bus frei?
    SSPCON2bits.SEN = 1;             // Ausl�sen einer START-Sequenze
    while( SSPCON2bits.SEN );        // Beendet, wenn SEN gel�scht wurde*
}
 
void restartI2C(void)
{
    idleI2C();                        // Ist der Bus frei?
    SSPCON2bits.RSEN=1;              // Ausl�sen einer RESTART-Sequenze
    while( SSPCON2bits.RSEN );       // Beendet, wenn RSEN gel�scht wurde*
}
 
void stopI2C(void)
{
    idleI2C();                        // Ist der Bus frei?
    SSPCON2bits.PEN = 1;             // Ausl�sen einer RESTART-Sequenze
    while( SSPCON2bits.PEN );        // Beendet, wenn RSEN gel�scht wurde*
}
 
// *Oder das Bit SSPxIF gesetzt wurde (siehe Datenblatt)

uint8_t sendI2C(uint8_t byte)
{
    idleI2C();                        // Ist der Bus verf�gbar?
    PIR1bits.SSPIF = 0;              // Flag l�schen (wird in waitI2C() abgefragt)
    SSPBUF = byte;                   // Durch f�llen des Puffers Sendevorgang ausl�sen
    waitI2C();                        // Warten bis �bertragung abgeschlossen ist
 
    return ~SSPCON2bits.ACKSTAT;     // Return 1: ACK empfangen, 0: kein ACK empfangen
}

uint8_t reciveI2C_nack(void)
{
    uint8_t incomming = 0;            // Einlesepuffer
 
    idleI2C();                        // Ist der Bus verf�gbar?
    PIR1bits.SSPIF = 0;              // Flag l�schen (wird in waitI2C() abgefragt)
    SSPCON2bits.RCEN = 1;            // Als Empf�nger konfigurieren (wird autom. wieder gel�scht)
    waitI2C();                        // Warten bis �bertragung (lesend) abgeschlossen ist
    SSPCON2bits.ACKDT = 1;           // Mit NACK quitieren (Nicht weiter einlesen)
    SSPCON2bits.ACKEN = 1;           // NACK aussenden
    while( SSPCON2bits.ACKEN );      // NACK abgeschlossen?
    incomming = SSPBUF;              // Empfangenes Byte in den Puffer
        
    return incomming;                 // Und zur�ck geben
}
 
uint8_t reciveI2C_ack(void)
{
    uint8_t incomming = 0;            // Einlesepuffer
 
    idleI2C();                        // Ist der Bus verf�gbar?
    PIR1bits.SSPIF = 0;              // Flag l�schen (wird in waitI2C() abgefragt)
    SSPCON2bits.RCEN = 1;            // Als Empf�nger konfigurieren (wird autom. wieder gel�scht)
    waitI2C();                        // Warten bis �bertragung (lesend) abgeschlossen ist
    SSPCON2bits.ACKDT = 0;           // Mit AACK quitieren (weiter einlesen)
    SSPCON2bits.ACKEN = 1;           // ACK aussenden
    while( SSPCON2bits.ACKEN );      // ACK abgeschlossen?
    incomming = SSPBUF;              // Empfangenes Byte in den Puffer
 
    return incomming;                 // Und zur�ck geben
}