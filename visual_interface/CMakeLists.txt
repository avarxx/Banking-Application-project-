cmake_minimum_required(VERSION 3.16)
project(BankingSystem LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Enable Qt meta-object compiler
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add source and header files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/bank_account.cpp
    src/bank_fields.cpp
    src/bank.cpp
    src/function.cpp
    src/user_data.cpp
)

set(HEADERS
    headers/mainwindow.h
    headers/bank_account.h
    headers/bank_fields.h
    headers/bank.h
    headers/function.h
    headers/user_data.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

# Windows-specific DLL copying
if(WIN32)
    set(QT_BINARY_DIR "${Qt5_DIR}/../../../bin")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_BINARY_DIR}/Qt5Core.dll"
        "${QT_BINARY_DIR}/Qt5Gui.dll"
        "${QT_BINARY_DIR}/Qt5Widgets.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()