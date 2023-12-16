#include <FastLED.h> // works with v3.6

#define LED_PIN     12 // D6
#define NUM_LEDS    256
#define BRIGHTNESS  5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define NUM_ROWS 16
#define NUM_COLS 16

#define UPDATES_PER_SECOND 50

#define UP 13     // D7
#define DOWN 5    // D1
#define LEFT 4    // D2
#define RIGHT 0   // D3
#define FIRE 14   // D5
#define NONE -1

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

struct point 
{
    int x;
    int y;
};
typedef struct point Point;

int direction = NONE;
int nextdirection = NONE;

int snakelen;
Point snake[256]; 

Point food;
int foodeaten;

int mode=0;
int speed = 6;

int speedmapping[10] = {600,500,400,300,200,150,100,80,60,40};

// just the numbers 0 to 9
int font[10][5] = {
      0x3E,0x41,0x41,0x41,0x3E,    
      0x0,0x42,0x7F,0x40,0x0,         
      0x42,0x61,0x51,0x49,0x46,            
      0x22,0x41,0x49,0x49,0x36,            
      0x18,0x14,0x12,0x7F,0x10,            
      0x27,0x45,0x45,0x45,0x39,            
      0x3E,0x49,0x49,0x49,0x32,            
      0x1,0x1,0x79,0x5,0x3,            
      0x36,0x49,0x49,0x49,0x36,
      0x26,0x49,0x49,0x49,0x3E            
};

void print(int number, int posx, int posy, CRGB color)
{
  for(int x = 0; x<5; x++)
  {
    byte v = font[number][x];

    if ((v & 1) > 0)
      Set(posy+6, posx+x, color);    
    if ((v & 2) > 0)
      Set(posy+5, posx+x, color);    
    if ((v & 4) > 0)
      Set(posy+4, posx+x, color);    
    if ((v & 8) > 0)
      Set(posy+3, posx+x, color);    
    if ((v & 16) > 0)
      Set(posy+2, posx+x, color);    
    if ((v & 32) > 0)
      Set(posy+1, posx+x, color);    
    if ((v & 64) > 0)
      Set(posy+0, posx+x, color);        
  }
}

