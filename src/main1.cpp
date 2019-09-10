#include "LedEffect.h"

extern "C" void showLedColor(int r, int g, int b);
extern "C" void hideLeds(void);
extern "C" void showLedFire(void);

LedRgbBlink blink;
LedFire fire;
LedColor *ledColor = 0;
LedEffect effect;

void showLedColor(int r, int g, int b) {
    hideLeds();
    ledColor = new LedColor(r, g, b);
    effect.start(ledColor);
}

void showLedFire() {
    hideLeds();
    effect.start(&fire);
}

void hideLeds(void) {
    effect.stop();
    if (ledColor != 0) {
        delete ledColor;
        ledColor = 0;
    }
}
