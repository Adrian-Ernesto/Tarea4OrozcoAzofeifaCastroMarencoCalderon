/*
 * File:   PIC 18F4550.c
 * Author: FrontDesk2
 *
 * Created on 13 de octubre de 2021, 09:51 PM
 */
// TAREA 4 IMPLEMENTACION PIC
//KARINA AZOFEIFA AMPIE
//ADRIAN ERNESTO OROZCO RIVERA
//DANIEL CASTRO SANCHEZ
//DIEGO MARENCO CHAVES
//LEONARDO CALDERON MONGE

#include <xc.h>
#include <stdbool.h>
#include <pic18f4550.h>

#pragma config WDT = OFF
#pragma config WDTPS = 1024

int cajas = 0;
int contador_segundos = 0;
int contador_ciclos = 0;




void main(void) {
    ////////////////////////////////////////////////////////////////////////////
    // SETUP DE PUERTOS
    ////////////////////////////////////////////////////////////////////////////
    ADCON1 = 0b00001111; //Set de todos los pines como digitales
    //Revisar más adelante==========================================
    TRISA = 0b00000000; //Set de RA4 como entrada y el resto salidas
    //Se setea puerto A1 en alto y el resto en bajo
    LATAbits.LA1 = 0;
    LATAbits.LA2 = 0;
    LATAbits.LA3 = 0;
    LATAbits.LA4 = 0;
    LATAbits.LA5 = 0;
    
    ////////////////////////////////////////////////////////////////////////////
    // SETUP DE INTERRUPCIONES
    ////////////////////////////////////////////////////////////////////////////
    //Se habilitan interrupciones de prioridad baja y alta
    //La idea es que el boton stopper sea prioridad alta
    //Mientras que sensores y timer de 5 segundos son prioridad baja
    RCONbits.IPEN     = 1; //Diferenciar prioridades altas y bajas
    INTCONbits.GIEH   = 1; //Habilitar prioridad alta
    INTCONbits.GIEL   = 1; //Habilitar prioridad baja

    //Setup de INT0
    //Usar para boton stopper de high priority
    INTCONbits.INT0IF   = 0; //Se limpia la bandera de INT0
    INTCON2bits.INTEDG0 = 1; //Setea el external interrupt 0 para rising edge
    INTCONbits.INT0IE   = 1; //Habilita INT0 como external interrupt
    
    //Setup de INT1
    //Usar para sensor entrada de low priority
    INTCON3bits.INT1IF   = 0; //Se limpia la bandera de INT1
    INTCON2bits.INTEDG1 = 1; //Setea el external interrupt 1 para rising edge
    INTCON3bits.INT1IE   = 1; //Habilita INT1 como external interrupt
    INTCON3bits.INT1IP = 0; //INT1 es baja prioridad
    
    //Setup de INT2
    //Usar para sensor salida de low priority
    INTCON3bits.INT2IF   = 0; //Se limpia la bandera de INT2
    INTCON2bits.INTEDG2 = 1; //Setea el external interrupt 1 para rising edge
    INTCON3bits.INT2IE   = 1; //Habilita INT2 como external interrupt
    INTCON3bits.INT2IP = 0; //INT2 es baja prioridad
    
    
    ////////////////////////////////////////////////////////////////////////////
    // SETUP DE TIMER 0
    ////////////////////////////////////////////////////////////////////////////
    //Timer para contar los 5 segundos y actualizar leds
    T0CONbits.T08BIT = 1; //Timer de 8 bits de dato
    T0CONbits.T0CS   = 0; //Clock interno CLK0
    // No importa el flanco
    //T0CONbits.T0SE   = 0; //Flanco positivo en patilla T0CK1
    T0CONbits.PSA    = 0; //Con prescale
    //El siguiente prescale sólo se usa para acelerar la simulación.
    //Para aplicarlo, setear T0CONbits.PSA = 0
    T0CONbits.T0PS2 = 1;
    T0CONbits.T0PS1 = 1;  // 1:256 Preescala de 4 XXXXXXXXXXXXXXXX
    T0CONbits.T0PS0 = 1;
    INTCON2bits.TMR0IP = 0; //TMR0 genera una interrupcion low priority
    INTCONbits.TMR0IE = 1; //Si TMR0 hace overflow (back to 0) se hace interrupt
    T0CONbits.TMR0ON = 1; //Se enciende el timer
    
    //Se inicia el timer faltando 5 segundos para overflow/interrupción
    TMR0L = 0x00;
    
    ////////////////////////////////////////////////////////////////////////////
    // CICLO PRINCIPAL
    ////////////////////////////////////////////////////////////////////////////
    while(1){
    }
    return;
}