void setup() {
    randomSeed(analogRead(0));
  
    delay( 2000 ); // power-up safety delay

    pinMode(RIGHT, INPUT_PULLUP);
    pinMode(LEFT, INPUT_PULLUP);
    pinMode(FIRE, INPUT_PULLUP);
    pinMode(DOWN, INPUT_PULLUP);
    pinMode(UP, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(RIGHT), joystickright, FALLING);
    attachInterrupt(digitalPinToInterrupt(LEFT), joystickleft, FALLING);
    attachInterrupt(digitalPinToInterrupt(UP), joystickup, FALLING);
    attachInterrupt(digitalPinToInterrupt(DOWN), joystickdown, FALLING);
    attachInterrupt(digitalPinToInterrupt(FIRE), joystickfire, FALLING);

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

ICACHE_RAM_ATTR void joystickfire()
{
  nextdirection = FIRE;
}
ICACHE_RAM_ATTR void joystickright()
{
  if (direction != LEFT)
    nextdirection = RIGHT;
}
ICACHE_RAM_ATTR void joystickleft()
{
  if (direction != RIGHT)
    nextdirection = LEFT;
}
ICACHE_RAM_ATTR void joystickup()
{
  if (direction != DOWN)
    nextdirection = UP;
}
ICACHE_RAM_ATTR void joystickdown()
{
  if (direction != UP)
    nextdirection = DOWN;
}

void startGame()
{  
    Clear();

    direction = RIGHT;
    nextdirection = RIGHT;
    foodeaten = 0;
    
    snakelen = 5;
    for (int i = 0; i< snakelen; i++)
    {
        snake[i].x = 7; 
        snake[i].y = 7-i;
    } 

    PlaceFood();    

    mode = 1;
}

void gameOver(CRGB color)
{
    Flicker(color);

    Clear();
    print(foodeaten/10,3,4,CRGB::Green);
    print(foodeaten%10,9,4,CRGB::Green);

    FastLED.show();

    mode = 2;    
}

void loop() {
  if (mode == 0) loopPreGame();
  if (mode == 1) loopGame();
  if (mode == 2) loopPostGame();
}

void loopPostGame() {
  if (nextdirection == FIRE)
  {
    Clear(); 
    mode = 0;
  }

  nextdirection = NONE;
  delay(300);  
}

void loopPreGame() {

  direction = NONE;

  if (nextdirection == DOWN && speed > 0)
  {
    speed--;
  }
  if (nextdirection == UP && speed < 9)
  {
    speed++;
  }
  if (nextdirection == FIRE)
  {
    Clear(); 
    startGame();
    return;
  }

  nextdirection = NONE;

  Clear();

  int x = (NUM_COLS - 5) / 2;
  int y = (NUM_ROWS - 7) / 2;
  print(speed,x,y,CRGB::Yellow);


  // update
  FastLED.show();
  delay(300);  
}

void loopGame() {

  // Clear
  for(int i = 0; i < snakelen; i++)
    Set(snake[i], CRGB::Black);
  Set(food, CRGB::Black);

  if (nextdirection != FIRE)
    direction = nextdirection;

  // calculate moved head
  Point head = Move(snake[0], direction);

  // touching myself?
  if (direction != NONE)
  {
    for(int i = 0; i < snakelen-1; i++)    
    {
      if (head.x == snake[i].x && head.y == snake[i].y)
        {
          gameOver(CRGB::FireBrick);
          return;
        }
    }
  }
  
  // Is food eaten?
  if (head.x == food.x && head.y == food.y)
  {
    foodeaten++;
    snakelen++;

    PlaceFood();
  }

  // Move Snake
  if (direction != NONE)
  {      
    for(int i = snakelen-1; i > 0; i--)
      snake[i] = snake[i-1];
    snake[0] = head;
  }

  // Draw Snake
  Set(snake[0], CRGB::Blue);
  for(int i = 1; i < snakelen; i++)
    Set(snake[i], CRGB::White);
  // Draw Food
  Set(food, CRGB::Green);

  // update
  FastLED.show();
  delay(speedmapping[speed]);
}

Point Move(Point current, int direction)
{
  int x = current.x;
  int y = current.y;
  
  if (direction == UP)
  {
    x = min(x+1, NUM_ROWS-1);
  }
  if (direction == DOWN)
  {
    x = max(x-1, 0);
  }
  if (direction == RIGHT)
  {
    y = min(y+1, NUM_COLS-1);
  }
  if (direction == LEFT)
  {
    y = max(y-1, 0);
  }

  Point next;
  next.x = x;
  next.y =y;
  return next;
}

void Set(Point p, CRGB color)
{
  Set(p.x, p.y, color);
}

void Set(int row, int col, CRGB color)
{
  int rowStart = row * NUM_COLS;

  int add;
  if (row % 2 == 0)
    add = col;
  else
    add = NUM_COLS - 1 - col;
  
  leds[ rowStart + add ] = color;
}

void PlaceFood()
{
  startPlaceFood:
  Point f = randomPoint();

  if (f.x == food.x && f.y == food.y)
    goto startPlaceFood;

  for(int i = 0; i<snakelen; i++)
  {
    if (f.x == snake[i].x && f.y == snake[i].y)
      goto startPlaceFood;
  }

  food = f;
}

void Clear()
{
  Full(CRGB::Black);     
} 

void Full(CRGB color)
{
      int c = NUM_ROWS * NUM_COLS;
       
      for(int i = 0; i<c;  i++){
        leds[i] = color;
      }
} 

void Flicker(CRGB color)
{
        Full(color);
        FastLED.show();
        delay(50);
        Clear();
        FastLED.show();
        delay(50);
        Full(color);
        FastLED.show();
        delay(50);
        Clear();
        FastLED.show();
        delay(50);
        Full(color);
        FastLED.show();
        delay(50);
        Clear();       
        FastLED.show(); 
}

Point randomPoint()
{
  Point p;
  p.x = random(0, NUM_ROWS-1);
  p.y = random(0, NUM_COLS-1);
  return p;
}
