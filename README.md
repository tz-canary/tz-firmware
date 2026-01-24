# tz-firmware
TF-M firmware for stack canary isolation

# How to apply

## 1. config/config\_base.cmake
```cmake
set(TFM_PARTITION_CANARY_TZ             ON          CACHE BOOL      "Enable Canary TZ Partition")
```

## 2. platform/ext/target/rpi/rp2350/manifest/tfm\_manifest\_list.yaml
```yaml
# manifest_list ...
    {
      "description": "TFM Canary TZ Partition",
      "manifest": "secure_fw/partitions/canary_tz/tfm_canary_tz.yaml",
      "output_path": "secure_fw/partitions/canary_tz",
      "conditional": "TFM_PARTITION_CANARY_TZ",
      "version_major": 0,
      "version_minor": 1,
      "pid": 280,
      "linker_pattern": {
        "library_list": [
           "*tfm_*partition_canary_tz*"
         ]
      }
    },
```

## 3. secure\_fw/CMakeLists.txt
```
target_include_directories(tfm_config
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/partitions/crypto>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/partitions/firmware_update>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/partitions/initial_attestation>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/partitions/internal_trusted_storage>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/partitions/platform>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/partitions/protected_storage>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spm/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated/interface/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/partitions/canary_tz> # 여기
)
```

## 4. secure\_fw/partitions/CMakeLists.txt
```
add_subdirectory(canary_tz)
```

## 5. tools/tfm\_manifest\_list.yaml
```
# manifest_list ...
    {
      "description": "TFM Canary TZ Partition",
      "manifest": "../secure_fw/partitions/canary_tz/tfm_canary_tz.yaml",
      "output_path": "secure_fw/partitions/canary_tz",
      "conditional": "TFM_PARTITION_CANARY_TZ",
      "version_major": 0,
      "version_minor": 1,
      "pid": 280,
      "linker_pattern": {
        "library_list": [
           "*tfm_*partition_canary_tz*"
         ]
      }
    }
```

## 6. platform/ext/rpi/rp2350/tfm\_hal\_platform.c
Replace with src/tfm\_hal\_platform.c.