#include <Wire.h>
 
#include <LiquidCrystal_I2C.h>
 
#include <RTClib.h>
 
#include "DHT.h"
 
#include <SPI.h>
 
#include <MFRC522.h>
 
#include <Keypad.h>
 
#include <EEPROM.h>
 
#define DHTPIN 2
 
#define DHTTYPE DHT22
 
#define LDR_PIN A0
 
#define BUZZER_PIN A3
 
#define LED_PIN 10
 
#define SS_PIN A1
 
#define RST_PIN A2
 
MFRC522 rfid(SS_PIN, RST_PIN);
 
RTC_DS3231 rtc;
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
DHT dht(DHTPIN, DHTTYPE);
 
const byte ROWS = 4;
 
const byte COLS = 3;
 
char keys[ROWS][COLS] = {
 
  {'1','2','3'},
 
  {'4','5','6'},
 
  {'7','8','9'},
 
  {'*','0','#'}
 
};
 
byte rowPins[ROWS] = {3, 4, 5, 6};
 
byte colPins[COLS] = {7, 8, 9};
 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
 
const byte uidAutorizado[] = {0xE6, 0xD2, 0x17, 0x96};
 
enum Estado { AGUARDANDO_RFID, MENU_PRINCIPAL, EDITANDO_LIMITE, MONITORANDO };
 
Estado estadoAtual = AGUARDANDO_RFID;
 
bool menuDesenhado = false;
 
String entradaTeclado = "";
 
String limiteSendoEditado = "";
 
float trigger_t_max = 30.0;
 
float trigger_u_max = 60.0;
 
int trigger_luz_max = 70;
 
const int numLeituras = 10;
 
int leiturasLuz[numLeituras];
 
int indexLuz = 0;
 
long somaLuz = 0;
 
int mediaLuz = 0;
 
const int eepromInicio = 2;
 
const int tamanhoRegistro = 16;
 
void registrarEvento(float t, float u, float l, DateTime tempo) {
 
  uint16_t enderecoAtual;
 
  EEPROM.get(0, enderecoAtual);
 
  if (enderecoAtual < eepromInicio || enderecoAtual > (EEPROM.length() - tamanhoRegistro)) {
 
    enderecoAtual = eepromInicio;
 
  }
 
  uint32_t timestamp = tempo.unixtime();
 
  EEPROM.put(enderecoAtual, timestamp);
 
  EEPROM.put(enderecoAtual + 4, t);
 
  EEPROM.put(enderecoAtual + 8, u);
 
  EEPROM.put(enderecoAtual + 12, l);
 
  enderecoAtual += tamanhoRegistro;
 
  if (enderecoAtual > (EEPROM.length() - tamanhoRegistro)) {
 
    enderecoAtual = eepromInicio;
 
  }
 
  EEPROM.put(0, enderecoAtual);
 
}
 
void limparEEPROM() {
 
  for (int i = 0; i < EEPROM.length(); i++) {
 
    EEPROM.write(i, 0xFF);
 
  }
 
  Serial.println("EEPROM apagada.");
 
}
 
void mostrarLogs() {
 
  Serial.println("=== Logs na EEPROM ===");
 
  for (int addr = eepromInicio; addr <= EEPROM.length() - tamanhoRegistro; addr += tamanhoRegistro) {
 
    uint32_t timestamp;
 
    float t, u, l;
 
    EEPROM.get(addr, timestamp);
 
    EEPROM.get(addr + 4, t);
 
    EEPROM.get(addr + 8, u);
 
    EEPROM.get(addr + 12, l);
 
    if (timestamp == 0xFFFFFFFF || timestamp == 0) continue;
 
    DateTime dt(timestamp);
 
    Serial.print(dt.day()); Serial.print("/");
 
    Serial.print(dt.month()); Serial.print("/");
 
    Serial.print(dt.year()); Serial.print(" ");
 
    Serial.print(dt.hour()); Serial.print(":");
 
    Serial.print(dt.minute()); Serial.print(":");
 
    Serial.print(dt.second()); Serial.print(" - ");
 
    Serial.print("T:"); Serial.print(t);
 
    Serial.print("C U:"); Serial.print(u);
 
    Serial.print("% L:"); Serial.print(l);
 
    Serial.println("%");
 
  }
 
}
 
bool verificaUID(MFRC522::Uid uid) {
 
  for (byte i = 0; i < uid.size; i++) {
 
    if (uid.uidByte[i] != uidAutorizado[i]) return false;
 
  }
 
  return true;
 
}
 
