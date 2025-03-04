set(SOURCE_FILES
        main.cxx
        Game.{hxx,cxx}
        util/LOG.{hxx,cxx}
        util/LOG.inl.hxx
        util/Filesystem.{hxx,cxx}
        util/IEquatable.hxx
        util/IEquatable.inl.hxx
        util/PriorityQueue.hxx
        util/PriorityQueue.inl.hxx
        util/Singleton.hxx
        util/Meta.hxx
        util/Exception.{hxx,cxx}
        services/Randomizer.{hxx,cxx}
        services/GameClock.{hxx,cxx}
        services/GameClock.inl.hxx
        services/ResourceManager.{hxx,cxx}
        services/ResourceManager.inl.hxx
        engine/MessageQueue.hxx
        engine/MessageQueue.inl.hxx
        engine/basics/Camera.{hxx,cxx}
        engine/basics/isoMath.{hxx,cxx}
        engine/basics/mapEdit.{hxx,cxx}
        engine/basics/point.hxx
        engine/basics/PointFunctions.{hxx,cxx}
        engine/basics/Settings.{hxx,cxx}
        engine/basics/GameStates.{hxx,cxx}
        engine/basics/signal.hxx
        engine/basics/utils.{hxx,cxx}
        engine/common/Constants.hxx
        engine/common/enums.hxx
        engine/common/JsonSerialization.hxx
        engine/GameObjects/MapNode.{hxx,cxx}
        engine/map/MapLayers.{hxx,cxx}
        engine/map/TerrainGenerator.{hxx,cxx}
        engine/ui/basics/UIElement.{hxx,cxx}
        engine/ui/basics/ButtonGroup.{hxx,cxx}
        engine/ui/basics/Layout.{hxx,cxx}
        engine/ui/widgets/Bar.{hxx,cxx}
        engine/ui/widgets/Button.{hxx,cxx}
        engine/ui/widgets/Combobox.{hxx,cxx}
        engine/ui/widgets/Checkbox.{hxx,cxx}
        engine/ui/widgets/Frame.{hxx,cxx}
        engine/ui/widgets/Image.{hxx,cxx}
        engine/ui/widgets/Slider.{hxx,cxx}
        engine/ui/widgets/Text.{hxx,cxx}
        engine/ui/widgets/TextField.{hxx,cxx}
        engine/ui/widgets/Tooltip.{hxx,cxx}
        engine/ui/menuGroups/IMenuGroup.hxx
        engine/Engine.{hxx,cxx}
        engine/EventManager.{hxx,cxx}
        engine/Map.{hxx,cxx}
        engine/Sprite.{hxx,cxx}
        engine/ResourcesManager.{hxx,cxx}
        engine/TileManager.{hxx,cxx}
        engine/UIManager.{hxx,cxx}
        engine/WindowManager.{hxx,cxx}
        game/ZoneArea.{hxx,cxx}
        game/ZoneManager.{hxx,cxx}
        game/GamePlay.{hxx,cxx}
        )

if (ANDROID)
    list(APPEND SOURCE_FILES util/android/Filesystem.cxx)
elseif (APPLE)
    list(APPEND SOURCE_FILES util/macOS/Filesystem.cxx)
else ()
    list(APPEND SOURCE_FILES util/desktop/Filesystem.cxx)
endif ()

if (ENABLE_AUDIO)
    list(
            APPEND SOURCE_FILES
            engine/audio/Soundtrack.{hxx,cxx}
            services/AudioMixer.{hxx,cxx}
    )
endif ()

if (USE_ANGELSCRIPT)
    list(APPEND SOURCE_FILES
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptbuilder/scriptbuilder.cpp
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptbuilder/scriptbuilder.h
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptstdstring/scriptstdstring.h
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptstdstring/scriptstdstring.cpp
            Scripting/ScriptEngine.{hxx,cxx}
            )
endif ()

if (ENABLE_MICROPROFILE)
    list(APPEND SOURCE_FILES
            ${microprofile_SOURCE_DIR}/microprofile.h
            ${microprofile_SOURCE_DIR}/microprofile_html.h
            ${microprofile_SOURCE_DIR}/microprofile.cpp
            )
endif (ENABLE_MICROPROFILE)

# Expand file extensions (i.e. path/to/file.{h,cpp} becomes path/to/file.h;path/to/file.cpp)
expand_file_extensions(SOURCE_FILES ${SOURCE_FILES})

# Generate source groups for use in IDEs
generate_source_groups(${SOURCE_FILES})

