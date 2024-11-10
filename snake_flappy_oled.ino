#include "U8glib.h"
U8GLIB_SH1106_128X64 oled(U8G_I2C_OPT_NONE);

// Constants for button pins
const int UP_BUTTON = 2;
const int DOWN_BUTTON = 3;
const int LEFT_BUTTON = 4;
const int RIGHT_BUTTON = 5;

// Game variables for Snake
int snakeX = 64, snakeY = 32; // Initial position of the snake
int foodX, foodY; // Position of the food
bool gameOver = false;
int direction = 0; // 0 = up, 1 = right, 2 = down, 3 = left
int score = 0;
bool inGame = false;
bool menuShown = true;
int currentGame = 0; // 0 = Snake, 1 = Flappy Ball

// Game variables for Flappy Ball
int ballX = 30, ballY = 32;
int velocity = 0;
int gravity = 1;
int flap = -6;
bool flappyGameOver = false;

int selectedGame = 0; // 0 = Snake, 1 = Flappy Ball

void setup() {
  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  
  randomSeed(analogRead(0)); // Seed random generator
  spawnFood(); // Place food for Snake
}

void loop() {
  if (menuShown) {
    displayMenu();
  } else {
    if (currentGame == 0) {
      runSnake();
    } else if (currentGame == 1) {
      runFlappyBall();
    }
  }
}

void displayMenu() {
  oled.firstPage();
  do {
    oled.setFont(u8g_font_profont12);
    oled.setPrintPos(25, 10);
    oled.print("Select Game:");
    oled.setPrintPos(22, 25);
    oled.print("Dev: Biswajit");
    oled.setPrintPos(10, 45);
    oled.print("1. Snake");
    oled.setPrintPos(10, 60);
    oled.print("2. Flappy Ball");

    // Highlight selected game
    if (selectedGame == 0) {
      oled.drawBox(0, 40, 5, 5); // Highlight Snake
    } else if (selectedGame == 1) {
      oled.drawBox(0, 55, 5, 5); // Highlight Flappy Ball
    }
  } while (oled.nextPage());

  // Navigate menu with UP/DOWN, select with RIGHT
  if (digitalRead(UP_BUTTON) == HIGH) selectedGame = 0;
  if (digitalRead(DOWN_BUTTON) == HIGH) selectedGame = 1;
  if (digitalRead(RIGHT_BUTTON) == HIGH) {
    currentGame = selectedGame; // Set the current game to selected one
    menuShown = false;
    inGame = true;
  }
}

void runSnake() {
  if (gameOver) {
    displayGameOver();
    return;
  }

  oled.firstPage();
  do {
    oled.setFont(u8g_font_profont12);
    oled.setPrintPos(0, 10);
    oled.print("Score: ");
    oled.print(score);
    
    oled.drawBox(snakeX, snakeY, 4, 4); // Draw snake as a box
    oled.drawBox(foodX, foodY, 4, 4); // Draw food as a box
  } while (oled.nextPage());
  
  updateDirection();
  moveSnake();
  checkCollision();
  delay(200);
}

void spawnFood() {
  foodX = random(0, 31) * 4;
  foodY = random(0, 15) * 4;
}

void updateDirection() {
  if (digitalRead(UP_BUTTON) == HIGH && direction != 2) direction = 0;
  if (digitalRead(RIGHT_BUTTON) == HIGH && direction != 3) direction = 1;
  if (digitalRead(DOWN_BUTTON) == HIGH && direction != 0) direction = 2;
  if (digitalRead(LEFT_BUTTON) == HIGH && direction != 1) direction = 3;
}

void moveSnake() {
  switch (direction) {
    case 0: snakeY -= 4; break; // Move up
    case 1: snakeX += 4; break; // Move right
    case 2: snakeY += 4; break; // Move down
    case 3: snakeX -= 4; break; // Move left
  }
}

void checkCollision() {
  // Check if snake hits the screen edge
  if (snakeX < 0 || snakeX >= 128 || snakeY < 0 || snakeY >= 64) {
    gameOver = true;
  }
  
  // Check if snake eats the food
  if (snakeX == foodX && snakeY == foodY) {
    score++;
    spawnFood();
  }
}

void displayGameOver() {
  oled.firstPage();
  do {
    oled.setFont(u8g_font_profont12);
    oled.setPrintPos(20, 30);
    oled.print("Game Over!");
    oled.setPrintPos(20, 50);
    oled.print("Score: ");
    oled.print(score);
  } while (oled.nextPage());
  
  // Reset game or go back to menu based on button press
  if (digitalRead(RIGHT_BUTTON) == HIGH) {
    snakeX = 64; snakeY = 32;
    score = 0;
    direction = 0;
    gameOver = false;
    spawnFood();
  }
  
  if (digitalRead(LEFT_BUTTON) == HIGH) {
    menuShown = true;
    inGame = false;
    currentGame = 0;
  }
}

void runFlappyBall() {
  if (flappyGameOver) {
    displayFlappyGameOver();
    return;
  }

  oled.firstPage();
  do {
    oled.setFont(u8g_font_profont12);
    oled.setPrintPos(0, 10);
    oled.print("Flappy Ball");
    oled.setPrintPos(0, 30);
    oled.print("Press UP to Flap");
    
    // Draw the ball
    oled.drawCircle(ballX, ballY, 3);
  } while (oled.nextPage());
  
  updateFlappyDirection();
  moveFlappyBall();
  checkFlappyCollision();
  delay(50);
}

void updateFlappyDirection() {
  if (digitalRead(UP_BUTTON) == HIGH) {
    velocity = flap; // Flap
  }
}

void moveFlappyBall() {
  velocity += gravity;
  ballY += velocity;
}

void checkFlappyCollision() {
  // Ball touches ground or goes off screen
  if (ballY >= 64 || ballY <= 0) {
    flappyGameOver = true;
  }
}

void displayFlappyGameOver() {
  oled.firstPage();
  do {
    oled.setFont(u8g_font_profont12);
    oled.setPrintPos(20, 30);
    oled.print("Game Over!");
  } while (oled.nextPage());
  
  // Reset Flappy Ball game or go back to menu based on button press
  if (digitalRead(RIGHT_BUTTON) == HIGH) {
    ballY = 32;
    velocity = 0;
    flappyGameOver = false;
  }
  
  if (digitalRead(LEFT_BUTTON) == HIGH) {
    menuShown = true;
    inGame = false;
    currentGame = 1;
  }
}
