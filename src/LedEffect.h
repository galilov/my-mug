#ifndef __LED_EFFECT_H__
#define __LED_EFFECT_H__

#include "mgos.h"
#include "mgos_neopixel.h"

const int LED_PIN = 5;
const int LED_COUNT = 24;
const int PERIOD = 50; // milliseconds

class LedEffectHandler {
public:
    virtual void doStep(mgos_neopixel *strip) = 0;

    virtual ~LedEffectHandler() {};
};


class LedRgbBlink : public LedEffectHandler {
public:
    LedRgbBlink();

    void doStep(mgos_neopixel *strip);

private:
    const int _d = 10;
    const int _min = 30;
    const int _max = 150;

    int _brightness;
    int _sign;
};

class LedFire : public LedEffectHandler {
private:
    typedef unsigned char byte;
    byte _heat[LED_COUNT];
    int _offset;
public:
    LedFire();

    void doStep(mgos_neopixel *strip);

private:
    void _setPixelHeatColor(mgos_neopixel *strip, int Pixel, byte temperature);

    int random(int min, int max);
};

class LedColor : public LedEffectHandler {
public:
    LedColor(int r, int g, int b);

    void doStep(mgos_neopixel *strip);

private:
    const int _d = 10;
    const int _min = 30;
    const int _max = 150;

    int _brightness;
    int _sign;

    int _r, _g, _b;
};

class LedEffect {
public:
private:
    mgos_timer_id _timerId;
    mgos_neopixel *_strip;
    LedEffectHandler *_handler;
public:
    void start(LedEffectHandler *handler);

    void stop();

    LedEffect();

    ~LedEffect();

private:
    LedEffect(const LedEffect &);

    static void _timer_cb(void *arg);
};

#endif
