include(FetchContent)

# CMSIS Core
FetchContent_Declare(cmsis
  URL https://github.com/ARM-software/CMSIS_5/releases/download/5.9.0/ARM.CMSIS.5.9.0.pack
  URL_HASH MD5=633d69b410dc79065e17bc17a3296cb8
)
FetchContent_MakeAvailable(cmsis)

add_library(CMSIS_CORE INTERFACE)
target_include_directories(CMSIS_CORE INTERFACE
  ${cmsis_SOURCE_DIR}/CMSIS/Core/Include
)

# CMSIS Device
FetchContent_Declare(cmsis_device_h7
  GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_device_h7/
  GIT_TAG v1.10.3
  GIT_SHALLOW true
)
FetchContent_MakeAvailable(cmsis_device_h7)

add_library(CMSIS_DEVICE_H7 STATIC)
target_include_directories(CMSIS_DEVICE_H7 PUBLIC
  ${cmsis_device_h7_SOURCE_DIR}/Include
)
target_sources(CMSIS_DEVICE_H7 PRIVATE
  ${MODULE_DIR}/sys/system_stm32h7xx.c
)
target_link_libraries(CMSIS_DEVICE_H7 PUBLIC
  CMSIS_CORE
  STM32H7XX_HAL_DRIVER
)

# STM32H7xx HAL Driver
FetchContent_Declare(stm32h7xx_hal_driver
  GIT_REPOSITORY https://github.com/STMicroelectronics/stm32h7xx_hal_driver/
  GIT_TAG v1.11.2
  GIT_SHALLOW true

  # NOTE: In-tree source patch until fixed upstream (volatile counters/loop unrolling)
  PATCH_COMMAND git apply --ignore-space-change --ignore-whitespace ${CMAKE_CURRENT_SOURCE_DIR}/stm32h7xx.patch

  # Do not reapply patch once populated
  UPDATE_DISCONNECTED true
)
FetchContent_MakeAvailable(stm32h7xx_hal_driver)

file(GLOB STM32H7XX_HAL_DRIVER_SOURCES ${stm32h7xx_hal_driver_SOURCE_DIR}/Src/*.c) # All .c files
list(FILTER STM32H7XX_HAL_DRIVER_SOURCES EXCLUDE REGEX "template.c$") # unless they're templates

add_library(STM32H7XX_HAL_DRIVER STATIC ${STM32H7XX_HAL_DRIVER_SOURCES})

target_include_directories(STM32H7XX_HAL_DRIVER PUBLIC
  ${stm32h7xx_hal_driver_SOURCE_DIR}/Inc
  ${stm32h7xx_hal_driver_SOURCE_DIR}/Inc/Legacy
  ${MODULE_DIR}/sys # for stm32h7xx_hal_conf.h
)

# Weird Circular dependency (HAL -> CMSIS-Device -> HAL)
target_link_libraries(STM32H7XX_HAL_DRIVER PUBLIC CMSIS_DEVICE_H7)

target_compile_options(STM32H7XX_HAL_DRIVER PRIVATE
    -Wno-unused-but-set-variable
)

target_compile_definitions(STM32H7XX_HAL_DRIVER PRIVATE
  USE_HAL_DRIVER
  USE_FULL_LL_DRIVER
)

## Middleware

# USB Device Library
FetchContent_Declare(stm32_mw_usb_device
  GIT_REPOSITORY https://github.com/STMicroelectronics/stm32_mw_usb_device/
  GIT_TAG v2.11.2

  # NOTE: In-tree source patch until fixed upstream (volatile counters/loop unrolling)
  PATCH_COMMAND git apply --ignore-space-change --ignore-whitespace ${CMAKE_CURRENT_SOURCE_DIR}/stm32_mw_usb_device_new.patch

  # Do not reapply patch once populated
  UPDATE_DISCONNECTED true
)
FetchContent_MakeAvailable(stm32_mw_usb_device)

add_library(STM32_USB_DEVICE_LIBRARY STATIC
  ${stm32_mw_usb_device_SOURCE_DIR}/Class/CDC/Src/usbd_cdc.c
  ${stm32_mw_usb_device_SOURCE_DIR}/Core/Src/usbd_core.c
  ${stm32_mw_usb_device_SOURCE_DIR}/Core/Src/usbd_ctlreq.c
  ${stm32_mw_usb_device_SOURCE_DIR}/Core/Src/usbd_ioreq.c
)
target_include_directories(STM32_USB_DEVICE_LIBRARY PUBLIC
  ${stm32_mw_usb_device_SOURCE_DIR}/Core/Inc
  ${stm32_mw_usb_device_SOURCE_DIR}/Class/CDC/Inc
  ${MODULE_DIR}/usbd # for conf
)
target_link_libraries(STM32_USB_DEVICE_LIBRARY PUBLIC
  CMSIS_DEVICE_H7
  STM32H7XX_HAL_DRIVER
)

# USB Host Library
add_library(STM32_USB_HOST_LIBRARY STATIC
  STM32_USB_Host_Library/Core/Src/usbh_core.c
  STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c
  STM32_USB_Host_Library/Core/Src/usbh_ioreq.c
  STM32_USB_Host_Library/Core/Src/usbh_pipes.c
  STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.c
  STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.c
  STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c
  STM32_USB_Host_Library/Class/MIDI/Src/usbh_midi.c
)
target_include_directories(STM32_USB_HOST_LIBRARY PUBLIC
  STM32_USB_Host_Library/Core/Inc
  STM32_USB_Host_Library/Class/MSC/Inc
  STM32_USB_Host_Library/Class/MIDI/Inc
  ${MODULE_DIR}/usbh # for conf
  ${MODULE_DIR} # for daisy_core.h
)
target_link_libraries(STM32_USB_HOST_LIBRARY PUBLIC
  CMSIS_DEVICE_H7
  STM32H7XX_HAL_DRIVER
)

add_library(FatFs STATIC
  FatFs/src/diskio.c
  FatFs/src/ff.c
  FatFs/src/ff_gen_drv.c

  # due to config (in src/sys) which has _USE_LFN = 1
  FatFs/src/option/unicode.c
)

target_include_directories(FatFs PUBLIC
  FatFs/src
  ${MODULE_DIR}/sys
  ${MODULE_DIR} # for util/bsp_sd_diskio.h
)
