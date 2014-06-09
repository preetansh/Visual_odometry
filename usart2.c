@@ -0,0 +1,157 @@
+/*
+ Servo motor control using ATmega32 with UART communication with PC.
+ See article on Hobby Servo Motors for more information.
+ 
+ Details: Controller waits for character 's' from computer to begin. 
+ Once 's' has been received, controller expects 3 char values representing angle.
+ This angle is then set on the servo.
+ 
+*/
+#define F_CPU 11059200UL
+#define USART_BAUDRATE 9600
+#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 
+// set minimum and maximum width for servo here
+#define MIN_PWM 0.6  // in ms 
+#define MAX_PWM 2.4  // in ms
+#define PWM_FREQUENCY 50  // in Hz
+#define PRESCALAR 256
+
+//size of 0.1ms pulse = 4.32 
+#define UNIT_PULSE ((F_CPU/(PRESCALAR*PWM_FREQUENCY) - 1)/20)  // timer counts for 1ms
+#define CCW 22      //hexadecimal value is 16
+#define ACCW 108    //hexadecimal value is 6c  
+#include <mega32.h>
+#include <delay.h>
+ 
+void init_usart(){
+// Load lower 8-bits of the baud rate value into the low byte of the UBRR register
+  UBRRL = BAUD_PRESCALE;             
+// Load upper 8-bits of the baud rate value into the high byte of the UBRR register 
+  UBRRH = (BAUD_PRESCALE >> 8);   
+      
+// Enable receiver and transmitter
+  UCSRB |= (1 << RXEN | 1 << TXEN);       
+// Set USART for 8bit data 
+  UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
+}
+ 
+void send_usart(unsigned char ch){
+ UDR = ch;                     
+ while (!(UCSRA & (1 << TXC)));
+ delay_ms(1);
+}
+ 
+unsigned char receive_usart(){
+// Do nothing until data have been received and is ready to be read from UDR
+ while (!(UCSRA & (1 << RXC))); 
+// Fetch the received byte value into the variable "ReceivedByte"   
+ return UDR;                    
+}
+ 
+void send_string_usart(char* string){
+  while(*string){
+  //Send value of pointer as data to USART
+  send_usart(*string);
+  //Increment string pointer
+  string++;
+  }
+}
+/********************************************************/
+
+
+
+void init_servo(){
+ TCCR1A |= (1 << COM1A1) | (1 << COM1B1); //non-inverting A and B
+ //FAST PWM with ICR1 as TOP
+ TCCR1A |= (1 << WGM11);
+ TCCR1B |= (1 << WGM12) | (1 << WGM13);   //fast pwm mode
+ //N = 256; Start Timer
+ TCCR1B |= (1 << CS12);                  
+ //PWM frequency 50Hz
+ ICR1H = 0x03;
+ ICR1L = 0x5F;
+}
+ 
+/********************************************************/
+
+void main(){
+ 
+ char ch = 'n';
+// unsigned short angle = 0;
+// unsigned short buffer[3];
+// unsigned char str[4];
+ // make str a string
+ //str[3] = '\0';
+ 
+ 
+ DDRD.6=1;    //Set PD.5 as output
+ PORTD.6=1;
+ init_servo();
+ init_usart();
+
+    DDRD.4=0;
+    DDRD.5=0;
+
+ 
+ // Wait for 's' 
+ ch = receive_usart();
+ while('s' != ch){ch = receive_usart();}
+ 
+ // echo 's' back
+ send_usart(ch);
+ send_usart('t');
+ send_usart('a');
+ send_usart('r');
+ send_usart('t');
+
+ // start main loop
+ while(1){
+   
+  ch = receive_usart(); 
+  if(ch>='a' && ch<='z')
+  PORTD.6=~PIND.6;
+  
+  if(ch=='f')
+  {             
+    OCR1A = 90;
+    OCR1B = 10;
+    DDRD.4=1;
+    DDRD.5=1;
+    
+  }
+  else if(ch=='b')
+  {
+    OCR1A = 10;
+    OCR1B = 90;
+    DDRD.4=1;
+    DDRD.5=1;
+     
+  }
+  else if(ch=='r')
+  {
+    OCR1A = 90;
+    OCR1B = 90;
+    DDRD.4=1;
+    DDRD.5=1;
+    
+  }
+  else if(ch=='l')
+  {
+    OCR1A = 10;
+    OCR1B = 10;
+    DDRD.4=1;
+    DDRD.5=1;
+    
+  }
+  else
+  {
+    delay_ms(10);
+    DDRD.4=0;
+    DDRD.5=0;  
+  }    
+  
+  
+ }//End of while
+ 
+}
+ 
