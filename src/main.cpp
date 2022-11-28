#include <Arduino.h>
#include <shaggy.h>

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

unsigned long current_ms;
// unsigned long prev_ms = 0;
// boolean is_red = false;

// vector to store sensor data
// int PPG_sample;
// int PPG_sample_filtered;
double ACDC_green, ACDC_red;
const float alpha_PPG = 0.1;  // tunable const in leaky integrator
const float alpha_ACDC = 0.2; // tunable const in leaky integrator

// bool last_extrema_was_max = false;
// bool last_extrema_was_min = true;

// const int PPG_average_size = sizeof(PPG_sample) - PPG_window_size;

double HR_filtered = 0;
float R;
const float alpha_HR = 0.05; // tunable const in leaky integrator

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

  // ledcWrite(RED_CHANNEL, RED_DUTY_CYCLE);
  // ledcWrite(GREEN_CHANNEL, 0);

  // Configure serial monitor
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // ledcWrite(GREEN_CHANNEL, GREEN_DUTY_CYCLE);
  // ledcWrite(RED_CHANNEL, 0);
  // ReadACDC(&ACDC_green, &HR_filtered, SENSOR_GPIO, alpha_PPG, alpha_HR, alpha_ACDC);

  ledcWrite(RED_CHANNEL, RED_DUTY_CYCLE);
  ledcWrite(GREEN_CHANNEL, 0);
  ReadACDC(&ACDC_red, &HR_filtered, SENSOR_GPIO, alpha_PPG, alpha_HR, alpha_ACDC);

  // Serial.print("HR:" + String(HR_filtered));
  // Serial.print(", ");
  // Serial.print("ACDC_green:" + String(ACDC_green));
  // Serial.print(", ");
  // Serial.println("ACDC_red:" + String(ACDC_red));

  // R = ACDC_red/ACDC_green;

  //* Debug printing, ignore this lol
  // if (HR_filtered != HR_printed)
  // {
  //   Serial.print("HR:" + String(HR_filtered));
  //   Serial.print(", ");
  //   Serial.println("t_pulse:" + String(t_pulse));
  //   HR_printed = HR_filtered;
  // }
  // Serial.print("HR:" + String(HR_filtered));
  // Serial.print(", ");
  // Serial.println("t_pulse:" + String(t_pulse));

  // TODO Compute mean values from last 8 estimations
  // TODO Select new threshold from LUT based on HR

  // // Change the color every second (see with interval variable)
  // if (current_ms - prev_ms >= light_switch_interval)
  // {
  //   is_red = !is_red;
  //   prev_ms = current_ms;
  // }

  // // Red LED pulse
  // if (is_red)
  // {
  //   ledcWrite(RED_CHANNEL, RED_DUTY_CYCLE);
  //   ledcWrite(GREEN_CHANNEL, 0);
  //   red_data = analogRead(SENSOR_GPIO);
  //   Serial.print("red:");
  //   Serial.println(red_data);
  // }

  // // Green LED pulse
  // else
  // {
  //   ledcWrite(RED_CHANNEL, 0);
  //   ledcWrite(GREEN_CHANNEL, GREEN_DUTY_CYCLE);
  //   green_data = analogRead(SENSOR_GPIO);
  //   Serial.print("green:");
  //   Serial.println(green_data);
  // }
}
