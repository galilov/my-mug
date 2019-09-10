#include "mgos.h"
#include "limits.h"
#include "mgos_bitbang.h"
#include "mgos_hal.h"
#include "mgos_adc.h"
#include "ntcTable.h"

extern void showLedColor(int r, int g, int b);

extern void hideLeds();

extern void showLedFire();

const int adcPin = 0;
const int r = 8700; // pull down resistor connected to ADC input pin, Ohms
const float vcc = 3.3f; // max ADC input, volts (3.3V for NodeMCU)

enum mgos_app_init_result mgos_app_init(void) {
    if (!mgos_adc_enable(adcPin)) {
        showLedColor(255, 0, 0);
        return MGOS_APP_INIT_ERROR;
    }

    showLedColor(255, 255, 0);

    return MGOS_APP_INIT_SUCCESS;
}

int getTemperature() {
    int val = mgos_adc_read(adcPin);
    if (val < 10 || val > 1000) { // out of range. Too cold or too hot.
        return INT_MIN; // return error
    }
    float voltage =
            val * vcc / 1024.0; // 1024 is the maximum value produced by ADC. Hmm I expected 1023 but... Please check!
    float current = voltage / r;
    float rNtc = vcc / current - r;

    for (size_t i = 0; i < sizeof(ntcTable) / sizeof(ntcTable[0]); i++) {
        if (rNtc < ntcTable[i].r) continue;
        return ntcTable[i].t;
    }
    return INT_MIN; // return error
}

bool isTemperatureError(int temperature) {
    return temperature == INT_MIN;
}
