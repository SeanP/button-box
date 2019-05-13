#include "types.h"
#include "config.h"

ButtonBoxConfig *buttonBoxConfig;
byte **keys;

void setup() {
#ifdef BUTTON_BOX_DEBUG
    Serial.begin(9600);
#endif

    byte numButtons = 0;

    buttonBoxConfig = getButtonBoxConfig();

    keys = (byte**) malloc(sizeof(byte*) * buttonBoxConfig->numMatrixes);

    for (byte i=0; i < buttonBoxConfig->numMatrixes; ++i) {
        MatrixConfig mc = buttonBoxConfig->matrixConfigs[i];
        byte buttonsInMatrix = 0;
        
        for (byte rowNum = 0; rowNum < mc.rows; ++rowNum) {
            pinMode(mc.rowPins[rowNum], INPUT_PULLUP);
        }

        for (byte columnNum = 0; columnNum < mc.columns; ++columnNum) {
            pinMode(mc.columnPins[columnNum], INPUT);
        }

        for (byte nodeNum=0; nodeNum < mc.rows * mc.columns; ++nodeNum) {
            buttonsInMatrix += getButtonCountForLocation(mc.locationConfiguration[nodeNum]);
            // This is atrocious, and strongly suggests my data structures are
            // wrong. Still, it should work.
            nodeNum += getOffsetForLocation(mc.locationConfiguration[nodeNum]);
        }

        numButtons += buttonsInMatrix;
        keys[i] = (byte*) (malloc(sizeof(byte) * buttonsInMatrix));
    }

    updateState();
}

void loop() {
    updateState();
    sendUpdate();
}

void updateState() {
    for (byte matrixNum=0; matrixNum < buttonBoxConfig->numMatrixes; ++matrixNum) {
        MatrixConfig *mc = &(buttonBoxConfig->matrixConfigs[matrixNum]);
        readMatrix(mc, keys[matrixNum]);
    }
}

/* With thanks to James Lewis of Bald Engineer.
 * https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
 */
void readMatrix(MatrixConfig *mc, byte* matrixKeys) {
    byte keyPos = 0;
    for (byte rowIndex=0; rowIndex < mc->rows; ++rowIndex) {
        byte rowPin = mc->rowPins[rowIndex];
        pinMode(rowPin, OUTPUT);
        digitalWrite(rowPin, LOW);

        for (byte colIndex=0; colIndex < mc->columns; ++colIndex) {
            byte colPin = mc->columnPins[colIndex];
            pinMode(colPin, INPUT_PULLUP);
            matrixKeys[keyPos++] = digitalRead(colPin);
            pinMode(colPin, INPUT);
        }
        pinMode(rowPin, INPUT);
    }
}

void sendUpdate() {

}