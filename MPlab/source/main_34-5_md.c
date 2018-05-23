/*******************************************************************************
Nome do arquivo:	main_34-4_md.c            
Data:	    		27 abril de 2018          
Versao:				1.4                              
MPLAB IDE:			MPLAB-X v.4.15 
Autor:				Wagner Zanco
Modificado por:     Carl�cio Silva         
Descri��o: Configura��o do TIMER0 para operar como temporizador no modo 16 bits, 
com interrup��o com a codifica��o desenvolvida dentro de configTMR0() com passagem
de par�metro e com a inicializa��o do TIMER desmembrada da configura��o. Com
uma fun��o para tratar a interrup��o em separado.     
*******************************************************************************/
#include <p18f4550.h>			//diretiva de compila��o
#include "MCC18_18F4550.h"      //diretiva de compila��o

//******************************************************************************
//Prot�tipos de fun��es
void configSFR(void);
void configTMR0(unsigned char); //configura o TIMER0 - modo 8 bits
void initTMR0_08BIT(unsigned char); //inicializa o contador de 8 bits do TIMER0
void initTMR0_16BIT(unsigned int); //inicializa o contador de 16 bits do TIMER0
void configInterruptTMR0(unsigned char); //inicializa ineterrup��o
void high_isr(void); //trata a interrup��o


//******************************************************************************
// Declara��o de vari�veis globais
unsigned char flagPORTD = 0;

//******************************************************************************
/* Vetor de interrup��o de alta prioridade.
 * Toda vez que ocorre uma interrup��o o fluxo do program � desviado para a 
 * regi�o de mem�ria definida por essa instru��o, mais especificamente, para o 
 * endere�o 0x08 da Mem�ria de Programa. */ 
#pragma code high_vector=0x08
void interrupt_at_high_vector(void)
{
    /*Desvia programa para rotina de tratamento da interrup��o*/
	_asm GOTO high_isr _endasm		
}
#pragma code

//******************************************************************************
void main(void){
    configSFR();
    // Configura��o do TIMER0
    configTMR0(0b10000111);     //Passo 1
    initTMR0_16BIT(65450);      //Passo 2
    configInterruptTMR0(1);     //Passo 3
    
	while(1);
}

/*******************************************************************************
Esta fun��o inicializa os registradores SFRs.*/
 void configSFR(void)
{
	TRISA = 0x00;		//PORTA sa�da
	TRISB = 0x00;		//PORTB sa�da
	TRISC = 0xFF;		//PORTC entrada
	TRISD = 0x00;		//PORTD sa�da
	TRISE = 0x00;		//PORTE sa�da
	ADCON1 = 0x0F;		//configura pinos dos PORTA e PORTE como digitais
	PORTA = 0;			//limpa PORTA
	PORTB = 0;			//limpa PORTB
	PORTC = 0;			//limpa PORTC
	PORTD = 0;  		//limpa PORTC
	PORTE = 0;			//limpa PORTE  
}
 
 /****************************************************************************** 
 * Rotina de tratamento de interrup��o (ISR)
 * Depois que ocorre a interrup��o, o vetor de interrup��o direciona o fluxo do
 * programa para a localiza��o da Mem�ria de Programa onde encontra-se essa 
 * instru��o, "high_isr".*/
#pragma interrupt high_isr
void high_isr(void)
{
    if(INTCONbits.TMR0IF){
        if(flagPORTD){
            PORTD = 0xFF;
            flagPORTD = 0x00;
        }
        else{
            PORTD = 0x00;
            flagPORTD = 0xFF;
        }
        INTCONbits.TMR0IF = 0;      //limpa a flag de estouro  
        initTMR0_16BIT(65450);
    }
} 
 
 /******************************************************************************
  Esta funcao inicializa os registradores SFRs do TIMER 0*/  
 void configTMR0(unsigned char configTMR0)
 {
//******************************************************************************
/* Passo 1: Configurar o TMR0 para operar como temporizador e estourar a cada 1s
 * T0CON<7> (TMR0ON = 1): Liga o TIMER 0
 * T0CON<6> (T08BIT = 0): Operar no modo 8 bits
 * T0CON<5> (T0CS = 0)  : Operar com o clock do Fosc/4
 * T0CON<4> (T0SE = 0)  : Operar na borda de subida **
 * T0CON<3> (PSA = 0)   : Operar a partir do prescaler
 * T0CON<2:0> (T0PS2 = 1; T0PS1 = 1; T0PS0 = 1): Configura o prescaler para 1:256,
 * com essa configura��o o prescaler proporciona um atrso de 128us. */   
     T0CON = configTMR0;  //0b10000111
 } 

 /******************************************************************************
Esta funcao habilita/desabilita a interrup��o por estouro do TIMER0*/ 
 void configInterruptTMR0(unsigned char interruptTMR0){
     if(interruptTMR0){
         INTCONbits.TMR0IE = 1;	//liga chave individual de interrup��o do TIMER 0     
         INTCONbits.GIE = 1;	//liga chave geral de interrup��o             
     }
     else{
         INTCONbits.TMR0IE = 0;	//desliga chave individual de interrup��o do TIMER 0     
         INTCONbits.GIE = 0;    //desliga chave geral de interrup��o            
     }
 }
 
/******************************************************************************
Esta funcao inicializa o registrador TIMER0 para o modo 8 bits*/ 
 void initTMR0_08BIT(unsigned char initTMR0){
     TMR0L = initTMR0;          
 }
 
 /******************************************************************************
Esta funcao inicializa o registrador TIMER0 para o modo 16 bits*/ 
 void initTMR0_16BIT(unsigned int initTMR0){
     TMR0H = initTMR0 >> 8;
     TMR0L = initTMR0;          
 }