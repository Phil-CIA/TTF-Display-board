/**
 * include/screen_manager.h — Screen state machine declarations
 */
#pragma once

#include "hmi_state.h"

extern void screen_manager_init();
extern void screen_manager_load(ScreenId id);
extern ScreenId screen_manager_get_active();
