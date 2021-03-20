#include "FastLED.h"
#define NUM_LEDS 60
#define LED_PIN 13
CRGB leds[NUM_LEDS];
int refreshSpeed = 5;

char P1Up=3, P1Down=4, P1Left=5, P1Right=6, P1Fire=2, P2Up=8, P2Down=9, P2Left=10, P2Right=11, P2Fire=7;

byte DefaultPlayerColors[4] = { 0, 50,   120,   160 };
byte P1Colors[4] = { 0, 50,   120,   160 };
byte P2Colors[4] = { 0, 50,   120,   160 };
             // ROT, GELB, GRUEN, BLAU



int startSpeed = 50;
int maxSpeed = 10;

int nearNeeded = 5;


int powerUpSpeed = startSpeed/5;
int powerUpNeeded = 3;
int playerSaturataion = 196;

int ballPosition = NUM_LEDS/2;
bool directionLeft = true;

int P1Length = 2;
int P2Length = 2;
int P1Color = 1;
int P2Color = 2;

int P1God = 0;
int P2God = 0;

int P1ColorFlip = 0;
int P2ColorFlip = 0;

const int PowerUpCount = 8;
int PowerUpColors[PowerUpCount] = {128,224,160,0,64,96,48,196};
int P1PowerUp[PowerUpCount] = {0,0,0,0,0,0,0,0};
int P2PowerUp[PowerUpCount] = {0,0,0,0,0,0,0,0};
bool P1CanPowerUp = true;
bool P2CanPowerUp = true;

bool P1HasPowerUp = false;
bool P2HasPowerUp = false;

int ballSpeed = startSpeed;
int ballColor = 0;

unsigned long oldBallMillis = 0;
unsigned long oldMillis = 0;

int losePoint = 20;


void setup(void)
{
  randomSeed(analogRead(0));
  ballColor = random(0,4);
  directionLeft = ballColor % 2;
  
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(P1Up, INPUT);
  pinMode(P1Down, INPUT);
  pinMode(P1Left, INPUT);
  pinMode(P1Right, INPUT);
  pinMode(P1Fire, INPUT);

  digitalWrite(P1Up, HIGH);
  digitalWrite(P1Down, HIGH);
  digitalWrite(P1Left, HIGH);
  digitalWrite(P1Right, HIGH);
  digitalWrite(P1Fire, HIGH);
  
  pinMode(P2Up, INPUT);
  pinMode(P2Down, INPUT);
  pinMode(P2Left, INPUT);
  pinMode(P2Right, INPUT);
  pinMode(P2Fire, INPUT);

  digitalWrite(P2Up, HIGH);
  digitalWrite(P2Down, HIGH);
  digitalWrite(P2Left, HIGH);
  digitalWrite(P2Right, HIGH);
  digitalWrite(P2Fire, HIGH);

  Serial.begin(9600); 
  Serial.println("a new dawn!");
  
  LEDS.setBrightness(32);
  leds[0] = CHSV(0,0,255);
  leds[NUM_LEDS-1] = CHSV(0,0,255);
  
  
  resetGame();
 
}

