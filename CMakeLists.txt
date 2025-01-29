# Mínimo requerido
cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(KISA_LOP)

# Carpeta con headers
include_directories(include)

# Archivos fuente
#file(GLOB_RECURSE SOURCES "src/*.cpp")

# Crear ejecutable
#add_executable(programa ${SOURCES})

# Configuración de carpetas de salida
set(MAIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/build)
set(TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/build/tests)

# Buscar todos los archivos fuente
file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")

# Separar archivos main/test y comunes
set(COMMON_SOURCES "")
set(MAIN_FILES "")
set(TEST_FILES "")

foreach(FILE ${ALL_SOURCES})
    if (FILE MATCHES ".(main).")
        list(APPEND MAIN_FILES ${FILE})  # Archivos main
    elseif (FILE MATCHES ".(test).")
        list(APPEND TEST_FILES ${FILE}) # Archivos test
    else()
        list(APPEND COMMON_SOURCES ${FILE})  # Archivos comunes
    endif()
endforeach()

# Crear un ejecutable para cada archivo main/test
foreach(MAIN_FILE ${MAIN_FILES})
    # Obtener el nombre base del archivo main/test (sin extensión)
    get_filename_component(EXECUTABLE_NAME ${MAIN_FILE} NAME_WE)

    # Crear ejecutable
    add_executable(${EXECUTABLE_NAME}
        ${MAIN_FILE}
        ${COMMON_SOURCES}
    )
    # Especificar la carpeta de destino
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}
    )
endforeach()

# Crear un ejecutable para cada archivo main/test
foreach(TEST_FILE ${TEST_FILES})
    # Obtener el nombre base del archivo main/test (sin extensión)
    get_filename_component(EXECUTABLE_NAME ${TEST_FILE} NAME_WE)

    # Crear ejecutable
    add_executable(${EXECUTABLE_NAME}
        ${TEST_FILE}
        ${COMMON_SOURCES}
    )
    # Especificar la carpeta de destino
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIR}
    )
endforeach()