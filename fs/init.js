load('api_config.js');
load('api_rpc.js');
load('api_timer.js');
load('api_config.js');
load('api_events.js');
load('api_gcp.js');
load('api_gpio.js');
load('api_mqtt.js');
load('api_timer.js');
load('api_sys.js');

print('*** START ****');

let heaterPin = 13; //D7

// WifiCaptivePortal.start(); // enabled in mos.yml

let getTemperature = ffi('int getTemperature()');
let isTemperatureError = ffi('bool isTemperatureError(int)');
let _showLedColor = ffi('void showLedColor(int, int, int)');
let _hideLeds = ffi('void hideLeds()');
let _showLedFire = ffi('void showLedFire()');

let online = false;
let timerId;

let toipicState = '/devices/' + Cfg.get('device.id') + '/state';
let topicConfig = '/devices/' + Cfg.get('device.id') + '/config';
let topicCommands = '/devices/' + Cfg.get('device.id') + '/commands';

GPIO.setup_output(heaterPin, 0);
GPIO.set_mode(heaterPin, GPIO.PULL_UP);
let heaterStatus = 0;
let powerStatus = 0;
let targetTemperature = -100;

let prevLedState = '';
let currLedState = '';

function hideLeds() {
    prevLedState = currLedState;
    currLedState = 'hide';
    if (prevLedState === currLedState) return;
    _hideLeds();
}

function showLedFire() {
    prevLedState = currLedState;
    currLedState = 'fire';
    if (prevLedState === currLedState) return;
    _showLedFire();
}

function showLedColor(r, g, b) {
    prevLedState = currLedState;
    currLedState = chr(r) + ':' + chr(g) + ':' + chr(b);
    if (prevLedState === currLedState) return;
    _showLedColor(r, g, b);
}

function showLedState() {
    if (heaterStatus === 1) {
        showLedFire();
    } else {
        if (online) {
            showLedColor(0, 128, 0);
        } else {
            showLedColor(128, 64, 0);
        }
    }
}

Timer.set(1000, Timer.REPEAT, function () {
    let t = getTemperature();
    let isError = isTemperatureError(t);
    let d = heaterStatus === 1 ? 7 : -2;
    heaterStatus = (powerStatus === 1 && !isError && t < targetTemperature + d) ? 1 : 0;
    GPIO.write(heaterPin, heaterStatus);
    showLedState();
}, null);


Event.on(Event.CLOUD_CONNECTED, function () {
    print('Cloud CONNECTED');
    online = true;
    showLedState();
    timerId = Timer.set(10000 /* milliseconds */, Timer.REPEAT, function () {
        let t = getTemperature();
        let isError = isTemperatureError(t);
        let msg = JSON.stringify({status: isError ? "ERR" : "OK", temperature: t});
        print(toipicState, '->', msg);
        MQTT.pub(toipicState, msg, 0);
    }, null);
}, null);

Event.on(Event.CLOUD_DISCONNECTED, function () {
    print('Cloud DISCONNECTED');
    Timer.del(timerId);
    online = false;
    showLedState();
}, null);


MQTT.sub(topicCommands, function (conn, topic, msg) {
    print('Topic:', topic, 'message:', msg);
    let obj = JSON.parse(msg) || {power: 0, temperature: -100};
    powerStatus = obj.power;
    targetTemperature = obj.temperature;
}, null);
