#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <Wire.h>
#include <can.h>
#include <mcp2515.h>
#include <Servo.h>
#include <SPI.h>
#define maxdeger 1940 //max 2000 oluyor escler 1000-2000 arası calisir
#define mindeger 1060
#define sabitleme_toleransi 30
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     20 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DHT dht(5,DHT11);

struct can_frame canSend;
struct can_frame canRcv;

MCP2515 mcp2515(8);
float hizBoleni = 1.0;
int button = 2;
bool button_deger = false;
bool clear_state = true;
int dis_sicaklik;
int dis_nem;
int basinc_deger;
int sicak_deger;

int pinJoyStick_X_1 = 1;
int pinJoyStick_Y_1 = 0;
int pinJoyStick_X_2 = 2;
int pinJoyStick_Y_2 = 3;
bool lcd_durum = false;
int valueJoyStick_X_1 = 0;
int valueJoyStick_Y_1 = 0;
int valueJoyStick_X_2 = 0;
int valueJoyStick_Y_2 = 0;


union ArrayToInteger {
  byte array[2];
  int integer;
} converter;
union ArrayToDouble {
  byte array[4];
  double number;
} doubler;

#define IUDSZ_BMPWIDTH  128
#define IUDSZ_BMPHEIGHT  60

const unsigned char bitmap_iudsz[] PROGMEM = {
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11000000,B00000011,B10000000,B00111000,B00000001,B11001111,B11111111,B00000001,B11100000,B00011100,B00001111,B00000000,B11110000,B00111000,B00000000,B00110000,
  B11000000,B00000011,B11000000,B00111100,B00000001,B11001111,B11111111,B00000001,B11100000,B00011100,B00000111,B10000001,B11100000,B00111100,B00000000,B00110000,
  B11000000,B00000111,B11000000,B00111110,B00000001,B11001111,B11111111,B00000011,B11110000,B00011100,B00000011,B10000001,B11000000,B01111100,B00000000,B00110000,
  B11000000,B00000111,B11100000,B00111110,B00000001,B11000000,B01110000,B00000011,B11110000,B00011100,B00000011,B11000011,B11000000,B01111110,B00000000,B00110000,
  B11000000,B00000111,B11100000,B00111111,B00000001,B11000000,B01110000,B00000111,B11110000,B00011100,B00000001,B11100011,B10000000,B01111110,B00000000,B00110000,
  B11000000,B00001110,B01100000,B00111111,B10000001,B11000000,B01110000,B00000111,B00111000,B00011100,B00000000,B11100111,B00000000,B11101110,B00000000,B00110000,
  B11000000,B00001110,B01100000,B00111011,B10000001,B11000000,B01110000,B00000111,B00111000,B00011100,B00000000,B11111111,B00000000,B11100111,B00000000,B00110000,
  B11000000,B00011100,B01110000,B00111001,B11000001,B11000000,B01110000,B00001110,B00011000,B00011100,B00000000,B01111110,B00000000,B11000111,B00000000,B00110000,
  B11000000,B00011100,B01110000,B00111001,B11100001,B11000000,B01110000,B00001110,B00011000,B00011100,B00000000,B00111100,B00000000,B11000111,B00000000,B00110000,
  B11000000,B00011100,B00110000,B00111000,B11110001,B11000000,B01110000,B00011110,B00011100,B00011100,B00000000,B00011100,B00000001,B11000011,B10000000,B00110000,
  B11000000,B00111000,B00111000,B00111000,B01111001,B11000000,B01110000,B00011100,B00011100,B00011100,B00000000,B00011100,B00000001,B11000011,B10000000,B00110000,
  B11000000,B00111100,B01111000,B00111000,B00111101,B11000000,B01110000,B00011100,B00011110,B00011100,B00000000,B00011000,B00000011,B11000011,B11000000,B00110000,
  B11000000,B01111111,B11111000,B00111000,B00011101,B11000000,B01110000,B00111111,B11111110,B00011100,B00000000,B00011000,B00000011,B11111111,B11000000,B00110000,
  B11000000,B01111111,B11111100,B00111000,B00011111,B11000000,B01110000,B00111111,B11111110,B00011100,B00000000,B00011000,B00000011,B11111111,B11000000,B00110000,
  B11000000,B01110000,B00011100,B00111000,B00001111,B11000000,B01110000,B00111000,B00000111,B00011100,B00000000,B00011100,B00000111,B00000000,B11100000,B00110000,
  B11000000,B11110000,B00001110,B00111000,B00000111,B11000000,B01110000,B01110000,B00000111,B00011111,B11110000,B00011100,B00000111,B00000000,B11100000,B00110000,
  B11000000,B11100000,B00001110,B00111000,B00000011,B11000000,B01110000,B01110000,B00000111,B00011111,B11110000,B00011000,B00000111,B00000000,B11100000,B00110000,
  B11000000,B11100000,B00001111,B00111000,B00000001,B11000000,B01110000,B11110000,B00000011,B10011111,B11110000,B00011100,B00001110,B00000000,B01110000,B00110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00110000,
  B11011100,B00000110,B00000111,B00000111,B00000001,B10000000,B11100001,B11100000,B01110000,B00111000,B00110000,B00011000,B00001110,B00001110,B00000111,B00110000,
  B11111110,B00011111,B00001111,B10001111,B10000111,B11000011,B11100011,B11100001,B11111000,B11111100,B11111000,B01111100,B00111111,B00111110,B00011111,B10110000,
  B11111000,B00111100,B00011110,B00011110,B00001111,B00000111,B10000111,B10000011,B11000001,B11100001,B11100000,B11110000,B01111100,B01111000,B00111100,B00110000,
  B11111000,B01111100,B00111100,B00111110,B00011111,B00001111,B00001111,B00000111,B11000001,B11100001,B11000001,B11110000,B01111000,B01111000,B00111100,B00110000,
  B11111000,B11111100,B01111110,B00111110,B00111111,B00001111,B10001111,B10001111,B11000011,B11100011,B11100011,B11110000,B11111100,B11111100,B11111100,B01110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B00000111,B11111111,B00011111,B11111100,B00011111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111100,B00000001,B11111110,B00001111,B11111000,B00000111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111100,B00000001,B11111110,B00001111,B11110000,B00000111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111000,B11111011,B11111110,B00000111,B11100001,B11001111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111000,B11111111,B11111100,B00000111,B11100011,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111000,B11111111,B11111100,B01100111,B11100001,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111000,B00111111,B11111000,B01100011,B11110000,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111100,B00001111,B11111000,B11100011,B11110000,B00111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111110,B00000011,B11110000,B11110001,B11111000,B00001111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B10000001,B11110000,B11110001,B11111110,B00000111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B11100000,B11110001,B11110001,B11111111,B10000011,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B11111000,B11100001,B11110000,B11111111,B11000011,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111111,B11111000,B11100000,B00000000,B11111111,B11100001,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111101,B11111100,B11100000,B00000000,B11110111,B11100001,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111000,B11111000,B11000011,B11111000,B01100011,B11100011,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111000,B00000000,B11000111,B11111100,B01100000,B00000011,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111100,B00000001,B10000111,B11111100,B01110000,B00000111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111110,B00111110,B00000011,B10000111,B11111100,B00111100,B00001111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000,
  B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11110000
};

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize I2C addr to 0x3C ( for 128x64 Display )
  display.clearDisplay();
  
  display.drawBitmap(0, 0, bitmap_iudsz, 126, 60, WHITE); // show Antalya İSAS logo
  display.display(); //çıkarılıp çıkarılamadığı test edilecek!!!
  delay(5000);
  display.clearDisplay();
  
  Serial.begin(9600);
  SPI.begin();
  
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("TCG Grida");
  display.display();
  delay(5000);
  display.clearDisplay();
  
  pinMode(button , INPUT);
  attachInterrupt(digitalPinToInterrupt(button),lcd_state,CHANGE);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  dht.begin();

}

