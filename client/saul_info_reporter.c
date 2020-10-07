/*
 * Copyright (C) 2019 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     app_lwm2m_client
 * @{
 *
 * @file
 * @brief       SAUL based LwM2M information reporter
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdlib.h>
#include <string.h>

#include "net/gcoap.h"
#include "phydat.h"
#include "saul_reg.h"
#include "thread.h"
#include "xtimer.h"
#include "saul_info_reporter.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static saul_reg_t *_saul_dev;
static phydat_t _temp_dat;


#define POW10_MAX ( 4)
#define POW10_MIN (-4)
static int pow10[] = {1, 10, 100, 1000, 10000};

#ifndef DECIMAL_CHAR
#define DECIMAL_CHAR '.'
#endif

ssize_t saul_info_print(int16_t val, int scale, char *val_buf, int buflen) {
    assert(buflen);

    int val_len;
    if (scale >= 0 && scale <= POW10_MAX) {
        val_len = snprintf(val_buf, buflen, "%d", val * pow10[scale]);
    }
    else if (scale < 0 && scale >= POW10_MIN) {
        /* insert decimal point into string representation */
        val_len = snprintf(val_buf, buflen, "%d", val);
        if (val_len > 0) {
            char *decimal_ptr = val_buf + val_len + scale;
            memmove(decimal_ptr + 1, decimal_ptr, abs(scale) + 1);
            *decimal_ptr = DECIMAL_CHAR;
            val_len += 1;
        }
    }
    else {
        val_len = -1;
    }
    return val_len;
}

int saul_info_read(phydat_t *data)
{
    /* take a temperature reading */
#ifdef BOARD_NATIVE
    memcpy(data, &_temp_dat, sizeof(phydat_t));
    data->val[0] += 1;
    int res = 1;
#else
    memset(data, 0, sizeof(phydat_t));
    int res = saul_reg_read(_saul_dev, data);
#endif

    if (res) {
        if (ENABLE_DEBUG) {
            char val_buf[10];
            if (saul_info_print(data->val[0], data->scale, val_buf, 10) > 0) {
                printf("temperature: %s, unit %u\n", val_buf, data->unit);
            }
        }
    }
    else {
        DEBUG("Sensor read failure: %d\n", res);
        return -1;
    }

    _temp_dat.val[0] = data->val[0];
    _temp_dat.scale = data->scale;

    return 0;
}

int saul_info_init(const char *driver)
{
#ifdef BOARD_NATIVE
    (void)driver;
    _saul_dev = NULL;
    _temp_dat.val[0] = 1;
    _temp_dat.scale = 0;
    _temp_dat.unit = UNIT_TEMP_C;
#else
    _saul_dev = saul_reg_find_name(driver);
    if (_saul_dev == NULL) {
        DEBUG("Can't find SAUL reader %s\n", SAUL_INFO_DRIVER);
        return -1;
    }
#endif
    return 0;
}

int saul_info_value(phydat_t *value)
{
    memcpy(value, &_temp_dat, sizeof(phydat_t));
    return 0;
}

