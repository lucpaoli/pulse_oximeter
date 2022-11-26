#include <Arduino.h>
#include <math.h>

const int GREEN_GPIO = 27;
const int RED_GPIO = 14;
const int SENSOR_GPIO = 25;

const int GREEN_CHANNEL = 5;
const int RED_CHANNEL = 0;
const int RESOLUTION = 8;
const int FREQ = 1000;           // Frequency of pulsing in Hz
const int RED_DUTY_CYCLE = 255;  // for 50% duty cycle (0 is off, 255 is on)
const int GREEN_DUTY_CYCLE = 70; // for 50% duty cycle (0 is off, 255 is on)

const unsigned long light_switch_interval = 2500;

int red_data;
int green_data;
unsigned long current_ms;
unsigned long prev_ms = 0;
boolean is_red = false;

// int min_val; // minimum value read from sensor
// int max_val; // maximum value read from sensor

// vector to store sensor data
int PPG_sample[100];
const int PPG_window_size = 10;
const int PPG_average_size = sizeof(PPG_sample) - PPG_window_size;

void setup()
{
  // put your setup code here, to run once:

  // Configure pin
  pinMode(RED_GPIO, OUTPUT);
  pinMode(GREEN_GPIO, OUTPUT);

  // Configure PWM
  ledcSetup(GREEN_CHANNEL, FREQ, RESOLUTION);
  ledcSetup(RED_CHANNEL, FREQ, RESOLUTION);

  // Attach GPIO and PWM channels
  ledcAttachPin(GREEN_GPIO, GREEN_CHANNEL);
  ledcAttachPin(RED_GPIO, RED_CHANNEL);

  // Configure serial monitor
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  current_ms = millis();

  // read sensor data into PPG_sample
  for (int i = 0; i < 100; i++)
  {
    PPG_sample[i] = analogRead(SENSOR_GPIO);
  }
  // calculate moving average of PPG_sample with a window size of PPG_sample_size
  int moving_avg[PPG_average_size];
  for (int i = 0; i < PPG_average_size; i++)
  {
    moving_avg[i] = 0;
    for (int j = 0; j < PPG_window_size; j++)
    {
      moving_avg[i] += PPG_sample[i + j];
    }
    moving_avg[i] /= PPG_window_size;
  }

  // search for local max & min
  int max = -32768;
  int min = 32767;
  for (int i = 0; i < PPG_average_size; i++)
  {
    if (moving_avg[i] > max)
    {
      max = moving_avg[i];
      // write time_max
      //! what is time_max?! defined as current_timeslot
    }
    if (moving_avg[i] < min)
    {
      min = moving_avg[i];
      // write time_min
    }
  }

  // Change the color every second (see with interval variable)
  if (current_ms - prev_ms >= light_switch_interval)
  {
    is_red = !is_red;
    prev_ms = current_ms;
  }

  // Red LED pulse
  if (is_red)
  {
    while
      ledcWrite(RED_CHANNEL, RED_DUTY_CYCLE);
    ledcWrite(GREEN_CHANNEL, 0);
    red_data = analogRead(SENSOR_GPIO);
    Serial.print("red:");
    Serial.println(red_data);
  }

  // Green LED pulse
  else
  {
    ledcWrite(RED_CHANNEL, 0);
    ledcWrite(GREEN_CHANNEL, GREEN_DUTY_CYCLE);
    green_data = analogRead(SENSOR_GPIO);
    Serial.print("green:");
    Serial.println(green_data);
  }
}

// // function that returns a moving average of the PPG_sample vector
// void moving_average(int PPG_sample[], int PPG_sample_size, int PPG_window_size)
// {
//   int moving_avg[PPG_sample_size - PPG_window_size];
//   for (int i = 0; i < PPG_sample_size - PPG_window_size; i++)
//   {
//     moving_avg[i] = 0;
//     for (int j = 0; j < PPG_window_size; j++)
//     {
//       moving_avg[i] += PPG_sample[i + j];
//     }
//     moving_avg[i] /= PPG_window_size;
//   }
// }