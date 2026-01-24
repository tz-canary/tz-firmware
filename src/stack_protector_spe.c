#include <zephyr/kernel.h>
#include <psa/client.h>
#include <stdio.h>
#include <stdlib.h>

#define TFM_CANARY_TZ_SID       0x0000F001
#define MSG_TYPE_GET_CANARY     1
#define MSG_TYPE_CHECK_CANARY   2

#define TZ_CANARY_DEBUG

#ifdef TZ_CANARY_DEBUG
#include <zephyr/sys/printk.h>
unsigned int called = 0;
unsigned int called_check = 0;
#endif

extern void __stack_chk_fail_weak(void);

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
        puts("*** psa_connect failed ***: terminated");
		abort();
    }

    psa_outvec outv[1] = {
        { .base = &out, .len = sizeof(out) }
    };

    status = psa_call(handle, MSG_TYPE_GET_CANARY, NULL, 0, outv, 1);
    psa_close(handle);

    if (status != PSA_SUCCESS) {
        puts("*** psa_call failed ***: terminated");
		abort();
    }

    uint32_t ret = out;

    *(volatile uint32_t *)&out = 0;
    __asm__ volatile ("" ::: "memory");

    return ret;
}

__attribute__((noinline, no_stack_protector))
void __stack_protector_spe_check(uint32_t canary)
{
    psa_handle_t handle;
    psa_status_t status;

    psa_invec in_vec[1] = {
		{ .base = &canary, .len  = sizeof(canary) }
	};

    handle = psa_connect(TFM_CANARY_TZ_SID, 1);
    if (handle <= 0) {
        puts("*** psa_connect failed ***: terminated");
		abort();
    }

    status = psa_call(handle, MSG_TYPE_CHECK_CANARY, in_vec, 1,NULL, 0);
    psa_close(handle);

    if (status != PSA_SUCCESS) {
        __stack_chk_fail_weak();
    }
}
