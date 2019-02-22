#include <avr/io.h>
#include <util/delay.h>
#include "CustomSoftwareSerial.h"
#include <EEPROM.h>

/** --------------------------------------------------------------- definitions for USART calculations--------------------------- **/
#define USART_BAUDRATE 9600 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 

/**--------------------------------------------------------------- helper function definitions as preprocessor directives --------- **/
#define bit_get(p,m) ((p) & (m)) 
#define bit_set(p,m) ((p) |= (m)) 
#define bit_clr(p,m) ((p) &= ~(m)) 
#define bit_flip(p,m) ((p) ^= (m)) 
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m)) 
#define BIT(x) (0x01 << (x))

#define IC_A_DIR 18 //A4 (RXx IC) LOW  (Bx to Ax)
#define IC_B_DIR 19 //A5 (TXx IC) HIGH (Ax to Bx)


/** SoftwareSerial pointer array **/
CustomSoftwareSerial* s[8];

/** -----------------------------------------------------------------------------Pin Assignments & Mappings ---------------------**/
int TX[8] = //IC_B
  {
    9, //TX0 = D9
    8, //TX1
    7, //TX2
    6, //TX3
    5, //TX4
    4, //TX5
    3, //TX6
    2  //TX7 = D7
  };

int RX[8] = //IC_A
  {
    10, //RX0 = D10
    11, //RX1
    12, //RX2
    13, //RX3
    A0, //14, //RX4 = A0
    A1, //15, //RX5
    A2, //16, //RX6
    A3 //17 //RX7 = A3
  };

/**  ----------------------------------------------------------------------Buffer ic initializations ----------- **/

void init_buffer_ics(){
  /** Both ICs were in Output Enable mode by default (hard wired)**/

  /** configuring signal direction**/
  digitalWrite(IC_A_DIR, 0); //RX IC
  digitalWrite(IC_B_DIR, 1); //TX IC
}


/**  ----------------------------------------------------------------------EEPROM implementations------------- **/

//write serial settings to eeprom
void eeprom_write(unsigned char serial_port, unsigned char setting){
  EEPROM.write(serial_port, setting); //byte
}

//read serial setting from eeprom
unsigned char eeprom_read(unsigned char serial_port){
  return (unsigned char) EEPROM.read(serial_port);
}


/**  ----------------------------------------------------------------------Software serial implementations------------ **/
int BAUD_RATE[] = {4800, 9600, 14400, 19200, 38400, 57600, 115200}; //7 (3bits)

void init_soft_serial(){
  for(int i=0; i<8; i++){
    s[i] = new CustomSoftwareSerial(RX[i], TX[i]); // rx, tx
    //load settings from eeprom if necessary
    load_software_serial_settings();
  }
}

int get_serial_setting(unsigned char conf){
    //configs
    //data,parity,stop
    //{5-8},{0,1,2},{1,2} =>> ex: 501 = 5N1, 721 = 7E1
    //[4:3][2:1][0]
    return
    ((conf & 0b00011000) >> 3)*100 +  //data
    ((conf & 0b00000110) >> 1)*10 +   //parity
    (conf & 0b00000001)+1;            //stop
        
}

void load_software_serial_settings(){
  unsigned char setting;
  for(unsigned char i =0; i<8; i++){
    setting = eeprom_read(i);
          //baud   
    if((setting & 0b11100000)!= 0b11100000){ //0b11100000 is for unconfigured
        apply_setting_software_serial(
          i, //serial port
          BAUD_RATE[(setting & 0b11100000)], //baud_rate
          get_serial_setting(setting & 0b00011111) //conf
        );
    }else{ // no valid saved settings, apply default
        apply_setting_software_serial(
          i, //serial port
          9600, //baud_rate
          CSERIAL_8N1 //conf
        );
    }
  }
}

void apply_setting_software_serial(unsigned char serial, int baud_rate, unsigned char conf){
   s[serial]->begin(baud_rate, conf);         // Baud rate: 9600, configuration: CSERIAL_8N1
}

void write_s_serial(unsigned char sn, unsigned char data){
  s[sn]->write(data);
}

unsigned char read_s_serial(unsigned char sn){
  return s[sn]->read();
}

/** ------------------------------------------------------------------------USART communication implementations-------------- **/

