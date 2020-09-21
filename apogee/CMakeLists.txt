cmake_minimum_required (VERSION 3.2)
project(OPL3ApogeeDriver VERSION 1.0.0 LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "WinDriver: This component is for Windows Platform only")
endif()

if(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
    option(WITH_WINMMDRV_PTHREADS    "Link libwinpthreads statically (when using pthread-based builds)" ON)
    option(WITH_WINMMDRV_MINGWEX     "Link libmingwex statically (when using vanilla MinGW builds)" OFF)
endif()

#====================================
#          Driver module
#====================================

set(OPL3SYNTH_fmopl3lib_SRC
    driver/fmopl3lib/opl3.cpp
    driver/fmopl3lib/opl3class.cpp
)

add_library(fmopl3lib STATIC ${OPL3SYNTH_fmopl3lib_SRC})

set(OPL3SYNTH_synthlib_SRC
    driver/synthlib/al_midi.cpp
)
add_library(synthlib STATIC ${OPL3SYNTH_synthlib_SRC})

set(OPL3SYNTH_DRIVER_SRC
    driver/src/winmm_drv.def
    driver/src/winmm_drv.cpp
    driver/src/MidiSynth.cpp
)

add_library(opl3apogee MODULE ${OPL3SYNTH_DRIVER_SRC})
set_target_properties(opl3apogee PROPERTIES PREFIX "")
target_link_libraries(opl3apogee PRIVATE winmm synthlib fmopl3lib)
# target_include_directories(opl3apogee PRIVATE config)
target_compile_definitions(opl3apogee PRIVATE
    -D_USRDLL
    -D_WINDLL
    -DMM_WIN32DRV_EXPORTS
    -DENABLE_REG_SERVER
    -DUNICODE
    -D_UNICODE
)

if(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
    set_property(TARGET opl3apogee APPEND_STRING PROPERTY LINK_FLAGS " -static-libgcc -static-libstdc++ -Wl,--enable-stdcall-fixup")
    if(WITH_WINMMDRV_PTHREADS)
        set_property(TARGET opl3apogee APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Bstatic,--whole-archive -lpthread -Wl,-Bdynamic,--no-whole-archive")
    endif()
    if(WITH_WINMMDRV_MINGWEX)
        set_property(TARGET opl3apogee APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Bstatic,--whole-archive -lmingwex -Wl,-Bdynamic,--no-whole-archive")
    endif()
endif()

if(NOT MSVC)
    target_compile_options(opl3apogee PRIVATE "-Wno-cast-function-type")
endif()



#====================================
#        Install utility
#====================================

set(DRIVER_INSTALLER_SRC
    installer/drvinst.c
)

if(NOT MSVC)
    list(APPEND DRIVER_INSTALLER_SRC
        installer/drvinst.rc
    )
endif()

add_executable(drvsetup ${DRIVER_INSTALLER_SRC})

target_compile_definitions(drvsetup PRIVATE
    -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
    -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1
    -D_CRT_SECURE_NO_WARNINGS
    -DUNICODE
    -D_UNICODE
)

if(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
    set_property(TARGET drvsetup APPEND_STRING PROPERTY LINK_FLAGS " -static-libgcc")
    if(WITH_WINMMDRV_MINGWEX)
        set_property(TARGET drvsetup APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Bstatic,--whole-archive -lmingwex -Wl,-Bdynamic,--no-whole-archive")
    endif()
endif()

configure_file(installer/install.bat install.bat COPYONLY)
configure_file(installer/uninstall.bat uninstall.bat COPYONLY)

#====================================
#  A unit test to verify the driver
#====================================

add_executable(drvtest test/test.c)
target_link_libraries(drvtest PRIVATE winmm)
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(drvtest PRIVATE "-Wno-cast-function-type")
    if(WITH_WINMMDRV_MINGWEX)
        set_property(TARGET drvtest APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Bstatic,--whole-archive -lmingwex -Wl,-Bdynamic,--no-whole-archive")
    endif()
endif()
