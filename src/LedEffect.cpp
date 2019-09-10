#include "LedEffect.h"
#include <math.h>
#include <stdlib.h>

LedEffect::LedEffect() {
    _timerId = 0;
}

LedEffect::~LedEffect() {
    stop();
}

void LedEffect::start(LedEffectHandler *handler) {
    if (_timerId != 0) return;
    _strip = mgos_neopixel_create(LED_PIN, LED_COUNT, MGOS_NEOPIXEL_ORDER_GRB);
    _timerId = mgos_set_timer(PERIOD, MGOS_TIMER_REPEAT, _timer_cb, this);
    _handler = handler;
}

void LedEffect::stop() {
    if (_timerId == 0) return;
    mgos_clear_timer(_timerId);
    mgos_neopixel_free(_strip);
    _timerId = 0;
    _handler = 0;
}

void LedEffect::_timer_cb(void *arg) {
    LedEffect *self = (LedEffect *) arg;
    if (self->_handler != 0) {
        self->_handler->doStep(self->_strip);
    }
}

LedRgbBlink::LedRgbBlink() {
    _brightness = _min;
    _sign = 1;
}

void LedRgbBlink::doStep(mgos_neopixel *strip) {
    for (int i = 0; i < LED_COUNT; i += 3) {
        mgos_neopixel_set(strip, i, _brightness, 0, 0);
        mgos_neopixel_set(strip, i + 1, 0, _brightness, 0);
        mgos_neopixel_set(strip, i + 2, 0, 0, _brightness);
    }
    _brightness += (_sign * _d);
    if (_brightness >= _max || _brightness < _min) {
        _sign = -_sign;
    }
    mgos_neopixel_show(strip);
}

LedFire::LedFire() {
    _offset = 0;
    memset(&_heat, 0, sizeof(_heat));
}

void LedFire::doStep(mgos_neopixel *strip) {
    // this code was taken from FastLED project http://fastled.io/
    // https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
    // (c) by Mark Kriegsman and Daniel Garcia
    const int Cooling = 10;
    const int Sparking = 50;
    int cooldown;

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < LED_COUNT; i++) {
        cooldown = random(0, ((Cooling * 10) / LED_COUNT) + 2);

        if (cooldown > _heat[i]) {
            _heat[i] = 0;
        } else {
            _heat[i] = _heat[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = LED_COUNT - 1; k >= 2; k--) {
        _heat[k] = (_heat[k - 1] + _heat[k - 2] + _heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if (random(0, 255) < Sparking) {
        _offset = random(0, LED_COUNT);
        int y = random(0, 7);
        _heat[y] = _heat[y] + random(160, 255);
        //heat[y] = random(160,255);
    }

    // Step 4.  Convert heat to LED colors
    for (int j = 0; j < LED_COUNT; j++) {
        _setPixelHeatColor(strip, (j + _offset) % LED_COUNT, _heat[j]);
    }

    mgos_neopixel_show(strip);
}

void LedFire::_setPixelHeatColor(mgos_neopixel *strip, int Pixel, byte temperature) {
    // this code was taken from FastLED project http://fastled.io/
    // https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
    // (c) by Mark Kriegsman and Daniel Garcia
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2; // scale up to 0..252

    // figure out which third of the spectrum we're in:
    if (t192 > 0x80) {                     // hottest
        mgos_neopixel_set(strip, Pixel, 255, 255, heatramp);
    } else if (t192 > 0x40) {             // middle
        mgos_neopixel_set(strip, Pixel, 255, heatramp, 0);
    } else {                               // coolest
        mgos_neopixel_set(strip, Pixel, heatramp, 0, 0);
    }
}

int LedFire::random(int min, int max) {
    return min + (int) ((double) rand() * (max - min) / (RAND_MAX + 1.0));
}

LedColor::LedColor(int r, int g, int b) : LedEffectHandler() {
    _brightness = _min;
    _sign = 1;
    _r = r;
    _g = g;
    _b = b;
}

void LedColor::doStep(mgos_neopixel *strip) {
    for (int i = 0; i < LED_COUNT; i++) {
        int r = _brightness * _r / 255;
        int g = _brightness * _g / 255;
        int b = _brightness * _b / 255;
        mgos_neopixel_set(strip, i + 0, r, g, b);
    }
    _brightness += (_sign * _d);
    if (_brightness >= _max || _brightness < _min) {
        _sign = -_sign;
    }
    mgos_neopixel_show(strip);
}
