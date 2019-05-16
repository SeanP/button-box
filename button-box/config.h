#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include "types.h"

LocationConfiguration *allocLC(u_int8_t rows, u_int8_t columns) {
    return (LocationConfiguration*) malloc(rows * columns * sizeof(LocationConfiguration));
}

ButtonBoxConfig *getButtonBoxConfig() {
    ButtonBoxConfig *buttonBoxConfig = (ButtonBoxConfig*) malloc(sizeof(ButtonBoxConfig));
    buttonBoxConfig->numMatrixes = 1;
    buttonBoxConfig->matrixConfigs = (MatrixConfig**) malloc(sizeof(MatrixConfig*));

    MatrixConfig *matrixConfig = (MatrixConfig*) malloc(sizeof(MatrixConfig));
    matrixConfig->rows = 1;
    matrixConfig->columns = 1;
    
    u_int8_t *rowPins = (u_int8_t*) malloc(sizeof(u_int8_t));
    rowPins[0] = 1;
    matrixConfig->rowPins = rowPins;

    u_int8_t *columnPins = (u_int8_t*) malloc(sizeof(u_int8_t));
    columnPins[0] = 2;
    matrixConfig->columnPins = columnPins;

    LocationConfiguration *lc = allocLC(matrixConfig->rows, matrixConfig->columns);
    lc->inputType = BUTTON;
    
    buttonBoxConfig->matrixConfigs[0] = matrixConfig;

    return buttonBoxConfig;
}

#endif 