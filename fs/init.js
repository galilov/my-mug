load('api_config.js');
load('api_rpc.js');
load('api_timer.js');
load('api_adc.js');
load('api_mqtt.js');

print('*** START ****');
let pin = 0;
let isEnabled = ADC.enable(pin);

print('isEnabled', isEnabled);

Timer.set(1000, true, function() {
  print('ADC:', ADC.read(pin));
}, null);

RPC.addHandler('ADC.Read', function(args) {
  return { value: ADC.read(pin) };
});

Timer.set(1000, Timer.REPEAT, function() {
  let ok = MQTT.pub('test/adc', JSON.stringify({ input: ADC.read(pin)}));
  print('mqtt message sent?', ok);
}, null);
