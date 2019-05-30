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

    MatrixConfig *matrixConfig = (MatrixConfig*) malloc(buttonBoxConfig->numMatrixes * sizeof(MatrixConfig));
    matrixConfig[0].rows = 4;
    matrixConfig[0].columns = 4;
    
    u_int8_t *rowPins = (u_int8_t*) malloc(matrixConfig->rows * sizeof(u_int8_t));
    rowPins[0] = 17;
    rowPins[1] = 16;
    rowPins[2] = 15;
    rowPins[3] = 14;
    matrixConfig[0].rowPins = rowPins;

    u_int8_t *columnPins = (u_int8_t*) malloc(matrixConfig->columns * sizeof(u_int8_t));
    columnPins[0] = 21;
    columnPins[1] = 20;
    columnPins[2] = 19;
    columnPins[3] = 18;
    matrixConfig[0].columnPins = columnPins;

    LocationConfiguration *lc = allocLC(13, 1);
    lc[0].inputType = GRAY_ENCODER;
    lc[0].encoderBits = 4;

    lc[1].inputType = BUTTON;
    lc[2].inputType = BUTTON;
    lc[3].inputType = BUTTON;
    lc[4].inputType = BUTTON;
    lc[5].inputType = BUTTON;
    lc[6].inputType = BUTTON;
    lc[7].inputType = BUTTON;
    lc[8].inputType = BUTTON;
    lc[9].inputType = BUTTON;
    lc[10].inputType = BUTTON;
    lc[11].inputType = BUTTON;
    lc[12].inputType = BUTTON;

    matrixConfig[0].locationConfiguration = lc;

    buttonBoxConfig->matrixConfigs[0] = matrixConfig;

    return buttonBoxConfig;
}

#endif 
