cmake_minimum_required( VERSION 2.8 )
project(CPIAS_DL)

if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

## Add a compiler flag
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

## Make this a GUI application on Windows
#if(WIN32)
#  set(CMAKE_WIN32_EXECUTABLE ON)
#endif()

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package (Qt5Widgets)
find_package (Qt5Core)
find_package (Qt5Gui)

# 添加资源文件
qt5_add_resources(RESOURCE_FILES resource/src.qrc)

include_directories(${QT_USE_FILE})

find_package( VTK 8.2.0 REQUIRED )
include_directories(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

SET( PROJECT_SRCS
    main.cpp
    mainwindow/mainwindow.cpp
    view/singleviewslicevisualization.cpp
    view/segmentationvisualization.cpp
    view/volumeviewvisualization.cpp

    basictool/imageloadingbox.cpp
    basictool/imageprocessingbox.cpp
    basictool/colormapTableBox.cpp
    )

SET( PROJECT_MOC_HDRS
    mainwindow/mainwindow.h
    view/singleviewslicevisualization.h
    view/segmentationvisualization.h
    view/volumeviewvisualization.h

    basictool/imageloadingbox.h
    basictool/imageprocessingbox.h
    basictool/colormapTableBox.h
)

SET( PROJECT_UIS
    mainwindow/mainwindow.ui
    view/singleviewslicevisualization.ui
    view/segmentationvisualization.ui
    view/volumeviewvisualization.ui
)

QT5_WRAP_UI( PROJECT_UIS_H
             ${PROJECT_UIS}
           )

QT5_WRAP_CPP( PROJECT_MOC_SRCS
              ${PROJECT_MOC_HDRS}
            )

ADD_EXECUTABLE( ${PROJECT_NAME}
                ${PROJECT_SRCS}
                ${PROJECT_UIS_H}
                ${PROJECT_MOC_SRCS}
                basictool/mydata.h
              )
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${OpenCV_LIBS} Qt5::Widgets Qt5::Charts)
