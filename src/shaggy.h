#include <limits.h>
#include <threshold.h>
#include <Arduino.h>

void ReadACDC(double *ACDC_filtered_ptr, double *HR_filtered_ptr, int SENSOR_GPIO, float alpha_PPG, float alpha_HR, float alpha_ACDC)
{
    int PPG_min_pre, PPG_max_pre, PPG_min_cur = INT_MAX, PPG_max_cur = INT_MIN;
    unsigned long current_ms, t_max_cur, t_max_pre = 0, t_min_cur, t_min_pre = 0, t_pulse;

    float PPG_sample_filtered, d, f, AC, DC, HR, threshold = threshold_from_HR(*HR_filtered_ptr);
    // float PPG_sample_filtered, d, f, AC, DC, HR, threshold = 340;
    int PPG_sample;
    bool last_extrema_was_max = false, last_extrema_was_min = true;

    int t1 = millis();
    while ((millis() - t1) < 10000)
    {
        // read sensor data into PPG_sample
        PPG_sample = analogRead(SENSOR_GPIO);

        PPG_sample_filtered = PPG_sample_filtered * alpha_PPG + (1 - alpha_PPG) * PPG_sample;

        // search for local max & min
        if (PPG_sample_filtered > PPG_max_cur)
        {
            PPG_max_cur = PPG_sample_filtered;
            t_max_cur = millis();
        }
        if (PPG_sample_filtered < PPG_min_cur)
        {
            PPG_min_cur = PPG_sample_filtered;
            t_min_cur = millis();
        }

        // check if current time exceeds threshold
        current_ms = millis();
        if (((current_ms - t_max_cur) > threshold) && (last_extrema_was_min))
        {
            // register max as local max
            last_extrema_was_max = true;
            last_extrema_was_min = false;

            // Calculate HR
            t_pulse = t_max_cur - t_max_pre;
            t_max_pre = t_max_cur;

            if (t_pulse > 0)
            {
                HR = 60000 / t_pulse;
                // Leaky integrator
                *HR_filtered_ptr = HR * alpha_HR + *HR_filtered_ptr * (1 - alpha_HR);

                threshold = threshold_from_HR(*HR_filtered_ptr);
                Serial.println("HR:" + String(*HR_filtered_ptr));
            }

            PPG_max_pre = PPG_max_cur;
            PPG_max_cur = INT_MIN;
        }
        if (((current_ms - t_min_cur) > threshold) && (last_extrema_was_max))
        {
            // register min as local min
            last_extrema_was_max = false;
            last_extrema_was_min = true;

            // calculate AC DC ratio
            // if (t_min_pre > 1)
            // {
            //     d = (PPG_min_cur - PPG_min_pre) / (t_min_cur - t_min_pre); // slope
            //     f = PPG_min_cur - d * t_min_cur;                           // y-intercept
            //     DC = d * t_max_cur + f;                                    // y = mx + f
            //     AC = PPG_max_cur - DC;
            //     *ACDC_filtered_ptr = *ACDC_filtered_ptr * alpha_ACDC + AC / DC * (1 - alpha_ACDC); // Leaky integrator
            // }

            t_min_pre = t_min_cur;
            PPG_min_pre = PPG_min_cur;
            PPG_min_cur = INT_MAX;
        }
    }
}
