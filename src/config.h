#ifndef CONFIG_H_
#define CONFIG_H_

#include "inc.h"

typedef struct {
    u32 tick_rate;
} Config;

extern Config config;

#endif