void mostrarLogo() {
 
  byte z0[8] = {B00000,B11111,B01111,B00011,B00001,B00000,B00000};
 
  byte z1[8] = {B00000,B11111,B11111,B11111,B11111,B10000,B00011,B00111};
 
  byte z2[8] = {B00000,B11111,B11111,B11111,B11111,B00111,B11111,B11111};
 
  byte z3[8] = {B00000,B11111,B11110,B11100,B11000,B10000,B10000,B00000};
 
  byte z4[8] = {B00000,B00000,B00001,B00011,B00111,B01111,B11111};
 
  byte z5[8] = {B01111,B11111,B11110,B11111,B11111,B11111,B11111};
 
  byte z6[8] = {B11111,B11110,B00000,B11111,B11111,B11111,B11111};
 
  byte z7[8] = {B00000,B00000,B10000,B11000,B11100,B11110,B11111};
 
  lcd.createChar(0, z0);
 
  lcd.createChar(1, z1);
 
  lcd.createChar(2, z2);
 
  lcd.createChar(3, z3);
 
  lcd.createChar(4, z4);
 
  lcd.createChar(5, z5);
 
  lcd.createChar(6, z6);
 
  lcd.createChar(7, z7);
 
  lcd.clear();
 
  for (int i = 0; i < 4; i++) {
 
    lcd.setCursor(i, 0);
 
    lcd.write(byte(i));
 
    delay(80);
 
  }
 
  lcd.setCursor(0, 1); lcd.write(byte(4)); delay(80);
 
  lcd.setCursor(1, 1); lcd.write(byte(5)); delay(80);
 
  lcd.setCursor(2, 1); lcd.write(byte(6)); delay(80);
 
  lcd.setCursor(3, 1); lcd.write(byte(7)); delay(80);
 
  for (int j = 0; j < 2; j++) {
 
    for (int i = 0; i < 12; i++) { lcd.scrollDisplayRight(); delay(80); }
 
    delay(100);
 
    for (int i = 0; i < 12; i++) { lcd.scrollDisplayLeft(); delay(80); }
 
  }
 
  lcd.print("etta Works");
 
  delay(3000);
 
  for (int i = 13; i >= 0; i--) {
 
    lcd.setCursor(i, 1);
 
    lcd.print(" ");
 
    delay(100);
 
  }
 
  for (int i = 3; i >= 0; i--) {
 
    lcd.setCursor(i, 0);
 
    lcd.print(" ");
 
    delay(100);
 
  }
 
}
 
void setup() {
 
  SPI.begin();
 
  rfid.PCD_Init();
 
  Serial.begin(9600);
 
  lcd.init();
 
  lcd.backlight();
 
  pinMode(BUZZER_PIN, OUTPUT);
 
  digitalWrite(BUZZER_PIN, LOW);
 
  pinMode(LED_PIN, OUTPUT);
 
  digitalWrite(LED_PIN, LOW);
 
  if (!rtc.begin()) {
 
    lcd.setCursor(0, 0);
 
    lcd.print("Erro no RTC!");
 
    while (1);
 
  }
 
  dht.begin();
 
  pinMode(LDR_PIN, INPUT);
 
  for (int i = 0; i < numLeituras; i++) {
 
    leiturasLuz[i] = analogRead(LDR_PIN);
 
    somaLuz += leiturasLuz[i];
 
  }
 
  mediaLuz = somaLuz / numLeituras;
 
  lcd.setCursor(0, 0);
 
  lcd.print("Sistema Iniciado");
 
  delay(1500);
 
  lcd.clear();
 
  mostrarLogs();
 
}
 
