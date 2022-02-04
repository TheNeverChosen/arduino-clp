#pragma once

enum LedStatus{ST_LED_RUN_IDLE=0, ST_LED_RUN_RECEIVED, ST_LED_RUN_RUNNING};

void set_led_monitor();
void set_led_wsConection(int status);
void set_led_run(enum LedStatus status);