void funcion_cambio_leds() {
   // EN CASO DE TENER CASOS SIN SENTIDO NEGATIVOS SE MAPEA A -1 CAJAS
    if(cajas < -1){
        cajas = -1;
    }
    switch(cajas){
        case -1: //En caso de error
            LATAbits.LA1 = 0;
            LATAbits.LA2 = 1;
            LATAbits.LA3 = 0;
            LATAbits.LA4 = 1;
            break;
        case 0:
            LATAbits.LA1 = 0;
            LATAbits.LA2 = 0;
            LATAbits.LA3 = 0;
            LATAbits.LA4 = 0;
            break;
        case 1:
            LATAbits.LA1 = 1;
            LATAbits.LA2 = 0;
            LATAbits.LA3 = 0;
            LATAbits.LA4 = 0;
            break;
        case 2:
            LATAbits.LA1 = 0;
            LATAbits.LA2 = 1;
            LATAbits.LA3 = 0;
            LATAbits.LA4 = 0;
            break;
        case 3:
            LATAbits.LA1 = 0;
            LATAbits.LA2 = 0;
            LATAbits.LA3 = 1;
            LATAbits.LA4 = 0;
            break;
        case 4:
            LATAbits.LA1 = 0;
            LATAbits.LA2 = 0;
            LATAbits.LA3 = 0;
            LATAbits.LA4 = 1;
            break;
        default: //cajas >=5
            LATAbits.LA1 = 1;
            LATAbits.LA2 = 1;
            LATAbits.LA3 = 1;
            LATAbits.LA4 = 1;
            break;
          
    }
}
// FUNCION USADA PARA ACTUALIZAR EL CONTEO DE CAJAS EN X SEGUNDOS 
// LA FRECUENCIA DEL TMR0 ES DE 1MHZ SIN PRESCALA. 
bool segundo(){
    //CONTADOR USADO PARA ASOCIAR LOS CICLOS DE RELOJ DEL PIC CON LA CANTIDAD CORRESPONDIENTE EN SEGUNDOS 
    contador_ciclos = contador_ciclos + 1;
    //CON ESTE CONDICIONAL SE DECIDE LA CANTIDAD DE SEGUNDOS DE ESPERA ANTES DE REFRESCAR EL CONTEO
    //EN ESTE EJEMPLO SE REQUIERE UN VALOR DE 15 PARA CONTAR 1 SEGUNDO
    if(contador_ciclos == 15){
        contador_ciclos = 0;
        contador_segundos = contador_segundos + 1; 
        
    }
    //CUANDO SE CUENTAN 5 SEGUNDOS SE RESETEA EL CONTADOR Y AVISA AL VECTOR DE INTERRUPCIONES
    if(contador_segundos == 5){
        contador_segundos = 0;
        return true;
    }
    else{
        return false;
    }
}

//Interrupciones High priority
void __interrupt() isr(void){ 
    if (INTCONbits.INT0IF == 1){
        LATAbits.LA5 = ~LATAbits.LA5;
        // Se Limpia INT0IF
        INTCONbits.INT0IF = 0;
        // Se deshabilitan o habilitan los sensores
        // y la interrupción por el timer
        RCONbits.IPEN = ~RCONbits.IPEN;
        //Devolver el contador a T-5s
        TMR0L = 0x00;
        // Se limpia el interrupt
        INTCON3bits.INT1IF = 0;
        // Se limpia el interrupt
        INTCON3bits.INT2IF = 0;
        // Se reinicia la Interrupcion
        INTCONbits.TMR0IF = 0;
    }
}

//Interrupciones Low Priority
void __interrupt(low_priority) Lowisr(void){
    //interrupt para el sensor de entrada
    if (INTCON3bits.INT1IF == 1){
        // Suma una caja
        cajas = cajas + 1;
        // Se limpia el interrupt
        INTCON3bits.INT1IF = 0;
    }
    
    //Interrupt para el sensor de salida
    if (INTCON3bits.INT2IF == 1){ //Cambiar por otro pin para sensor de salida
        // Resta una caja
        cajas = cajas - 1;
        // Se limpia el interrupt
        INTCON3bits.INT2IF = 0;
    }
    
    // Handler particular para al interrupción TMR0
    // Interrupcion porque se alcanzan los 5 segundos
    if(INTCONbits.TMR0IF) {
        if(segundo() == true){
            // Llamar a funcion para cambiar estados de leds
            funcion_cambio_leds();
        };
        // Se reinicia la Interrupcion
        INTCONbits.TMR0IF = 0;
        
        //Devolver el contador a T-5s
        TMR0L = 0x00;
        
    }
}
