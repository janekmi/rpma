#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2020, Intel Corporation
#

include(../cmake/ctest_helpers.cmake)

build_test_src(example-connection-test
       example-connection-test.c
       ${LIBRPMA_SOURCE_DIR}/conn.c
       ${LIBRPMA_SOURCE_DIR}/conn_req.c
       ${LIBRPMA_SOURCE_DIR}/ep.c
       ${LIBRPMA_SOURCE_DIR}/info.c
       ${LIBRPMA_SOURCE_DIR}/peer.c
       ${LIBRPMA_SOURCE_DIR}/private_data.c
       ${LIBRPMA_SOURCE_DIR}/rpma.c
       ${LIBRPMA_SOURCE_DIR}/rpma_err.c
       ${CMAKE_SOURCE_DIR}/examples/connection/connection-client.c
       ${CMAKE_SOURCE_DIR}/examples/connection/connection-server.c)

target_link_libraries(example-connection-test test-rpma-utils)

target_compile_definitions(example-connection-test
       PUBLIC TEST_MOCK_MAIN)

set_target_properties(example-connection-test
       PROPERTIES
       LINK_FLAGS "-Wl,--wrap=_test_malloc")

add_test_generic(NAME example-connection-test TRACERS none)
