#pragma once

#include "Globals.hpp"
#include "Log.hpp"

void workerCycle(void);
Event pullEvent(void);
void handleEvent(Event event);