#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include "types.h"

LocationConfiguration *allocLC(u_int8_t rows, u_int8_t columns) {
    return (LocationConfiguration*) malloc(rows * columns * sizeof(LocationConfiguration));
}

ButtonBoxConfig *getButtonBoxConfig() {
    ButtonBoxConfig *buttonBoxConfig = (ButtonBoxConfig*) malloc(sizeof(ButtonBoxConfig));
    buttonBoxConfig->numMatrixes = 2;
    buttonBoxConfig->matrixConfigs = (MatrixConfig**) malloc(sizeof(MatrixConfig*));

    MatrixConfig *matrixConfig = (MatrixConfig*) malloc(2 * sizeof(MatrixConfig));
    matrixConfig[0].rows = 1;
    matrixConfig[0].columns = 4;
    matrixConfig[1].rows = 1;
    matrixConfig[1].columns = 4;
    
    u_int8_t *rowPins = (u_int8_t*) malloc(matrixConfig->rows * sizeof(u_int8_t));
    rowPins[0] = 31;
    rowPins[1] = 32;
    matrixConfig[0].rowPins = &(rowPins[0]);
    matrixConfig[1].rowPins = &(rowPins[1]);

    u_int8_t *columnPins = (u_int8_t*) malloc(matrixConfig->columns * sizeof(u_int8_t));
    columnPins[0] = 36;
    columnPins[1] = 35;
    columnPins[2] = 34;
    columnPins[3] = 33;
    matrixConfig[0].columnPins = columnPins;
    matrixConfig[1].columnPins = columnPins;

    LocationConfiguration *lc = allocLC(5, 1);
    lc[0].inputType = GRAY_ENCODER;
    lc[0].encoderBits = 4;

    lc[1].inputType = BUTTON;
    lc[2].inputType = BUTTON;
    lc[3].inputType = BUTTON;
    lc[4].inputType = BUTTON;
    // lc[1].inputType = BUTTON;

    matrixConfig[0].locationConfiguration = & (lc[0]);
    matrixConfig[1].locationConfiguration = & (lc[1]);
    
    buttonBoxConfig->matrixConfigs[0] = &matrixConfig[0];
    buttonBoxConfig->matrixConfigs[1] = &matrixConfig[1];

    return buttonBoxConfig;
}

#endif 