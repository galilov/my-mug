load('api_config.js');
load('api_rpc.js');
load('api_timer.js');
load('api_adc.js');
load('api_mqtt.js');
load('api_gpio.js')
load('api_neopixel.js')
//load('api_wifiportal.js');

print('*** START ****');
let adcPin = 0; // ADC0
let heaterPin = 13; //D7


let neopixelPin = 5, numPixels = 3, colorOrder = NeoPixel.GRB;

let strip = NeoPixel.create(neopixelPin, numPixels, colorOrder);

strip.clear()
strip.setPixel(0 /* pixel */, 32, 0, 0);
strip.setPixel(1 /* pixel */, 0, 32, 0);
strip.setPixel(2 /* pixel */, 0, 0, 32);
strip.show();


// WifiCaptivePortal.start(); // enabled in mos.yml



let isEnabled = ADC.enable(adcPin);
print('isEnabled', isEnabled);

GPIO.setup_output(heaterPin, 0);
GPIO.set_mode(heaterPin, GPIO.PULL_UP);
let heaterStatus = 0;

Timer.set(1000, true, function() {
  print('ADC:', ADC.read(adcPin));
}, null);

RPC.addHandler('ADC.Read', function(args) {
  return { value: ADC.read(adcPin) };
});

Timer.set(1000, Timer.REPEAT, function() {
  let ok = MQTT.pub('test/adc', JSON.stringify({ input: ADC.read(adcPin)}));
  print('mqtt message sent?', ok);
}, null);

Timer.set(3000, Timer.REPEAT, function() {
  let level = heaterStatus === 0 ? 1 : 0;
  GPIO.write(heaterPin, level);
  print('heater level:', level);
  heaterStatus = level;
}, null);
