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
    u_int8_t bits;
};

struct LocationConfiguration {
    InputType inputType;
    EncoderConfig *encoderConfig;
};

struct MatrixConfig {
    u_int8_t rows;
    u_int8_t columns;

    u_int8_t *rowPins;
    u_int8_t *columnPins;

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
    u_int8_t numMatrixes;
    MatrixConfig **matrixConfigs;
};

#endif