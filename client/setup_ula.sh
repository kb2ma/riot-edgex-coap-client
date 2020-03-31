#!/bin/sh
# Sets up networking and runs a terminal for ethos based networking 
# for a RIOT 'usb' flavor network setup.
# The node is on the same network prefix as the external network.
#
# Parameters in order passed in:
# PORT --- serial port, like /dev/ttyACMx
# TAP ---- name of TAP device, like tap0
# PREFIX - external network prefix, like fd00:bbbb

ETHOS_DIR="/home/kbee/dev/riot/repo/dist/tools/ethos"

create_tap() {
    ip tuntap add ${TAP} mode tap user ${USER}
    sysctl -w net.ipv6.conf.${TAP}.forwarding=1
    sysctl -w net.ipv6.conf.${TAP}.accept_ra=0
    ip link set ${TAP} up
    ip a a fe80::1/64 dev ${TAP}
    ip a a ${PREFIX}::1/128 dev lo
}

remove_tap() {
    ip tuntap del ${TAP} mode tap
}

cleanup() {
    echo "Cleaning up..."
    remove_tap
    ip a d ${PREFIX}::1/128 dev lo
    trap "" INT QUIT TERM EXIT
}

PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix> [baudrate]"
    exit 1
}

[ ! -z $4 ] && {
    BAUDRATE=$4
}

trap "cleanup" INT QUIT TERM EXIT


create_tap && "${ETHOS_DIR}/ethos" ${TAP} ${PORT} ${BAUDRATE}