void init_usart(){
   UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry 
   UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes 

   UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register 
   UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
}

unsigned char read_usart(){
  while ((UCSR0A & (1 << RXC0)) == 0) {}; // Do nothing until data have been received and is ready to be read from UDR 
  return UDR0; // Fetch the received byte value into the variable "ByteReceived" 
}

void write_usart(unsigned char data){
  while ((UCSR0A & (1 << UDRE0)) == 0) {}; // Do nothing until UDR is ready for more data to be written to it 
  UDR0 = data; // Echo back the received byte back to the computer
}

/** -----------------------------------------------------------------------------------controller program--------------- **/

#define M_CONF_WRITE    0b00000001
#define M_CONF_READ     0b00000010
#define M_DATA_READ     0b00000100
#define M_DATA_WRITE    0b00001000

#define M_FROM_DEVICE_CONF_WRITE_ACK    0b00010001
#define M_FROM_DEVICE_CONF_READ         0b00010010
#define M_FROM_DEVICE_DATA_READ         0b00010100
#define M_FROM_DEVICE_DATA_WRITE_ACK    0b00011000

#define M_FROM_DEVICE_DATA_SERIAL       0b00011111


void change_state(){
  unsigned char cmd = read_usart(); //contains both the serial port & mode information

  unsigned char serial = (cmd >> 5); // serial port, specifies which serial port.
  unsigned char mode = (cmd & 0b00011111); // mode

    
  if(mode == M_CONF_WRITE){ // configuration write mode
    
    unsigned char data = read_usart(); //read configurations to be written

    //extract baud_rate, conf
    // baud [7:5] conf[4:0]
    apply_setting_software_serial(
          serial, //serial port
          BAUD_RATE[(data & 0b11100000)], //baud_rate
          get_serial_setting(data & 0b00011111) //conf
    );
    
    //change serial port setting, save and setting in the eeprom
    eeprom_write(serial, data); //write to memory

     //applying setting to the serial

    write_usart(M_FROM_DEVICE_CONF_WRITE_ACK | (serial<<5)); //ACK
 
    
  }else if(mode == M_DATA_WRITE){ // data write mode

    unsigned char data = read_usart(); //read data

    // write data to serial using software serial
    write_s_serial(serial, data);
    
    write_usart(M_FROM_DEVICE_DATA_WRITE_ACK | (serial<<5)); //ACK
    

    
  }else if(mode == M_DATA_READ){ //data read mode
    // read data from serial using software serial
    unsigned char resp = read_s_serial(serial);
    // now send that
    //write_usart(M_FROM_DEVICE_DATA_READ | (serial<<5)); //token
    write_usart(resp); //data
    //write_usart('3');
    
  }else if(mode == M_CONF_READ){ //configuration read mode
    // read running config from relevant to the serial
    unsigned char conf = eeprom_read(serial);
    
    //write_usart(M_FROM_DEVICE_CONF_READ | (serial<<5)); //token
    write_usart(conf);
    //write_usart('2');
    
  }


  //check serial
//  for(unsigned char i=0; i<8;i++){
//    if(s[i]->available()){
//      
//      write_usart(M_FROM_DEVICE_DATA_SERIAL | (i<<5)); //token
//
//      //todo check this, whether actually gives only byte
//      write_usart(s[i]->read()); //data
//    }
//  }

}

/** echo program for testing purpose **/
void echo(){
  unsigned char ReceivedByte;
  ReceivedByte = read_usart(); // Fetch the received byte value into the variable "ByteReceived" 
  write_usart(ReceivedByte);
}

void echo2(){
  write_usart('E'); delay(1);
}

int send_stable(){
  write_usart('K');write_usart('_');write_usart('@');write_usart('Q'); return 0;
}


/** ---------------------------------------------------------------------------------------------MAIN()--------------- **/

int main(void){
   
   /** initializing buffer ics **/
   init_buffer_ics();

   /** Initializing the USART between PC and the Arduino **/
   init_usart();

   /** sending at the beginning special character sequence [K_@Q]**/
   //send_stable();

   /** initializing software serial **/
   init_soft_serial();


  /** put the device into working mode **/
   for (;;){ // Loop forever

      //echo2();
      //send_stable();
      change_state();
   
   }
   
}
