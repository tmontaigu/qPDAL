cmake_minimum_required( VERSION 3.0 )

# CloudCompare example for standard plugins

# REPLACE ALL 'qPDAL' OCCURENCES BY YOUR PLUGIN NAME
# AND ADAPT THE CODE BELOW TO YOUR OWN NEEDS!

# Add an option to CMake to control whether we build this plugin or not
option( PLUGIN_QPDAL "Check to install example plugin" OFF )

if ( PLUGIN_QPDAL )
	# Name the plugin
    project( qPDAL )
    

    # PDAL library
    find_package( PDAL 1.7.0 REQUIRED CONFIG )

    if ( NOT PDAL_FOUND )
        message( SEND_ERROR "PDAL package not found!")
    else()
        include_directories( ${PDAL_INCLUDE_DIRS} )
        link_directories( ${PDAL_LIBRARY_DIRS} )
    endif()

    set( JSON_ROOT_DIR "" CACHE PATH "Jsoncpp root dir (PDAL/vendor/jsoncpp/dist)" )
    if( NOT JSON_ROOT_DIR )
        message( WARNING "Jsoncpp root dir is not specified (JSON_ROOT_DIR)" )
    else()
        include_directories( ${JSON_ROOT_DIR} )
    endif()

    include( ../../CMakePluginTpl.cmake )

    # Link PDAL
    add_definitions( ${PDAL_DEFINITIONS} )

    target_link_libraries( ${PROJECT_NAME} ${PDAL_LIBRARIES} jsoncpp)

    if( WIN32 )
        if ( MSVC_VERSION GREATER_EQUAL 1900 ) # Visual Studio 2017
            add_definitions( -DWIN32_LEAN_AND_MEAN)
        endif()

        set( PDAL_DLL_DIR ${PDAL_LIBRARY_DIRS}/../bin )

        file( GLOB PDAL_DLL_FILES ${PDAL_DLL_DIR}/pdal*.dll )

        copy_files( "${PDAL_DLL_FILES}" ${CLOUDCOMPARE_DEST_FOLDER} )

        if( ${OPTION_BUILD_CCVIEWER} )
            copy_files( "${PDAL_DLL_FILES}" ${CCVIEWER_DEST_FOLDER} )
        endif()

        target_link_libraries( ${PROJECT_NAME} "pdal_util")
    endif()
endif()
