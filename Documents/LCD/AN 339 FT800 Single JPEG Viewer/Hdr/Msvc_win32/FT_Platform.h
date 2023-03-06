#ifndef _FT_PLATFORM_H_
#define _FT_PLATFORM_H_

#define MSVC_PLATFORM

//*******************************************
// DISPLAY SELECTION
//*******************************************
//Select either WQVGA or QVGA display #define

#define SAMAPP_DISPLAY_WQVGA

//#define SAMAPP_DISPLAY_QVGA

//*******************************************

#ifdef MSVC_PLATFORM
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include "ftd2xx.h"
#include "LibMPSSE_spi.h"
#endif

#include "FT_DataTypes.h"
#include "FT_Gpu_Hal.h"
#include "FT_Gpu.h"
#include "FT_CoPro_Cmds.h"


#ifdef MSVC_PLATFORM
#define BUFFER_OPTIMIZATION
#define MSVC_PLATFORM_SPI
#endif

#endif /*_FT_PLATFORM_H_*/
/* Nothing beyond this*/




