#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "psa/service.h"
#include "psa/crypto.h"
#include "psa_manifest/tfm_canary_tz.h"
#include "common_defs.h"
#include <arm_cmse.h>


uint32_t canary_value = 0xdeadbeef;
uint8_t initialized = 0;

static void init_canary(void) {
    if (initialized) {
        return;  // panic!
    }

    psa_status_t status;
    
    status = psa_generate_random((uint8_t *)&canary_value, sizeof(canary_value));

    psa_status_t st = psa_generate_random((uint8_t *)&canary_value, sizeof(canary_value));
    if (st != PSA_SUCCESS) {
        return st;
    }

    *((char *)&canary_value + 3) = '\0';
    initialized = 1;

    return PSA_SUCCESS;
}

/* -------------------------------------------------------------------------
 * 핸들러: MSG_TYPE_GET_CANARY (Prologue)
 * ------------------------------------------------------------------------- */
static psa_status_t handle_get_canary(psa_msg_t *msg) {
    // psa_write(msg->handle, 0, &canary, sizeof(canary));
    return canary_value;
}

static psa_status_t handle_check_canary(psa_msg_t *msg) {
    uint32_t received_canary = 0;

    psa_read(msg->handle, 0, &received_canary, sizeof(received_canary));

    if (received_canary != canary_value) {
        return PSA_ERROR_DATA_INVALID;
    }

    return PSA_SUCCESS;
}

uint32_t __attribute__((cmse_nonsecure_entry, naked)) __stack_protector_spe(void) {
    asm volatile(
        "   ldr    r0, =canary_value                         \n"
        "   ldr    r0, [r0]                                 \n"
        "   bxns   lr                                         \n"
    );
}

void __attribute__((cmse_nonsecure_entry, naked)) __stack_protector_spe_check(uint32_t canary)
{
    asm volatile(
        "   ldr    r1, =canary_value                         \n"
        "   ldr    r1, [r1]                                 \n"
        "   cmp    r0, r1                                   \n"
        "   bne    stack_protector_fail                      \n"
        "   bxns   lr                                       \n"

        "stack_protector_fail:                              \n"
        "   bl     psa_panic                                \n"
    );
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
