/**
 * 参考
 * http://blog.goo.ne.jp/jh3kxm/e/0a71a45afbed826346d605347b83ded0
 * KEY定数一覧
 * https://github.com/PaulStoffregen/cores/blob/master/teensy/keylayouts.h
 * Keypad.h
 * https://github.com/Chris--A/Keypad/blob/master/src/Keypad.h
 * Key.h
 * https://github.com/Chris--A/Keypad/blob/master/src/Key.h
 *
 *
 * sw1 => a       , sw2 => cmd + v
 * sw3 => cmd + c , sw4 => cmd + z
 */
#include <Keypad.h>
#include <EEPROM.h>

// レイヤー表示用LEDピン
const int ledL1 = 14;
const int ledL2 = 15;
const int ledPin = 13;

// mac
//const int CMD_KEY = MODIFIERKEY_GUI;
// win
const int CMD_KEY = MODIFIERKEY_CTRL;

const int KEY_LAYER = -1;
const int KEY_CMD_A = -2;
const int KEY_CMD_C = -3;
const int KEY_CMD_X = -4;
const int KEY_CMD_V = -5;
const int KEY_CMD_ALT_DEL = -6;

// LIST_MAX = 10
const byte ROWS = 1; //four rows
const byte COLS = 6; //three columns
const byte LAYER_COUNT = 2;

// int layer_count = 0;
int key_maps[LAYER_COUNT][COLS] = {
        {KEY_CMD_A,KEY_CMD_C, KEY_CMD_X, KEY_CMD_V, KEY_CMD_ALT_DEL, KEY_LAYER},
        {KEY_MEDIA_PREV_TRACK, KEY_MEDIA_PLAY_PAUSE, KEY_MEDIA_NEXT_TRACK, KEY_MEDIA_VOLUME_DEC, KEY_MEDIA_VOLUME_INC, KEY_LAYER}
};
// ここは実際のキーでは無く、マッピング用のcharをセット
// あとで、charとマッピングリストを照らし合わせて、実際にpressするキーコードを実行する
char keys[ROWS][COLS] = {
        {'1','2','3','4', '5','6'}
};

byte rowPins[ROWS] = {5};
byte colPins[COLS] = {6,7,8,9,10,11};

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void ready() {
        pinMode(ledPin, OUTPUT);
        pinMode(ledL1, OUTPUT);
        pinMode(ledL2, OUTPUT);

        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);

        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
        digitalWrite(ledPin, HIGH);
}

void setup() {
        Serial.begin(9600);
        ready();
        updateLayer();

}

void toggleLayer() {
        if (getLayer() == 0) {
                setLayer(1);
        } else {
                setLayer(0);
        }
        updateLayer();
}

void updateLayer() {
        if (getLayer() == 0) {
                digitalWrite(ledL1, HIGH);
                digitalWrite(ledL2, LOW);
        } else {
                digitalWrite(ledL2, HIGH);
                digitalWrite(ledL1, LOW);
        }

}

void loop() {

        detectKeys();
}

void detectKeys() {
        if (kpd.getKeys()) {

                for (int i=0; i<LIST_MAX; i++) {
                        if ( kpd.key[i].stateChanged ) {
                                int input_key_code = kpd.key[i].kcode;
                                switch (kpd.key[i].kstate) {
                                case PRESSED:
                                        printk(input_key_code);
                                        pressKey(input_key_code);
                                        break;
                                case HOLD:
                                        break;
                                case RELEASED:
                                        // Keyboard.release(output_key_code);
                                        releaseKey(input_key_code);
                                        break;
                                case IDLE:
                                        break;
                                }
                        }
                }

        }
}

void printk(int _kcode) {
        int col = _kcode % COLS;
        // int row = _kcode / COLS;

        int fix_key_code = key_maps[getLayer()][col];
        Serial.println(">>>>>>>>>>>>>");
        Serial.print("_kcode:");
        Serial.println(_kcode);
        Serial.print("col:");
        Serial.println(col);
        Serial.print("fix_key_code:");
        Serial.println(fix_key_code);
        byte hoge = getLayer();
        Serial.println(">>>>>>>>>>>>>");
        Serial.print("getLayer:");
        Serial.println(hoge);

}

void pressKey(int _kcode) {
        int col = _kcode % COLS;
        // int row = _kcode / COLS;
        int fix_key_code = key_maps[getLayer()][col];
        switch (fix_key_code) {
        case KEY_LAYER:
                toggleLayer();
                break;
        case KEY_CMD_A:
                exec_macro(KEY_A);
                break;
        case KEY_CMD_C:
                exec_macro(KEY_C);
                break;
        case KEY_CMD_V:
                exec_macro(KEY_V);
                break;
        case KEY_CMD_X:
                exec_macro(KEY_X);
                break;
        case KEY_CMD_ALT_DEL:
                exec_hyper();
                break;
        default:
                Keyboard.press(fix_key_code);
                break;
        }
}
void releaseKey(int _kcode) {
        int col = _kcode % COLS;
        // int row = _kcode / COLS;
        int fix_key_code = key_maps[getLayer()][col];
        switch (fix_key_code) {
        case KEY_LAYER:
                break;
        case KEY_CMD_A:
                break;
        case KEY_CMD_C:
                break;
        case KEY_CMD_V:
                break;
        case KEY_CMD_X:
                break;
        case KEY_CMD_ALT_DEL:
                break;
        default:
                Keyboard.release(fix_key_code);
                break;
        }
}

// cmd + alt + del
void reset_macro() {
        Keyboard.set_modifier(0);
        Keyboard.set_key1(0);
        Keyboard.send_now();
}
void exec_hyper() {
        // cmd + shift + ctrl + o
        // Keyboard.set_modifier(CMD_KEY | MODIFIERKEY_SHIFT | MODIFIERKEY_CTRL);
        // Keyboard.set_key1(KEY_O);

//
// press and hold CTRL
        Keyboard.set_modifier(CMD_KEY);
        Keyboard.send_now();

// press ALT while still holding CTRL
        Keyboard.set_modifier(CMD_KEY | MODIFIERKEY_ALT);
        Keyboard.send_now();

// press DELETE, while CLTR and ALT still held
        Keyboard.set_key1(KEY_DELETE);
        Keyboard.send_now();
        reset_macro();

}
void exec_macro(int _kcode) {
        Keyboard.set_modifier(CMD_KEY);
        Keyboard.set_key1(_kcode);
        Keyboard.send_now();
        reset_macro();
}
/************** メモリ書き込み *********/
// 書き込みポインタ
const int ADDR = 999;

byte getLayer() {
        byte val = EEPROM.read(ADDR);
        if (val == 255) {
                val = 0;
        }
        return val;
}
void setLayer(byte _layer) {
        EEPROM.write(ADDR, _layer);
}

