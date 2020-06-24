
cmake_minimum_required( VERSION 3.5.0 )

project( AzeroIotDemo )

set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_STANDARD_REQUIRED ON )


set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Wall -Wno-covered-switch-default -Wno-missing-braces -O2 -s -fPIC")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -Wall -Wno-covered-switch-default -Wno-missing-braces -g -fPIC")

set(DEMO_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/azero_iot_sdk_api.h
)

set(DEMO_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

add_executable(AzeroIotDemo
    ${DEMO_HEADER_FILES}
    ${DEMO_SOURCE_FILES}
)

target_include_directories( AzeroIotDemo PUBLIC include )

target_link_libraries(AzeroIotDemo PRIVATE -lpthread)

if(IOT_BUILD_FOR_LINUX_ARM32)
    target_link_libraries(AzeroIotDemo PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/link-libs/arm-linux-gnueabihf/libazeroiot.so
    )
elseif(IOT_BUILD_FOR_LINUX_AARCH64)
    target_link_libraries(AzeroIotDemo PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/link-libs/aarch64-linux-gnu/libazeroiot.so
    )
elseif(IOT_BUILD_FOR_LINUX64)
    target_link_libraries(AzeroIotDemo PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/link-libs/linux64/libazeroiot.so
    )
endif()

