/*
 * Copyright (c) 2020 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     app_edgex_device
 * @{
 *
 * @file
 * @brief       RIOT native EdgeX device
 *
 * Main entry point for the EdgeX device application.
 * 
 * Start the app via the 'edgex start' command from the CLI.
 *
 * Handles a post request to /a1r/d1/temp for the temperature value.
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#ifndef EDGEX_DEVICE_H
#define EDGEX_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    States for the client
 * @{
 */
#define DEVICE_STATE_INIT        (0)     /**< Initialized */
#define DEVICE_STATE_INFO_OK     (1)     /**< SAUL info report up to date */
#define DEVICE_STATE_INFO_RENEW  (2)     /**< SAUL info report required */
#define DEVICE_STATE_INIT_FAIL  (-1)     /**< Initialization failed */
/** @} */

/**
 * @brief Provides the current state of the device.
 *
 * Useful for testing.
 *
 * @return DEVICE_STATE... macro value
 */
int edgex_device_state(void);

#ifdef __cplusplus
}
#endif

#endif /* EDGEX_DEVICE_H */
/** @} */
