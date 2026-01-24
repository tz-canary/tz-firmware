#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <psa/client.h>

#define TFM_CANARY_TZ_SID       0x0000F001
#define MSG_TYPE_GET_CANARY     1

#ifdef TZ_CANARY_DEBUG
unsigned int called = 0;
#endif

__attribute__((noinline, no_stack_protector))
uint32_t __stack_protector_spe(void)
{
    psa_handle_t handle;
    psa_status_t status;
    uint32_t out = 0;

	#ifdef TZ_CANARY_DEBUG
	called++;
	#endif

    handle = psa_connect(TFM_CANARY_TZ_SID, 1);
    if (handle <= 0) {
        k_thread_abort(k_current_get());
    }

    psa_outvec outv[1] = {
        { .base = &out, .len = sizeof(out) }
    };

    status = psa_call(handle, MSG_TYPE_GET_CANARY, NULL, 0, outv, 1);
    psa_close(handle);

    if (status != PSA_SUCCESS) {
        k_thread_abort(k_current_get());
    }

    uint32_t ret = out;

    *(volatile uint32_t *)&out = 0;
    __asm__ volatile ("" ::: "memory");

    return ret;
}
