#include <Arduino.h>
// #include <shaggy.h>
#include <ReadHR.h>
// #include <circular_buffer.hpp>

const int GREEN_GPIO = 27;
const int RED_GPIO = 14;
const int SENSOR_GPIO = 25;

const int GREEN_CHANNEL = 5;
const int RED_CHANNEL = 0;
const int RESOLUTION = 8;
const int FREQ = 1000;            // Frequency of pulsing in Hz
const int RED_DUTY_CYCLE = 255;   // for 50% duty cycle (0 is off, 255 is on)
const int GREEN_DUTY_CYCLE = 255; // for 50% duty cycle (0 is off, 255 is on)

// const unsigned long light_switch_interval = 2500;

// unsigned long current_ms;
// unsigned long prev_ms = 0;
// boolean is_red = false;

// vector to store sensor data
// int PPG_sample;
// int PPG_sample_filtered;
// double ACDC_green, ACDC_red;
// const float alpha_PPG = 0.1;  // tunable const in leaky integrator
// const float alpha_ACDC = 0.2; // tunable const in leaky integrator

// bool last_extrema_was_max = false;
// bool last_extrema_was_min = true;

// const int PPG_average_size = sizeof(PPG_sample) - PPG_window_size;

// double HR_filtered = 0;
float HR_green = 75, HR_red = 75;
float ACDC_green = 0, ACDC_red = 0;
float R;
// const float alpha_HR = 0.4; // tunable const in leaky integrator

// circular buffer to calculate HR average
// jm::circular_buffer<float, 8> HR_avg{};

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
  // if (isnan(HR_green))
  // {
  //   HR_green = 75;
  // }
  ledcWrite(GREEN_CHANNEL, GREEN_DUTY_CYCLE);
  ledcWrite(RED_CHANNEL, 0);
  ReadHR(&HR_green, &ACDC_green, SENSOR_GPIO);
  // ReadACDC(&ACDC_green, &HR_filtered, SENSOR_GPIO, alpha_PPG, alpha_HR, alpha_ACDC);

  // if (isnan(HR_red))
  // {
  //   HR_red = 75;
  // }
  ledcWrite(RED_CHANNEL, RED_DUTY_CYCLE);
  ledcWrite(GREEN_CHANNEL, 0);
  ReadHR(&HR_red, &ACDC_red, SENSOR_GPIO);
  R = ACDC_green / ACDC_red;
  Serial.print(String(R));
  Serial.print('\n');

  // Serial.print("HR_green:");
  // Serial.print(String(HR_green));
  // Serial.print(", HR_red:");
  // Serial.print(String(HR_red));
  // Serial.print(", R:");
  // Serial.println(String(R));

  // ReadACDC(&ACDC_red, &HR_filtered, SENSOR_GPIO, alpha_PPG, alpha_HR, alpha_ACDC);

  // R = ACDC_red/ACDC_green;

  // Serial.print(", ");
  // Serial.println("R:" + String(R));
  // Serial.print(", ");
  // Serial.print("ACDC_green:" + String(ACDC_green));
  // Serial.print(", ");
  // Serial.println("ACDC_red:" + String(ACDC_red));
}