void lcd_state()
{
  if(lcd_durum == false) lcd_durum = true;
  else lcd_durum = false;
  clear_state = true;
  Serial.println(lcd_durum);
}

void loop()
{
//  currentTime = millis();
  if (Serial.available())
  {
    int incomingByte = Serial.read();
    hizBoleni = incomingByte / 10.0;
  }

  // Araçtan gelen değerler bölünerek USB Üzerine yazılıyor.
  // Eğer araçtan bir bilgi yukarı gelecek ise bunu Can_ID'si üzerinden belirtmeniz gerekir.
  if (mcp2515.readMessage(&canRcv) == MCP2515::ERROR_OK)
  {
    if (canRcv.can_id == 0x03)
    {
      doubler.array[0] = canRcv.data[0];
      doubler.array[1] = canRcv.data[1];
      doubler.array[2] = canRcv.data[2];
      doubler.array[3] = canRcv.data[3];
      Serial.print('b'); // basinc belirteci
      Serial.println(doubler.number);
      basinc_deger = doubler.number;
      doubler.array[0] = canRcv.data[4];
      doubler.array[1] = canRcv.data[5];
      doubler.array[2] = canRcv.data[6];
      doubler.array[3] = canRcv.data[7];
      Serial.print('s'); // sicaklik belirteci
      Serial.println(doubler.number);
      sicak_deger = doubler.number;
    }
  }
  else{
    //Serial.println("hata");
    }
  
  dis_sicaklik = dht.readTemperature();
  dis_nem = dht.readHumidity();

  // Analogread 0-1023 arasında okuma yapar, burada esc değerleri olan 1000-2000 arasına eşitleniyor.
  // Herhangi bir joystick değerini 3000'den çıkarmak, joystick eksenini ters çevirme anlamına gelir.
  valueJoyStick_X_1 = analogRead(pinJoyStick_X_1) + 1000;
  valueJoyStick_Y_1 = 3000 - (analogRead(pinJoyStick_Y_1) + 1000);
  valueJoyStick_X_2 = analogRead(pinJoyStick_X_2) + 1000;
  valueJoyStick_Y_2 = 3000 - (analogRead(pinJoyStick_Y_2) + 1000);
  // Joystick değerlerini merkezi değiştirmeden bölme işlemleri
  valueJoyStick_X_1 = 1500 + (valueJoyStick_X_1 - 1500) / hizBoleni;
  valueJoyStick_Y_1 = 1500 + (valueJoyStick_Y_1 - 1500) / hizBoleni;
  valueJoyStick_X_2 = 1500 + (valueJoyStick_X_2 - 1500) / hizBoleni;
  valueJoyStick_Y_2 = 1500 + (valueJoyStick_Y_2 - 1500) / hizBoleni;
  
  if (valueJoyStick_X_1 > maxdeger) valueJoyStick_X_1 = maxdeger;
  if (valueJoyStick_Y_1 > maxdeger) valueJoyStick_Y_1 = maxdeger;
  if (valueJoyStick_X_2 > maxdeger) valueJoyStick_X_2 = maxdeger;
  if (valueJoyStick_Y_2 > maxdeger) valueJoyStick_Y_2 = maxdeger;

  if (valueJoyStick_X_1 < mindeger) valueJoyStick_X_1 = mindeger;
  if (valueJoyStick_Y_1 < mindeger)valueJoyStick_Y_1 = mindeger;
  if (valueJoyStick_X_2 < mindeger) valueJoyStick_X_2 = mindeger;
  if (valueJoyStick_Y_2 < mindeger)valueJoyStick_Y_2 = mindeger;

  // joystick'ler belli bi toleransla ortadayken 1500'e sabitliyoruz
  if (valueJoyStick_X_1 < 1500 + sabitleme_toleransi / hizBoleni && valueJoyStick_X_1 > 1500 - sabitleme_toleransi / hizBoleni)
    valueJoyStick_X_1 = 1500;
  if (valueJoyStick_Y_1 < 1500 + sabitleme_toleransi / hizBoleni && valueJoyStick_Y_1 > 1500 - sabitleme_toleransi / hizBoleni)
    valueJoyStick_Y_1 = 1500;
  if (valueJoyStick_X_2 < 1500 + sabitleme_toleransi / hizBoleni && valueJoyStick_X_2 > 1500 - sabitleme_toleransi / hizBoleni)
    valueJoyStick_X_2 = 1500;
  if (valueJoyStick_Y_2 < 1500 + sabitleme_toleransi / hizBoleni && valueJoyStick_Y_2 > 1500 - sabitleme_toleransi / hizBoleni)
    valueJoyStick_Y_2 = 1500;
  // Joystick degerleri 8 byte'a siralanip yollaniyor
  canSend.can_id = 0x02;
  canSend.can_dlc = 8;
  canSend.data[0] = highByte(valueJoyStick_X_1);
  canSend.data[1] = lowByte(valueJoyStick_X_1);
  canSend.data[2] = highByte(valueJoyStick_Y_1);
  canSend.data[3] = lowByte(valueJoyStick_Y_1);
  canSend.data[4] = highByte(valueJoyStick_X_2);
  canSend.data[5] = lowByte(valueJoyStick_X_2);
  canSend.data[6] = highByte(valueJoyStick_Y_2);
  canSend.data[7] = lowByte(valueJoyStick_Y_2);

  // q-w-e-r her degerin bilgisayardan ayirt edilmesi icin ayri kodlar
  /*Serial.print("X1");
  Serial.println(valueJoyStick_X_1);
  Serial.print("Y1");
  Serial.println(valueJoyStick_Y_1);
  Serial.print("X2");
  Serial.println(valueJoyStick_X_2);
  Serial.print("Y2");
  Serial.println(valueJoyStick_Y_2);
  */
  
  if(!lcd_durum)
  {
      if(clear_state == true) display.clearDisplay();
      clear_state = false;
    //her harfin eni 12 piksel dolayısıyla karakter sayısı x 12+2(2 boşluk bırakmak için gerekiyor) yeni cursor posziyonu
    // (:) işaretini alandan kazanmak için özel cursor yeri belirleyerek yazdım 3-4 pixel kazandık böylece diper türlü önceki kelimenin son harfi ve (:) arasında çok boşluk oluyordu.
      display.setTextSize(2); 
      display.setTextColor(WHITE);
      display.setCursor(0,16);
      display.print("Derinlik");
      display.setCursor(90, 16);
      display.print(":");
      display.setCursor(99, 16);
      display.print(basinc_deger); //pressure value
    
      display.setCursor(0,32);
      display.print("Sicaklik:");
      display.setCursor(90, 32);
      display.print(":");
      display.setCursor(99,32);
      display.print(sicak_deger); //temp value
      display.display();
      
  }
  
  else if(lcd_durum)
  {
    if(clear_state == true) display.clearDisplay();
    clear_state = false;
    
    display.setCursor(0,0);
    display.print("x1:");
    display.setCursor(3,0);
    display.print(valueJoyStick_X_1);
    display.setCursor(8,0);
    display.print("x2:");
    display.setCursor(11,0);
    display.print(3000 - (valueJoyStick_Y_2)); // Burada neden 3000'den çıkartıyoruz? 201. satırda çıkarmıştık zaten.
    display.setCursor(0,1);
    display.print("y2:");
    display.setCursor(3,1);
    display.print(valueJoyStick_X_2);
    display.setCursor(8,1);
    display.print("%");

    // Show humidity level
    if(dis_nem < 10)
    {
      display.setCursor(9,1);
      display.print("0");
      display.setCursor(10,1);
      display.print(dis_nem);
    }
    else
    {
      display.setCursor(9,1);
      display.print(dis_nem);
    }

    // Show temperature level (Celcius)
    if(dis_sicaklik < 10)
    {
      display.setCursor(12,1);
      display.print("0");
      display.setCursor(13,1);
      display.print(dis_sicaklik);
    }
    else
    {
      display.setCursor(12,1);
      display.print(dis_sicaklik);
    }
    display.setCursor(14,1);
    display.print("C");
    
    display.display();
    delay(5000);
  }
  
  mcp2515.sendMessage(&canSend);
  delay(20);
  
}
