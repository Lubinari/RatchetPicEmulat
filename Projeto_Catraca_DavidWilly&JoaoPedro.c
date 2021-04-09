#include <Projeto_Catraca_DavidWilly&JoaoPedro.h>
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)

//cadastro de senha * # NAO PODEM APARECER
//cadastro ate 255

#define Ratchet  PIN_C0

BYTE CountDigit = 0;
BYTE ScreenStatus = 0;
char PassKeys[PASS_LENGTH + 1];

BOOLEAN recebendo_dados = FALSE;
BOOLEAN dado_disponivel = FALSE;
BYTE dado;
long dado_count = 0;

#INT_RDA
void  RDA_isr(void) {
   recebendo_dados = TRUE;
   dado = getc();
   dado_disponivel = TRUE;
}


BYTE str2byte(char *str) {
    BYTE num = 0;
    for (BYTE c = 0; c < 3; c++) {
        num *= 10;
        num += str[c] - '0';
    }return num;
}


void SetMenu (BYTE Status){
   CountDigit = 0;
   ScreenStatus = Status;

   if (Status == 0){ // MENU INICIAL/PRINCIPAL
      printf(lcd_escreve, "\fFitnessLifestyle\nKey: "); // Tela Principal
   }else if (Status == 1){ // ADM VERIFICAÇÃO
         printf(lcd_escreve, "\fEnter Mode ADM?\n* - Join"); //Confirmar entrar no modo ADM
   }else if (Status == 2){ // MENU ADM
      printf(lcd_escreve, "\f1-Add  3-View\n"); 
      printf(lcd_escreve, "2-Del  4-Upload");
   }else if (Status == 3){ //ADM ADD
      printf(lcd_escreve, "\fUser Register\nID: "); //Cadastro Usuario ID
   }else if (Status == 4){ //ADM Del
      printf(lcd_escreve, "\fDelete User\nID: "); //Deletar Usuario
   }else if (Status == 5){ //ADM Vew
      printf(lcd_escreve, "\fView User\n1- ID    2- Pass"); //Visualizar Usuario
   }else if (Status == 6){ //ADM Upload
      printf(lcd_escreve, "\fUpload data?\n* - send"); //RealizandoUpload
   }else if (Status == 7){ //ADM view ID
      printf(lcd_escreve, "\fView User\nID: ");  // Vizualizar usuario por ID
   }else if (Status == 8){ //ADM view Pass
      printf(lcd_escreve, "\fView User\nPass: "); // Vizualizar usuario por PASS
   }else if (Status == 9){ //ADM Key
      printf(lcd_escreve, "\fKey ADM: "); //  Esperando a senha do ADM
   }else if (Status == 10){ //ADM ADD Pass
      printf(lcd_escreve, "\fUser Register\nPass: "); //Cadastro Usuario PASS
   }
}


void AdmMenu (char Key){
   if (Key == '1'){ //Add
      SetMenu(3);
   }else if (Key == '2'){ // Del
      SetMenu(4);
   }else if (Key == '3'){ // View
      SetMenu(5);
   }else if (Key == '4'){ // Upload
      SetMenu(6);
   }else if (Key == '#'){ // Retornar a tela principal
      SetMenu(0);
   }else{ // Mensagem de erro e retornar a espera para presionar a tecla
      printf(lcd_escreve, "\fInvalid!");
      delay_ms(1500);
      SetMenu(2);
   }
}


BOOLEAN KeyPassword (char Key, BYTE Status) {
   if (Key == '#'){
      SetMenu(Status);
   } else if (CountDigit <= PASS_LENGTH){
      lcd_escreve('*');
      if(CountDigit == PASS_LENGTH){
         delay_ms(500);
         return TRUE;
      }
   }
   return FALSE;
}


void Password (char Key){
   if (KeyPassword(Key,ScreenStatus)){
      search_user(PassKeys);
      if (user.mem_address == USER_NOT_FOUND){
         printf(lcd_escreve, "\fPassword\nIncorrect");
         delay_ms(2000);
      }else {
         get_user();
         if (user.status) {
            printf(lcd_escreve, "\fWelcome!\nID %u",user.ID);
            output_high(Ratchet);
         } else {
            printf(lcd_escreve, "\fSearch manager\nID %u",user.ID);
         }         
         delay_ms(5000);
         output_low(Ratchet);
      }
   SetMenu(0);
   }
}


