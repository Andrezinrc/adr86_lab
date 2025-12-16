#include "decoder.h"

bool modrm_reg_reg(uint8_t modrm, uint8_t *reg, uint8_t *rm) {
    if ((modrm >> 6) != 3) return false;
    *reg = (modrm >> 3) & 7;
    *rm  = modrm & 7;
    return true;
}
