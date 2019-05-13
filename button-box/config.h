#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include "types.h"

LocationConfiguration *allocLC(byte rows, byte columns) {
    return (LocationConfiguration*) malloc(rows * columns * sizeof(LocationConfiguration));
}

ButtonBoxConfig *getButtonBoxConfig() {
    ButtonBoxConfig *buttonBoxConfig = (ButtonBoxConfig*) malloc(sizeof(ButtonBoxConfig));
    buttonBoxConfig->numMatrixes = 1;

    MatrixConfig *matrixConfig = (MatrixConfig*) malloc(sizeof(MatrixConfig));
    matrixConfig->rows = 1;
    matrixConfig->columns = 1;
    
    byte *rowPins = (byte*) malloc(sizeof(byte));
    rowPins[0] = 1;
    matrixConfig->rowPins = rowPins;

    byte *columnPins = (byte*) malloc(sizeof(byte));
    columnPins[0] = 2;
    matrixConfig->columnPins = columnPins;

    LocationConfiguration *lc = allocLC(matrixConfig->rows, matrixConfig->columns);
    lc->inputType = BUTTON;

    return buttonBoxConfig;
}

#endif 