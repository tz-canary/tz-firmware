#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "psa/service.h"
#include "psa/crypto.h"
#include "psa_manifest/tfm_canary_tz.h"
#include "common_defs.h"

uint32_t canary = 0xdeadbeef;
uint8_t initialized = 0;

static void init_canary(void) {
    if (initialized) {
        return;  // panic!
    }

    psa_status_t status;
    
    status = psa_generate_random((uint8_t *)&canary, sizeof(canary));

    psa_status_t st = psa_generate_random((uint8_t *)&canary, sizeof(canary));
    if (st != PSA_SUCCESS) {
        return st;
    }

    *((char *)&canary + 3) = '\0';
    initialized = 1;

    return PSA_SUCCESS;
}

/* -------------------------------------------------------------------------
 * 핸들러: MSG_TYPE_GET_CANARY (Prologue)
 * ------------------------------------------------------------------------- */
static psa_status_t handle_get_canary(psa_msg_t *msg) {
    if (msg->out_size[0] < sizeof(canary)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    
    psa_write(msg->handle, 0, &canary, sizeof(canary));
    return PSA_SUCCESS;
}

static psa_status_t handle_check_canary(psa_msg_t *msg) {
    uint32_t received_canary = 0;

    if (msg->in_size[0] != sizeof(uint32_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_read(msg->handle, 0, &received_canary, sizeof(received_canary));

    if (received_canary != canary) {
        return PSA_ERROR_DATA_INVALID;
    }

    return PSA_SUCCESS;
}

/* -------------------------------------------------------------------------
 * 메인 엔트리 포인트
 * ------------------------------------------------------------------------- */
void tfm_canary_tz_init(void) {
    psa_signal_t signals = 0;
    psa_msg_t msg;

    psa_crypto_init();
    init_canary();

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        if (signals & TFM_CANARY_TZ_SERVICE_SIGNAL) {
            if (psa_get(TFM_CANARY_TZ_SERVICE_SIGNAL, &msg) == PSA_SUCCESS) {
                switch (msg.type) {
                    case PSA_IPC_CONNECT:
                    case PSA_IPC_DISCONNECT:
                        psa_reply(msg.handle, PSA_SUCCESS);
                        break;

                    case MSG_TYPE_GET_CANARY:
                        psa_reply(msg.handle, handle_get_canary(&msg));
                        break;
                    case MSG_TYPE_CHECK_CANARY:
                         psa_reply(msg.handle, handle_check_canary(&msg));
                        break;

                    default:
                        psa_reply(msg.handle, PSA_ERROR_NOT_SUPPORTED);
                        break;
                }
            }
        }
    }
}
