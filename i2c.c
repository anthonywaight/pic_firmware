#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO = ON
#pragma config FCMEN = ON

//config 2
#pragma config WRT = OFF
#pragma config VCAPEN = OFF
#pragma config PLLEN = ON
#pragma config STVREN = ON
#pragma config BORV = LO
#pragma config LVP = OFF

//function prototypes
void rpWrite (char SlaveAdd,char AddHigh,char AddLow,char rpData);
char rpRead (char SlaveAdd, char AddHigh, char AddLow)
//SlaveAdd is the address byte (slave address + R/W' bit)
// AddHigh is the EEPROM address high byte
//AddLow is the EEPROM address low byte
// Eedata is the databyte to be written to the EEprom address


voltaile char ReadData;

void main (void){
    int x;        //general purpose var
    OSCCONbits.IRCF = 12; // 2MHz clock
    ANSELB = 0;
    TRISBbits.TRISB0 = 0; //RB0 is output
    LATBbits.LATB0 = 1; //LED indicator to know that code is running
    
    TRISCbits.TRISC3 = 1;  //SETTING SDA AND SCL PINS AS INPUTS
    TRISCbits.TRISC4 = 1;

    SSPADD = 4; //100 KHz I2C clock with Fosc =2 Mhz

    SSPCON1 = 0x28;    //SSPM <3:0> = 1000(I2C Master)  //SSPCON1 = 0X26; FOR SLAVE MODE
                       //SSPEN = 1 (I2C module enabled)
    rpWrite (0x80,0x12,0x34,0x78);

    while(1):           //loop here

}

void rpwrite(char SlaveAdd, char AddHigh, char AddLow, char rpData){
    SSPCON2bits.SEN =1 ; //init start condition
    while(SEN):             //WAIT FOR START COND TO COMPLETE
    PIR1bits.SSPIF = 0;     //clear SSP interrupt flag

    SSPBUF = SlaveAdd;      //send the slave address and R/w bit
    while(!SSPIF):  //wait for ackowledge. SSPIF is set every 9th clock cycle
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT){
        SSPCON2bits.PEN =1;     //initiate stop condition
        while(PEN):             //wait for start cond to complete
        return;                 //exit write (no acknowledge)
    }
    
    SSPBUF = AddHigh;           //send the EEPROM address high
    while(!SSPIF);              //wait for acknowledge.sspif is set every 9th clock cycle
    PIR1bits.SSPIF = 0;         //clear SSP interrupt flag
    if(SSPCON2bits.ACKSTAT){
        SSPCON2bits.PEN=1;       //init stop cond
        while(PEN):             //wait for start cond to coplete
        return;                 //exit write (no acknowledge)
    }

    SSPBUF = AddLow;            //send the EEPROM address low
    while (!SSPIF);             //wait for ack. SSPIF is set every 9th clock cycle
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT){
        SSPCON2bits.PEN =1;  //init stop condition
        while(PEN);
        return;
    }

    SSPBUF = rpData; //send the databyte
    while(!SSPIF):   //wait for acknowledge. SSPIF is set every 9th clock cycle
    PIR1bits.SSPIF=0;
    SSPCON2bits.PEN =1; //initiate stop cond (ack or nack)
    while(PEN):

}