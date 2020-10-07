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
 * @brief       EdgeX device CLI
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#ifndef EDGEX_CLI_H
#define EDGEX_CLI_H

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts the CLI thread.
 *
 * Runs at a lower priority than the main EdgeX device thread.
 *
 * @param edgex_pid PID for the main EdgeX device thread
 *
 * @return 0 if thread started OK
 * @return -EALREADY if CLI thread already started
 */
int edgex_cli_start(kernel_pid_t device_pid);

#ifdef __cplusplus
}
#endif

#endif /* EDGEX_CLI_H */
/** @} */
