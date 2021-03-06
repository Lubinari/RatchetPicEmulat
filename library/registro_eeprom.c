#include <2404.c> // driver EEPROM

BYTE qtd_users = 0;
BYTE qtd_del_users = 0;

#define REG_LENGTH  6
#define PASS_LENGTH 4
#define ID_LENGTH   3

#define ADDRESS_LAST_ID     (qtd_users - 1) * REG_LENGTH
#define USER_NOT_FOUND      1 // mem_address eh sempre multiplo de REG_LENGTH, nunca 1

// Ler byte do ID do usuario atual
#define get_ID()           read_ext_eeprom(user.mem_address)
// Endereco dos IDs deletados (final da EEPROM, antes da SENHA do ADM)
#define address_del_ID(i)  EEPROM_SIZE - PASS_LENGTH - (i)
#define get_del_ID(i)      read_ext_eeprom(address_del_ID(i))

struct {
   EEPROM_ADDRESS mem_address;
   BYTE ID;
   BOOLEAN status;
   char *pass;
} user;

// Limpa IDs deletados memorizados
void clear_del_IDs() {
   for (BYTE i = 1; i <= qtd_del_users; i++) {
      write_ext_eeprom(address_del_ID(i), 0xFF);
   }

   qtd_del_users = 0;
}

// Inicia a EEPROM, conta os usuarios salvos e os apagados
void init_registro_eeprom() {
   init_ext_eeprom();

   user.mem_address = 0; // Inicio da EEPROM
   // Incrementa qtd_users ate encontrar ID vazio
   for (qtd_users = 0; get_ID() != 0xFF; qtd_users++) {
      user.mem_address += REG_LENGTH;
   }

   // Incrementa qtd_del_users ate encontrar ID deletado vazio
   // (final da EEPROM antes da SENHA do ADM)
   for (qtd_del_users = 0; get_del_ID(qtd_del_users + 1) != 0xFF; qtd_del_users++);
}

// Apaga tudo exceto a SENHA do ADM
void clear_mem() {
   for (EEPROM_ADDRESS i = 0; i < EEPROM_SIZE - PASS_LENGTH; i++) {
      write_ext_eeprom(i, 0xFF);
   }

   qtd_users = 0;
   qtd_del_users = 0;
}

// Guarda SENHA do ADM invertida no final da EEPROM
void set_adm_pass(char pass[]) {
   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      write_ext_eeprom(EEPROM_SIZE - 1 - i, pass[i]);
   }
}

// Verifica se SENHA do ADM esta correta
BOOLEAN verify_adm_pass(char pass[]) {
   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      if (read_ext_eeprom(EEPROM_SIZE - 1 - i) != pass[i]) {
         return FALSE;
      }
   }

   return TRUE;
}

#define get_status()  read_ext_eeprom(user.mem_address + 1)

// Ler SENHA do USUARIO
char *get_pass() {
   static char pass[PASS_LENGTH + 1]; // Ultimo char = \0

   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      pass[i] = read_ext_eeprom(user.mem_address + 2 + i);
   }

   return pass;
}

// Carregar dados na struct user
void get_user() {
   user.ID = get_ID();
   user.status = get_status();
   user.pass = get_pass();
}

// Procurar usuario pelo ID
void search_user(BYTE ID) {
   // Percorre a EEPROM pulando a cada REG_LENGTH
   for (user.mem_address = 0; user.mem_address < qtd_users * REG_LENGTH; user.mem_address += REG_LENGTH) {
      if (get_ID() == ID) {
         return;
      }
   }

   user.mem_address = USER_NOT_FOUND;
}

// Procurar usuario pela SENHA
void search_user(char pass[]) {
   BOOLEAN founded;

   for (user.mem_address = 0; user.mem_address < qtd_users * REG_LENGTH; user.mem_address += REG_LENGTH) {
      founded = TRUE;

      for (BYTE i = 0; i < PASS_LENGTH; i++) {
         if (read_ext_eeprom(user.mem_address + 2 + i) != pass[i]) {
            founded = FALSE;
            break;
         }
      }

      if (founded) {
         return;
      }
   }

   user.mem_address = USER_NOT_FOUND;
}

// Escrever usuario na EEPROM
void write_user() {
   write_ext_eeprom(user.mem_address, user.ID);
   write_ext_eeprom(user.mem_address + 1, user.status);

   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      write_ext_eeprom(user.mem_address + 2 + i, user.pass[i]);
   }
}

#define mem_full() \
   (qtd_users + 1) * REG_LENGTH >= EEPROM_SIZE - PASS_LENGTH - qtd_del_users


// Adicionar usuario depois do ultimo
void add_user() {
   qtd_users++;
   user.mem_address = ADDRESS_LAST_ID;
   write_user();
}

// Coloca o ultimo registro no lugar do deletado e apaga o ultimo
// Retorna FALSE caso o ID nao seja encontrado
BOOLEAN del_user(BYTE ID) {
   search_user(ID);
   if (user.mem_address == USER_NOT_FOUND) return FALSE;
   
   EEPROM_ADDRESS address_user_del = user.mem_address;
   user.mem_address = ADDRESS_LAST_ID;
   get_user();
   user.mem_address = address_user_del;
   write_user();
   
   // Apaga ultimo usuario
   for (BYTE i = 0; i < REG_LENGTH; i++) {
      write_ext_eeprom(ADDRESS_LAST_ID + i, 0xFF);
   }

   qtd_users--;

   // Escreve ID deletado no fim da EEPROM (antes de ADM_PASS)
   qtd_del_users++;
   write_ext_eeprom(address_del_ID(qtd_del_users), ID);

   return TRUE;
}
