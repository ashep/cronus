#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME = cronus
EXTRA_COMPONENT_DIRS := ${AESPL_PATH} ${AGFXL_PATH}
EXCLUDE_COMPONENTS := bootloader coap console esp-wolfssl esp_https_ota fatfs freemodbus jsmn libsodium mdns mqtt \
	protobuf-c protocomm smartconfig_ack spi_ram openssl wear_levelling wifi_provisioning

include $(IDF_PATH)/make/project.mk
