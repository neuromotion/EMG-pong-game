

#include <Keyboard.h>

// if this line is not commented out, print messages to serial monitor instead of sending key events
//#define DEBUG_MODE

// the number of samples for calibration and baseline
#define CALIBRATION_BIN_SIZE 1000
#define BASELINE_BIN_SIZE 100

// ms to wait between sample reads.
// sample rate is imprecise, will be slower if a key event is sent, and will
//   be a bit faster during calibration than during normal operation.
#define SAMPLE_PERIOD 1

// the number of bins of training data used to set the channel thresholds
#define NUM_CALIBRATION_BINS 20

// the number of players
#define NUM_PLAYERS 6
#define NUM_CHANNELS 6

//number of history time points to use for moving average
#define HISTORY_ORDER 25

//threshold buffer (how much higher than the max relaxed EMG to set threshold as)
#define THRESH_BUFFER 10

// the names of the analog input channels, length must be at least NUM_CHANNELS
//const uint8_t channels[] = {A7, A6, A5, A4, A2, A1};
const uint8_t channels[] = {A5, A4, A3, A2, A1, A0};

// left / right / shoot
// length must be at least NUM_CHANNELS
//char keys[] = {'k', 'm', 'a' ,'z'};
char keys[] = {'e', 'a', 'd', 'o', 'j', 'l'};

// store thresholds used to define when a press happens
// high and low thresholds create hysteresis
uint8_t high_thresholds[NUM_CHANNELS];
uint8_t low_thresholds[NUM_CHANNELS];

//signal zero value
uint16_t zero_value[NUM_CHANNELS];

// stores one bin of data in loop()
//uint8_t bin[NUM_CHANNELS] = {0};
uint16_t bin[NUM_CHANNELS] = {0};

// stores previous state of the key for each channel
bool is_pressed[NUM_CHANNELS] = {0};

const int n = 10;
//uint8_t history[n] = {0};
uint16_t history[NUM_CHANNELS][HISTORY_ORDER] = {0};
uint16_t average_EMG[NUM_CHANNELS] = {0};

void setup() {
#ifdef DEBUG_MODE
  Serial.begin(115200);
#else
  Keyboard.begin();
#endif
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  setbaseline();
  calibrate();
}

void loop() {
  update_history(average_EMG);

  // uncomment one of these 2 functions
  send_sustained_presses();
  //send_press_and_release(bin);
  /*
    history[n] = map(analogRead(0), 0, 1023, 0, 255);
    Serial.println(history[n]);
  */
}

//void send_sustained_presses(uint8_t* bin){
//void send_sustained_presses(uint16_t* bin) {
//  /* if a channel rises above threshold, press corresponding key.
//     if a channel falls below threshold, release corresponding key. */
//  for (int c = 0; c < NUM_CHANNELS; c++) {
//    if (bin[c] > high_thresholds[c]) {
//      // above threshold
//      if (!is_pressed[c]) {
//        // new press
//        press(keys[c]);
//        is_pressed[c] = 1;
//      }
//    }
//    else if (bin[c] < low_thresholds[c]) {
//      // below threshold
//      if (is_pressed[c]) {
//        // new release
//        release(keys[c]);
//        is_pressed[c] = 0;
//      }
//    }
//  }
//}


void send_sustained_presses() {
  /* if a channel rises above threshold, press corresponding key.
     if a channel falls below threshold, release corresponding key. */
  for (int c = 0; c < NUM_CHANNELS; c++) {
    if (average_EMG[c] > high_thresholds[c]) {
      // above threshold
      if (!is_pressed[c]) {
        // new press
        press(keys[c]);
        is_pressed[c] = 1;
        #ifdef DEBUG_MODE
        Serial.print("Channel Pressed: ");
        Serial.println(c);
        #endif
      }
    }
    else if (average_EMG[c] < high_thresholds[c]) {
      // below threshold
      if (is_pressed[c]) {
        // new release
        release(keys[c]);
        is_pressed[c] = 0;
        #ifdef DEBUG_MODE
        Serial.print("Channel Released: ");
        Serial.println(c);
        #endif
      }
    }
  }
    Serial.print(is_pressed[2]*40);
    Serial.print(" ");
    Serial.print(average_EMG[2]);
    Serial.print(" ");
    Serial.print(is_pressed[3]*40);
    Serial.print(" ");
    Serial.println(average_EMG[3]);
}