void loop() {
 
  DateTime now = rtc.now();
 
  if (estadoAtual == AGUARDANDO_RFID) {
 
    lcd.setCursor(0, 0);
 
    lcd.print("Passe o cartao...");
 
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
 
      if (verificaUID(rfid.uid)) {
 
        lcd.clear();
 
        lcd.print("Acesso Liberado");
 
        delay(1000);
 
        mostrarLogo();
 
        estadoAtual = MENU_PRINCIPAL;
 
        menuDesenhado = false;
 
      } else {
 
        lcd.clear();
 
        lcd.print("Acesso Negado");
 
        delay(1500);
 
      }
 
      rfid.PICC_HaltA();
 
      rfid.PCD_StopCrypto1();
 
      return;
 
    }
 
  }
 
  if (estadoAtual == MENU_PRINCIPAL) {
 
    if (!menuDesenhado) {
 
      lcd.clear();
 
      lcd.setCursor(0, 0);
 
      lcd.print("1:Tmax 2:Umax");
 
      lcd.setCursor(0, 1);
 
      lcd.print("3:Lmax 9:Monitor");
 
      menuDesenhado = true;
 
    }
 
    char tecla = keypad.getKey();
 
    if (tecla) {
 
      if (tecla == '8') {
 
        mostrarLogs();
 
      } else if (tecla == '7') {
 
        limparEEPROM();
 
        lcd.clear();
 
        lcd.print("EEPROM limpa");
 
        delay(1500);
 
        lcd.clear();
 
      } else {
 
        entradaTeclado = "";
 
        lcd.clear();
 
        if (tecla == '1') {
 
          lcd.print("Novo Tmax:");
 
          limiteSendoEditado = "TMAX";
 
          estadoAtual = EDITANDO_LIMITE;
 
        } else if (tecla == '2') {
 
          lcd.print("Novo Umax:");
 
          limiteSendoEditado = "UMAX";
 
          estadoAtual = EDITANDO_LIMITE;
 
        } else if (tecla == '3') {
 
          lcd.print("Novo Lmax:");
 
          limiteSendoEditado = "LMAX";
 
          estadoAtual = EDITANDO_LIMITE;
 
        } else if (tecla == '9') {
 
          estadoAtual = MONITORANDO;
 
          lcd.clear();
 
        }
 
      }
 
    }
 
  }
 
  if (estadoAtual == EDITANDO_LIMITE) {
 
    char tecla = keypad.getKey();
 
    if (tecla) {
 
      if (tecla == '#') {
 
        float valor = entradaTeclado.toFloat();
 
        if (limiteSendoEditado == "TMAX") trigger_t_max = valor;
 
        else if (limiteSendoEditado == "UMAX") trigger_u_max = valor;
 
        else if (limiteSendoEditado == "LMAX") trigger_luz_max = valor;
 
        lcd.clear();
 
        lcd.print("Salvo: ");
 
        lcd.print(valor);
 
        delay(1500);
 
        estadoAtual = MENU_PRINCIPAL;
 
        menuDesenhado = false;
 
      } else if (tecla == '*') {
 
        entradaTeclado.remove(entradaTeclado.length() - 1);
 
      } else if ((tecla >= '0' && tecla <= '9') || tecla == '.') {
 
        entradaTeclado += tecla;
 
      }
 
      lcd.setCursor(0, 1);
 
      lcd.print("                ");
 
      lcd.setCursor(0, 1);
 
      lcd.print(entradaTeclado);
 
    }
 
  }
 
  if (estadoAtual == MONITORANDO) {
 
    float temperature = dht.readTemperature();
 
    float humidity = dht.readHumidity();
 
    int novaLeitura = analogRead(LDR_PIN);
 
    somaLuz = somaLuz - leiturasLuz[indexLuz] + novaLeitura;
 
    leiturasLuz[indexLuz] = novaLeitura;
 
    indexLuz = (indexLuz + 1) % numLeituras;
 
    mediaLuz = somaLuz / numLeituras;
 
    int luzPct = map(mediaLuz, 0, 1023, 100, 0);
 
    bool fora = false;
 
    if (temperature > trigger_t_max || humidity > trigger_u_max || luzPct > trigger_luz_max) {
 
      fora = true;
 
    }
 
    static bool estavaFora = false;
 
    if (!estavaFora && fora) {
 
      registrarEvento(temperature, humidity, luzPct, now);
 
    }
 
    estavaFora = fora;
 
    digitalWrite(BUZZER_PIN, fora ? HIGH : LOW);
 
    digitalWrite(LED_PIN, fora ? HIGH : LOW);
 
    if (fora) {
 
      lcd.clear();
 
      lcd.setCursor(0, 0);
 
      lcd.print("!!! ALERTA !!!");
 
      lcd.setCursor(0, 1);
 
      lcd.print("                ");
 
      lcd.setCursor(0, 1);
 
      lcd.print("Fora do limite!");
 
      delay(1500);
 
    }
 
    lcd.setCursor(0, 0);
 
    lcd.print("T:"); lcd.print((int)temperature); lcd.print("C ");
 
    lcd.print("U:"); lcd.print((int)humidity); lcd.print("%   ");
 
    lcd.setCursor(0, 1);
 
    lcd.print("                ");
 
    lcd.setCursor(0, 1);
 
    lcd.print("L:"); lcd.print(luzPct); lcd.print("% ");
 
    lcd.print(now.hour() < 10 ? "0" : ""); lcd.print(now.hour()); lcd.print(":");
 
    lcd.print(now.minute() < 10 ? "0" : ""); lcd.print(now.minute());
 
    delay(2000);
 
  }
 
}