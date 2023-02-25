//https://github.com/olikraus/u8glib/wiki/fontsize
//Bibliotecas
#include <INA.h>
#include <U8glib.h>
#include <stdlib.h>

#define correcaoTensao 1.18469                     
#define correcaoCorrente 1.18203                   
#define offsetCorrente 9.76                        

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
INA_Class INA;

float valorCorrente = 0;
float valorTensao = 0;
float valorPotencia = 0;
float valorConsumo = 0;
float consumoParcial = 0;

long tempoAtual = 0;
long tempoAnterior = 0;
long tempoDelta = 0;

void setup() {
  Serial.begin(9600);

#ifdef __AVR_ATmega32U4__
  delay(2000);
#endif

  INA.begin(80, 1000);
  INA.setBusConversion(8500);
  INA.setShuntConversion(8500);
  INA.setAveraging(128);
  INA.alertOnConversion(true);
  INA.setMode(INA_MODE_CONTINUOUS_BOTH);
  INA.alertOnBusOverVoltage(true, 40000);

  u8g.begin();
}

void loop() {
  tempoAnterior = tempoAtual;
  tempoAtual = millis();
  tempoDelta = tempoAtual - tempoAnterior;

  valorCorrente = (float)INA.getBusMicroAmps() / 1000.0;
  valorCorrente = (valorCorrente + offsetCorrente)*correcaoCorrente;
  
  valorTensao = (float)INA.getBusMilliVolts() / 1000.0;
  valorTensao *= correcaoTensao;
  
  valorPotencia = (float)INA.getBusMicroWatts() / 1000000.0;
  valorPotencia *= correcaoTensao;

  consumoParcial = valorPotencia * (tempoDelta) * 0.001;
  valorConsumo = valorConsumo + consumoParcial;

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_courR10);
    u8g.setPrintPos(0, 15);
    u8g.print(valorTensao);
    u8g.print(" V");

    u8g.setPrintPos(0, 37);
    u8g.print(valorCorrente);
    u8g.print(" mA");
    
    u8g.setPrintPos(0, 60);
    u8g.print(valorConsumo);
    u8g.print(" J");

  } while ( u8g.nextPage() );
}
