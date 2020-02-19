//for easy writing, import module
#include <pitches.h>

//twinkle twinkle little star 
int music[] = {
  NOTE_C7, NOTE_C7, NOTE_G7, NOTE_G7, NOTE_A7, NOTE_A7, NOTE_G7, NOTE_F7, NOTE_F7, NOTE_E7, NOTE_E7,
  NOTE_D7, NOTE_D7, NOTE_C7, NOTE_G7, NOTE_G7, NOTE_F7, NOTE_F7, NOTE_E7, NOTE_E7, NOTE_D7, NOTE_G7,
  NOTE_G7, NOTE_F7, NOTE_F7, NOTE_E7, NOTE_E7, NOTE_D7, NOTE_C7, NOTE_C7, NOTE_G7, NOTE_G7, NOTE_A7,
  NOTE_A7, NOTE_G7, NOTE_F7, NOTE_F7, NOTE_E7, NOTE_E7, NOTE_D7, NOTE_D7, NOTE_C7
};

//When the respond is correcct, playing music
int corMusic[] = {NOTE_C7, NOTE_E7, NOTE_G7};

//LED pin number
int ledList[3] = {15, 0, 13};

//BUTTON pin number
int buttonList[3] = {2, 4, 5};

//BUZZER pin number
const int BUZZER = 14;

//correct answer
int answer[100];
//input of player
int respond[100];

//to check button, buttonSate is LOW == player push the button, HIGH == release the button
int buttonState = 0;
//play or end
int gameMode = 0;

//to check button
int state = 0;

void setup() {

  //setup you PINMODES here
  Serial.begin(19200);  // needed for serial debugging
  initializeGame();
  Serial.println("Simon Game!");

}// end of setup

void initializeGame() {
  // put code here that does any initialization - demonstrate somehow that the game is starting -perhaps flash a light?
  for (int i = 0; i < 3; i++) {
    pinMode(ledList[i], OUTPUT);
    pinMode(buttonList[i], INPUT_PULLUP);
  }
  pinMode(BUZZER, OUTPUT);
  randomSeed(analogRead(0));

  Serial.println("----------------------");
  Serial.println("Game Start!");
}

//play music by one scale
void gameSound(int times) {
  if (times > 0) {
    for (int i = 0; i < times; i++) {
      int temp = answer[i];
      tone(BUZZER, music[i], 300);
      digitalWrite(ledList[temp], HIGH);
      Serial.printf("%d play", i);
      delay(500);
      Serial.printf("answer is %d\n", temp);
      digitalWrite(ledList[temp], LOW);
      delay(500);
    }
  }
  int ranInt = random(3);
  Serial.println(ranInt);
  Serial.println(ledList[ranInt]);
  tone(BUZZER, music[times], 300);
  digitalWrite(ledList[ranInt], HIGH);
  answer[times] = ranInt;
  delay(500);
  Serial.printf("%d play ", times);
  Serial.printf("answer is %d\n", ranInt);
  digitalWrite(ledList[ranInt], LOW);
  delay(500);
}

//input of player
void pushMode(int times) {
  for (int j = 0; j < times + 1; j++) {
    //to check which button player push? red or blue or green
    for (int k = 0; k < 3; k++) {
      buttonState = digitalRead(buttonList[k]);
      if (buttonState == HIGH) {
        if (state == 0) {
          delay(10);
          state = 1;
        }
      }
      if (buttonState == LOW && buttonList[k] == 2) {
        if (state == 1) {
          tone(BUZZER, music[j], 300);
          digitalWrite(ledList[k], HIGH);
          Serial.println("pushed Red button");
          delay(500);
          digitalWrite(ledList[k], LOW);
          respond[j] = 0;
          tone(BUZZER, 0, 300);
        }
      }
      if (buttonState == LOW && buttonList[k] == 4) {
        if (state == 1) {
          tone(BUZZER, music[j], 300);
          digitalWrite(ledList[k], HIGH);
          Serial.println("pushed Green button");
          delay(500);
          digitalWrite(ledList[k], LOW);
          respond[j] = 1;
          tone(BUZZER, 0, 300);
        }
      }
      if (buttonState == LOW && buttonList[k] == 5) {
        if (state == 1) {
          tone(BUZZER, music[j], 300);
          digitalWrite(ledList[k], HIGH);
          Serial.println("pushed Blue button");
          delay(500);
          digitalWrite(ledList[k], LOW);
          respond[j] = 2;
          tone(BUZZER, 0, 300);
        }
      }
    }
    delay(1000);
  }
}

//when we push correct answer, the music is playing
void correct() {
  int sizeM = sizeof(corMusic) / sizeof(int);
  for(int i = 0; i< sizeM ; i++) {
    tone(BUZZER, corMusic[i], 300);
    digitalWrite(ledList[i], HIGH);
    delay(500);
    digitalWrite(ledList[i], LOW);
  }
}

/*to prevent loop
 * Only when player push correct button, the game is playing.
 * To restart, player have to push "reset"button.
 */
void game() {
  while (gameMode == 0) {
    int sizeM = sizeof(music) / sizeof(int);
    for (int i = 0; i < sizeM; i++) {
      gameSound(i);
      pushMode(i);
      if (answer[i] != respond[i]) {
            fail();
            gameMode = 1;
            break;
      }
      Serial.printf("answer : %d, respond : %d\n", answer[i], respond[i]);
      if(gameMode == 1) break;
      correct();
      delay(1000);
    }
  }
}

//When player push incorrect button, "beep" sound is playing
void fail() {
  tone(BUZZER, NOTE_C5, 2000);
  Serial.println("You missed answer");
  Serial.println("Game over");
  Serial.println("Please start again from the beginning.");
  Serial.println("***************************************");
}
void loop() {
  game();
  delay(5000);
}
