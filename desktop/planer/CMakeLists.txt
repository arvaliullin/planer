cmake_minimum_required(VERSION 3.16)

project(planer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.2 COMPONENTS Quick QuickControls2 Network REQUIRED)

qt_policy(SET QTP0001 OLD)
qt_policy(SET QTP0004 OLD)

set(PLANER_SOURCES
    main.cpp
    src/core/Config.cpp
    src/domain/entities/HealthStatus.cpp
    src/domain/usecases/CheckHealthUseCase.cpp
    src/data/api/HttpClient.cpp
    src/data/dto/HealthDto.cpp
    src/data/mappers/HealthMapper.cpp
    src/data/repositories/HealthRepository.cpp
    src/presentation/AppContext.cpp
    src/presentation/viewmodels/HealthViewModel.cpp
)

qt_add_executable(appplaner ${PLANER_SOURCES})

qt_add_qml_module(appplaner
    URI planer
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/views/SettingsPage.qml
        qml/components/ComponentStatusRow.qml
)

target_include_directories(appplaner PRIVATE src)

target_link_libraries(appplaner
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Network
)

if(TARGET Qt6::Core)
    get_target_property(_qt6_core_location Qt6::Core IMPORTED_LOCATION)
    if(NOT _qt6_core_location)
        get_target_property(_qt6_core_location Qt6::Core IMPORTED_LOCATION_RELEASE)
    endif()
    if(_qt6_core_location AND _qt6_core_location MATCHES "\\.a$")
        target_compile_definitions(appplaner PRIVATE PLANER_QT_STATIC)
        qt_import_qml_plugins(appplaner)
        qt6_import_plugins(appplaner
            INCLUDE_BY_TYPE platforms Qt::QXcbIntegrationPlugin
            INCLUDE_BY_TYPE xcbglintegrations Qt::QXcbEglIntegrationPlugin
            INCLUDE_BY_TYPE imageformats Qt::QSvgPlugin
        )
    endif()
endif()

set_target_properties(appplaner PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS appplaner
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
