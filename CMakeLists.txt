cmake_minimum_required(VERSION 3.16)

# 设置项目名称和版本
project(MarkdownConverter VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6库
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

# 如果使用QtWebEngine
# find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)

# 设置自动处理UI、资源和MOC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 添加可执行文件
add_executable(MarkdownConverter
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
)

# 链接Qt库
target_link_libraries(MarkdownConverter PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    # Qt6::WebEngineWidgets # 如果使用WebEngine
)

# 设置应用程序属性
set_target_properties(MarkdownConverter PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# 安装目标
install(TARGETS MarkdownConverter
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 复制资源文件
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/appicon.ico")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/appicon.ico"
        "${CMAKE_CURRENT_BINARY_DIR}/appicon.ico"
        COPYONLY
    )
endif()

# 在Windows上设置应用程序图标
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_BINARY_DIR}/appicon.ico")
    if(EXISTS "${APP_ICON_RESOURCE_WINDOWS}")
        add_custom_command(TARGET MarkdownConverter POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${APP_ICON_RESOURCE_WINDOWS}"
                "$<TARGET_FILE_DIR:MarkdownConverter>/"
        )
    endif()
endif()