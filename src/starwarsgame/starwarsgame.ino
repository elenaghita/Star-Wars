#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
#include <Fonts/FreeSans9pt7b.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Tones
const int toneC = 261;
const int toneD = 294;
const int toneE = 329;
const int toneF = 349;
const int toneG = 391;
const int toneGS = 415;
const int toneA = 440;
const int toneAS = 455;
const int toneB = 466;
const int toneCH = 523;
const int toneCSH = 554;
const int toneDH = 587;
const int toneDSH = 622;
const int toneEH = 659;
const int toneFH = 698;
const int toneFSH = 740;
const int toneGH = 784;
const int toneGSH = 830;
const int toneAH = 880;

// Bitmaps
const unsigned char PROGMEM playerShip [] = {
  0x00,0x00,0x00,0x00,0x1C,0x00,0x3F,0xF0,0x3C,0x00,0x3C,0x00,0xFF,0x00,0x7F,0xFF,
  0x7F,0xFF,0xFF,0x00,0x3C,0x00,0x3C,0x00,0x1F,0xF0,0x1C,0x00,0x00,0x00,0x00,0x00
};
const unsigned char PROGMEM enemyIcon [] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xC0,0x00,
  0x1F,0xFF,0xF0,0x00,0x1C,0x00,0x70,0x00,0x30,0x00,0x18,0x00,
  0x60,0x00,0x0C,0x00,0x63,0xFC,0x0C,0x00,0xC7,0xFE,0x06,0x00,
  0xCF,0xFF,0xE6,0x00,0xCF,0xFF,0xE6,0x00,0xCF,0xFF,0xE6,0x00,
  0xCF,0xFF,0xE6,0x00,0xC7,0xFF,0xE6,0x00,0x63,0xFF,0xCC,0x00,
  0x60,0x00,0x0C,0x00,0x30,0x00,0x18,0x00,0x38,0x00,0x38,0x00,
  0x1F,0xFF,0xF0,0x00,0x07,0xFF,0xC0,0x00
};

// Game variables
int bulletX = 0, bulletY = 0, bulletActive = 0;
int enemyY = 8, enemyDirection = 0, gameOver = 0;
int enemyBulletX1 = 95, enemyBulletY1 = 0;
int enemyBulletX2 = 95, enemyBulletY2 = 0;
int enemyBulletX3 = 95, enemyBulletY3 = 0;
int enemyBulletX4 = 95, enemyBulletY4 = 0;
int score = 0, lives = 5, fireReady = 0, enemyBulletsFired = 0, level = 1;
int enemyBulletSpeed = 3, bulletsAtOnce = 1, minInterval = 600, maxInterval = 1200, enemyRadius = 10;

int enemyCenterX = 95;
int playerY = 30;
unsigned long startMillis = 0, fireInterval = 0, currentMillis = 0, lastLevelMillis = 0;

