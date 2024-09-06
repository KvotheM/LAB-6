#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define Pot1 15
#define Pot2 34
#define Bot1 18
#define Bot2 19

#define I2C_SDA 25
#define I2C_SCL 26


void IRAM_ATTR suma(void);
void IRAM_ATTR resta(void);

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
volatile unsigned long debounce1 = 0;

uint8_t un, decenas, decimal;

float volt1 = 0.0;
float volt2 = 0.0;
int dig1[3] = {0, 0, 0};
int dig2[3] = {0, 0, 0};
int cambio = 0;
int envio = 0;
int contador = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup(){
  Serial.begin(115200);

  pinMode(Bot1, INPUT_PULLDOWN);
  pinMode(Bot2, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(Bot1), suma, RISING);
  attachInterrupt(digitalPinToInterrupt(Bot2), resta, RISING);

  lcd.init(I2C_SDA, I2C_SCL);
	lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Hola");
  delay(1000);
}

void loop(){
  float volt1V, volt2V;
  volt1V = volt1;
  volt2V = volt2;
  volt1 = analogReadMilliVolts(Pot1)/10.0;
  volt2 = analogReadMilliVolts(Pot2)/10.0;

  Serial.println("Voltajes");
  Serial.println(volt1);
  Serial.println(volt2);

  dig1[0] = volt1/100;
  dig1[1] = ((int)volt1%100)/10;
  dig1[2] = ((int)volt1%10);

  dig2[0] = volt2/100;
  dig2[1] = ((int)volt2%100)/10;
  dig2[2] = ((int)volt2%10);
  
  if(volt1 != volt1V | volt2 != volt2V){
    envio = 1;
  }

  if(envio == 1){
    if(cambio == 1 & contador != 255){
      contador++;
    }
    else if(cambio == 0 & contador != 0){
      contador--;
    }
    Serial.println("Contador");
    Serial.println(contador);
  }
  if(envio == 1){
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(1,0);
    lcd.print("Pot1:");
    lcd.setCursor(7,0);
    lcd.print("Pot2:");
    lcd.setCursor(13, 0);
    lcd.print("CPU:");
    
    lcd.setCursor(1, 1);
    lcd.print(dig1[0]);
    lcd.setCursor(2, 1);
    lcd.print(".");
    lcd.setCursor(3, 1);
    lcd.print(dig1[1]);
    lcd.setCursor(4, 1);
    lcd.print(dig1[2]);
    lcd.setCursor(5, 1);
    lcd.print("V");

    lcd.setCursor(7, 1);
    lcd.print(dig2[0]);
    lcd.setCursor(8, 1);
    lcd.print(".");
    lcd.setCursor(9, 1);
    lcd.print(dig2[1]);
    lcd.setCursor(10, 1);
    lcd.print(dig2[2]);
    lcd.setCursor(11, 1);
    lcd.print("V");

    lcd.setCursor(13, 1);
    lcd.print(contador);
    envio = 0;
  }
  delay(300);
}

void IRAM_ATTR suma(void){
  unsigned long currentTime = millis();
  if ((currentTime - debounce1) > 100) {
    portENTER_CRITICAL_ISR(&mux);
    envio = 1;
    cambio = 1;
    portEXIT_CRITICAL_ISR(&mux);
    debounce1 = currentTime;
  }
}

void IRAM_ATTR resta(void){
  unsigned long currentTime = millis();
  if ((currentTime - debounce1) > 100) {
    portENTER_CRITICAL_ISR(&mux);
    envio = 1;
    cambio = 0;
    portEXIT_CRITICAL_ISR(&mux);
    debounce1 = currentTime;
  }
}
