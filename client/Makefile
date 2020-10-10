# name of your application
APPLICATION = lwm2m-client

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT


# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
# Specify the mandatory networking modules
ifeq (1,$(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_netdev
  USEMODULE += lwip_ipv6_autoconfig
  USEMODULE += lwip_sock_ip
  USEMODULE += lwip_sock_udp
  # Activates recv timeout capability
  CFLAGS += -DLWIP_SO_RCVTIMEO
else ifeq (1,$(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_netdev
  USEMODULE += lwip_arp
  USEMODULE += lwip_dhcp_auto
  USEMODULE += lwip_sock_ip
  USEMODULE += lwip_sock_udp
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  # Activates recv timeout capability
  CFLAGS += -DLWIP_SO_RCVTIMEO
else
  # GNRC default
  USEMODULE += gnrc_netdev_default
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_ipv6_default
  USEMODULE += gnrc_sock_udp
  # Additional networking modules that can be dropped if not needed
  USEMODULE += gnrc_icmpv6_echo
endif
USEMODULE += gcoap

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# Include and auto-initialize all available sensors
ifneq (native,$(BOARD))
  USEMODULE += saul_default
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Specific the server address
SERVER_ADDR ?= '"fd00:dead:beef::1"'

ifneq (,$(SERVER_ADDR))
  CFLAGS += -DEDGEX_SERVER_ADDR=$(SERVER_ADDR)
endif

# Path for data URL on EdgeX server
CFLAGS += -DEDGEX_SERVER_PATH='"/a1r/d1/float"'

# Set to 1 to show CLI in terminal; otherwise app starts automatically
CFLAGS += -DEDGEX_CLI_START=0

CFLAGS += -DTHREAD_PRIORITY_MAIN=15

# SAUL driver that measures temperature; hardware dependent
SAUL_DRIVER ?= jc42
ifneq (native,$(BOARD))
  USEMODULE += $(SAUL_DRIVER)
  CFLAGS += -DSAUL_INFO_DRIVER='"$(SAUL_DRIVER)"'
else
  CFLAGS += -DSAUL_INFO_DRIVER=NULL
endif

include $(RIOTBASE)/Makefile.include