void setup() {
  pinMode(12, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(3,  INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();


  long millivolts = readVcc();
  char buffer[6];
  dtostrf(millivolts / 1000.0, 4, 2, buffer);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("V:"); display.println(buffer);

  // Graphic intro
  display.drawBitmap(6,11,enemyIcon,48,48,1);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setCursor(65,14); display.println("xWing");
  display.setFont();
  display.setCursor(65,17); display.println("vs");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(65,39); display.println("Death");
  display.setFont();
  display.setCursor(65,42); display.println("star");
  display.setTextSize(1);
  display.setCursor(65,55); display.println("by Elena");
  display.display();

  // start song
  beep(toneA,500); beep(toneA,500); beep(toneA,500);
  beep(toneF,350); beep(toneCH,150);
  beep(toneA,500); beep(toneF,350);
  beep(toneCH,150); beep(toneA,650);
  delay(1000);
}

void loop() {
  if (!gameOver) {
    display.clearDisplay();
    drawStars();

    // Enemy fire prep
    if (!fireReady) {
      startMillis  = millis();
      fireInterval = random(minInterval, maxInterval);
      fireReady    = 1;
    }
    currentMillis = millis();

    // Level up
    if (currentMillis - lastLevelMillis > 50000) {
      lastLevelMillis = currentMillis;
      level++; enemyBulletSpeed++;
      if (level % 2 == 0) { bulletsAtOnce++; enemyRadius--; }
      minInterval -= 50; maxInterval -= 50;
    }

    // Fire enemy bullets
    if (startMillis + fireInterval < currentMillis) {
      fireReady = 0; enemyBulletsFired++;
      if (enemyBulletsFired == 1) { enemyBulletX1=enemyCenterX; enemyBulletY1=enemyY; }
      if (enemyBulletsFired == 2)  { enemyBulletX2=enemyCenterX; enemyBulletY2=enemyY; }
      if (enemyBulletsFired == 3) { enemyBulletX3=enemyCenterX; enemyBulletY3=enemyY; }
      if (enemyBulletsFired == 4) { enemyBulletX4=enemyCenterX; enemyBulletY4=enemyY; }
    }

    // Draw & move enemy projectiles
    if (enemyBulletsFired>0) { display.drawCircle(enemyBulletX1,enemyBulletY1,2,1); enemyBulletX1-=enemyBulletSpeed; }
    if (enemyBulletsFired>1) { display.drawCircle(enemyBulletX2,enemyBulletY2,1,1); enemyBulletX2-=enemyBulletSpeed; }
    if (enemyBulletsFired>2) { display.drawCircle(enemyBulletX3,enemyBulletY3,4,1); enemyBulletX3-=enemyBulletSpeed; }
    if (enemyBulletsFired>3) { display.drawCircle(enemyBulletX4,enemyBulletY4,2,1); enemyBulletX4-=enemyBulletSpeed; }

    // Player controls
    if (!digitalRead(12) && playerY>=2)  playerY-=2;
    if (!digitalRead(11) && playerY<=46) playerY+=2;
    if (!digitalRead(3) && !bulletActive) {
      bulletActive=1; bulletX=6; bulletY=playerY+8; tone(9,1200,20);
    }
    if (bulletActive) {
      bulletX+=8;
      display.drawLine(bulletX,bulletY,bulletX+4,bulletY,1);
      if (bulletX>128) bulletActive=0;
    }

    // Draw the player's ship and the enemy
    display.drawBitmap(4,playerY,playerShip,16,16,1);
    display.fillCircle(enemyCenterX,enemyY,enemyRadius,1);
    display.fillCircle(enemyCenterX+2,enemyY+3,enemyRadius/3,0);
    enemyY += (enemyDirection==0? bulletsAtOnce : -bulletsAtOnce);
    if (enemyY >= 64-enemyRadius) enemyDirection=1;
    if (enemyY <= enemyRadius)    enemyDirection=0;

    // Bullet–enemy collision
    if (bulletY>=enemyY-enemyRadius && bulletY<=enemyY+enemyRadius &&
        bulletX>enemyCenterX-enemyRadius && bulletX<enemyCenterX+enemyRadius) {
      bulletX=-20; tone(9,500,20); score++; bulletActive=0;
    }

    // Player-projectile collisions
    int pc = playerY+8;
    if (checkCollision(enemyBulletX1,enemyBulletY1,pc)) { enemyBulletX1=enemyCenterX;  enemyBulletY1=-50; loseLife(); }
    if (checkCollision(enemyBulletX2,enemyBulletY2,pc)) { enemyBulletX2=-50; enemyBulletY2=-50; loseLife(); }
    if (checkCollision(enemyBulletX3,enemyBulletY3,pc)) { enemyBulletX3=-50; enemyBulletY3=-50; loseLife(); }
    if (checkCollision(enemyBulletX4,enemyBulletY4,pc)) {
      enemyBulletX4=200; enemyBulletY4=-50; enemyBulletsFired=0; loseLife();
    }
    if (enemyBulletX4<1) { enemyBulletsFired=0; enemyBulletX4=200; }

    if (lives==0) gameOver=1;

    // UI
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(33,57); display.print("score:"); display.setCursor(68,57); display.println(score);
    display.setCursor(33, 0); display.print("lives:"); display.setCursor(68, 0); display.println(lives);
    display.setCursor(110,0); display.print("L:");      display.setCursor(122,0); display.println(level);
    display.setCursor(108,57); display.println(currentMillis/1000);

    display.display();
  }
  else {
    // Game Over
    if (lives==0) {
      tone(9,200,300); delay(300);
      tone(9,250,200); delay(200);
      tone(9,300,300); delay(300);
      lives=5;
    }
    display.clearDisplay();
    display.setFont();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(7,10);  display.println("GAME OVER!");
    display.setTextSize(1);
    display.setCursor(7,30);  display.print("score:");  display.setCursor(44,30); display.println(score);
    display.setCursor(7,40);  display.print("level:");  display.setCursor(44,40); display.println(level);
    display.setCursor(7,50);  display.print("time(s):");display.setCursor(60,50); display.println(currentMillis/1000);
    display.display();
    if (!digitalRead(3)) {
      tone(9,280,300); delay(300);
      tone(9,250,200); delay(200);
      tone(9,370,300); delay(300);
      resetGame();
    }
  }
}

bool checkCollision(int x, int y, int playerCenter) {
  return (y >= playerCenter - 8 && y <= playerCenter + 8) && (x < 12 && x > 4);
}

void loseLife() {
  tone(9,100,100);
  lives--;
}

void drawStars() {
  const uint8_t xs[] = {50,30,60,55,25,100,117,14,24,78,80,107,150,5,8,70,10,70};
  const uint8_t ys[] = {30,17,18,16,43,43,52,49,24,36,57,11,11,5,7,12,56,25};
  for (uint8_t i = 0; i < sizeof(xs); i++) {
    display.drawPixel(xs[i], ys[i], 1);
  }
}

void beep(int toneVal, int duration) {
  tone(9, toneVal, duration);
  delay(duration);
  noTone(9);
}

long readVcc() {
  
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);

  delay(2); 
  ADCSRA |= _BV(ADSC); 
  while (bit_is_set(ADCSRA, ADSC)); 

  uint8_t low = ADCL;
  uint8_t high = ADCH;
  long result = (high << 8) | low;

  
  return 1125300L / result;
}


void resetGame() {
  asm volatile("jmp 0");
}