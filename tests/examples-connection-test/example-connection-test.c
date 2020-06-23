/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2020, Intel Corporation
 */

/*
 * connection-test.c -- integration tests using the connection example
 */

#include <rdma/rdma_cma.h>
#include <stdlib.h>

#include "cmocka_headers.h"
#include "librpma.h"

struct rdma_event_channel *
rdma_create_event_channel(void)
{
	return NULL;
}

void
rdma_destroy_event_channel(struct rdma_event_channel *channel)
{
}

int
rdma_migrate_id(struct rdma_cm_id *id, struct rdma_event_channel *channel)
{
	return -1;
}

int
rdma_get_cm_event(struct rdma_event_channel *channel,
		struct rdma_cm_event **event_ptr)
{
	return -1;
}

int
rdma_ack_cm_event(struct rdma_cm_event *event)
{
	return -1;
}

int
rdma_disconnect(struct rdma_cm_id *id)
{
	return -1;
}

void
rdma_destroy_qp(struct rdma_cm_id *id)
{
}

int
rdma_destroy_id(struct rdma_cm_id *id)
{
	return -1;
}

struct ibv_cq *
ibv_create_cq(struct ibv_context *context, int cqe, void *cq_context,
		struct ibv_comp_channel *channel, int comp_vector)
{
	return NULL;
}

int
ibv_destroy_cq(struct ibv_cq *cq)
{
	return -1;
}

int
rdma_accept(struct rdma_cm_id *id, struct rdma_conn_param *conn_param)
{
	return -1;
}

int
rdma_connect(struct rdma_cm_id *id, struct rdma_conn_param *conn_param)
{
	return -1;
}

int
rdma_reject(struct rdma_cm_id *id, const void *private_data,
		uint8_t private_data_len)
{
	return -1;
}

int
rdma_create_id(struct rdma_event_channel *channel,
		struct rdma_cm_id **id, void *context,
		enum rdma_port_space ps)
{
	return -1;
}

int
rdma_resolve_route(struct rdma_cm_id *id, int timeout_ms)
{
	return -1;
}

int
rdma_listen(struct rdma_cm_id *id, int backlog)
{
	return -1;
}

int
rdma_getaddrinfo(const char *node, const char *service,
		const struct rdma_addrinfo *hints, struct rdma_addrinfo **res)
{
	return -1;
}

void
rdma_freeaddrinfo(struct rdma_addrinfo *res)
{
}

int
rdma_resolve_addr(struct rdma_cm_id *id, struct sockaddr *src_addr,
		struct sockaddr *dst_addr, int timeout_ms)
{
	return -1;
}

int
rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr)
{
	return -1;
}

int
rdma_create_qp(struct rdma_cm_id *id, struct ibv_pd *pd,
		struct ibv_qp_init_attr *qp_init_attr)
{
	return -1;
}

struct ibv_pd *
ibv_alloc_pd(struct ibv_context *ibv_ctx)
{
	return NULL;
}

int
ibv_dealloc_pd(struct ibv_pd *pd)
{
	return -1;
}

void *__real__test_malloc(size_t size);

/*
 * __wrap__test_malloc -- malloc() mock
 */
void *
__wrap__test_malloc(size_t size)
{
	errno = mock_type(int);

	if (errno)
		return NULL;

	return __real__test_malloc(size);
}

int connection_client_main(int argc, char *argv[]);

static void
test_client(void **unused)
{
	char *argv[] = {"client", "127.0.0.1", "45000"};
	(void) connection_client_main(3, argv);
}

int
main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_client),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
