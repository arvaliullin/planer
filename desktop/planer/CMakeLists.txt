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
    src/core/ApiError.cpp
    src/core/Config.cpp
    src/core/TokenStorage.cpp
    src/domain/entities/Enums.cpp
    src/domain/entities/HealthStatus.cpp
    src/domain/usecases/CheckHealthUseCase.cpp
    src/data/api/HttpClient.cpp
    src/data/api/client/dto/HealthDto.cpp
    src/data/api/client/mappers/HealthMapper.cpp
    src/data/api/client/mock/MockPlanerApiClient.cpp
    src/data/api/client/http/PlanerHttpClient.cpp
    src/presentation/mappers/EntityMapper.cpp
    src/presentation/AppContext.cpp
    src/presentation/viewmodels/AppViewModel.cpp
    src/presentation/viewmodels/SessionViewModel.cpp
    src/presentation/viewmodels/ProjectsViewModel.cpp
    src/presentation/viewmodels/TasksViewModel.cpp
    src/presentation/viewmodels/KanbanViewModel.cpp
    src/presentation/viewmodels/GanttViewModel.cpp
    src/presentation/viewmodels/TaskDetailViewModel.cpp
    src/presentation/viewmodels/HealthViewModel.cpp
)

qt_add_executable(appplaner ${PLANER_SOURCES})

set_source_files_properties(qml/theme/PlanerTheme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(appplaner
    URI planer
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/theme/PlanerTheme.qml
        qml/components/SectionHeader.qml
        qml/components/EmptyState.qml
        qml/components/LoadingOverlay.qml
        qml/components/ToastHost.qml
        qml/components/TaskKeyLabel.qml
        qml/components/StatusBadge.qml
        qml/components/PriorityBadge.qml
        qml/components/TaskCard.qml
        qml/components/ComponentStatusRow.qml
        qml/views/auth/LoginPage.qml
        qml/views/auth/RegisterPage.qml
        qml/views/shell/MainShell.qml
        qml/views/shell/MainShellLayout.qml
        qml/views/shell/ProjectSidebar.qml
        qml/views/shell/TaskDetailPanel.qml
        qml/views/tasks/TaskListView.qml
        qml/views/tasks/KanbanView.qml
        qml/views/tasks/GanttView.qml
        qml/dialogs/CreateProjectDialog.qml
        qml/dialogs/CreateTaskDialog.qml
        qml/dialogs/CommandPaletteStub.qml
        qml/dialogs/HealthCheckDialog.qml
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
