#include "App.h"

#define SUPPORT_CAMERA_PORTS 1

static uint32_t* DisplayBuffer     = (uint32_t*)SDRAM_MEM_BASE;
static uint32_t  DisplayBufferSize = 0x300000;

I2CPortStruct I2CPort0;
#ifdef I2C1_PERIPH
I2CPortStruct I2CPort1;
#endif

DisplayStruct*      display = NULL;
DisplayLayerStruct* layer   = NULL;

static volatile bool isCSIEndSync = false;

static void displayChangeBufferEvent(DisplayLayerStruct* layer) {
    __NOP();
}

static void CSISyncEvent(CameraStruct* camera, bool isStartSync) {
    // This function is called by interrupt, do not add large code here.
    if (!isStartSync) {
        isCSIEndSync = true;
    }
}

void CSICameraTask(void) {
    // frame refresh task
    if (!isCSIEndSync)
        return;
    CameraStruct* camera = &CSICamera;

    if (display->Port == DisplayPortDBI)
        display->Refresh(display, display->Layer->Buffer->Pointer, display->HorizontalActive, display->VerticalActive, 0, 0, NULL);

    if (layer != NULL) {
        void* nextBuffer = (void*)layer->CurrentBuffer;
        layer->ChangeBuffer(layer, NULL, displayChangeBufferEvent);
        camera->CSIHandle->MemoryBuffer = nextBuffer;
    }
    // Task End, resume CSI Port
    isCSIEndSync = false;
    if (camera->IsSnapshot)
        camera->Start(camera, true);
}

bool CSICameraConfig(CameraStruct* camera) {
    /* Snapshot mode: */
    camera->IsSnapshot   = true;
    camera->SyncCallback = CSISyncEvent;

    if (layer != NULL) {
        camera->MemoryColor = layer->Color == DisplayColorRGB565 ? CameraColorRGB565 : CameraColorRGB888;

        camera->CropWindow.X = camera->Window.X;
        camera->CropWindow.Y = camera->Window.Y;

        if (camera->CropWindow.X > layer->Width)
            camera->CropWindow.X = layer->Width;

        uint16_t cameraMaxHeight = layer->Height / SUPPORT_CAMERA_PORTS;

        if (camera->CropWindow.Y > cameraMaxHeight)
            camera->CropWindow.Y = cameraMaxHeight;

        // Memory Window only avaliabled in CSI-IPI
        camera->MemoryWindow.XOffset = 0;
        camera->MemoryWindow.YOffset = 0;
        camera->MemoryWindow.X       = layer->Width;
        camera->MemoryWindow.Y       = layer->Height / SUPPORT_CAMERA_PORTS;
    }

    return true;
}

int main() {
	SystemDelay(500);
    SDRAMSetup();

    // select and setup an available display interface
    display = DisplaySelectSetup(DisplayBuffer, DisplayBufferSize);

    if (display) {
#if CONFIG_BOARD
        DisplayTestPattern(display);
#endif
        display->Start(display, true);
        layer = display->Layer;
    }

    I2CPortStruct* i2cPort0 = &I2CPort0;
    I2CPortConstractor(i2cPort0, I2C0_PERIPH, I2C0_SCL_IO, I2C0_SDA_IO);
    if (!i2cPort0->Init(i2cPort0)) {
        while (1) {}
    }

#ifdef I2C1_PERIPH
    I2CPortStruct* i2cPort1 = &I2CPort1;
    I2CPortConstractor(i2cPort1, I2C1_PERIPH, I2C1_SCL_IO, I2C1_SDA_IO);
    if (!i2cPort1->Init(i2cPort1)) {
        while (1) {}
    }
#endif

    uint32_t* cameraBuffer = layer->Buffer[0].Pointer;

    if (CSICameraSetup(CSI_I2C, cameraBuffer)) {
        cameraBuffer += layer->Width * layer->Height * layer->Color >> 3;
    }

    while (1) {
        CSICameraTask();
    }
}
