/* common_defs.h */
#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <stdint.h>

/* 서비스 ID (Manifest와 일치해야 함) */
#define CANARY_SERVICE_SID      0x0000F001
#define CANARY_SERVICE_VERSION  1

/* 메시지 타입 */
#define MSG_TYPE_GET_CANARY     1  /* Prologue: 카나리 요청 */
#define MSG_TYPE_CHECK_CANARY   2  /* Epilogue: 카나리 검증 (나중에 구현) */

/* 통신 페이로드 (입력: 스레드 ID) */
typedef struct {
    uint32_t thread_id;
} canary_req_t;

#endif /* COMMON_DEFS_H */
