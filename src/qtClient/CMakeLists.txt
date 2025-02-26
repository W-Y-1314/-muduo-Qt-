project(chatClient)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# 帮助找Qt文件
list(APPEND CMAKE_PREFIX_PATH "/home/wy/Qt/6.5.3/gcc_64")
set(CMAKE_PREFIX_PATH /home/wy/Qt/6.5.3/gcc_64)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets) 
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp   
    LoginResDlg.h
    LoginResDlg.cpp  
    EventFilterObject.h 
    EventFilterObject.cpp
    storeMess.h
    storeMess.cpp
    "Resource.qrc"
    ChatWindow.ui
    ui_ChatWindow.h 
    ChatWindow.h 
    ChatWindow.cpp
    ChatBubble.h  
    ChatBubble.cpp
)

add_executable(chatClient ${PROJECT_SOURCES})

target_link_libraries(chatClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)