#pragma once

#include "connect.h"

void led_monitor_reset();
void led_monitor_set_wsConnection(bool status);
void led_monitor_set_run(enum PlcRunStates status);