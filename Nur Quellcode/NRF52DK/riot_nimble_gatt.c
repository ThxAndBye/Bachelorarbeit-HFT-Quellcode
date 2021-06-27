/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2018 Codecoup
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       BLE SHT3x example using NimBLE
 *
 * Have a more detailed look at the api here:
 * https://mynewt.apache.org/latest/tutorials/ble/bleprph/bleprph.html
 *
 * More examples (not ready to run on RIOT) can be found here:
 * https://github.com/apache/mynewt-nimble/tree/master/apps
 *
 * Test this application e.g. with Nordics "nRF Connect"-App
 * iOS: https://itunes.apple.com/us/app/nrf-connect/id1054362403
 * Android: https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Andrzej Kaczmarek <andrzej.kaczmarek@codecoup.pl>
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @author      Tjark Blunk <72bltj1bif@hft-stuttgart.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* BLE */
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "net/bluetil/ad.h"
#include "nimble_riot.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define GATT_ENVIRONMENTAL_SENSING_UUID 0x181A
#define GATT_TEMPERATURE_UUID 0x2A6E
#define GATT_HUMIDITY_UUID 0x2A6F

/* SHT3X sensor */
#include "sht3x.h"
#include "sht3x_params.h"

sht3x_dev_t sht31;
int res;

static const char *device_name = "NimBLE SHT31-D example";

static int gatt_temperature_cb(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_humidity_cb(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);

static void start_advertise(void);

/* define several bluetooth services for our device */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {/* Service: Environmental Sensing */
     .type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(GATT_ENVIRONMENTAL_SENSING_UUID),
     .characteristics = (struct ble_gatt_chr_def[]){
         {
             /* Characteristic: Read DHT temperautre */
             .uuid = BLE_UUID16_DECLARE(GATT_TEMPERATURE_UUID),
             .access_cb = gatt_temperature_cb,
             .flags = BLE_GATT_CHR_F_READ,
         },
         {
             /* Characteristic: Read DHT humidity */
             .uuid = BLE_UUID16_DECLARE(GATT_HUMIDITY_UUID),
             .access_cb = gatt_humidity_cb,
             .flags = BLE_GATT_CHR_F_READ,
         },
         {
             0, /* No more characteristics in this service */
         },
     }},
    {
        0, /* No more services */
    },
};

static int gatt_temperature_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    puts("service 'environmental sense: temperature' callback triggered");

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int return_value = -1;
    int16_t temperature;

    if ((res = sht3x_read(&sht31, &temperature, 0)) == SHT3X_OK) {
        return_value = os_mbuf_append(ctxt->om, &temperature, sizeof(temperature));
        printf("Temperature [°C]: %d.%d\n", temperature / 100, temperature % 100);
    }

    return return_value;
}

static int gatt_humidity_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    puts("service 'environmental sense: humidity' callback triggered");

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int return_value = -1;
    int16_t humidity;

    if ((res = sht3x_read(&sht31, 0, &humidity)) == SHT3X_OK) {
        return_value = os_mbuf_append(ctxt->om, &humidity, sizeof(humidity));
        printf("Relative Humidity [%%]: %d.%d\n", humidity / 100, humidity % 100);
    }

    return return_value;
}

static int gap_event_cb(struct ble_gap_event *event, void *arg) {
    (void)arg;

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status) {
                start_advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            start_advertise();
            break;
    }

    return 0;
}

static void start_advertise(void) {
    struct ble_gap_adv_params advp;
    int rc;

    memset(&advp, 0, sizeof advp);
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, BLE_HS_FOREVER,
                           &advp, gap_event_cb, NULL);
    assert(rc == 0);
    (void)rc;
}

int main(void) {
    puts("SHT31-d with NimBLE GATT Server Example");

    int rc = 0;
    (void)rc;

    puts("+------------Initializing------------+\n");
    puts("SHT3x ... ");
    if ((res = sht3x_init(&sht31, &sht3x_params[0])) != SHT3X_OK) {
        puts("Failed!\n");
        return 1;
    }
    else {
        puts("Done.\n");
    }

    puts("BLE ... ");
    /* verify and add our custom services */
    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    assert(rc == 0);
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    assert(rc == 0);

    /* set the device name */
    ble_svc_gap_device_name_set(device_name);
    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* configure and set the advertising data */
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    bluetil_ad_t ad;
    bluetil_ad_init_with_flags(&ad, buf, sizeof(buf), BLUETIL_AD_FLAGS_DEFAULT);
    bluetil_ad_add_name(&ad, device_name);
    ble_gap_adv_set_data(ad.buf, ad.pos);

    /* start to advertise this node */
    start_advertise();
    puts("Done.\n");
    puts("+------------Service Ready------------+\n");

    return 0;
}
