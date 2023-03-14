cmake_minimum_required(VERSION 3.16)
project(LabCD VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

qt_standard_project_setup()

qt_add_executable(LabCD WIN32 MACOSX_BUNDLE
    labcd.cpp labcd.h
    main.cpp
    utils/colormap.cpp utils/colormap.h
    utils/fileworker.cpp utils/fileworker.h
    utils/imgpress.cpp utils/imgpress.h
    utils/label.cpp utils/label.h
    widgets/annotationscence.cpp widgets/annotationscence.h
    widgets/annotationview.cpp widgets/annotationview.h
    widgets/canvas.cpp widgets/canvas.h
    widgets/filelist.cpp widgets/filelist.h
    widgets/labeltable.cpp widgets/labeltable.h
    widgets/labgrid.cpp widgets/labgrid.h
    widgets/labline.cpp widgets/labline.h
    widgets/labpolygon.cpp widgets/labpolygon.h
    widgets/multcanvas.cpp widgets/multcanvas.h
    widgets/opttypes.h
)
target_include_directories(LabCD PRIVATE
    depends
    depends/include
)

target_link_libraries(LabCD PRIVATE
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/depends/lib/
    Qt::Core
    Qt::Gui
    Qt::Widgets
    gdal_i
)


# Resources:
set(labcd_resource_files
    "resources/Icon.png"
)

qt_add_resources(LabCD "labcd"
    PREFIX
        "/main"
    FILES
        ${labcd_resource_files}
)
set(labcd1_resource_files
    "resources/Chinese.png"
    "resources/ClearMask.png"
    "resources/Convert.png"
    "resources/English.png"
    "resources/Folder.png"
    "resources/Github.png"
    "resources/Help.png"
    "resources/Merge.png"
    "resources/Split.png"
)

qt_add_resources(LabCD "labcd1"
    PREFIX
        "/menu"
    FILES
        ${labcd1_resource_files}
)
set(labcd2_resource_files
    "resources/Color.png"
    "resources/DeleteAllPolygons.png"
    "resources/DeletePolygon.png"
    "resources/Enlarge.png"
    "resources/Full.png"
    "resources/Last.png"
    "resources/Narrow.png"
    "resources/Next.png"
    "resources/Reference.png"
    "resources/Save.png"
)

qt_add_resources(LabCD "labcd2"
    PREFIX
        "/tools"
    FILES
        ${labcd2_resource_files}
)
set(labcd3_resource_files
    "resources/AddLabel.png"
    "resources/CantDelete.png"
    "resources/Delete.png"
)

qt_add_resources(LabCD "labcd3"
    PREFIX
        "/docks"
    FILES
        ${labcd3_resource_files}
)
set(labcd4_resource_files
    "configs/colormap.txt"
)

qt_add_resources(LabCD "labcd4"
    PREFIX
        "/configs"
    FILES
        ${labcd4_resource_files}
)
set(labcd5_resource_files
    "i18n/English.qm"
)

qt_add_resources(LabCD "labcd5"
    PREFIX
        "/translate"
    FILES
        ${labcd5_resource_files}
)

if(WIN32 AND CONFIG(release,debug OR release))
    target_link_libraries(LabCD PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/depends/lib/
        jsoncpp_static
        opencv_world455
    )
endif()

if((NOT (WIN32 AND CONFIG(release,debug OR release))) AND (WIN32 AND CONFIG(debug,debug OR release)))
    target_link_libraries(LabCD PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/depends/lib/
        jsoncpp_staticd
        opencv_world455d
    )
endif()

if(((NOT (WIN32 AND CONFIG(release,debug OR release))) AND (NOT (WIN32 AND CONFIG(debug,debug OR release)))) AND (UNIX))
    target_link_libraries(LabCD PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/depends/lib/
        jsoncpp_static
        opencv_world455
    )
endif()

install(TARGETS LabCD
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET LabCD
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