void loop(void)
{
  
  if(P1Length >= losePoint){
    endAnimation(1);
  }
  
  if(P2Length >= losePoint){
    endAnimation(2);
  }
  
  int P1State = !digitalRead(P1Fire) << 4 | !digitalRead(P1Up) << 3 | !digitalRead(P1Down) << 2 | !digitalRead(P1Left) << 1 | !digitalRead(P1Right);
  int P2State = !digitalRead(P2Fire) << 4 | !digitalRead(P2Up) << 3 | !digitalRead(P2Down) << 2 | !digitalRead(P2Left) << 1 | !digitalRead(P2Right);

  if(P1State & 16){
  /*
   Serial.print("Player1 FIRED PowerUp!!!");
   for(int i = 0; i<PowerUpCount;i++){
     Serial.print(P1PowerUp[i]);
   }
   Serial.println("");
   */
       
    // COLOR FLIP
    if(PlayerHasPowerUp(1,7)){
        shuffleColors(2);
        P1ColorFlip = 3;
        cleanPowerUps(1);
        //Serial.println("P1 fired COLOR FLIP!!");
    }
    
    // GODLIKE
    if(PlayerHasPowerUp(1,6)){
      Serial.println("P1 fired GODMODE!!");
      P1God = 3;
      cleanPowerUps(1);
    }
    // SPEEDBALL
    else if(PlayerHasPowerUp(1,5)){
      //Serial.println("P1 fired SPEEDBALL!");
      ballSpeed = powerUpSpeed;
      cleanPowerUps(1);
     
    }

    // DIRECTION CHANGE   
    else if(PlayerHasPowerUp(1,4)){
      //Serial.println("P1 fired DIRECTION CHANGE!");
        directionLeft = !directionLeft;
        changeBallColorPowerUp(1);
        cleanPowerUps(1);
    }
    // BALL COLOR
    else if(PlayerHasPowerUp(1,3)){
      //Serial.println("P1 fired BALLCOLOR!");
      changeBallColorPowerUp(1);
      cleanPowerUps(1);
    }
  }
  
  if(P2State & 16){
    
  /*
   Serial.print("Player2 FIRED PowerUp!!!");
   for(int i = 0; i<PowerUpCount;i++){
     Serial.print(P2PowerUp[i]);
   }
   Serial.println("");
   */
    // COLOR FLIP
    if(PlayerHasPowerUp(2,7)){
        shuffleColors(1);
        P2ColorFlip = 3;
        cleanPowerUps(2);
        //Serial.println("P2 fired COLOR FLIP!!");
    }
    
    // GODLIKE
    else if(PlayerHasPowerUp(2,6)){
        P2God = 3;
        cleanPowerUps(2);
        //Serial.println("P2 fired GODMODE!!");
    }
    // SPEEDBALL
    else if(PlayerHasPowerUp(2,5)){
        ballSpeed = powerUpSpeed;
        cleanPowerUps(2);
        //Serial.println("P2 fired SPEEDBALL!");
    }
    // DIRECTION CHANGE   
    else if(PlayerHasPowerUp(2,4)){
        directionLeft = !directionLeft;
        changeBallColorPowerUp(2);
        cleanPowerUps(2);
        
        //Serial.println("P2 fired DIRECTION CHANGE!");
    }
    // BALL COLOR
    else if(PlayerHasPowerUp(2,3)){
        changeBallColorPowerUp(2);
        cleanPowerUps(2);
        //Serial.println("P2 fired BALLCOLOR!");
    }
  }

  
  

  if(P1State != 0 && P1State < 16){
    P1Color = getColorFromState(P1State);
    if((ballPosition <= NUM_LEDS/2) && !directionLeft && (ballColor == P1Color) && P1CanPowerUp && !P1HasPowerUp){
      int near = ballPosition - P1Length;
         P1CanPowerUp = false;
         if(near <= PowerUpCount && near <= nearNeeded){
           PlayerSetPowerup(1,near);
         }        
         /*
        Serial.print("Player1 Powerups:");
        for(int i = 0; i<PowerUpCount;i++){
          Serial.print(P1PowerUp[i]);
        }
        Serial.println("");
        */
    }
  }
  
  if(P2State != 0 && P2State < 16){
    P2Color = getColorFromState(P2State);
    if((ballPosition >= NUM_LEDS/2) && directionLeft && (ballColor == P2Color) && P2CanPowerUp && !P2HasPowerUp){
      int near = NUM_LEDS - P2Length - ballPosition;
       P2CanPowerUp = false;
       if(near <= PowerUpCount && near <= nearNeeded){
         PlayerSetPowerup(2,near);
       }  
       /*
       Serial.print("Player2 Powerups:");
       for(int i = 0; i<PowerUpCount;i++){
         Serial.print(P2PowerUp[i]);
       }
       Serial.println("");
       */

    }
  }
  setPlayerPositions();
  setBallPosition();
  refreshScreen();

}

void startAnimation(){
  party(10);
  clearScreen();
}

void endAnimation(int player){
  int winColor = 0;
  
  for(int k = 255; k>8; k--){
    if(player == 1){
      winColor = P2Color;
      for(int i = 0; i < P1Length; i++){
        leds[i] = CHSV(P2Colors[winColor],playerSaturataion,k);
      }
    }
    else{
       winColor = P1Color;
       for(int i = NUM_LEDS-P2Length; i <= NUM_LEDS; i++){
        leds[i] = CHSV(P1Colors[winColor],playerSaturataion,k);
      }
    }
    delay(5);
    FastLED.show();
  }
  if(player == 2){
    for(int i = 0; i <NUM_LEDS; i++){
      leds[i] = CHSV(P2Colors[winColor],playerSaturataion,255);
      delay(10);
      FastLED.show();
    }
  }
  else{
    for(int i = NUM_LEDS-1; i>=0; i--){
      leds[i] = CHSV(P1Colors[winColor],playerSaturataion,255);
      delay(10);
      FastLED.show();
    }
  }
  delay(2000);
  resetGame();
  
}
  
