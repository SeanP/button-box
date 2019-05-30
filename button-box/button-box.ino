#include "types.h"
#include "config.h"

ButtonBoxConfig *buttonBoxConfig;
u_int8_t **nodes;
u_int8_t **shadowNodes;

u_int8_t numButtons = 0;
u_int8_t *numLocations;

u_int8_t **locationDwellFrames;

u_int8_t GRAY_CODES[] = {
    0,  // 0
    1,  // 1
    3,  // 2
    2,  // 3
    7,  // 4
    6,  // 5
    4,  // 6
    5,  // 7
    15, // 8
    14, // 9
    12, // 10
    13, // 11
    8,  // 12
    9,  // 13
    11, // 14
    10, // 15
};

int ledState = HIGH;
const int UPDATE_HERTZ = 125;
const u_int8_t LED_PIN = 13;
const u_int8_t IRACING_BUTTON_LIMIT = 48;

// #define BUTTON_BOX_DEBUG

#ifdef BUTTON_BOX_DEBUG
#define log(arg) Serial.print(arg)
#define logln(arg) Serial.println(arg)
#else
#define log(arg)
#define logln(arg)
#endif

void setup() {
#ifdef BUTTON_BOX_DEBUG
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
#endif

    logln("Fetching config");
    buttonBoxConfig = getButtonBoxConfig();

    nodes = (u_int8_t**) malloc(sizeof(u_int8_t*) * buttonBoxConfig->numMatrixes);
    shadowNodes = (u_int8_t**) malloc(sizeof(u_int8_t*) * buttonBoxConfig->numMatrixes);
    numLocations = (u_int8_t*) malloc(sizeof(u_int8_t) * buttonBoxConfig->numMatrixes);
    locationDwellFrames = (u_int8_t**) malloc(sizeof(u_int8_t*) * buttonBoxConfig->numMatrixes);

    logln("Initialized buffers");

    for (u_int8_t i=0; i < buttonBoxConfig->numMatrixes; ++i) {
        MatrixConfig *mc = buttonBoxConfig->matrixConfigs[i];
        numLocations[i] = 0;

        logln("Got matrix");

        u_int8_t buttonsInMatrix = 0;
        
        for (u_int8_t rowNum = 0; rowNum < mc->rows; ++rowNum) {
            pinMode(mc->rowPins[rowNum], INPUT_PULLUP);
            log("Configure pin as INPUT_PULLUP: ");
            logln(mc->rowPins[rowNum]);
        }

        logln("Finished row pin initialization");

        for (u_int8_t columnNum = 0; columnNum < mc->columns; ++columnNum) {
            pinMode(mc->columnPins[columnNum], INPUT);
            log("Configure pin as INPUT: ");
            logln(mc->columnPins[columnNum]);
        }

        logln("Finished column pin initialization");

        u_int8_t numNodes = mc->rows * mc->columns;
        for (u_int8_t nodeNum=0; nodeNum < numNodes; ++nodeNum) {
            buttonsInMatrix += getButtonCountForLocation(& (mc->locationConfiguration[nodeNum]));
            // This is atrocious, and strongly suggests my data structures are
            // wrong. Still, it should work.

            nodeNum += getNodeOffsetForLocation(& (mc->locationConfiguration[nodeNum]));
            ++numLocations[i];
        }

        log("Counted the buttons in the matrix: ");
        logln(buttonsInMatrix);

        numButtons += buttonsInMatrix;
        nodes[i] = (u_int8_t*) (malloc(sizeof(u_int8_t) * buttonsInMatrix));
        shadowNodes[i] = (u_int8_t*) (malloc(sizeof(u_int8_t) * buttonsInMatrix));
        locationDwellFrames[i] = (u_int8_t*) malloc(sizeof(u_int8_t) * numLocations[i]);

        for (u_int8_t locNum = 0; locNum < numLocations[i]; ++locNum) {
            locationDwellFrames[i][locNum] = 0;
        }
    }

    logln("Initialized matrixes.");

#ifdef BUTTON_BOX_DEBUG
    if (IRACING_BUTTON_LIMIT < numButtons) {
        logln("Too many buttons configured!");
    }
#endif

    updateState();
    swapNodeBuffers();
    initJoystick();

    // Light up the LED if we successfully configured.
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);
}

