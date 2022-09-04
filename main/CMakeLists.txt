idf_component_register(SRCS "main.c" "fileserver.c" "flasher.c"
    INCLUDE_DIRS "include"
    EMBED_FILES "favicon.ico" ../frontend/index.html)

spiffs_create_partition_image(www "${CMAKE_CURRENT_SOURCE_DIR}/../frontend" FLASH_IN_PROJECT)