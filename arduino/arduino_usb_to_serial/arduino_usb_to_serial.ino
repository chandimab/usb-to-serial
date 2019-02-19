#include <avr/io.h>
#include <util/delay.h>
#include <CustomSoftwareSerial.h>

/** definitions for USART calculations **/
#define USART_BAUDRATE 9600 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 

/** helper function definitions as preprocessor directives **/
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

/** Pin Assignments & Mappings **/
int TX[8] = //IC_B
  {
    2, //TX0 = D2
    3, //TX1
    4, //TX2
    5, //TX3
    6, //TX4
    7, //TX5
    8, //TX6
    9  //TX7 = D9
  };

int RX[8] = //IC_A
  {
    10, //RX0 = D10
    11, //RX1
    12, //RX2
    13, //RX3
    14, //RX4 = A0
    15, //RX5
    16, //RX6
    17 //RX7 = A3
  };




void init_soft_serial(){
  for(int i=0; i<8; i++){
    s[i] = new CustomSoftwareSerial(RX[i], TX[i]); // rx, tx
    s[i]->begin(9600, CSERIAL_8N1);         // Baud rate: 9600, configuration: CSERIAL_8N1
    //customSerial->write("Test message");            // Write testing data
  }
}

void write_s_serial(unsigned char sn, unsigned char data){
  s[sn]->write(data);
}

unsigned char read_s_serial(unsigned char sn){
  return s[sn]->read();
}

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

#define M_CONF_WRITE    0b00000001
#define M_CONF_READ     0b00000010
#define M_DATA_READ     0b00000100
#define M_DATA_WRITE    0b00001000

void change_state(){
  unsigned char cmd = read_usart();

  unsigned char serial = (cmd >> 5); // serial port
  unsigned char mode = (cmd & 0b00011111); // mode

    
  if(mode == M_CONF_WRITE){ // config
    
    unsigned char data = read_usart(); //read data
    
    //change serial port setting, save and setting in the eeprom
    //write_usart(data);
    write_usart('1');

    
  }else if(mode == M_DATA_WRITE){ //write

    unsigned char data = read_usart(); //read data
    
    // write data to serial using software serial
    write_s_serial(serial, data);
    //write_usart(data);
    write_usart('4');

    
  }else if(mode == M_DATA_READ){ //read
    // read data from serial using software serial
    unsigned char resp = read_s_serial(serial);
    // now send that
    //write_usart(resp); //data
    write_usart('3');


    
  }else if(mode == M_CONF_READ){ //read - config
    // read running config from relevant to the serial
    write_usart('2');
    
  }else{
    write_usart('5');
    
  }


}

void change_state2(){
  unsigned char cmd = read_usart();

  unsigned char serial = (cmd >> 5); // serial port
  unsigned char mode = (cmd & 0b00011111); // mode

  unsigned char data = read_usart(); //read data
    

  write_usart(cmd);
  write_usart(serial);

  write_usart(data);
  

}

int send_stable(){
  write_usart('K');write_usart('_');write_usart('@');write_usart('Q'); return 0;
}

void init_buffer_ics(){
  /** Both ICs were in Output Enable mode by default (hard wired)**/

  /** configuring signal direction**/
  digitalWrite(IC_A_DIR, 0); //RX IC
  digitalWrite(IC_B_DIR, 1); //TX IC
}

int main(void){
   
   unsigned char ReceivedByte;
   /** initializing buffer ics **/
   init_buffer_ics();

   /** Initializing the USART between PC and the Arduino **/
   init_usart();

   /** sending at the beginning special character sequence [K_@Q]**/
   send_stable();

   /** initializing software serial **/
   init_soft_serial();


//   Serial.begin(9600);
//   while(Serial.available()){
//    Serial.println(Serial.read());
//   }

  /** put the device into working mode **/
   for (;;){ // Loop forever
    
      //ReceivedByte = read_usart(); // Fetch the received byte value into the variable "ByteReceived" 
      //write_usart(ReceivedByte);
      change_state();
      //write_usart(0b00000001); 
   }
   
}
