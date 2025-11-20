# CW_beacon_ESP32_Adafruit

ESP32 + Si5351 CW beacon 


This program allows you to generate a CW (Morse) beacon in the amateur radio bands from 8 kHz to 150 MHz 
(Tested between 471kHz and 28 MHz with Adafruir  Si5351 library) using an ESP32 (Lilygo TTGO T-Display) 
and a Si5351 (https://ea5jtt.blogspot.com/2025/09/si5351-primeros-pasos.html )with a power of 7 dB and 
Adafruir  Si5351 library https://github.com/adafruit/Adafruit_Si5351_Library

You must calibrate you si5351 for each frecuency band

This program is based on [https://github.com/ta2bgh/cw-beacon](https://github.com/joanpao/CW_beacon_ESP32)  and was created by EA5JTT Juanpa in 20251118

Compared to the original program:
- Use Adafruit SI5351 library (It allows for better and cleaner control over the frequency by setting it to a lower level than the Ethekit Si5351 library.)
- no need calibrate you si5351 

ATENCION PLEASE!!!
- You can comment on the unused frequencies in the carousel table.
- Morse code generates harmonics, the more harmonics the higher the speed, so a suitable low-pass filter must be provided.
- You can only operate this program if you have a valid amateur radio license in your territory.

More information in spanish language : https://ea5jtt.blogspot.com/2025/10/cw-tx-beacon-esp32-si5351.html
