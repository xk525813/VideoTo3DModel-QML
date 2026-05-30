set(SOURCES
    main.cpp
    app/Application.cpp
    core/ProjectManager.cpp
    core/HardwareDetector.cpp
    core/FrameExtractor.cpp
    core/SfMStage.cpp
    core/MVSStage.cpp
    core/TextureStage.cpp
    core/PipelineOrchestrator.cpp
    core/ExportManager.cpp
    bridge/PipelineBridge.cpp
)

set(HEADERS
    core/IStage.h
    core/ProjectManager.h
    core/HardwareDetector.h
    core/FrameExtractor.h
    core/SfMStage.h
    core/MVSStage.h
    core/TextureStage.h
    core/PipelineOrchestrator.h
    core/ExportManager.h
    bridge/PipelineBridge.h
    app/Application.h
)

qt_add_executable(VideoTo3D ${SOURCES} ${HEADERS}
    ui/qml.qrc
)

target_include_directories(VideoTo3D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(VideoTo3D PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Qml
    Qt6::Gui
    Qt6::Concurrent
    Qt6::OpenGL
)

# OpenCV
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs videoio)
target_link_libraries(VideoTo3D PRIVATE ${OpenCV_LIBS})
target_include_directories(VideoTo3D PRIVATE ${OpenCV_INCLUDE_DIRS})
