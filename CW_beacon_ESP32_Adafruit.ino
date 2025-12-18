// BALIZA DE CW-MORSE PARA BANDAS BAJAS
// CREADO POR EA5JTT 20251118
//
// Programa que permite generar una baliza o beacon CW (Morse)
// en las bandas de radioaficionado desde 8 kHz hasta los 150 MHz 
// mediante un ESP32 (Lilygo TTGO T-Display) y un Si5351 con una potencia de 7 dB
// usando las librerias ADAFRUIT SI5351 https://github.com/adafruit/Adafruit_Si5351_Library/blob/master/examples/si5351/si5351.ino
//
// LIMITACIOENS Y PROBLEMAS
// Este programa solamente puede ser ejecutado si cuenta con la correspondiente licencia de radioaficionado valida en su territorio
// La modulación MORSE genera muchas espuereas, tanto mas cuanto mayor sea la velocidad de trnasmisión, por ello
// - Debe usar velocidades bajas
// - Debe analizar las espureas  con un analizador de espectros
// - Debe dotar la saldia de un filtro pasabanda o pasabajos adecuado
// - La potencia de salida que suministra el SI5351 es de 7 dBm 
// que en CW no son suficientes para tener una cobertura mas allá del QTH, por lo que en la practica precisa de un amplificador de potencia, sirve un LNA 
// adaptando el nivel de entrada mediante un atenuador adeucado
// 
// AYUDAS A LA PROGRAMACION
// OSCILADOR LOCAL PLL - UNICO
// Frecuencia entre 600 y 900 MHz con reloj a 25 MHz 
// modos fraccionados
// PLLB = 25 MHz * (24 + 2/3) = 616.666666 MHz
// clockgen.setupPLL(SI5351_PLL_B, 24, 2, 3);
// modos enteros (ojo precisa modo entero en PLL)
// divisor por 4, 6  y 8;  MULTISYNTH_DIV_4 = 4 MULTISYNTH_DIV_6 = 6 MULTISYNTH_DIV_8 = 8 
// clockgen.setupPLLInt(SI5351_PLL_A, 36);
// DIVISORES SI5351_PLL_A y SI5351_PLL_B puede generar dos valores
// DIVISOR DE SALIDA INDIVIDUAL APRA CADA CLK
// divide la salida todavia por mas 64
// clockgen.setupRdiv(2, SI5351_R_DIV_64);
// R_DIV_1, R_DIV_2, R_DIV_4, R_DIV_8, R_DIV_16, R_DIV_32, R_DIV_64, R_DIV_128

