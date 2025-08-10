cmake_minimum_required(VERSION 3.16)

project(MarkdownConverter
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt6路径（在GitHub Actions中自动设置）
if(NOT DEFINED Qt6_DIR AND DEFINED ENV{Qt6_DIR})
    set(Qt6_DIR $ENV{Qt6_DIR})
endif()

# 查找所有必需的Qt6组件
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    WebEngineWidgets 
    WebEngineCore 
    WebChannel
)

# 添加可执行文件
add_executable(MarkdownConverter
    main.cpp
)

# 链接Qt库
target_link_libraries(MarkdownConverter PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebEngineCore
    Qt6::WebChannel
)

# 设置自动包含moc文件
set_target_properties(MarkdownConverter PROPERTIES
    AUTOMOC ON
)