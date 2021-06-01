#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

#define espacoEEPROM 1000

/************************ DEFINIÇÃO DE SENHA ******************************* */
int senha;
/************************ CONFIGURAÇÕES TECLADO ******************************* */

const byte LINHAS = 4;
const byte COLUNAS = 4;

char key[LINHAS][COLUNAS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pinosLinha[LINHAS] = {11, 10, 9, 8};
byte pinosColuna[COLUNAS] = {7, 6, 5, 4};

Keypad keypad = Keypad(makeKeymap(key), pinosLinha, pinosColuna, LINHAS, COLUNAS);

/************************ CONFIGURAÇÕES DISPLAY ******************************* */
LiquidCrystal_I2C lcd(0x3F, 20, 2);

/************************ CONFIGURAÇÕES EEPROM ******************************* */

void EEPROMWriteInt(int address, int value); //2 Bytes
int EEPROMReadInt(int address);

/************************  VARIAVEIS ******************************* */
int tempo;
int cont;
int tentativas;
int estado_da_porta = 0;
int botao_abrir;
int botao_fechar;
int estado;
char senha_adm[6];
char senha_porta[4];
int adm_int;
int porta_int;
int cont2;
int opcao = 1;
int contop = 1;
int estado_menu = 0;

void setup()
{
    //for (int nL = 0; nL < espacoEEPROM; nL++) {EEPROM.write(nL, 0);}
    pinMode(1, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);
    pinMode(3, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    lcd.init();
    lcd.backlight();
    lcd.begin(20, 4);
    porta_int = EEPROMReadInt(1);
    adm_int = EEPROMReadInt(3);
    estado = EEPROMReadInt(5);
    if (estado == 0)
    {
        configuracao();
    }
    else
    {
        fechada();
    }
}

void configuracao()
{
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("PARA INICIAR");
    lcd.setCursor(3, 1);
    lcd.print("A CONFIGURACAO");
    lcd.setCursor(5, 2);
    lcd.print("PRECIONE #");
}

void fechada()
{
    digitalWrite(13, HIGH);
    tone(3, 500, 800);
    delay(1000);
    digitalWrite(13, LOW);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Porta Fechada");
    lcd.setCursor(2, 1);
    lcd.print("Digite Sua Senha:");
}
void aberta()
{
    digitalWrite(12, HIGH);
    tone(3, 500, 800);
    delay(1000);
    digitalWrite(12, LOW);
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Porta Aberta");
    lcd.setCursor(2, 1);
    lcd.print("Use # para fechar");
}
void menu()
{
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Menu ADM");
    lcd.setCursor(2, 1);
    lcd.print("Redefinir Senha");
    lcd.setCursor(2, 2);
    lcd.print("Estado de fabrica");
}

void loop()
{
    botao_abrir = digitalRead(1);
    botao_fechar = digitalRead(2);
    char key = keypad.getKey();

    switch (estado)
    {

    case 0:
        if (key == '#')
        {
            tone(3, 4000, 100);
            estado = 1;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CRIE A SENHA DE ADM");
            lcd.setCursor(4, 1);
            lcd.print("6 CARACTERES");
            lcd.setCursor(6, 2);
            lcd.write(byte(126));
        }
        break;

    case 1:
        if (key)
        {
            tone(3, 4000, 100);
            lcd.setCursor(cont + 7, 2);
            lcd.print("*");
            senha_adm[cont] = key;
            cont++;
        }
        if (cont == 6)
        {
            delay(500);
            adm_int = atoi(senha_adm);
            EEPROMWriteInt(3, adm_int);
            lcd.clear();
            lcd.setCursor(3, 0);
            lcd.print("CRIE UMA SENHA");
            lcd.setCursor(5, 1);
            lcd.print("PARA PORTA");
            lcd.setCursor(4, 2);
            lcd.print("4 CARACTERES");
            lcd.setCursor(6, 3);
            lcd.write(byte(126));
            estado = 2;
            cont = 0;
        }
        break;

    case 2:
        if (key)
        {
            tone(3, 4000, 100);
            lcd.setCursor(cont + 7, 3);
            lcd.print("*");
            senha_porta[cont] = key;
            cont++;
        }
        if (cont == 4)
        {
            delay(500);
            porta_int = atoi(senha_porta);
            EEPROMWriteInt(1, porta_int);
            lcd.clear();
            lcd.setCursor(3, 1);
            lcd.print("CONFIGURADO COM");
            lcd.setCursor(5, 2);
            lcd.print("SUSSESO");
            estado = 3;
            EEPROMWriteInt(5, estado);
            cont = 0;
            delay(1000);
            asm volatile("  jmp 0");
        }
        break;

    case 3:
        if (key == 'A' && estado_da_porta == 0)
        {
            tone(3, 4000, 100);
            estado = 4;
            estado_menu = 0;
            cont = 0;
            menu();
        }

        else
        {
            if (estado_da_porta == 0)
            {
                if (tentativas < 3)
                {
                    if (key || botao_abrir == LOW)
                    {
                        tone(3, 4000, 100);
                        if (key)
                        {
                            lcd.setCursor(cont + 7, 2);
                            tone(3, 4000, 100);
                            lcd.print("*");
                            senha_porta[cont] = key;
                            cont++;
                        }

                        if (cont == 4 || botao_abrir == LOW)
                        {
                            senha = atoi(senha_porta);
                            if (senha == porta_int || botao_abrir == LOW)
                            {
                                delay(500);
                                lcd.clear();
                                lcd.setCursor(3, 1);
                                lcd.print("Senha correta");
                                lcd.setCursor(2, 2);
                                lcd.print("Abrindo porta...");
                                delay(1000);
                                cont = 0;
                                senha = 0;
                                estado_da_porta = 1;
                                tentativas = 0;
                                aberta();
                            }

                            else
                            {
                                delay(500);
                                lcd.clear();
                                if (tentativas < 2)
                                {
                                    lcd.setCursor(3, 1);
                                    lcd.print("Senha Incorreta");
                                    lcd.setCursor(3, 2);
                                    lcd.print("Tente Novamente");
                                    tone(3, 1000, 800);
                                    delay(1000);
                                }
                                senha = 0;
                                cont = 0;
                                tentativas++;
                                lcd.clear();
                                lcd.setCursor(4, 0);
                                lcd.print("Porta Fechada");
                                lcd.setCursor(2, 1);
                                lcd.print("Digite Sua Senha:");
                            }
                        }
                    }
                }
                else
                {
                    lcd.clear();
                    lcd.setCursor(1, 0);
                    lcd.print("Voce errou 3 vezes");
                    lcd.setCursor(1, 1);
                    lcd.print("Tente novamente em:");
                    lcd.setCursor(7, 2);
                    lcd.print(" Segundos");
                    for (int i = 30; i > 0; i--)
                    {
                        if (i >= 10)
                        {
                            lcd.setCursor(5, 2);
                            lcd.print(i);
                        }
                        if (i < 10)
                        {
                            lcd.setCursor(6, 2);
                            lcd.print(i);
                            lcd.setCursor(5, 2);
                            lcd.print(0);
                        }
                        tone(3, 2500, 100);
                        delay(1000);
                    };
                    tentativas = 0;
                    lcd.clear();
                    lcd.setCursor(4, 0);
                    lcd.print("Porta Fechada");
                    lcd.setCursor(2, 1);
                    lcd.print("Digite Sua Senha:");
                }
            }

            else
            {
                if (key == '#' || botao_fechar == LOW)
                {
                    tone(3, 4000, 100);
                    delay(700);
                    estado_da_porta = 0;
                    lcd.clear();
                    lcd.setCursor(2, 1);
                    lcd.print("Fechando porta...");
                    delay(500);
                    fechada();
                }
            }
        }
        break;

    case 4:
        if(estado_menu == 0)
        {
            if (key == 'B')
            {
                tone(3, 4000, 100);
                contop = 0;
                opcao = 1;
                contop++;
            }
            
            if (key == 'D')
            {
                tone(3, 4000, 100);
                contop = 0;
                opcao = 2;
                contop++;
            }
        }
        if (key == 'A')
        {
            tone(3, 4000, 100);
            estado = 3;
            estado_menu = 1;
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("Porta Fechada");
            lcd.setCursor(2, 1);
            lcd.print("Digite Sua Senha:");
        }
        
        if(estado_menu == 0)
        {
            if(opcao == 1)
            {
                if(contop == 1){
                    lcd.setCursor(1, 2);
                    lcd.write(byte(254));
                    lcd.setCursor(1, 1);
                    lcd.write(byte(126));
                }
            }
            if(opcao == 2)
            {
                if(contop == 1){
                    lcd.setCursor(1, 1);
                    lcd.write(byte(254));
                    lcd.setCursor(1, 2);
                    lcd.write(byte(126));
                }
            }
        }

        switch (estado_menu)
        {
        case 1:
            if (key )
            {
                tone(3, 4000, 100);
                lcd.setCursor(cont + 7, 3);
                lcd.print("*");
                senha_adm[cont] = key;
                cont++;
            }
            if (cont == 6)
            {
                if(adm_int == atoi(senha_adm))
                {
                    estado_menu = 3;
                }
                else
                {
                    delay(500);
                    lcd.clear();
                    lcd.setCursor(3, 1);
                    lcd.print("Senha Incorreta");
                    lcd.setCursor(3, 2);
                    lcd.print("Tente Novamente");
                    tone(3, 1000, 800);
                    delay(1300);
                    lcd.clear();
                    lcd.setCursor(1, 0);
                    lcd.print("Digite a senha de");
                    lcd.setCursor(2, 1);
                    lcd.print("Adiministrador:");
                    cont = 0;
                }
                cont = 0;
            }
            break;
        }

        if (key == 'C')
        {
            tone(3, 4000, 100);
            estado_menu = 1;
            cont = 0;
            if (opcao == 1)
            {
                estado_menu = 1;
                lcd.clear();
                lcd.setCursor(1, 0);
                lcd.print("Digite a senha de");
                lcd.setCursor(2, 1);
                lcd.print("Adiministrador:");
            }
            if (opcao == 2)
            {
                estado_menu = 2;
                lcd.clear();
                lcd.setCursor(1, 0);
                lcd.print("Digite a senha de");
                lcd.setCursor(2, 1);
                lcd.print("Adiministrador:");
            }
            
        }
        
        
    }
}
// GRAVAR INT NA EEPROM
void EEPROMWriteInt(int address, int value)
{
    byte hiByte = highByte(value);
    byte loByte = lowByte(value);

    EEPROM.write(address, hiByte);
    EEPROM.write(address + 1, loByte);
}

int EEPROMReadInt(int address)
{
    byte hiByte = EEPROM.read(address);
    byte loByte = EEPROM.read(address + 1);

    return word(hiByte, loByte);
}