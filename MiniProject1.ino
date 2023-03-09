//mgProject 1 - Guitar Hero
//Program outputs and displays notes falling down the LED Matrix
//Takes in sonar readings as user input to determine if notes are played
//Plays a frequency and flashes the note as green if successful
//Increases level and speed for every 10 points earned
//Karan Patel and Karen Hughes

const int dataPin  = 2;       //    DS - data serial
const int latchPin = 3;       // ST_CP - storage register, latch clock pin
const int clockPin = 4;       // SH_CP - shift register clock pin
const int buzzerPin = 5;      
const int trigPin = 12;
const int echoPin = 11;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin,  OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  pinMode(buzzerPin, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  Serial.begin(9600);  
}

int buttonState1 = 0;
int buttonState1_prev = 0;
int buttonState1_cnt = 0;
int buttonState2 = 0;
int buttonState2_prev = 0;
int buttonState2_cnt = 0;
int buttonState3 = 0;
int buttonState3_prev = 0;
int buttonState3_cnt = 0;

int score = 0;
int prev_score = 0;
int reduce = 0;
int level = 1;
int cycle = 0;
int keys[] = {0B00111111, 0B11100111, 0B11111100};
int GameOver[] = {0B01010101, 
                  0B10101010,
                  0B01010101, 
                  0B10101010,
                  0B01010101, 
                  0B10101010,
                  0B01010101, 
                  0B10101010};
                  
int levels[] = {0B01010101, 
                0B10101010,
                0B01010101, 
                0B10101010,
                0B01010101, 
                0B10101010,
                0B01010101, 
                0B10101010};

//Old to Joy by Beethoven               
char notes[] = "eefggfedccdeeddeefggfedccdedccddecdefecdefedcdgeefggfedccdedcc";

//Plays the proper frequency for each note
void playTone(int frequency, int duration) {
  for (long i = 0; i < duration*1000L; i += frequency*2) {
      digitalWrite(buzzerPin, HIGH);
      delayMicroseconds(frequency);
      digitalWrite(buzzerPin, LOW);
      delayMicroseconds(frequency);      
  }  
}

//Plays notes corresponding to note sheet
void playNote(char note, int duration) {
  char noteName[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
  int frequency[] = {1136, 1014, 1915, 1700, 1519, 1432, 1275};

  for(int i = 0; i < 7; i++) {
    if (noteName[i] == note) {
      playTone(frequency[i], duration);  
    } 
  }
}

//Converts sonar readings into distance value
int SonarReading(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  return distance;
}

void loop(){
  int row = 0B10000000;
  int new_row = row;
  int red_key = keys[rand()%3];
  int buttons = 0B11111111;
  cycle++;

  //A level is gained and speed of notes increase for every 10 points in score
  if (score % 10 == 0 && score != prev_score) {
    prev_score = score;
    reduce += 5;
    level += 1;
    Serial.print("Level up: ");
    Serial.print(level);
    Serial.print("\n");

    //Flashes to indicate a new level is reached
    for(int j = 0; j < 100; j++){
      row = 0B10000000;
      for(int i = 0; i < 8; i++){
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, levels[i]); //Col-green
        shiftOut(dataPin, clockPin, LSBFIRST, levels[(i+1)%8]); //Col-red
        shiftOut(dataPin, clockPin, LSBFIRST, row>>i); //Row
        digitalWrite(latchPin, HIGH);
      }
    }
  }

  //The game is over when 10 consecutive notes are missed
  //The screen flashes green and red repeatedly
  if(cycle - 10 > score){
    for(int j = 0; j < 100; j++){
      row = 0B10000000;
      for(int i = 0; i < 8; i++){
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, GameOver[i]); //Col-green
        shiftOut(dataPin, clockPin, LSBFIRST, GameOver[(i+1)%8]); //Col-red
        shiftOut(dataPin, clockPin, LSBFIRST, row>>i); //Row
        digitalWrite(latchPin, HIGH);
      }
    }
    for(int j = 0; j < 100; j++){
      row = 0B10000000;
      for(int i = 0; i < 8; i++){
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, GameOver[(i+1)%8]); //Col-green
        shiftOut(dataPin, clockPin, LSBFIRST, GameOver[i]); //Col-red
        shiftOut(dataPin, clockPin, LSBFIRST, row>>i); //Row
        digitalWrite(latchPin, HIGH);
      }
    }
  }
  //Otherwise continue game
  else{
    for(int i = 0; i < 8; i++){
      digitalWrite(latchPin, LOW);

      new_row = row>>i;

      int distance = SonarReading();

      //Plays note if corresponding note is on the screen and user is at the proper distance
      //Leftmost note
      if(distance < 5 && (new_row == 0B00000001) && red_key == 0B00111111){
        buttons = 0B00111111;
        red_key = 0B11111111;
        playNote(notes[score%62], 90 - reduce);
        score++;
        Serial.println(score);
      }
      //Middle note
      else if((distance >= 5 && distance < 10) && (new_row == 0B00000001) && red_key == 0B11100111){
        buttons = 0B11100111;
        red_key = 0B11111111;
        playNote(notes[score%62], 90 - reduce);
        score++;
        Serial.println(score);
      }
      //Rightmost note
      else if((distance >= 10 && distance < 20) && (new_row == 0B00000001) && red_key == 0B11111100){
        buttons = 0B11111100;
        red_key = 0B11111111;
        playNote(notes[score%62], 90 - reduce);
        score++;
        Serial.println(score);
      }

      shiftOut(dataPin, clockPin, LSBFIRST, buttons); //Col-green
      shiftOut(dataPin, clockPin, LSBFIRST, red_key); //Col-red
      shiftOut(dataPin, clockPin, LSBFIRST, new_row); //Row
      digitalWrite(latchPin, HIGH);
      delay(90 - reduce);
    }
  }
}
