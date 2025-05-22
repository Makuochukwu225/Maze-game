#include <LedControl.h>

LedControl lc = LedControl(11, 13, 10, 1);

// Joystick pins
const int VRx = A0;
const int VRy = A1;

// Buzzer pin
const int buzzer = 3;

// Player position
int playerX = 1, playerY = 1;
bool hasWon = false;

// Maze layout
byte maze[8] = {
  B11111111,
  B10000001,
  B10111001,
  B10100001,
  B10101111,
  B10100001,
  B10000001,
  B11111111
};

// Win position
const int goalX = 6;
const int goalY = 6;

// Bumblebee notes
int melody[] = {
  880, 988, 1047, 1175, 1319, 1175, 1047, 988,
  880, 988, 1047, 1175, 1319, 1175, 1047, 988
};
int noteDuration = 100;
int noteIndex = 0;
unsigned long lastNoteTime = 0;

unsigned long lastMoveTime = 0;
int moveDelay = 200;

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  pinMode(buzzer, OUTPUT);
  drawMaze();
  drawPlayer();
}

void loop() {
  if (!hasWon) {
    handleJoystick();
  }
  playBumblebee();
}

void drawMaze() {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, maze[row]);
  }
}

void drawPlayer() {
  lc.setLed(0, playerY, playerX, true);
}

void clearPlayer() {
  lc.setLed(0, playerY, playerX, false);
}

void handleJoystick() {
  if (millis() - lastMoveTime < moveDelay) return;

  int xVal = analogRead(VRx);
  int yVal = analogRead(VRy);
  int dx = 0, dy = 0;

  if (xVal < 400) dx = -1;
  else if (xVal > 600) dx = 1;

  if (yVal < 400) dy = -1;
  else if (yVal > 600) dy = 1;

  if (dx != 0 || dy != 0) {
    int newX = playerX + dx;
    int newY = playerY + dy;

    if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
      if (bitRead(maze[newY], 7 - newX) == 0) {
        clearPlayer();
        playerX = newX;
        playerY = newY;
        drawPlayer();
        checkWin();
      }
    }
    lastMoveTime = millis();
  }
}

void checkWin() {
  if (playerX == goalX && playerY == goalY) {
    hasWon = true;
    winAnimation();
  }
}

void winAnimation() {
  for (int i = 0; i < 3; i++) {
    lc.clearDisplay(0);
    delay(200);
    drawMaze();
    drawPlayer();
    delay(200);
  }
  // Light all LEDs to indicate game end
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, B11111111);
  }
}

void playBumblebee() {
  if (millis() - lastNoteTime >= noteDuration) {
    int note = melody[noteIndex];
    tone(buzzer, note, noteDuration - 10);
    noteIndex = (noteIndex + 1) % (sizeof(melody) / sizeof(melody[0]));
    lastNoteTime = millis();
  }
}
