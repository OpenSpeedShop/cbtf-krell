################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################


include(CheckIncludeFile)

set(MRNetConvertorPlugin_SOURCES
	MRNetConverters.cpp
)

add_library(MRNetConvertorsPlugin MODULE
	${MRNetConvertorPlugin_SOURCES}
)

target_include_directories(MRNetConvertorsPlugin PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(MRNetConvertorsPlugin
	cbtf-core
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(MRNetConvertorsPlugin PROPERTIES PREFIX "")
set_target_properties(MRNetConvertorsPlugin PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS MRNetConvertorsPlugin
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)

# Create and install static version

add_library(MRNetConvertorsPlugin-static STATIC
	${MRNetConvertorPlugin_SOURCES}
)

target_include_directories(MRNetConvertorsPlugin-static PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(MRNetConvertorsPlugin-static
	cbtf-core
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(MRNetConvertorsPlugin-static PROPERTIES PREFIX "")
set_target_properties(MRNetConvertorsPlugin-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS MRNetConvertorsPlugin-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)

set(AggregationPlugin_SOURCES
	AddressAggregatorComponent.cpp
	AddressBufferComponent.cpp
)

add_library(AggregationPlugin MODULE
	${AggregationPlugin_SOURCES}
)

target_include_directories(AggregationPlugin PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(AggregationPlugin
	cbtf-core
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-collector
	cbtf-messages-converters-collector
	cbtf-messages-events
	cbtf-messages-converters-events
	cbtf-messages-perfdata
	cbtf-messages-converters-perfdata
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(AggregationPlugin PROPERTIES PREFIX "")
set_target_properties(AggregationPlugin PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS AggregationPlugin
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)

# Create and install static version

add_library(AggregationPlugin-static STATIC
	${AggregationPlugin_SOURCES}
)

target_include_directories(AggregationPlugin-static PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(AggregationPlugin-static
	cbtf-core
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-collector
	cbtf-messages-converters-collector
	cbtf-messages-events
	cbtf-messages-converters-events
	cbtf-messages-perfdata
	cbtf-messages-converters-perfdata
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(AggregationPlugin-static PROPERTIES PREFIX "")
set_target_properties(AggregationPlugin-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS AggregationPlugin-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)


set(SymbolPlugin_SOURCES
	SymbolComponent.cpp
)

add_library(SymbolPlugin MODULE
	${SymbolPlugin_SOURCES}
)

target_include_directories(SymbolPlugin PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(SymbolPlugin
	cbtf-core
	cbtf-core-symtabapi
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-events
	cbtf-messages-converters-events
	cbtf-messages-symtab
	cbtf-messages-converters-symtab
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(SymbolPlugin PROPERTIES PREFIX "")
set_target_properties(SymbolPlugin PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS SymbolPlugin
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)

# Create and install static version

add_library(SymbolPlugin-static STATIC
	${SymbolPlugin_SOURCES}
)

target_include_directories(SymbolPlugin-static PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(SymbolPlugin-static
	cbtf-core
	cbtf-core-symtabapi
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-events
	cbtf-messages-converters-events
	cbtf-messages-symtab
	cbtf-messages-converters-symtab
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(SymbolPlugin-static PROPERTIES PREFIX "")
set_target_properties(SymbolPlugin-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS SymbolPlugin-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)


set(ThreadPlugin_SOURCES
	ThreadEventComponent.cpp
)

add_library(ThreadPlugin MODULE
	${ThreadPlugin_SOURCES}
)

target_include_directories(ThreadPlugin PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/thread
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(ThreadPlugin
	cbtf-core
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-events
	cbtf-messages-converters-events
	cbtf-messages-thread
	cbtf-messages-converters-thread
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(ThreadPlugin PROPERTIES PREFIX "")
set_target_properties(ThreadPlugin PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS ThreadPlugin
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)

# Create and install static version

add_library(ThreadPlugin-static STATIC
	${ThreadPlugin_SOURCES}
)

target_include_directories(ThreadPlugin-static PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/thread
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(ThreadPlugin-static
	cbtf-core
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-events
	cbtf-messages-converters-events
	cbtf-messages-thread
	cbtf-messages-converters-thread
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(ThreadPlugin-static PROPERTIES PREFIX "")
set_target_properties(ThreadPlugin-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS ThreadPlugin-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)


set(LinkedObjectPlugin_SOURCES
	LinkedObjectComponent.cpp
)

add_library(LinkedObjectPlugin MODULE
	${LinkedObjectPlugin_SOURCES}
)

target_include_directories(LinkedObjectPlugin PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/thread
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(LinkedObjectPlugin
	cbtf-core
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-events
	cbtf-messages-converters-events
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(LinkedObjectPlugin PROPERTIES PREFIX "")
set_target_properties(LinkedObjectPlugin PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS LinkedObjectPlugin
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)

# Create and install static version

add_library(LinkedObjectPlugin-static STATIC
	${LinkedObjectPlugin_SOURCES}
)

target_include_directories(LinkedObjectPlugin-static PUBLIC
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/thread
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(LinkedObjectPlugin-static
	cbtf-core
	cbtf-messages-base
	cbtf-messages-converters-base
	cbtf-messages-events
	cbtf-messages-converters-events
	${CBTF_LIBRARIES}
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(LinkedObjectPlugin-static PROPERTIES PREFIX "")
set_target_properties(LinkedObjectPlugin-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS LinkedObjectPlugin-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
)