long interval = 1000/UPDATE_HERTZ; 
unsigned long previousMillis = 0;
void loop() {
    
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis < interval) {
        return;
    }
    previousMillis = currentMillis;

    logln("===================================");
    updateState();
    buildUpdate();

    Joystick.send_now();

    swapNodeBuffers();
    log("Time=");
    log(millis()-currentMillis);
    logln("ms");
}

void updateState() {
    for (u_int8_t matrixNum=0; matrixNum < buttonBoxConfig->numMatrixes; ++matrixNum) {
        log("Matrix:\t");
        logln(matrixNum);
        MatrixConfig *mc = buttonBoxConfig->matrixConfigs[matrixNum];
        readMatrix(mc, nodes[matrixNum]);
    }
}

/* With thanks to James Lewis of Bald Engineer.
 * https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
 */
void readMatrix(MatrixConfig *mc, u_int8_t* matrixKeys) {
    u_int8_t keyPos = 0;
    for (u_int8_t rowIndex=0; rowIndex < mc->rows; ++rowIndex) {
        u_int8_t rowPin = mc->rowPins[rowIndex];
        pinMode(rowPin, OUTPUT);
        digitalWrite(rowPin, LOW);

        for (u_int8_t colIndex=0; colIndex < mc->columns; ++colIndex) {
            u_int8_t colPin = mc->columnPins[colIndex];
            pinMode(colPin, INPUT_PULLUP);
            delayMicroseconds(10);
            matrixKeys[keyPos] = 1^digitalRead(colPin);
            log("Row[");
            log(rowIndex);
            log("], Column[");
            log(colIndex);
            log("] reads as ");
            logln(matrixKeys[keyPos]);
            ++keyPos;
            pinMode(colPin, INPUT);
        }
        pinMode(rowPin, INPUT);
    }
}

