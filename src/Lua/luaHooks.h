#pragma once

#include "helpers.h"
extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include "luaLE.h"

void runFormatter(const char *formatter,
                  void *object,
                  int16_t value,
                  char *buffer,
                  int maxLength);
void runFunction(const char *function, void *object, int16_t value);
uint8_t runTemplateFunction(const char *function, void *object, int16_t value);