/* SUBBANDAS DE CW y CW/QRP 
   472 -     479
 1.815 -   1.830
 3.500 -   3.568500 3560/3570 QRP
 5.351,5 - 5.354,0
 7.000 -  7.038500
10.100 - 10.130   
14.000 - 14.070   
18.068 - 18.095  
21.000 - 21.070   
24.890 - 24.915   
28.225 - 28.300	  
50.400 - 50.500	  
144.400-144.491	  
*/
/*
 * Copyright (C) 2025 Juan Pablo Sanchez EA5JTT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
// SI5351 ADAFRUIT
#include <Adafruit_SI5351.h>
Adafruit_SI5351 clockgen = Adafruit_SI5351();
// OLED
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include "Fixed8x16.h"
#define OLED_RST NOT_A_PIN
#define OLED_SDA 21
#define OLED_SCL 22
Adafruit_SSD1306 display(OLED_RST);

// Velocidad CW (ms por punto)
// uint16_t dit = 80;   // ~15 wpm aprox  normal
   uint16_t dit = 120;     // ~11 wpm aprox lento 
// uint16_t dit = 160;     // ~7,5 wpm aprox lento 


// TABLA DE FRECUENCIAS
struct FreqEntry {
  uint16_t a;
  uint32_t b;
  uint32_t c;
  const char *name;
};

FreqEntry freqs[] = {
  {1298, 14, 57,  "475 kHz"},
  {338,  31, 68,  "1822 kHz"},
  {172,  89, 91,  "3565 kHz"},
  {115,  27, 122, "5352 kHz"},
  {87,   107,118, "7015 kHz"},
  {60,  4402, 4559, "10115 kHz"},
  {43,  9178, 9787, "14035 kHz"},
  {34,  597,  5357, "18078 kHz"},
  {29,  930,  2941, "21035 kHz"},
  {24,  572,  747,  "24900 kHz"},
  {21,  2767, 3268, "28227 kHz"},
  {12,  676,  3027, "50450 kHz"},
  {4,   1261, 4684, "144445 kHz"}
};

const int NUM_FREQS = sizeof(freqs) / sizeof(freqs[0]);

//--------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("CW Si5351 TX multi-frecuencia ... ");

  if (clockgen.begin() != ERROR_NONE) {
    Serial.println("Si5351: KO");
    while (1);
  }
  Serial.println("Si5351: OK");

  // PLL base
  clockgen.setupPLL(SI5351_PLL_B, 24, 2, 3);
  Serial.println("PLL: OK");

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, OLED_SDA, OLED_SCL);
  display.clearDisplay();
  display.setFont(&Fixed8x16);
  display.setTextColor(WHITE);
  display.setCursor(0, 12);
  display.println("CW TX - EA5JTT");
  display.display();
  delay(2000);
  Serial.println("OLED: OK");
  // Emisión apagada
  clockgen.enableOutputs(false);
}


// Funciones CW
void tx(bool on) {
  clockgen.enableOutputs(on);
}
// PUNTO .
void dit_f() {
  tx(true);  delay(dit);
  tx(false); delay(dit);
}
// RAYA -
void dah_f() {
  tx(true);  delay(3 * dit);
  tx(false); delay(dit);
}
// ESPACIO ENTRE LETRAS Y PALABRAS
void inter_letter() { delay(2 * dit); }
void inter_word()   { delay(6 * dit); }
// TABLA CODIGO MORSE
String morse(char c) {
  c = toupper(c);
  switch (c) {
    case 'A': return ".-";   case 'B': return "-...";
    case 'C': return "-.-."; case 'D': return "-..";
    case 'E': return ".";    case 'F': return "..-.";
    case 'G': return "--.";  case 'H': return "....";
    case 'I': return "..";   case 'J': return ".---";
    case 'K': return "-.-";  case 'L': return ".-..";
    case 'M': return "--";   case 'N': return "-.";
    case 'O': return "---";  case 'P': return ".--.";
    case 'Q': return "--.-"; case 'R': return ".-.";
    case 'S': return "...";  case 'T': return "-";
    case 'U': return "..-";  case 'V': return "...-";
    case 'W': return ".--";  case 'X': return "-..-";
    case 'Y': return "-.--"; case 'Z': return "--..";

    case '1': return ".----"; case '2': return "..---";
    case '3': return "...--"; case '4': return "....-";
    case '5': return "....."; case '6': return "-....";
    case '7': return "--..."; case '8': return "---..";
    case '9': return "----."; case '0': return "-----";
    case '/': return "-..-.";
  }
  return "";
}

void sendCW(String msg) {
  for (int i = 0; i < msg.length(); i++) {
    char c = msg[i];

    if (c == ' ') { inter_word(); continue; }

    String m = morse(c);
    for (int j = 0; j < m.length(); j++) {
      if (m[j] == '.') dit_f();
      else dah_f();
    }
    inter_letter();
  }
}



void setFrequency(int idx) {
  FreqEntry f = freqs[idx];
  clockgen.enableOutputs(false);
  clockgen.setupMultisynth(2, SI5351_PLL_B, f.a, f.b, f.c);
  clockgen.enableOutputs(true);
  Serial.print("Frecuencia configurada: ");
  Serial.println(f.name);
}

void showFreqOLED(const char* txt) {
  // Limpia solo la parte inferior
  display.fillRect(0, 20, 128, 40, BLACK);
  display.setCursor(0, 30);
  display.print("Frec: ");
  display.println(txt);
  display.display();
}


void loop() {
  for (int i = 0; i < NUM_FREQS; i++) {
    setFrequency(i);
    delay(300);   // estabilización del oscilador
    Serial.print("TX en ");
    Serial.println(freqs[i].name);
    showFreqOLED(freqs[i].name);
    sendCW("CQ CQ CQ DE EA5JTT/B EA5JTT/B EA5JTT/B K");
    delay(2000);  // tiempo entre bandas
  }
}
