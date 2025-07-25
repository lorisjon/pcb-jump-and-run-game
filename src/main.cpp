#include <TFT_eSPI.h> // Hardware-spezifische Bibliothek
#include <SPI.h>

#define TFT_GREY 0x5AEB
#define JUNGLE_GREEN 0x1C43
#define PLAYER_COLOR 0xF800 // Rot
#define GROUND_COLOR 0x8241 // Braun
#define OBSTACLE_COLOR 0x07E0 // Grün

TFT_eSPI tft = TFT_eSPI();

float playerX = 20;
float playerY = 100;
float playerVelY = 0;
bool isJumping = false;
const float gravity = 0.3;
const float jumpForce = -5.0;

float obstacleX[3] = {240, 340, 440};
float obstacleSpeed = 1.5;
int score = 0;
int gameState = 0; // 0 = Startbildschirm, 1 = Spielen, 2 = Game Over

void setup(void) {
  pinMode(21, INPUT_PULLUP); // Runter (nicht verwendet)
  pinMode(22, INPUT_PULLUP); // Hoch (Springen)
  pinMode(17, INPUT_PULLUP); // Rechts (nicht verwendet)
  pinMode(2, INPUT_PULLUP);  // Links (nicht verwendet)
  pinMode(13, INPUT_PULLUP); // Button A (Start/Neustart)

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void drawJungleBackground() {
  tft.fillScreen(JUNGLE_GREEN);
  // Boden zeichnen
  tft.fillRect(0, 110, 240, 25, GROUND_COLOR);
  // Einige einfache Bäume/Sträucher
  tft.fillTriangle(50, 110, 60, 80, 70, 110, TFT_GREEN);
  tft.fillTriangle(150, 110, 160, 90, 170, 110, TFT_GREEN);
}

void drawPlayer() {
  tft.fillRect(playerX, playerY, 10, 10, PLAYER_COLOR);
}

void drawObstacles() {
  for (int i = 0; i < 3; i++) {
    tft.fillRect(obstacleX[i], 100, 10, 20, OBSTACLE_COLOR);
  }
}

void updatePlayer() {
  // Sprungmechanik
  if (digitalRead(22) == 0 && !isJumping) {
    playerVelY = jumpForce;
    isJumping = true;
  }

  playerVelY += gravity;
  playerY += playerVelY;

  // Bodenkollision
  if (playerY > 100) {
    playerY = 100;
    playerVelY = 0;
    isJumping = false;
  }
}

void updateObstacles() {
  for (int i = 0; i < 3; i++) {
    obstacleX[i] -= obstacleSpeed;
    
    // Hindernis zurücksetzen, wenn es den Bildschirm verlässt
    if (obstacleX[i] < -10) {
      obstacleX[i] = 240 + random(50, 100);
      score += 1;
    }
  }
}

bool checkCollision() {
  for (int i = 0; i < 3; i++) {
    if (playerX + 10 > obstacleX[i] && 
        playerX < obstacleX[i] + 10 &&
        playerY + 10 > 100) {
      return true;
    }
  }
  return false;
}

void loop() {
  if (gameState == 0) { // Startbildschirm
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(50, 50, 2);
    tft.setTextColor(TFT_WHITE);
    tft.print("Jungle Runner");
    tft.setCursor(40, 70, 2);
    tft.print("Press A to Start");

    while (digitalRead(13) == 1) {
      delay(10);
    }
    gameState = 1;
    
    // Spiel zurücksetzen
    playerX = 20;
    playerY = 100;
    playerVelY = 0;
    isJumping = false;
    obstacleX[0] = 240;
    obstacleX[1] = 340;
    obstacleX[2] = 440;
    score = 0;
  }

  if (gameState == 1) { // Spielphase
    // Update
    updatePlayer();
    updateObstacles();

    // Kollision prüfen
    if (checkCollision()) {
      gameState = 2;
    }

    // Zeichnen
    drawJungleBackground();
    drawPlayer();
    drawObstacles();

    // Punktestand anzeigen
    tft.setCursor(200, 0, 2);
    tft.setTextColor(TFT_WHITE);
    tft.fillRect(200, 0, 40, 16, TFT_BLACK);
    tft.print(score);

    delay(20); // Spielgeschwindigkeit anpassen
  }

  if (gameState == 2) { // Game Over
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(50, 50, 2);
    tft.setTextColor(TFT_WHITE);
    tft.print("Game Over!");
    tft.setCursor(50, 70, 2);
    tft.print("Score: ");
    tft.print(score);
    tft.setCursor(40, 90, 2);
    tft.print("Press A to Restart");

    while (digitalRead(13) == 1) {
      delay(10);
    }
    gameState = 0;
  }
}