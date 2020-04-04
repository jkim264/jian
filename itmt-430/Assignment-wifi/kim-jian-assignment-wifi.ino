//for easy writing, import module
#include <pitches.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "SO070VOIP6271"
#define STAPSK  "EEB7006270"
#endif

int ledTone[4] = {NOTE_E6, NOTE_G7, NOTE_G6, NOTE_C6};

//LED pin number
int ledList[4] = {0, 15, 12, 16};

//BUTTON pin number
int buttonList[4] = {4, 2, 5, 3};

//BUZZER pin number
const int BUZZER = 14;

//correct answer
int problem[100];
//input of player
int answer[100];

//to check button, buttonSate is LOW == player push the button, HIGH == release the button
int buttonState = 0;

//to check button
int state = 0;

//to find "N"th play
int count = 0;

//to control game
int gamemode = 2;


const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

void handleRoot() {
  char temp[800];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 800,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>SIMON GAME PAGE</title>\
    <style>\
      body { background-color: #FFFFFF; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; text-align:center; }\
    </style>\
  </head>\
  <body>\
    <h1>This is SIMON GAME!</h1>\
    <p>If you want to \start, \press the yellow button</p>\
    <h2>%d th play!</h2>\
    <p>Play Time: %02d:%02d:%02d </p>\
    <img src=\"/color.svg\" />\
    <p>If you lose and want to restart, press the reset button</p>\
  </body>\
</html>",

           count, hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}


//when we pushed button, then svg image color is changed
void draw() {
  String out;
  out.reserve(2600);
  int btnState[4] = {1, 1, 1, 1};
  btnState[0] = digitalRead(buttonList[0]);
  btnState[1] = digitalRead(buttonList[1]);
  btnState[2] = digitalRead(buttonList[2]);
  btnState[3] = digitalRead(buttonList[3]);
  //to check which button player push? red or blue or green or yellow
  if (btnState[0] == LOW) {
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"100\" height=\"100\">\n";
    out += "<circle cx=\"50\" cy=\"50\" r = \"50\" stroke = \"rgb(250, 0, 0)\" fill=\"rgb(250, 0, 0)\" />\n";
  }
  else if (btnState[1] == LOW) {
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"100\" height=\"100\">\n";
    out += "<circle cx=\"50\" cy=\"50\" r = \"50\" stroke = \"rgb(0, 250, 0)\" fill=\"rgb(0, 250, 0)\" />\n";
  }
  else if (btnState[2] == LOW) {
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"100\" height=\"100\">\n";
    out += "<circle cx=\"50\" cy=\"50\" r = \"50\" stroke = \"rgb(0, 0, 250)\" fill=\"rgb(0, 0, 250)\" />\n";
  }
  else if (btnState[3] == LOW) {
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"100\" height=\"100\">\n";
    out += "<circle cx=\"50\" cy=\"50\" r = \"50\" stroke = \"rgb(250, 250, 0)\" fill=\"rgb(250, 250, 0)\" />\n";
  }
  else {
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"100\" height=\"100\">\n";
    out += "<circle cx=\"50\" cy=\"50\" r = \"50\" stroke = \"rgb(203, 212, 217)\" fill=\"rgb(203, 212, 217)\" />\n";

  }
  out += "</svg>";
  server.send(200, "image/svg+xml", out);
}

void setup() {
  Serial.begin(19200);  // needed for serial debugging
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("SIMON GAME")) {
    Serial.println("SIMON GAME started");
  }

  server.on("/", handleRoot);
  server.on("/color.svg", draw);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  //setup you PINMODES here
  initializeGame();
  Serial.println("Simon Game!");

}// end of setup

//control pinmode
void initializeGame() {
  pinMode(BUZZER, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(ledList[i], OUTPUT);
    pinMode(buttonList[i], INPUT_PULLUP);
  }
  randomSeed(analogRead(0));
}


void loop() {
  server.handleClient();
  MDNS.update();
  if (gamemode == 2)doFirst();
  while (gamemode == 0) game();
  if (gamemode == 1) {
    fail();
    gamemode = 3;
  }
}

//before start game, play tone two times
void doFirst() {
  gameSetUp();
  for (int n = 0; n < 4; n++) {
    digitalWrite(ledList[n], HIGH);
    digitalWrite(ledList[n], LOW);
  }
  for (int j = 0; j < 3; j++) {
    for (int n = 0; n < 4; n++) {
      digitalWrite(ledList[n], HIGH);
      delay(100);
      ligSound(n, 200);
      digitalWrite(ledList[n], LOW);
    }
  }
  for (int n = 0; n < 4; n++) {
    digitalWrite(ledList[n], HIGH);
  }
  delay(300);
  for (int n = 0; n < 4; n++) {
    digitalWrite(ledList[n], LOW);
  }
  Serial.println("----------------------");
  Serial.println("Game Start!");
}

//To start game, player have to press a button
void gameSetUp() {
  Serial.println("Do you want to play Simon Game?");
  Serial.println("To start game, press the yellow button");
  while (gamemode == 2) {
    server.handleClient();
    MDNS.update();
    int temp = 0;
    delay(10);
    temp = digitalRead(buttonList[3]);
    if (temp == LOW) {
      state = 1;
    }
    if(state == 1) {
      gamemode = 0;
    }
  }
  state = 0;
}

//When we pushed button, the tone is playing
void ligSound(int n, int second) {
  tone(BUZZER, ledTone[n], second);
  digitalWrite(ledList[n], HIGH);
  delay(second);
  digitalWrite(ledList[n], LOW);
  tone(BUZZER, 0, 500);
}

void game() {
  delay(100);
  Serial.printf("%dth play\n", count + 1);
  if (count > 0) {
    for (int i = 0; i < count; i++) {
      int temp = problem[i];
      ligSound(temp, 300);
      delay(800);
    }
  }
  int n = random(4);
  ligSound(n, 300);
  delay(800);
  problem[count] = n;
  server.handleClient();
  MDNS.update();
  pushMode(count);
  if (problem[count] != answer[count]) {
    gamemode = 1;
  }
  count++;
}

//input of player
void pushMode(int times) {
  for (int j = 0; j < times + 1; j++) {
    //to check which button player push? red or blue or green
    for (int k = 0; k < 4; k++) {
      buttonState = digitalRead(buttonList[k]);
      if (buttonState == HIGH) {
        if (state == 0) {
          delay(10);
          state = 1;
        }
      }
      else if (buttonState == LOW && buttonList[k] == 4) {
        if (state == 1) {
          ligSound(k, 300);
          answer[j] = k;
          delay(300);
        }
      }
      else if (buttonState == LOW && buttonList[k] == 2) {
        if (state == 1) {
          ligSound(k, 300);
          answer[j] = k;
          delay(300);
        }
      }
      else if (buttonState == LOW && buttonList[k] == 5) {
        if (state == 1) {
          ligSound(k, 300);
          answer[j] = k;
          delay(300);
        }
      }
      else if (buttonState == LOW && buttonList[k] == 3) {
        if (state == 1) {
          ligSound(k, 300);
          answer[j] = k;
          delay(300);
        }
      }
      else answer[j] = 9;
      server.handleClient();
      MDNS.update();
    }
    if (answer[j] != problem[j]) {
      gamemode = 1;
      break;
    }
    delay(500);
  }
}

//When player push incorrect button, "beep" sound is playing
void fail() {
  tone(BUZZER, NOTE_C5, 2000);
  Serial.println("You missed answer");
  Serial.println("Game over");
  Serial.println("To play again, press the reset button.");
  Serial.println("***************************************");
}