void KeysProcess_MainMenu (char Key){ // ScreenStatus == 0
   PassKeys[CountDigit++] = Key;
   if (Key == '#'){
      SetMenu(0);
      return;
   }else if (Key == '*'){
      SetMenu(1);
      return;
   }else {
      Password(Key);
   }
}


void KeysProcess_CheckADMMode (char Key){ // ScreenStatus == 1
   PassKeys[CountDigit++] = Key;
   if(Key == '*'){
      SetMenu(9);
      CountDigit = 0;
   }else{
      SetMenu(0);
   }
}


void KeysProcess_ADMMenu (char Key){ // ScreenStatus == 2
   PassKeys[CountDigit++] = Key;
   AdmMenu(Key);
}


void KeysProcess_ADMAddID (char Key){ // ScreenStatus == 3
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(2);
   }else if(Key == '*'){
      SetMenu(3);
   }else if(CountDigit == ID_LENGTH){
      user.ID = str2byte(PassKeys);
      search_user(user.ID);
      if (user.mem_address == USER_NOT_FOUND){
         delay_ms(500);
         SetMenu(10);
      }else{
         delay_ms(500);
         printf(lcd_escreve, "\fExisting\nID"); //Senha ja existe
         delay_ms(2000);
         SetMenu(3);
      }
   }   
}


void KeysProcess_ADMDelet (char Key){ // ScreenStatus == 4
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(2);
   }else{
      if (CountDigit == ID_LENGTH){
      delay_ms(500);
      user.ID = str2byte(PassKeys);
      if (del_user(user.ID)){
         printf(lcd_escreve, "\fdeletion was\nsuccessful"); //exclusão realizada com sucesso
         delay_ms(2000);                                           
         SetMenu(4);
      }else{
         printf(lcd_escreve, "\fID not Found"); 
         delay_ms(2000);
         SetMenu(4);
         }
      }
   }
}


void KeysProcess_ADMView (char Key){ // ScreenStatus == 5
   PassKeys[CountDigit++] = Key;
   if(Key == '#'){
      SetMenu(2);
   }else{
      if (Key == '1'){
         SetMenu(7);
      }else if (Key == '2'){
         SetMenu(8);
      }else{
         printf(lcd_escreve, "\fInvalid!"); 
         SetMenu(5);
      }
   }
}


void KeysProcess_ADMUpload (char Key){ // ScreenStatus == 6
   if(Key == '*'){
      printf(lcd_escreve, "\fUploading\ndon't disconect "); //Realizando Upload

       for (BYTE i = 1; i <= qtd_del_users; i++) {
         user.ID = get_del_ID(i);
         fprintf(PORT1, "%c", user.ID);
         delay_ms(10);
      }

      clear_del_IDs();
      fprintf(PORT1, "\r");

      for (i = 0; i < qtd_users; i++) {
         user.mem_address = i * REG_LENGTH;
         get_user();

         fprintf(PORT1, "%c%c%s", user.ID, user.status, user.pass);
         delay_ms(10);
      }
      fprintf(PORT1, "\n");
      lcd_escreve("\f\n  Completed!");
      delay_ms(1000);
      SetMenu(2);
   }else{
      SetMenu(2);
   }

}


void KeysProcess_ADMViewID (char Key){ // ScreenStatus == 7
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(2);
   }else if(Key == '*'){
      SetMenu(7);
   }else{
      if (CountDigit == ID_LENGTH){
         delay_ms(500);
         user.ID = str2byte(PassKeys);
         search_user(user.ID);
         if (user.mem_address == USER_NOT_FOUND){
            printf(lcd_escreve,"\fID not Found"); 
            delay_ms(2000);
            SetMenu(5);
         }else{
            get_user();
            printf(lcd_escreve, "\fID %u",user.ID);
            printf(lcd_escreve, " Status %u",user.status);
            printf(lcd_escreve, "\nPass %s",user.pass);
            delay_ms(5000);
            SetMenu(5);
         }
      }
   }
}