void buildUpdate() {
    u_int8_t buttonPos = 0;

    for (u_int8_t matrixNum = 0; matrixNum < buttonBoxConfig->numMatrixes; ++matrixNum) {
        MatrixConfig *mc = buttonBoxConfig->matrixConfigs[matrixNum];
        u_int8_t *node = nodes[matrixNum];
        u_int8_t *shadowNode = shadowNodes[matrixNum];
        for (u_int8_t nodeNum = 0, locationNum = 0;
                locationNum < numLocations[matrixNum];
                ++nodeNum, ++locationNum) {
            LocationConfiguration *lc = &(mc->locationConfiguration[locationNum]);
            switch (lc->inputType) {
                case BUTTON:
                    log("Node[");
                    log(nodeNum);
                    log("]\tbutton state\tnew: ");
                    log(node[nodeNum]);
                    log("\told: ");
                    logln(shadowNode[nodeNum]);
                    if (node[nodeNum] != shadowNode[nodeNum]) {
                        log("*** UPDATE BUTTON ");
                        log(nodeNum);
                        log(" TO NEW STATE ");
                        log(node[nodeNum]);
                        logln(" ***");
                        Joystick.button(buttonPos + 1, node[nodeNum]);
                    }
                    break;
                case INCREMENTAL_ENCODER:
                    {
                        if (multiNodeNoChange(&node[nodeNum], &shadowNode[nodeNum], 2)) {
                            Joystick.button(buttonPos + 1, 0);
                            Joystick.button(buttonPos + 2, 0);
                        } else {
                            u_int8_t currentLocation = resolveQuadrature(&node[nodeNum]);
                            u_int8_t oldLocation = resolveQuadrature(&shadowNode[nodeNum]);
                            int8_t direction = currentLocation - oldLocation;
                            if (-3 == direction || 1 == direction) {
                                Joystick.button(buttonPos + 1, 0);
                                Joystick.button(buttonPos + 2, 1);
                            } else if (3 == direction || -1 == direction) {
                                Joystick.button(buttonPos + 1, 1);
                                Joystick.button(buttonPos + 2, 0);
                            } else {
                                logln("UNHANDLED DIRECTION");
                            }
                        }
                    }
                    break;
                case GRAY_ENCODER:
                    {
                        if (0 == --locationDwellFrames[matrixNum][locationNum]) {
                            // Reset it.
                            for (u_int8_t i = 0; i < getButtonCountForLocation(lc); ++i) {
                                Joystick.button(buttonPos + 1 + i, 0);
                            }
                        }

                        u_int8_t currentLocation = resolveGrayCode(&node[nodeNum]);
                        u_int8_t oldLocation = resolveGrayCode(&shadowNode[nodeNum]);

                        // If it moved, hit the switch
                        if (currentLocation != oldLocation) {
                            // Reset the encoder.
                            for (u_int8_t i = 0; i < getButtonCountForLocation(lc); ++i) {
                                Joystick.button(buttonPos + 1 + i, 0);
                            }

                            // Set dwell time.
                            locationDwellFrames[matrixNum][locationNum] = 5;

                            // Hit the button.
                            Joystick.button(buttonPos + 1 + currentLocation, 1);
                        }
                    }
                    break;
                default:
                    break;
            }

            buttonPos += getButtonCountForLocation(lc);
            nodeNum += getNodeOffsetForLocation(lc);
        }
    }
}

void swapNodeBuffers() {
    u_int8_t **oldNodes = shadowNodes;
    shadowNodes = nodes;
    nodes = oldNodes;
}

bool multiNodeNoChange(u_int8_t node[], u_int8_t shadowNode[], size_t bits) {
    bool noChange = true;
    for (size_t offset = 0; offset < bits; ++offset) {
        noChange &= node[offset] == shadowNode[offset];
    }
    return noChange;
}

u_int8_t resolveGrayCode(u_int8_t node[]) {
    return GRAY_CODES[node[0] + (node[1] << 1) + (node[2] << 2) + (node[3] << 3)];
}

u_int8_t resolveBinaryCode() {
    return 0;
}

u_int8_t resolveQuadrature(u_int8_t node[]) {
    if (0 == node[1]) {
        return node[0];
    } else {
        return 2 + (1^node[0]);
    }
}

void initJoystick() {
    Joystick.useManualSend(true);
    for (u_int8_t i = 1; i <= 4; ++i) {
        Joystick.hat(i, -1);
    }

    for (u_int8_t i = 1; i <=17 ; ++i) {
        Joystick.slider(i, 0);
    }

    Joystick.X(0x7FFF);
    Joystick.Y(0x7FFF);
    Joystick.Z(0x7FFF);
    Joystick.Xrotate(0x7FFF);
    Joystick.Yrotate(0x7FFF);
    Joystick.Zrotate(0x7FFF);
    Joystick.send_now();
}


u_int8_t getButtonCountForLocation(LocationConfiguration *lc) {
    switch(lc -> inputType) {
        case BUTTON:
            return 1;
        case INCREMENTAL_ENCODER:
            return 2;
        case GRAY_ENCODER:
        case BINARY_ENCODER:
            return 1 << (lc->encoderBits);
        case NOT_CONNECTED:
        default:
            return 0;
    }
}

u_int8_t getNodeOffsetForLocation(LocationConfiguration *lc) {
    switch(lc -> inputType) {
        case INCREMENTAL_ENCODER:
            return 1;
        case GRAY_ENCODER:
        case BINARY_ENCODER:
            return lc->encoderBits - 1;
        default:
            return 0;
    }
}