void cleanPowerUps(int player){
  if(player == 1){
   P1CanPowerUp = true;
   for(int p = 0; p<PowerUpCount; p++){
       P1PowerUp[p] = 0;
   }
   leds[0] = CHSV(0,0,255);
   P1HasPowerUp = false;
  }
  else{
   P2CanPowerUp = true;
   for(int p = 0; p<PowerUpCount; p++){
      P2PowerUp[p] = 0;
   }
   leds[NUM_LEDS-1] = CHSV(0,0,255);
   P2HasPowerUp = false;
  }
  
  

}

void resetGame(){
  startSpeed = 50;
  while(digitalRead(P1Fire) && digitalRead(P2Fire)){
    party(1);
  }
  
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CHSV(0,0,0);
  }
  
  while(digitalRead(P1Fire) == 0 || digitalRead(P2Fire) == 0){
    
  }
  printSpeedSettings(startSpeed);
  int c = 0;
  while(digitalRead(P1Fire) == 1 || digitalRead(P2Fire) == 1){
    if(!digitalRead(P1Up) || !digitalRead(P2Up)){
        if(startSpeed < NUM_LEDS)
          startSpeed++;
        delay(50);
        printSpeedSettings(startSpeed);
    }
    if(!digitalRead(P1Down) || !digitalRead(P2Down)){
        if(startSpeed > 5)
          startSpeed--;
        delay(50);
        printSpeedSettings(startSpeed);
      
    }
  }
  powerUpSpeed = startSpeed/5;
  ampel();
  
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CHSV(0,0,0);
  }
  
  P1Length = 2;
  P2Length = 2;
  P1Color = 1;
  P2Color = 2;
  P1HasPowerUp = false;
  P2HasPowerUp = false;
  
  
  for(int p = 0; p<PowerUpCount; p++){
      P1PowerUp[p] = 0;
      P2PowerUp[p] = 0;
  }
  
  leds[0] = CHSV(0,0,255);
  leds[NUM_LEDS-1] = CHSV(0,0,255);
  
  ballSpeed = startSpeed;
  ballColor = 0;

  ResetDefaultPlayerColors(1);
  ResetDefaultPlayerColors(2);
  
  setPlayerPositions();
  
  setBallPosition();
  refreshScreen();
  
  delay(1000);

}

void setPlayerPositions(){
  for(int i = 1; i < P1Length; i++){
    if(P1God <= 0){
      leds[i] = CHSV(P1Colors[P1Color],playerSaturataion,255);
    }
    else{
      leds[i] = CHSV(P1Colors[P1Color],0,255);
    }
  }
  for(int i = NUM_LEDS-P2Length; i <= NUM_LEDS-2; i++){
    if(P2God <= 0){
      leds[i] = CHSV(P2Colors[P2Color],playerSaturataion,255);
    }
    else{
      leds[i] = CHSV(P2Colors[P2Color],0,255);
    }
  }
}

void setBallPosition(){
  if(millis() > (oldBallMillis + ballSpeed)){
    oldBallMillis = millis();

  if(directionLeft){
    if(ballPosition == (NUM_LEDS/2-1))
      changeBallColor();
    ballPosition++;
        
    if(ballPosition >= NUM_LEDS - P2Length-1){ // PLAYER2 Bounce
    P1CanPowerUp = true;
      if(ballSpeed > maxSpeed)
        ballSpeed -=2;
      if(ballColor != P2Color){
        for(int p = 0; p<PowerUpCount; p++){
          if(P2PowerUp[p]<powerUpNeeded)
            P2PowerUp[p] = 0;
        }
        if(!(P2God-- > 0)){
          setPlayerPositions();
          P2Length++;
          ballSpeed = startSpeed;
          P2God = 0;
        }
        if(!(P2ColorFlip-- > 0)){
          ResetDefaultPlayerColors(2);
          P2ColorFlip = 0;
        }
          
      }

      directionLeft = false;
    }
    leds[ballPosition-1] = CRGB::Black;
  }
  else{ 
    if(ballPosition == NUM_LEDS/2)
      changeBallColor();
    ballPosition--;
    
    if(ballPosition <= P1Length){ // PLAYER1 Bounce
      P2CanPowerUp = true;
      if(ballSpeed > maxSpeed)
        ballSpeed -= 2;
      
      if(ballColor != P1Color){
        for(int p = 0; p<PowerUpCount; p++){
          if(P1PowerUp[p]<powerUpNeeded)
            P1PowerUp[p] = 0;
        }
        if(!(P1God-- > 0)){
          setPlayerPositions();
          P1Length++;
          ballSpeed = startSpeed;
          P1God = 0;
        }
        if(!(P1ColorFlip-- > 0)){
          ResetDefaultPlayerColors(1);
          P1ColorFlip = 0;
        }
      }
      directionLeft = true;
    }
    
    leds[ballPosition+1] = CRGB::Black;
  }
  leds[ballPosition] = CHSV(P1Colors[ballColor],playerSaturataion,255); 
  }
}

