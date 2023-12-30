
project(Platform)
set(CMAKE_C_STANDARD 11)

include_directories(Src BEFORE)

add_library(Platform STATIC
        Include/Platform/Core.h
        Include/Platform/IO.h
        Include/Platform/Text.h
        Src/Core/Error.c
        Src/Core/Logger.c
        Src/Core/Math.c
        Src/Text/String.c
        Src/Text/StringBuilder.c
        Src/IO/File.c
        Src/Codec/PNG.c
        Include/Platform/Codec.h
)
target_include_directories(Platform PUBLIC Src)
#set_target_properties(Platform PROPERTIES
#        #        FRAMEWORK TRUE
#        PUBLIC_HEADER "Src/IO/IO.h;Src/Core/Core.h;Src/Text/Text.h"
#)

target_sources(Platform PUBLIC FILE_SET HEADERS
        BASE_DIRS Include
        FILES Include/Platform/IO.h Include/Platform/Core.h
)

install(TARGETS Platform
        #No idea what these paths are used for
        #FRAMEWORK DESTINATION THIS_IS_STUPID
        #LIBRARY DESTINATION Lib
        #PUBLIC_HEADER DESTINATION Include
        FILE_SET HEADERS
)
