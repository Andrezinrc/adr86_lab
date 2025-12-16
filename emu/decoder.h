#pragma once
#include <stdint.h>
#include <stdbool.h>

bool modrm_reg_reg(uint8_t modrm, uint8_t *reg, uint8_t *rm);