set(MAIN_EXE_SOURCE_FILES "${SOURCE_FILES}" PARENT_SCOPE)

if (MSVC)
    set(SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/src/Windows/icon_cytopia.rc)
endif ()

if (APPLE)
    file(GLOB_RECURSE MAC_RESOURCE_FILES ${CMAKE_SOURCE_DIR}/data/resources/* ${CMAKE_SOURCE_DIR}/data/languages/*)
    # file(GLOB_RECURSE MAC_RESOURCE_FILES ${CMAKE_BINARY_DIR}/resources/*)

    # not working now, we need a special icon file for apple
    set(ICON_PATH ${CMAKE_SOURCE_DIR}/data/resources/images/app_icons/cytopia_icon_iso.icns)
    set(MACOSX_BUNDLE_ICON_FILE cytopia_icon_iso.icns)

    # set each file as resources file for the app image.
    foreach (resources_file ${MAC_RESOURCE_FILES})
        get_filename_component(buildDirRelFilePath ${resources_file} DIRECTORY)
        string(REPLACE "${CMAKE_SOURCE_DIR}/data/" "" buildDirRelFilePath ${buildDirRelFilePath})
        message(STATUS "resources: ${resources_file}")
        set_source_files_properties(${resources_file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${buildDirRelFilePath})
    endforeach ()

    # set_source_files_properties(${PROJECT_SOURCE_DIR}/Cytopia_Resources/music  PROPERTIES MACOSX_PACKAGE_LOCATION Resources/resources/audio/music)
    set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES} ${ICON_PATH} ${MAC_RESOURCE_FILES})

elseif (ANDROID)
    # On android the final binary is a shared library not an executable.
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
else ()
    # create a new executable with the same name as the project
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif ()

# enable C++17
set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

list(APPEND _compile_definitions VERSION="${PROJECT_NAME} ${PROJECT_VERSION} - Urban Update")

target_include_directories(
        ${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/external/header_only
        engine
        engine/basics
        engine/common
        engine/GameObjects
        engine/map
        engine/ui/basics/
        engine/ui/widgets
        engine/ui/menuGroups
        util
)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${_compile_definitions})
target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        LIBPNG::LIBPNG
        SDL::SDL
        SDL::TTF
        SDL::SDL_image
        LibNoise::LibNoise
        ZLIB::ZLIB
)

if (ANDROID)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ANDROID_LOG_LIBRARY} ${ANDROID_LIBRARY_LIBRARY})
endif ()

if (ENABLE_AUDIO)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenAL::OpenAL VorbisFile::VorbisFile)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_AUDIO)
endif ()

if (USE_MICROPROFILE)
    target_link_libraries(${PROJECT_NAME} PRIVATE Microprofile::Microprofile)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MICROPROFILE_ENABLED MICROPROFILE_GPU_TIMERS=0)
endif ()

if (USE_ANGELSCRIPT)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/as_add_on)
    target_link_libraries(${PROJECT_NAME} PRIVATE Angelscript::Angelscript)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ANGELSCRIPT)
endif ()

# copy the resources to the compiled directory. On Apple those files are used from the resources dir, not from the root dir.
if (NOT APPLE)
    fast_copy("${CMAKE_SOURCE_DIR}/data/resources" "${RUNTIME_OUTPUT_DIRECTORY}/resources")
    fast_copy("${CMAKE_SOURCE_DIR}/data/languages" "${RUNTIME_OUTPUT_DIRECTORY}/languages")
endif ()

if (MSVC)
    # set startup project for Visual Studio Builds
    set_property(
            DIRECTORY ${PROJECT_SOURCE_DIR}
            PROPERTY
            VS_STARTUP_PROJECT ${PROJECT_NAME}
    )
    # set Visual Studio working directory to the Configuration directory
    set_target_properties(
            ${PROJECT_NAME}
            PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif ()

install(TARGETS ${PROJECT_NAME} DESTINATION .)

if (WIN32)
    install(DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>/" DESTINATION . FILES_MATCHING PATTERN "*.dll")
endif ()

if (NOT APPLE)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/resources/ DESTINATION resources)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/languages/ DESTINATION languages FILES_MATCHING PATTERN "*.mo")
endif ()

install(FILES ${CMAKE_SOURCE_DIR}/ReleaseNotes.txt ${CMAKE_SOURCE_DIR}/credits.txt DESTINATION .)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE RESOURCE TRUE)
endif ()
