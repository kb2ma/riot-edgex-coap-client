# RIOT native EdgeX temperature app

This application is a RIOT native EdgeX device to read a temperature sensor. It uses [gcoap](http://doc.riot-os.org/group__net__gcoap.html) for CoAP messaging and [SAUL](http://doc.riot-os.org/group__sys__saul__reg.html) to read a temperature sensor. The temperature value is pushed to EdgeX as the `/a1r/{device-name}/float` resource, and the device name defaults to `d1`.

See the RIOT [JC42 driver](https://github.com/RIOT-OS/RIOT/tree/master/drivers/jc42) for a suitable SAUL temperature reader. The Microchip MCP9808 sensor supports this standard, and for example Adafruit provides a convenient [breakout board](https://www.adafruit.com/product/1782) for development.

The app also can be run with a "native" board without a SAUL sensor. In this case the temperature value simply increases with each measurement.


## Setup
The server address is defined in the Makefile as `SERVER_ADDR`, and the resource path is defined as `EDGEX_SERVER_PATH`. The SAUL driver is defined as `SAUL_DRIVER`, and defaults to "jc42".

The measurement interval is defined in `saul_info_reporter.h` as `SAUL_INFO_INTERVAL`, and defaults to 60 seconds. 


## Compile and run

Review and modify `Makefile` as required. The shell CLI is disabled by default so the app may be run without a terminal. See the definition of `EDGEX_CLI_START`.

Then compile and run the client with:

```shell
    BOARD=<board> make clean all flash
```

If the CLI is enabled, add the `term` target to the command above.

## Shell commands, etc.

- `edgex start`: Registers with the LwM2M server and starts taking temperature measurements
- `edgex state`: Debugging tool to show current application state

## Debugging
Turn on DEBUG in `edgex_device.c` to follow state changes.
