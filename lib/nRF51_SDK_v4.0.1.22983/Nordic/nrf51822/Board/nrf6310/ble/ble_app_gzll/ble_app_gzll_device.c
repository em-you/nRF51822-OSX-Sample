/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @{
 * @ingroup ble_app_gzll_gazell_part
 */

#include "ble_app_gzll_device.h"
#include <stdint.h>
#include "nordic_common.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_gzll.h"
#include "ble_nrf6310_pins.h"
#include "ble_app_gzll_ui.h"


#define GET_GAZELL_ERROR()\
    do {\
        nrf_gzll_error_code_t ERR_CODE = nrf_gzll_get_error_code();\
        UNUSED_VARIABLE(ERR_CODE);\
        APP_ERROR_HANDLER(false);\
    } while (0)

static uint8_t gzll_packet[8];


/**@brief Start Gazell functionality.
 */
void gzll_app_start(void)
{
    bool gzll_call_ok;

    gzll_call_ok  = nrf_gzll_init(NRF_GZLL_MODE_DEVICE);
    if (!gzll_call_ok)
    {
        GET_GAZELL_ERROR();
    }
    
    gzll_call_ok = nrf_gzll_set_max_tx_attempts(100);
    if (!gzll_call_ok)
    {
        GET_GAZELL_ERROR();
    }
    
    gzll_call_ok = nrf_gzll_enable();
    if (!gzll_call_ok)
    {
        GET_GAZELL_ERROR();
    }

    // Add a packet to the TX FIFO to start the data transfer. 
    // From here on more data will be added through the Gazell callbacks
    gzll_packet[0] = 0x80;
    gzll_call_ok = nrf_gzll_add_packet_to_tx_fifo(0, gzll_packet, 8);
    if (!gzll_call_ok)
    {
        GET_GAZELL_ERROR();
    }
}


void gzll_app_stop()
{
    // Disable gazell
    nrf_gzll_disable();
    
    // Wait for Gazell to shut down
    while (nrf_gzll_is_enabled())
    {
        // Do nothing.
    }
    
    // Clean up after Gazell
    NVIC_DisableIRQ(RADIO_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);   
}


void nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    bool       push_ok;
    static int cpt = 0;
    uint8_t dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;
    

    // If an ACK was received, we send another packet. 
    nrf_gpio_pin_set(GZLL_TX_SUCCESS_LED_PIN_NO);
    nrf_gpio_pin_clear(GZLL_TX_FAIL_LED_PIN_NO);

    if (tx_info.payload_received_in_ack)
    {
        // if data received attached to the ack, pop them.
        push_ok = nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length);
        if (!push_ok)
        {
            GET_GAZELL_ERROR();
        }
    }

    cpt++;
    if (cpt > 3)
    {
        cpt = 0;
        gzll_packet[0] = ~(gzll_packet[0]);
        if (gzll_packet[0] == 0x80)
        {
            gzll_packet[0] = 0x00;
        }
        gzll_packet[0] <<= 1;
        if (gzll_packet[0] == 0)
        {
            gzll_packet[0]++;
        }
        gzll_packet[0] = ~(gzll_packet[0]);
    }
    push_ok = nrf_gzll_add_packet_to_tx_fifo(0, gzll_packet, 8);
    if (!push_ok)
    {
        GET_GAZELL_ERROR();
    }
}


void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    bool push_ok;
    uint8_t dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    // If the transmission failed, send a new packet.
    nrf_gpio_pin_set(GZLL_TX_FAIL_LED_PIN_NO);
    nrf_gpio_pin_clear(GZLL_TX_SUCCESS_LED_PIN_NO);

    if (tx_info.payload_received_in_ack)
    {
        // if data received attached to the ack, pop them.
        push_ok = nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length);
        if (!push_ok)
        {
            GET_GAZELL_ERROR();
        }
    }

    push_ok = nrf_gzll_add_packet_to_tx_fifo(0, gzll_packet, 8);
    if (!push_ok)
    {
        GET_GAZELL_ERROR();
    }
}


void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
    // We dont expect to receive any data in return, but if it happens we flush the RX fifo
    bool push_ok = nrf_gzll_flush_rx_fifo(pipe);
    if (!push_ok)
    {
        GET_GAZELL_ERROR();
    }
}


void nrf_gzll_disabled()
{
}

/** 
 * @}
 */
