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
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckCSourceCompiles)

CHECK_LIBRARY_EXISTS(rt timer_create "time.h" HAVE_TIMER_CREATE)
if (NOT HAVE_TIMER_CREATE)
   set(CMAKE_EXTRA_INCLUDE_FILES time.h)
   CHECK_FUNCTION_EXISTS(timer_create HAVE_TIMER_CREATE)
   SET(CMAKE_EXTRA_INCLUDE_FILES)
endif()
CHECK_LIBRARY_EXISTS(rt timer_settime "time.h" HAVE_TIMER_SETTIME)
if (NOT HAVE_TIMER_SETTIME)
   set(CMAKE_EXTRA_INCLUDE_FILES time.h)
   CHECK_FUNCTION_EXISTS(timer_settime HAVE_TIMER_SETTIME)
   SET(CMAKE_EXTRA_INCLUDE_FILES)
endif()

CHECK_SYMBOL_EXISTS(SIGEV_THREAD_ID "signal.h;time.h" HAVE_SIGEV_THREAD_ID)

# Check for the Linux-specific POSIX timers API.
IF(HAVE_TIMER_CREATE AND HAVE_TIMER_SETTIME AND HAVE_SIGEV_THREAD_ID)
  SET(HAVE_POSIX_TIMERS 1 CACHE INTERNAL "Have POSIX timer-related functions")
  add_definitions(-DHAVE_POSIX_TIMERS)
ENDIF()
message(STATUS "HAVE_POSIX_TIMERS ${HAVE_POSIX_TIMERS}")

CHECK_C_SOURCE_COMPILES(
"
#include <signal.h>
int main(int ac, char **av)
{
    sigset_t ss;
    struct sigaction sa;
    sigemptyset(&ss); sigsuspend(&ss);
    sigaction(SIGINT, &sa, (struct sigaction *) 0);
    sigprocmask(SIG_BLOCK, &ss, (sigset_t *) 0);
}
"
  HAVE_POSIX_SIGNALS)


set(SERVICES_TIMER_SOURCES
	TimerHandler.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/services/include
)

add_library(cbtf-services-timer SHARED
	${SERVICES_TIMER_SOURCES}
)

target_link_libraries(cbtf-services-timer
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-timer PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-timer PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")
set_target_properties(cbtf-services-timer PROPERTIES POSITION_INDEPENDENT_CODE ON)


install(TARGETS cbtf-services-timer
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-services-timer-static STATIC
	${SERVICES_TIMER_SOURCES}
)

target_link_libraries(cbtf-services-timer-static
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-timer-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-timer-static PROPERTIES OUTPUT_NAME cbtf-services-timer)
set_target_properties(cbtf-services-timer-static PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")
set_target_properties(cbtf-services-timer-static PROPERTIES POSITION_INDEPENDENT_CODE ON)


install(TARGETS cbtf-services-timer-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
