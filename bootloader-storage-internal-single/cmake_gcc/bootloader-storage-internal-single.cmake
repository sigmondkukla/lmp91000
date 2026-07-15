####################################################################
# Automatically-generated file. Do not edit!                       #
####################################################################

set(SDK_PATH "/Users/zacharythornton/.silabs/slt/installs/conan/p/simpl965e19baece23/p")
set(COPIED_SDK_PATH "simplicity_sdk_2025.12.2")
set(PKG_PATH "/Users/zacharythornton/.silabs/slt/installs")

add_library(slc OBJECT
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_bootload.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_core.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_main.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_parse.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_reset.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/flash/btl_internal_flash.c"
    "${SDK_PATH}/bootloader/platform/bootloader/debug/btl_debug.c"
    "${SDK_PATH}/bootloader/platform/bootloader/debug/btl_debug_swo.c"
    "${SDK_PATH}/bootloader/platform/bootloader/driver/btl_driver_util.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/gbl/btl_gbl_custom_tags.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/gbl/btl_gbl_format.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/gbl/btl_gbl_parser.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_crc16.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_crc32.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_aes.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_ecdsa.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_sha256.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_tokens.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/ecc/ecc.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/btl_sha256.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/crypto_sha.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/cryptoacc_sha.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/se_sha.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/bootloadinfo/btl_storage_bootloadinfo_single.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/btl_storage.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/btl_storage_library.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/internal_flash/btl_storage_internal_flash.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/internal_flash/btl_storage_internal_flash_raw.c"
    "${SDK_PATH}/devices/platform/Device/SiliconLabs/EFR32BG24/Source/startup_efr32bg24.c"
    "${SDK_PATH}/devices/platform/Device/SiliconLabs/EFR32BG24/Source/system_efr32bg24.c"
    "${SDK_PATH}/platform_common/platform/common/src/sl_assert.c"
    "${SDK_PATH}/platform_common/platform/common/src/sl_syscalls.c"
    "${SDK_PATH}/platform_core/platform/common/src/sl_core_cortexm.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_acmp.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_burtc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_cmu.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_dbg.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_emu.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_eusart.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_gpcrc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_gpio.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_i2c.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_iadc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_ldma.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_letimer.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_msc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_opamp.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_pcnt.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_prs.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_rmu.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_system.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_timer.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_usart.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_vdac.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_wdog.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_dynamic_reservation.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_pool.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_pool_common.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_region.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_retarget.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sli_memory_manager_common.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/src/se_aes.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/src/sl_mbedtls.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_common.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_init.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_aead.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_builtin_keys.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_cipher.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_derivation.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_management.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_mac.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_signature.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_aead.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_cipher.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_mac.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_key_derivation.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_aead.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_cipher.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_hash.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_mac.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_key_derivation.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_version_dependencies.c"
    "${SDK_PATH}/security_mbedtls_source/library/aes.c"
    "${SDK_PATH}/security_mbedtls_source/library/constant_time.c"
    "${SDK_PATH}/security_mbedtls_source/library/platform.c"
    "${SDK_PATH}/security_mbedtls_source/library/platform_util.c"
    "${SDK_PATH}/security_mbedtls_source/library/psa_crypto_client.c"
    "${SDK_PATH}/security_mbedtls_source/library/psa_util.c"
    "${SDK_PATH}/security_mbedtls_source/library/threading.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_attestation.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_cipher.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_entropy.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_hash.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_key_derivation.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_key_handling.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_signature.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_util.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sli_se_manager_mailbox.c"
    "${SDK_PATH}/security_tfm/lib/fih/src/fih.c"
    "${SDK_PATH}/security_tfm/platform/ext/target/siliconlabs/hse/sli_se.c"
)