void changeBallColor(){
    long rnd = random(0, 4);
    if(directionLeft){
      while(rnd == P2Color){
        rnd = random(0, 4);
      }
    }
    else{
      while(rnd == P1Color){
        rnd = random(0, 4);
      }
    }
    ballColor = rnd;
}

void changeBallColorPowerUp(int player){
    long rnd = random(0, 4);
    if(player == 1){
      while(rnd == P2Color){
        rnd = random(0, 4);
      }
    }
    else{
      while(rnd == P1Color){
        rnd = random(0, 4);
      }
    }
    ballColor = rnd;
}

int getColorFromState(int state){
  int result = 0;
  switch(state){
    case 1:
      //Serial.println("R"); 
      result = 0;
      break;
    case 2:
      //Serial.println("L"); 
      result = 1;
      break;
    case 4:
      //Serial.println("D"); 
      result = 2;
      break;
    case 8:
      //Serial.println("U"); 
      result = 3;
      break;
    case 10:
      //Serial.println("UL");
      break;
    case 9:
      //Serial.println("UR");
      break;
    case 5:
      //Serial.println("DL");
      break;
    case 6:
      //Serial.println("DR");
      break;
    default:
      break;
  }
  return result;
}

void refreshScreen(){
  
  if(millis() > (oldMillis + refreshSpeed)){
    oldMillis = millis();
    FastLED.show();
  }
  
}

void clearScreen(){
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}


void ampel(){
  int ampelFarben[6] = {0, 48, 96, 96, 48, 0};
  int ampelValue[6] = {255, 255, 255, 255, 255, 255};
  
  for(int p = 0; p<3;p++){
    if(p==1){
        ampelValue[0] = 0;
        ampelValue[5] = 0;
    }
    if(p==2){
        ampelValue[1] = 0;
        ampelValue[4] = 0;
    }
    int k = 0;
    for(int i=1;i<NUM_LEDS+1;i++){
      leds[i-1] = CHSV(ampelFarben[k],255,ampelValue[k]);
      if(i % 10 == 0 && k<5){
        k++;
      }
    }
    FastLED.show();
    delay(1000);
  }
}
void party(uint8_t wait) 
{
	uint16_t hue;
	FastLED.clear();

	for(hue=0; hue<255; hue++) 
	{       
		fill_rainbow( &(leds[0]), NUM_LEDS , hue);		
		FastLED.show();
		delay(wait);
	}
        clearScreen();
	return;
}

void printSpeedSettings(int newSpeed){
  clearScreen();
  if(newSpeed > 60)
    newSpeed = 60;
  for(int i=0;i<newSpeed;i++){
    leds[i] = CHSV(map(i,0,60,0,96),255,255);
  }
  FastLED.show();
}

void shuffleColors(int player){
  if(player == 1){
    for (int a=0; a<4; a++)
    {
      int r = random(a,3);
      int temp = P1Colors[a];
      P1Colors[a] = P1Colors[r];
      P1Colors[r] = temp;
    }
  }
  else{
   for (int a=0; a<4; a++)
    {
      int r = random(a,3);
      int temp = P2Colors[a];
      P2Colors[a] = P2Colors[r];
      P2Colors[r] = temp;
    }
  }
}

void ResetDefaultPlayerColors(int player){
  for(int i = 0; i<4; i++){
    if(player == 1)
      P1Colors[i] = DefaultPlayerColors[i];
    else
      P2Colors[i] = DefaultPlayerColors[i];
  }
}

bool PlayerHasPowerUp(int player, int powerup){
  if(player == 1){
    if(P1PowerUp[powerup] >= powerUpNeeded)
      return true;
  }
  else{
     if(P2PowerUp[powerup] >= powerUpNeeded)
      return true;
  }
  return false;
}

void PlayerSetPowerup(int player,int near){
  if(near < 1)
    near = 1;
  if(player == 1){
     if(++P1PowerUp[PowerUpCount - near] >= powerUpNeeded){
       leds[0] = CHSV(PowerUpColors[near - 1],196,255);
       P1HasPowerUp = true;
     }
  }
  else{
     if(++P2PowerUp[PowerUpCount - near] >= powerUpNeeded){
       leds[NUM_LEDS-1] = CHSV(PowerUpColors[near - 1],196,255);
       P2HasPowerUp = true;
     }
  }
}
