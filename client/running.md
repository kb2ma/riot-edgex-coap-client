# Running EdgeX Temperature Device

## Local Leshan networking

### Single board client via WiFi (Internet networking)
Setup for a single physical board client node connected via WiFi to a server node in the cloud. I use ESP-12x (8266) Adafruit Feather board because it has WiFi on board, and so does not require a USB connection to an Internet gateway. This approach means we expect the WiFi access point to provide IPv4 Internet connectivity. See `build.sh` for the `lwip4.inet` case.
```
lwm2m-client node
board esp-12x
routeable IPv6 addr (auto-assigned)
   |
  WiFi/Internet
   |
Leshan node
cloud instance (Linux)
routeable cloud address (see SERVER_ADDR below)
```

Must define environment variables for `Makefile`:

```
export RIOT_WIFI_SSID="<ssid>"
export RIOT_WIFI_PASS="<passphrase>"
export SERVER_ADDR=\\\"<server addr>\\\"
```
