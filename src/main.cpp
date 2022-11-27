#include <Arduino.h>
#include <threshold.h>

const int GREEN_GPIO = 27;
const int RED_GPIO = 14;
const int SENSOR_GPIO = 25;

const int GREEN_CHANNEL = 5;
const int RED_CHANNEL = 0;
const int RESOLUTION = 8;
const int FREQ = 1000;           // Frequency of pulsing in Hz
const int RED_DUTY_CYCLE = 255;  // for 50% duty cycle (0 is off, 255 is on)
const int GREEN_DUTY_CYCLE = 255; // for 50% duty cycle (0 is off, 255 is on)

// const unsigned long light_switch_interval = 2500;

// int red_data;
// int green_data;
unsigned long current_ms;
// unsigned long prev_ms = 0;
// boolean is_red = false;

int min_val = INT_MAX; // minimum value read from sensor
int max_val = INT_MIN; // maximum value read from sensor
unsigned long t_max_cur;
unsigned long t_max_pre = 0;
unsigned long t_min_cur;
unsigned long t_min_pre = 0;
double t_pulse;

// vector to store sensor data
int PPG_sample;
int PPG_sample_filtered;
const float alpha_PPG = 0.1; // tunable const in lowpass filter

bool last_extrema_was_max = false;
bool last_extrema_was_min = true;
// const int PPG_average_size = sizeof(PPG_sample) - PPG_window_size;

float threshold = 400;
float HR = 0;
float HR_filtered = 0;
float HR_printed = 0;
const float alpha_HR = 0.1; // tunable const in lowpass filter

int i = 0;

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
  ledcWrite(GREEN_CHANNEL, RED_DUTY_CYCLE);
  ledcWrite(RED_CHANNEL, 0);

  // Configure serial monitor
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // read sensor data into PPG_sample
  PPG_sample = analogRead(SENSOR_GPIO);

  PPG_sample_filtered = PPG_sample_filtered*alpha_PPG + (1-alpha_PPG)*PPG_sample;
  // apply moving average
  // sum = sum + PPG_sample_vec[idx];
  // idx = (idx + 1) % window_size;

  // PPG_sample = sum / window_size;

  // search for local max & min
  if (PPG_sample_filtered > max_val)
  {
    max_val = PPG_sample_filtered;
    t_max_cur = millis();
  }
  if (PPG_sample_filtered < min_val)
  {
    min_val = PPG_sample_filtered;
    t_min_cur = millis();
  }

  // check if current time exceeds threshold
  current_ms = millis();
  if (((current_ms - t_max_cur) > threshold) && (last_extrema_was_min))
  {
    // register max as local max
    last_extrema_was_max = true;
    last_extrema_was_min = false;

    // TODO calculate HR
    t_pulse = t_max_cur - t_max_pre;
    t_max_pre = t_max_cur;

    if (t_pulse > 0)
    {
      HR = 60000 / t_pulse;
    }
    else
    {
      HR = 0;
    }

    HR_filtered = HR*alpha_HR + HR_filtered*(1-alpha_HR);
    max_val = INT_MIN;
  }
  if (((current_ms - t_min_cur) > threshold) && (last_extrema_was_max))
  {
    // register min as local min
    last_extrema_was_max = false;
    last_extrema_was_min = true;
    t_min_pre = t_min_cur;
    
    // TODO calculate ACred, DCred, ACir, DCir, R
    // TODO Calculate SpO2
    min_val = INT_MAX;
  }

  
  // if (i > 500) {
  //   Serial.println("PPG_sample:" + String(PPG_sample));
  //   i = 0;
  // }
  // i += 1;
  if (HR_filtered != HR_printed) {
    Serial.print("HR:" + String(HR_filtered));
    Serial.print(", ");
    Serial.println("t_pulse:" + String(t_pulse));
    HR_printed = HR_filtered;
  }
  // Serial.print("HR:" + String(HR_filtered));
  // Serial.print(", ");
  // Serial.println("t_pulse:" + String(t_pulse));

  // TODO Compute mean values from last 8 estimations
  // TODO Select new threshold from LUT based on HR
  threshold = threshold_from_HR(HR_filtered);

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

