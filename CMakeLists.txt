cmake_minimum_required(VERSION 3.20)
project(ash LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
include(CTest)

qt_add_executable(ash
    src/main.cpp
    core/fs/fs.cpp
    core/nav/nav.cpp
    core/ops/ops.cpp
    sys/io/io.cpp
    sys/linux/sys.cpp
    util/ut.cpp
    view/bars/bar.cpp
    view/main/ui.cpp
    view/main/fv.cpp
    view/panels/panel.cpp
    view/widgets/item.cpp
    resources/assets.qrc
)

target_include_directories(ash PRIVATE
    inc
    core/fs
    core/nav
    core/ops
    sys/io
    sys/linux
    util
    view/bars
    view/main
    view/panels
    view/widgets
)
target_link_libraries(ash PRIVATE Qt6::Widgets)

configure_file(
    ash.desktop.in
    "${CMAKE_CURRENT_BINARY_DIR}/ash.desktop"
    @ONLY
)

install(TARGETS ash
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ash.desktop"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
)
install(FILES icons/ash.svg
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
)

if(BUILD_TESTING)
    add_executable(ash_unit
        tests/unit/test.cpp
        core/nav/nav.cpp
        core/ops/ops.cpp
        util/ut.cpp
    )
    target_include_directories(ash_unit PRIVATE core/nav core/ops util)
    target_link_libraries(ash_unit PRIVATE Qt6::Core)
    add_test(NAME ash_unit COMMAND ash_unit)
endif()

add_executable(ash_bench EXCLUDE_FROM_ALL
    tests/bench/bench.cpp
    core/ops/ops.cpp
)
target_include_directories(ash_bench PRIVATE core/ops)
target_link_libraries(ash_bench PRIVATE Qt6::Core)
