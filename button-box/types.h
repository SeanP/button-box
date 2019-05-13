#ifndef __TYPES_H__
#define __TYPES_H__

enum InputType {
    NOT_CONNECTED,
    BUTTON,
    INCREMENTAL_ENCODER,
    GRAY_ENCODER,
    BINARY_ENCODER // NOT IMPLEMENTED
};

struct EncoderConfig {
    byte bits;
};

struct LocationConfiguration {
    InputType inputType;
    EncoderConfig *encoderConfig;
};

struct MatrixConfig {
    byte rows;
    byte columns;

    byte *rowPins;
    byte *columnPins;

    /* This is an ordered list in row->column order.
     * 
     * Restriction: Encoders must not roll over between rows. If you want to
     *              use a 10-bit encoder, all 10 of those inputs must live on
     *              the same row and should be wired from left to right (in
     *              terms of column pins) according to your encoder's
     *              datasheet.
     */
    LocationConfiguration *locationConfiguration;
};

struct ButtonBoxConfig { 
    byte numMatrixes;
    MatrixConfig *matrixConfigs;
};

byte getButtonCountForLocation(LocationConfiguration lc) {
    switch(lc.inputType) {
        case BUTTON:
            return 1;
        case INCREMENTAL_ENCODER:
            return 2;
        case GRAY_ENCODER:
        case BINARY_ENCODER:
            return 1 << (lc.encoderConfig->bits);
        case NOT_CONNECTED:
        default:
            return 0;
    }
}

byte getOffsetForLocation(LocationConfiguration lc) {
    switch(lc.inputType) {
        case INCREMENTAL_ENCODER:
            return 1;
        case GRAY_ENCODER:
        case BINARY_ENCODER:
            return lc.encoderConfig->bits - 1;
        default:
            return 0;
    }
}

#endif