#include <Keyboard.h>
// if this line is not commented out, print messages to serial monitor instead of sending key events
//#define DEBUG_MODE
// the number of samples for calibration and baseline
#define CALIBRATION_BIN_SIZE 2000 //Time period to run calibration for all channels
#define BASELINE_BIN_SIZE 100 //time period to get baseline voltage of all channels
// ms to wait between sample reads.
// sample rate is imprecise, will be slower if a key event is sent, and will
//   be a bit faster during calibration than during normal operation.
#define SAMPLE_PERIOD 1 //period between channel readings
// the number of bins of training data used to set the channel thresholds
//#define NUM_CALIBRATION_BINS 20 
// the number of players
#define NUM_PLAYERS 4
#define NUM_CHANNELS 4
//number of history time points to use for moving average
#define HISTORY_ORDER 25
//threshold buffer (how much higher than the max relaxed EMG to set threshold as)
#define THRESH_BUFFER 10
// the names of the analog input channels, length must be at least NUM_CHANNELS
const uint8_t channels[] = {A0, A1, A2, A4};
// left / right / shoot
// length must be at least NUM_CHANNELS
//char keys[] = {'k', 'm', 'a' ,'z'};
char keys[] = {KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW ,KEY_RIGHT_ARROW}; //keys controlled by each player
// store thresholds used to define when a press happens
// high and low thresholds create hysteresis
uint8_t high_thresholds[NUM_CHANNELS];
uint8_t low_thresholds[NUM_CHANNELS];
//signal zero value
uint16_t zero_value[NUM_CHANNELS]; //calculated during baseline, finds the average voltage on each channel
// stores one bin of data in loop()
//uint8_t bin[NUM_CHANNELS] = {0};
//uint16_t bin[NUM_CHANNELS] = {0}; 
// stores previous state of the key for each channel
bool is_pressed[NUM_CHANNELS] = {0}; //tells you which keys are currently pressed
//const int n = 10;
//uint8_t history[n] = {0};
uint16_t history[NUM_CHANNELS][HISTORY_ORDER] = {0}; //array holding previous EMG values up to HISTORY_ORDER time points
uint16_t average_EMG[NUM_CHANNELS] = {0}; //the average_EMGs for each channel
void setup() {
  Keyboard.begin();
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  setbaseline(); //get average voltage for each channel during relaxation and set as the baseline
  calibrate(); //
}
void loop() {
  update_history(average_EMG); //get reading of EMG channels
  // uncomment one of these 2 functions
  send_sustained_presses(); //using the EMG readings, decide which buttons should be pressed
  //send_press_and_release(bin);
  /*
    history[n] = map(analogRead(0), 0, 1023, 0, 255);
    Serial.println(history[n]);
  */
}
void send_sustained_presses() {
  /* if a channel rises above threshold, press corresponding key.
     if a channel falls below threshold, release corresponding key. */
  for (int c = 0; c < NUM_CHANNELS; c++) { //go through each channel
    if (average_EMG[c] > high_thresholds[c]) { //if the EMG at that channel is above the high threshold, press the button (if it's not already pressed)
      // above threshold
      if (!is_pressed[c]) { //check if button isn't pressed already
        // new press
        press(keys[c]); //press it
        is_pressed[c] = 1; //tell program that button is pressed now
        #ifdef DEBUG_MODE
        Serial.print("Channel Pressed: ");
        Serial.println(c);
        #endif
      }
    }
    else if (average_EMG[c] < high_thresholds[c]) { //if the EMG at that channel is below the low threshold, release the button (if it is pressed)
      // below threshold
      if (is_pressed[c]) { //if button is pressed
        // new release
        release(keys[c]); //release key
        is_pressed[c] = 0; //say the key is now released
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
//void send_press_and_release(uint16_t* bin) {
//  /* if a channel is above threshold, press and immediately release
//     the corresponding key */
//  for (int c = 0; c < NUM_CHANNELS; c++) {
//    if (bin[c] > high_thresholds[c]) {
//      press_and_release(keys[c]);
//    }
//  }
//}
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
  int current_value; //absolute EMG voltage
  
  for (int j = 0; j < NUM_CHANNELS; j++) { //go through each channel
    for (int i = 1; i < HISTORY_ORDER; i++) { //go through each of your history time points 
      history[j][i - 1] = history[j][i]; //shift all your columns in your history buffer over 1
    }
    int32_t temp_reading = analogRead(channels[j]) - zero_value[j]; //reads EMG voltage for this time point and for this channel, subtract the baseline out of the voltage 
    current_value = abs(temp_reading); //take absolute value
    history[j][HISTORY_ORDER - 1] = current_value; //save it to your history buffer array
    for (int i = 0; i < HISTORY_ORDER; i++) { //find average of all history time points (moving average filter)
      average[j] += history[j][i]; //sum all time points
    }
    average[j] = average[j] / HISTORY_ORDER; //divide by total number of time points
    delay(SAMPLE_PERIOD); //wait until next sampling time
  }
}
void calibrate() {
  // get calibration data
  uint16_t calibration_data[NUM_CHANNELS][CALIBRATION_BIN_SIZE] = {0}; //initialize array of all voltages of all channels during calbration
  get_reading(calibration_data); //read CALIBRATION_BIN_SIZE samples and save into array
#ifdef DEBUG_MODE
  Serial.println("calibration will start");  //xiaoxiao
#endif
  // compute the threshold for each channel
  uint16_t max_EMG[NUM_CHANNELS]; //array to hold maximum EMG voltage for each channel
  for (int c = 0; c < NUM_CHANNELS; c++) {
    // find the max EMG during calibration (relaxation)
    max_EMG[c] = 0; //initial "maxium" EMG voltage (initialize with 0)
#ifdef DEBUG_MODE
    Serial.print("calibration channel# = ");
    Serial.println(c);
#endif
    for (int n = 0; n < CALIBRATION_BIN_SIZE; n++) { //go through each time point
      if (calibration_data[c][n] > max_EMG[c]) { //if the time point is larger than your current maximum, it becomes your new maximum
        max_EMG[c] = calibration_data[c][n];
      }
    }
    // set channel threshold to BUFFER more than the max relaxation reading
    low_thresholds[c] = max_EMG[c]; //threshold for releasing button (EMG falls below these values)
    high_thresholds[c] = max_EMG[c] + THRESH_BUFFER; //threshold for pressing button (EMG increases above these values)
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
  uint16_t baseline_data[NUM_CHANNELS] = {0}; //initialize array
  
  get_average(baseline_data); //get reading of BASELINE_BIN_SIZE samples and calculate average
  for (int j = 0; j < NUM_CHANNELS; j++) {
    zero_value[j] = baseline_data[j]; //save baseline voltages to the zero_value global variable
  }
}
//

