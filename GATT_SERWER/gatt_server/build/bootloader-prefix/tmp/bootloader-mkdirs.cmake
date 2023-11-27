# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/mati/esp/esp-idf/components/bootloader/subproject"
  "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader"
  "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader-prefix"
  "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader-prefix/tmp"
  "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader-prefix/src"
  "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/mati/Documents/IoT/GATT_SERWER_PODST/gatt_server/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
