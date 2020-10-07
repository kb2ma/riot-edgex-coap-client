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
 * @author      Ken Bannister <kb2ma@runbox.com>
 * @}
 */

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "net/gcoap.h"
#include "fmt.h"
#include "edgex_cli.h"
#include "edgex_device.h"
#include "phydat.h"
#include "saul_info_reporter.h"
#include "thread.h"
#include "timex.h"

/*
 * Device internal variables
 */
typedef struct {
    int state;                    /* current state, a LWM2M_STATE_* macro */
    uint32_t next_info_time;      /* time for next SAUL information report,
                                   * in seconds */
    sock_udp_ep_t server_ep;      /* EdgeX server endpoint */
} edgex_device_t;

static edgex_device_t _device = {
    .state = DEVICE_STATE_INIT,
    .next_info_time = 0
};

int edgex_device_state(void)
{
    return _device.state;
}

/*
 * Initialize server address/endpoint from EDGEX_SERVER_ADDR.
 *
 * @return 0 on success
 * @return <0 if can't create address
 */
static int _init_server_addr(void)
{
#ifdef SOCK_HAS_IPV6
    _device.server_ep.family = AF_INET6;
#else        
    _device.server_ep.family = AF_INET;
#endif

#ifdef MODULE_GNRC_SOCK_UDP
    /* parse for interface specifier, like %1 */
    char *iface = ipv6_addr_split_iface(EDGEX_SERVER_ADDR);
    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            _device.server_ep.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            _device.server_ep.netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            DEBUG("lwm2m: interface not valid\n");
            return -1;
        }
        _device.server_ep.netif = pid;
    }
#else
    _device.server_ep.netif = SOCK_ADDR_ANY_NETIF;
#endif

    /* build address */
#ifdef SOCK_HAS_IPV6
    ipv6_addr_t addr;
    if (!ipv6_addr_from_str(&addr, EDGEX_SERVER_ADDR)) {
        DEBUG("lwm2m: unable to parse destination address\n");
        return -1;
    }
#ifdef MODULE_GNRC_SOCK_UDP
    if ((_device.server_ep.netif == SOCK_ADDR_ANY_NETIF)
            && ipv6_addr_is_link_local(&addr)) {
        DEBUG("lwm2m: must specify interface for link local target\n");
        return -1;
    }
#endif  /* MODULE_GNRC_SOCK_UDP */
    memcpy(&_device.server_ep.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));
#else  /* SOCK_HAS_IPV6 */
    ipv4_addr_t addr;
    if (!ipv4_addr_from_str((ipv4_addr_t *)&addr, EDGEX_SERVER_ADDR)) {
        puts("gcoap_cli: unable to parse destination address");
        return -1;
    }
    memcpy(&_device.server_ep.addr.ipv4[0], &addr.u8[0], sizeof(addr.u8));
#endif  /* SOCK_HAS_IPV6 */

    _device.server_ep.port = CONFIG_GCOAP_PORT;

    return 0;
}

/*
 * Send temperature to EdgeX server.
 */
static void _send(phydat_t *data)
{
    coap_pkt_t pdu;
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    
    gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_POST,
                  EDGEX_SERVER_PATH);
    coap_hdr_set_type(pdu.hdr, COAP_TYPE_CON);
    coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);
    size_t len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);

    if (pdu.payload_len >= 10) {
        len += saul_info_print(data->val[0], data->scale,
                               (char *)pdu.payload, 10);
        /* no response handler */
        gcoap_req_send(buf, len, &_device.server_ep, NULL, NULL);
    }
}

/*
 * Initialize and maintain application state loop.
 */
int main(void)
{
    timex_t time;

#if EDGEX_CLI_START
    edgex_cli_start(thread_getpid());
    DEBUG("edgex: waiting for start from CLI\n");
    thread_sleep();
#endif

    if (_init_server_addr() < 0) {
        _device.state = DEVICE_STATE_INIT_FAIL;
    }

    while (1) {
        xtimer_now_timex(&time);
        DEBUG("edgex: state %d @ %u\n", _device.state, (unsigned)time.seconds);

        switch (_device.state) {
        case DEVICE_STATE_INIT:
            _device.state = DEVICE_STATE_INFO_OK;
            break;

        case DEVICE_STATE_INFO_RENEW: {
            phydat_t data;
            if (!saul_info_read(&data)) {
              _send(&data);
            }
            /* time is sanity checked for wraparound in OK case */
            _device.next_info_time = time.seconds + SAUL_INFO_INTERVAL;
            /* don't wait for response to send the next info message */
            _device.state = DEVICE_STATE_INFO_OK;
            break;
        }

        case DEVICE_STATE_INFO_OK:
            if (_device.next_info_time == 0) {
                saul_info_init(SAUL_INFO_DRIVER);
                _device.next_info_time = time.seconds + SAUL_INFO_INTERVAL;
            }
            /* includes sanity check for next info/reg time */
            if (_device.next_info_time > time.seconds) {
                DEBUG("edgex: sleeping for %u\n",
                      (unsigned) (_device.next_info_time - time.seconds));
                xtimer_sleep(_device.next_info_time - time.seconds);
            }
            else {
                DEBUG("edgex: unexpected next info time %u\n",
                      (unsigned) _device.next_info_time);
                _device.next_info_time = time.seconds;
            }
            _device.state = DEVICE_STATE_INFO_RENEW;
            break;

        default:
            /* expecting an event, or has a terminal error */
            xtimer_sleep(1);
        }
    }
    return 0;
}
