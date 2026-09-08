#include "TouchBSP.h"

I2CPortStruct TouchI2CPort;

TouchStruct TouchPort;

static bool TouchVendorConstractor(TouchStruct* touch) {
#if USE_COMPONENT_TOUCH_GOODIX
    if (GoodixTouchConstractor(touch))
        return true;
#endif
#if USE_COMPONENT_TOUCH_SITRONIX
    if (SitronixTouchConstractor(touch))
        return true;
#endif
#if USE_COMPONENT_TOUCH_HYNITRON
    if (HynitronTouchConstractor(touch))
        return true;
#endif
#if USE_COMPONENT_TOUCH_CHIPSEMI
    if (ChipsemiTouchConstractor(touch))
        return true;
#endif
#if USE_COMPONENT_TOUCH_ILITEK
    if (IlitekTouchConstractor(touch))
        return true;
#endif
    return false;
}

#define __TOUCH_EXTI_CONTACT_END(exti, suffix) exti##suffix
#define __TOUCH_EXTI_CONTACT(x...)             __TOUCH_EXTI_CONTACT_END(x)

bool TouchSetup(uint16_t x, uint16_t y, bool reverseX, bool reverseY, bool switchXY) {
    // I2CPort
    I2CPortStruct* i2cPort = &TouchI2CPort;
    I2CPortConstractor(i2cPort, TOUCH_I2C_PERIPH, TOUCH_I2C_SCL_IO, TOUCH_I2C_SDA_IO);
    i2cPort->Speed = TOUCH_I2C_SPEED;

    if (!i2cPort->Init(i2cPort))
        return false;

    // Touch
    TouchStruct* touch = &TouchPort;
    TouchConstractor(touch, i2cPort);
    touch->ResetIO = TOUCH_RST_IO;
    touch->IntIO   = TOUCH_INT_IO;

    touch->X = x;
    touch->Y = y;

    touch->ReverseX = reverseX;
    touch->ReverseY = reverseY;
    touch->SwitchXY = switchXY;

    if (!TouchVendorConstractor(touch))
        return false;

    if (!touch->Init(touch))
        return false;

    // Initialize IntIO
    TouchEventInit(touch);

#ifdef TOUCH_INT_EXTI
    // TODO: dynamic set EXTI interrupt
    NVIC_SetPriority(__TOUCH_EXTI_CONTACT(TOUCH_INT_EXTI, IRQn), 4);
    NVIC_EnableIRQ(__TOUCH_EXTI_CONTACT(TOUCH_INT_EXTI, IRQn));
#endif

    return true;
}

#ifdef TOUCH_INT_EXTI
// TODO: dynamic set EXTI interrupt
void __TOUCH_EXTI_CONTACT(TOUCH_INT_EXTI, IRQHandler)(void) {
    TouchEvent(&TouchPort);
}
#endif
