set(srcs main.c ../../components/esp-serial-flasher/examples/common/example_common.c)
set(include_dirs . ../../components/esp-serial-flasher/examples/common)

idf_component_register(
    SRCS ${srcs} "main.c" "fileserver.c" "flasher.c"
    INCLUDE_DIRS ${include_dirs} "include"
    EMBED_FILES "favicon.ico" ../frontend/index.html
)

set(target ${COMPONENT_LIB})

# Embed binaries into the app.
# In ESP-IDF this can also be done using EMBED_FILES option of idf_component_register.
# Here an external tool is used to make file embedding similar with other ports.
include(${CMAKE_CURRENT_LIST_DIR}/../../components/esp-serial-flasher/examples/common/bin2array.cmake)
spiffs_create_partition_image(www "${CMAKE_CURRENT_SOURCE_DIR}/../frontend" FLASH_IN_PROJECT)
