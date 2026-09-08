#ifndef __XS9950_H__
#define __XS9950_H__

#include "Camera.h"

// ------- Config Definitions --------
#ifndef CONFIG_XS9950_LANES
#define CONFIG_XS9950_LANES 1
#endif

#ifndef CONFIG_XS9950_LANE_RATE
#define CONFIG_XS9950_LANE_RATE 1000
#endif

extern bool XS9950Constractor(CameraStruct* camera);

#endif