target_include_directories(slc PUBLIC
   "../config"
   "../autogen"
    "${SDK_PATH}/devices/platform/Device/SiliconLabs/EFR32BG24/Include"
    "${SDK_PATH}/platform_common/platform/common/inc"
    "${SDK_PATH}/bootloader/platform/bootloader"
    "${SDK_PATH}/bootloader/platform/bootloader/api"
    "${SDK_PATH}/bootloader/platform/bootloader/debug"
    "${SDK_PATH}/bootloader/platform/bootloader/parser"
    "${SDK_PATH}/bootloader/platform/bootloader/core/flash"
    "${SDK_PATH}/bootloader/platform/bootloader/security"
    "${SDK_PATH}/cmsis/Core/Include"
    "${SDK_PATH}/platform_core/platform/emlib/inc"
    "${SDK_PATH}/platform_core/platform/common/errno_error_codes/inc"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/config"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/config/preset"
    "${SDK_PATH}/security_mbedtls_source/include"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/inc"
    "${SDK_PATH}/security_mbedtls_source/library"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/inc"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/inc"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/inc"
    "${SDK_PATH}/platform_core/platform/common/inc"
    "${SDK_PATH}/security_tfm/lib/fih/inc"
    "${SDK_PATH}/security_tfm/platform/include"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/sli_psec_osal/inc"
)

target_compile_definitions(slc PUBLIC
    "EFR32BG24L210F768IM40=1"
    "SL_CODE_COMPONENT_SYSTEM=system"
    "SE_MANAGER_CONFIG_FILE=\"btl_aes_ctr_stream_block_cfg.h\""
    "BOOTLOADER_ENABLE=1"
    "BOOTLOADER_SECOND_STAGE=1"
    "SL_RAMFUNC_DISABLE=1"
    "__START=main"
    "__STARTUP_CLEAR_BSS=1"
    "SYSTEM_NO_STATIC_MEMORY=1"
    "BOOTLOADER_SUPPORT_INTERNAL_STORAGE=1"
    "BOOTLOADER_SUPPORT_STORAGE=1"
    "SL_COMPONENT_CATALOG_PRESENT=1"
    "MBEDTLS_CONFIG_FILE=<sl_mbedtls_trustzone_config.h>"
    "SL_CODE_COMPONENT_MEMORY_MANAGER=memory_manager"
    "MBEDTLS_PSA_CRYPTO_CONFIG_FILE=<psa_crypto_config.h>"
    "SL_CODE_COMPONENT_SE_MANAGER=se_manager"
    "SL_CODE_COMPONENT_CORE=core"
    "SL_CODE_COMPONENT_PSEC_OSAL=psec_osal"
    "SL_TRUSTZONE_SECURE=1"
)

target_link_libraries(slc PUBLIC
    "-Wl,--start-group"
    "gcc"
    "c"
    "m"
    "nosys"
    "-Wl,--end-group"
)
target_compile_options(slc PUBLIC
    $<$<COMPILE_LANGUAGE:C>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:C>:-mthumb>
    $<$<COMPILE_LANGUAGE:C>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:C>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:C>:-mcmse>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Os>
    $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:C>:-g>
    $<$<COMPILE_LANGUAGE:C>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:C>:-Wno-ignored-qualifiers>
    $<$<COMPILE_LANGUAGE:C>:-Wno-sign-compare>
    $<$<COMPILE_LANGUAGE:C>:-fno-lto>
    $<$<COMPILE_LANGUAGE:CXX>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:CXX>:-mthumb>
    $<$<COMPILE_LANGUAGE:CXX>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:CXX>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-mcmse>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Os>
    $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:CXX>:-g>
    $<$<COMPILE_LANGUAGE:CXX>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-ignored-qualifiers>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-sign-compare>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-lto>
    $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:ASM>:-mthumb>
    $<$<COMPILE_LANGUAGE:ASM>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=hard>
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:-x assembler-with-cpp>"
)

set(post_build_command ${POST_BUILD_EXE} postbuild "./bootloader-storage-internal-single.slpb" --parameter build_dir:"$<TARGET_FILE_DIR:bootloader-storage-internal-single>")
set_property(TARGET slc PROPERTY C_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_EXTENSIONS OFF)

target_link_options(slc INTERFACE
    -mcpu=cortex-m33
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -T${CMAKE_CURRENT_LIST_DIR}/../autogen/linkerfile.ld
    --specs=nano.specs
    "SHELL:-Xlinker -Map=$<TARGET_FILE_DIR:bootloader-storage-internal-single>/bootloader-storage-internal-single.map"
    "SHELL:-Wl,--wrap=_free_r -Wl,--wrap=_malloc_r -Wl,--wrap=_calloc_r -Wl,--wrap=_realloc_r"
    -fno-lto
    -Wl,--gc-sections
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA