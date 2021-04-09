#ifndef TECL_C1
   #define TECL_L1    PIN_B0
   #define TECL_L2    PIN_B1
   #define TECL_L3    PIN_B2
   #define TECL_L4    PIN_B3
   
   #define TECL_C1    PIN_B4
   #define TECL_C2    PIN_B5
   #define TECL_C3    PIN_B6
   #define TECL_C4    PIN_B7
#endif

#define inputTECL_COL   input(TECL_C1) << 3 | \
                        input(TECL_C2) << 2 | \
                        input(TECL_C3) << 1 | \
                        input(TECL_C4)

void setTECL_LIN(BOOLEAN L1, BOOLEAN L2, BOOLEAN L3, BOOLEAN L4) {
   output_bit(TECL_L1, L1);
   output_bit(TECL_L2, L2);
   output_bit(TECL_L3, L3);
   output_bit(TECL_L4, L4);

   delay_ms(20);
}

void kbd_init() {
   setTECL_LIN(0, 0, 0, 0);
}

char returnTecla() {
   char k;

   switch (inputTECL_COL) {
   case 0b0111:
      setTECL_LIN(0, 1, 1, 1);
      while (!input(TECL_C1)) k = '1';
      
      setTECL_LIN(1, 0, 1, 1);
      while (!input(TECL_C1)) k = '4';
      
      setTECL_LIN(1, 1, 0, 1);
      while (!input(TECL_C1)) k = '7';
      
      setTECL_LIN(1, 1, 1, 0);
      while (!input(TECL_C1)) k = '*';
      
      break;
   
   case 0b1011:
      setTECL_LIN(0, 1, 1, 1);
      while (!input(TECL_C2)) k = '2';
      
      setTECL_LIN(1, 0, 1, 1);
      while (!input(TECL_C2)) k = '5';
      
      setTECL_LIN(1, 1, 0, 1);
      while (!input(TECL_C2)) k = '8';
      
      setTECL_LIN(1, 1, 1, 0);
      while (!input(TECL_C2)) k = '0';
      
      break;
   
   case 0b1101:
      setTECL_LIN(0, 1, 1, 1);
      while (!input(TECL_C3)) k = '3';
      
      setTECL_LIN(1, 0, 1, 1);
      while (!input(TECL_C3)) k = '6';
      
      setTECL_LIN(1, 1, 0, 1);
      while (!input(TECL_C3)) k = '9';
      
      setTECL_LIN(1, 1, 1, 0);
      while (!input(TECL_C3)) k = '#';
      
      break;
   
   case 0b1110:
      setTECL_LIN(0, 1, 1, 1);
      while (!input(TECL_C4)) k = 'A';
      
      setTECL_LIN(1, 0, 1, 1);
      while (!input(TECL_C4)) k = 'B';
      
      setTECL_LIN(1, 1, 0, 1);
      while (!input(TECL_C4)) k = 'C';
      
      setTECL_LIN(1, 1, 1, 0);
      while (!input(TECL_C4)) k = 'D';

      break;

   default:
      k = 0;
   }
   
   setTECL_LIN(0, 0, 0, 0);

   if (k) {
      delay_ms(200);
   }

   return k;
}
