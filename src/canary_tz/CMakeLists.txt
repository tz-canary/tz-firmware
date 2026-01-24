if (NOT TFM_PARTITION_CANARY_TZ)
    return()
endif()

cmake_minimum_required(VERSION 3.21)

add_library(tfm_app_rot_partition_canary_tz STATIC)

add_dependencies(tfm_app_rot_partition_canary_tz manifest_tool)

target_include_directories(tfm_app_rot_partition_canary_tz
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/canary_tz
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/canary_tz
)

target_sources(tfm_app_rot_partition_canary_tz
    PRIVATE
        canary_tz.c
)

# The generated sources
target_sources(tfm_app_rot_partition_canary_tz
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/canary_tz/auto_generated/intermedia_tfm_canary_tz.c
)
target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/canary_tz/auto_generated/load_info_tfm_canary_tz.c
)

target_link_libraries(tfm_app_rot_partition_canary_tz
    PRIVATE
        secure_fw
        platform_s
        tfm_config
        tfm_sprt
        psa_crypto_config
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_app_rot_partition_canary_tz
)

target_compile_definitions(tfm_config
    INTERFACE
        TFM_PARTITION_CANARY_TZ
)