void KeysProcess_ADMViewPass (char Key){ // ScreenStatus == 8
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(2);
   }else if(Key == '*'){
      SetMenu(7);
   }else{
      if (CountDigit == PASS_LENGTH){
         delay_ms(500);
         user.pass = PassKeys;
         search_user(user.pass);
         if (user.mem_address == USER_NOT_FOUND){
            printf(lcd_escreve,"\fID not Found"); 
            delay_ms(2000);
            SetMenu(5);
         }else{
            get_user();
            printf(lcd_escreve, "\fID %u",user.ID);
            printf(lcd_escreve, " Status %u",user.status);
            printf(lcd_escreve, "\nPass %s",user.pass);
            delay_ms(5000);
            SetMenu(5);
         }
      }
   }
}


void KeysProcess_KeyADMcheck (char Key){ // ScreenStatus == 9
   PassKeys[CountDigit++] = Key;
   lcd_escreve('*');
   if(Key == '#'){
      SetMenu(0);
   }else if (CountDigit == PASS_LENGTH){
      delay_ms(500);
      if(verify_adm_pass(PassKeys)){
         SetMenu(2);
      }else{
         printf(lcd_escreve, "\fKey for ADM\nIncorrect");
         delay_ms(2000);
         SetMenu(0);
      }
   }
}


void KeysProcess_ADMAddPass (char Key){ // ScreenStatus == 10
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(3);
   }else if(Key == '*'){
      SetMenu(10);
   }else if(CountDigit == PASS_LENGTH){
      delay_ms(500);
      user.pass = PassKeys;
      search_user(user.pass);
      if (user.mem_address == USER_NOT_FOUND){
         user.status = TRUE; // DEFINE O STATUS COMO VALIDO OU INVALIDO (TRUE OR FALSE)
         add_user();
         SetMenu(10);
         printf(lcd_escreve, "\fUser added\nsuccessfully"); // usuário adicionado com sucesso
         delay_ms(1000);
         SetMenu(2);
      }else{
      printf(lcd_escreve, "\fExisting\npassword"); // usuário adicionado com sucesso
      delay_ms(1000);
      SetMenu(10);
      }
   }
}



void main(){
   init_registro_eeprom();
   

   lcd_ini();
   
   kbd_init();
   port_b_pullups(TRUE);
   set_tris_b(0xF0);
   output_high(PIN_C1);

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   SetMenu(0);

   //clear_mem();
   set_adm_pass((char*) "A19D");


   while(TRUE){
      if (recebendo_dados && dado == '\r') {
         output_toggle(PIN_D0);
         dado_disponivel = FALSE;
         clear_mem();
         lcd_escreve("\fUpdating...\n");

         while (recebendo_dados) {
            if (dado_disponivel) {
               output_toggle(PIN_D0);

               if (dado == '\n') {
                  recebendo_dados = FALSE;
                  dado_count = 0;
               } else {
                  write_ext_eeprom(dado_count++, dado);
                  dado_disponivel = FALSE;
               }
            }
            delay_ms(1);
         }
         init_registro_eeprom();
         SetMenu(0);
         continue;
      }

      char Key = returnTecla();

      if (Key) {
         if (ScreenStatus == 0){
            KeysProcess_MainMenu(Key);
         }else if (ScreenStatus == 1){
            KeysProcess_CheckADMMode(Key);
         }else if (ScreenStatus == 2){
            KeysProcess_ADMMenu(Key);
         }else if (ScreenStatus == 3){
            KeysProcess_ADMAddID(Key);
         }else if (ScreenStatus == 4){
            KeysProcess_ADMDelet(Key);
         }else if (ScreenStatus == 5){
            KeysProcess_ADMView(Key);
         }else if (ScreenStatus == 6){
            KeysProcess_ADMUpload(Key);
         }else if (ScreenStatus == 7){
            KeysProcess_ADMViewID(Key);
         }else if (ScreenStatus == 8){
            KeysProcess_ADMViewPass(Key);
         }else if (ScreenStatus == 9){
            KeysProcess_KeyADMcheck(Key);
         }else if (ScreenStatus == 10){
            KeysProcess_ADMAddPass(Key);
         }
      }
      delay_ms(1);
   }
   
}

