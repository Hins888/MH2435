#include "App.h"

int main(void) {
    RetargetIOSetup(CONFIG_RETARGETIO_DEFAULT_SERIAL);

    USBSetup();

    while (1) {
        USBLoop();
    }
}