void send_press_and_release(uint16_t* bin) {
  /* if a channel is above threshold, press and immediately release
     the corresponding key */
  for (int c = 0; c < NUM_CHANNELS; c++) {
    if (bin[c] > high_thresholds[c]) {
      press_and_release(keys[c]);
    }
  }
}

void press(char character) {
#ifdef DEBUG_MODE
  Serial.print("press ");
  Serial.println(character);
#else
  Keyboard.press(character);
#endif
}

void release(char character) {
#ifdef DEBUG_MODE
  Serial.print("release ");
  Serial.println(character);
#else
  Keyboard.release(character);
#endif
}

void press_and_release(char character) {
  // TODO is the delay here really necessary?
  press(character);
  delay(1);
  release(character);
}


//void get_bin(uint8_t* values){
void get_average(uint16_t* values) {
  /* Fill up the array with one value per channel.
     Each value is the mean of BIN_SIZE samples, and is in range 0-255.  */

  for (int t = 0; t < BASELINE_BIN_SIZE; t++) {
    for (int c = 0; c < NUM_CHANNELS; c++) {
      //values[c] += map(analogRead(channels[c]), 0, 1023, 0, 255);

      //      int current_value = abs(analogRead(channels[c]) - 512);
      //      values[c] += map(current_value, 0, 1023, 0, 255);
      int current_value = analogRead(channels[c]);
      values[c] += current_value;
      //delay(0.1);
    }
    delay(SAMPLE_PERIOD);
  }
  for (int c = 0; c < NUM_CHANNELS; c++) {
    values[c] /= BASELINE_BIN_SIZE;
  }
}

void get_reading(uint16_t values[NUM_CHANNELS][CALIBRATION_BIN_SIZE]) {
  for (int t = 0; t < CALIBRATION_BIN_SIZE; t++) {
    update_history(average_EMG);
    for (int c = 0; c < NUM_CHANNELS; c++) {
      uint16_t current_value = average_EMG[c];
      values[c][t] = current_value;
      //delay(0.1);
    }
    delay(SAMPLE_PERIOD);
  }
}

void update_history(uint16_t* average) {

  int current_value;
  for (int j = 0; j < NUM_CHANNELS; j++) {

    for (int i = 1; i < HISTORY_ORDER; i++) {
      history[j][i - 1] = history[j][i];
    }

    int32_t temp_reading = analogRead(channels[j]) - zero_value[j];
    current_value = abs(temp_reading);
    history[j][HISTORY_ORDER - 1] = current_value;


    for (int i = 0; i < HISTORY_ORDER; i++) {
      average[j] += history[j][i];
    }
    average[j] = average[j] / HISTORY_ORDER;
    delay(SAMPLE_PERIOD);
  }

}

void calibrate() {
  // get calibration data
  uint16_t calibration_data[NUM_CHANNELS][CALIBRATION_BIN_SIZE] = {0};
  get_reading(calibration_data);

#ifdef DEBUG_MODE
  Serial.println("calibration will start");  //xiaoxiao
#endif

  // compute the threshold for each channel
  uint16_t max_EMG[NUM_CHANNELS];
  for (int c = 0; c < NUM_CHANNELS; c++) {
    // find the max EMG during calibration (relaxation)
    max_EMG[c] = 0;

#ifdef DEBUG_MODE
    Serial.print("calibration channel# = ");
    Serial.println(c);
#endif

    for (int n = 0; n < CALIBRATION_BIN_SIZE; n++) {
      if (calibration_data[c][n] > max_EMG[c]) {
        max_EMG[c] = calibration_data[c][n];
      }
    }

    // set channel threshold to BUFFER more than the max relaxation reading
    low_thresholds[c] = max_EMG[c];
    high_thresholds[c] = max_EMG[c] + THRESH_BUFFER;
#ifdef DEBUG_MODE
    Serial.print("max_EMG = ");
    Serial.println(max_EMG[c]);
    Serial.print("low threshold = ");
    Serial.println(low_thresholds[c]);
    Serial.print("high threshold = ");
    Serial.println(high_thresholds[c]);
#endif
  }
#ifdef DEBUG_MODE
  Serial.println("calibration will stop");  //xiaoxiao
#endif

  //Flash LED on board to show that calibration is done
  digitalWrite(13, HIGH);
  delay(250);
  digitalWrite(13, LOW);
}


void setbaseline() {
  uint16_t baseline_data[NUM_CHANNELS] = {0};
  get_average(baseline_data);
  for (int j = 0; j < NUM_CHANNELS; j++) {
    zero_value[j] = baseline_data[j];
  }
}



//

