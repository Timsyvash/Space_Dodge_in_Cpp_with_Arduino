#include <Adafruit_NeoPixel.h>

const short matrix_pin = 5;
const short width = 8;
const short height = 8;
const short num_pixels = width * height;
const short pinX = A0;

// Координати гравця (гравець живе на нижньому рядку: Y = 7)
short playerX = 3;
const short playerY = 7;

// Координати перешкоди
short obstacleX = 0;
short obstacleY = 0;

Adafruit_NeoPixel matrix(num_pixels, matrix_pin, NEO_GRB + NEO_KHZ800);

unsigned long lastMoveTime = 0;
unsigned long lastInputTime = 0;
int gameSpeed = 250;  // Швидкість падіння блоків (мс)
int inputDelay = 150; // Затримка опитування джойстика (мс)
bool gameOver = false;

// Функція для переведення X та Y у лінійний індекс матриці
int getIndex(short x, short y)
{
  // Формула для стандартної матриці (зліва направо, згори вниз)
  return (y * width) + x;
}

void resetGame()
{
  playerX = 3;                  // Повертаємо гравця в центр
  obstacleX = random(0, width); // Нова випадкова колонка для перешкоди
  obstacleY = 0;                // Починає з верхнього рядка
  gameOver = false;
  matrix.clear();
}

void setup()
{
  randomSeed(analogRead(A1)); // Ініціалізація генератора випадкових чисел (вільний пін)
  matrix.begin();
  matrix.setBrightness(15); // 1 — занадто тьмяно, поставимо трохи яскравіше
  matrix.clear();
  matrix.show();
  resetGame();
}

void loop()
{
  // Ефект програшу (реалізований через прості delay, бо гра все одно зупинена)
  if (gameOver)
  {
    for (int i = 0; i < 3; i++)
    {
      matrix.fill(matrix.Color(150, 0, 0));
      matrix.show();
      delay(200);
      matrix.clear();
      matrix.show();
      delay(200);
    }
    resetGame();
  }

  // КЕРУВАННЯ ГРАВЦЕМ (Зчитування джойстика за таймером)
  if (millis() - lastInputTime > inputDelay)
  {
    short valX = analogRead(pinX);

    if (valX < 300 && playerX > 0)
    {
      playerX--;
      lastInputTime = millis();
    }
    else if (valX > 700 && playerX < width - 1)
    {
      playerX++;
      lastInputTime = millis();
    }
  }

  // РУХ ПЕРЕШКОДИ (Падіння згори вниз за таймером)
  if (millis() - lastMoveTime > gameSpeed)
  {
    lastMoveTime = millis();
    obstacleY++;

    // Якщо перешкода впала за межі екрана, створюємо нову згори
    if (obstacleY >= height)
    {
      obstacleY = 0;
      obstacleX = random(0, width);
    }

    // ПЕРЕВІРКА ЗІТКНЕННЯ (Якщо координати збіглися)
    if (obstacleX == playerX && obstacleY == playerY)
    {
      gameOver = true;
    }
  }

  // ОНОВЛЕННЯ ЕКРАНА
  matrix.clear();

  // Малюємо гравця (Зелений) на нижньому рядку
  matrix.setPixelColor(getIndex(playerX, playerY), matrix.Color(0, 255, 0));

  // Малюємо перешкоду (Червоний)
  matrix.setPixelColor(getIndex(obstacleX, obstacleY), matrix.Color(255, 0, 0));

  matrix.show();
}
