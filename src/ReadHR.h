#include <threshold.h>
#include <Arduino.h>

void ReadHR(float *HR_ptr, float *ACDC_ptr, int SENSOR_GPIO)
{
    // Timings
    unsigned long current_us, t_max_cur, t_max_pre = 0, t_min_cur, t_min_pre = 0, t_pulse;
    // Reading threshold
    int threshold = threshold_from_HR(*HR_ptr);
    // PPG readings
    int PPG_sample, PPG_min_pre, PPG_max_pre, PPG_min_cur = INT_MAX, PPG_max_cur = INT_MIN;
    float PPG_sample_filtered, alpha_PPG = 0.5;
    // search booleans
    bool last_extrema_was_max = false, last_extrema_was_min = true;

    float HR = 0;
    // ACDC calculation
    int denom;
    float d, f, AC, DC, DC_temp;
    // Initialise integrator
    PPG_sample_filtered = analogRead(SENSOR_GPIO);

    int nt = 10, n = 0;
    while (n < nt)
    {
        // Read PPG sample
        PPG_sample = analogRead(SENSOR_GPIO);
        current_us = micros();
        // Apply moving average
        PPG_sample_filtered = PPG_sample_filtered * alpha_PPG + (1 - alpha_PPG) * PPG_sample;

        // Serial.print("PPG_sample_filtered:");

        Serial.print(String(current_us));
        Serial.print(",");
        Serial.print(String(PPG_sample_filtered));

        // search for local max & min
        if (PPG_sample_filtered > PPG_max_cur)
        {
            PPG_max_cur = PPG_sample_filtered;
            t_max_cur = current_us;
        }
        if (PPG_sample_filtered < PPG_min_cur)
        {
            PPG_min_cur = PPG_sample_filtered;
            t_min_cur = current_us;
        }

        // check if current time exceeds threshold
        // current_ms = millis();
        if (((current_us - t_max_cur) > threshold) && (last_extrema_was_min))
        {
            Serial.print(",");
            Serial.print(String(t_max_cur));
            Serial.print(",");
            Serial.print(String(PPG_max_cur));
            // register max as local max
            last_extrema_was_max = true;
            last_extrema_was_min = false;

            // Calculate HR
            t_pulse = t_max_cur - t_max_pre;
            t_max_pre = t_max_cur;

            if (t_pulse > 0)
            {
                HR += 60000.0 / t_pulse;
                n++;
            }

            PPG_max_pre = PPG_max_cur;
            PPG_max_cur = INT_MIN;
        }
        else if (((current_us - t_min_cur) > threshold) && (last_extrema_was_max))
        {
            Serial.print(",");
            Serial.print(String(t_min_cur));
            Serial.print(",");
            Serial.print(String(PPG_min_cur));
            // register min as local min
            last_extrema_was_max = false;
            last_extrema_was_min = true;

            // Calculate AC DC ratio
            denom = t_min_cur - t_min_pre;
            if (denom > 0)
            {
                d = (PPG_min_cur - PPG_min_pre) / denom; // slope
                f = PPG_min_cur - d * t_min_cur;         // y-intercept
                DC_temp = d * t_max_cur + f;             // y = mx + f
                DC += DC_temp;
                //! AC is coming out as negative, which should NOT be possible
                AC += PPG_max_cur - DC_temp;
            }

            t_min_pre = t_min_cur;
            PPG_min_pre = PPG_min_cur;
            PPG_min_cur = INT_MAX;
        }
        Serial.print('\n');
    }
    // Serial.print("HR: ");
    // Serial.print(String(HR));
    // Serial.print(", HR/n:");
    // Serial.print(String(HR / n));
    // Serial.print(", AC: ");
    // Serial.print(String(AC));
    // Serial.print(", DC: ");
    // Serial.println(String(DC));

    *HR_ptr = HR / n;
    *ACDC_ptr = AC / DC;
}