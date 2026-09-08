#include "DisplayBSP.h"

DisplayStruct* DisplaySelectSetup(uint32_t* buffer, uint32_t size) {
    if (DPIDisplaySetup(buffer, size))
        return &DPIDisplay;

    return NULL;
}
