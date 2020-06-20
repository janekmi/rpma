/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2020, Intel Corporation
 */

/*
 * rpma.c -- entry points for librpma
 */

#include <rdma/rdma_cma.h>

#include "librpma.h"
#include "rpma_err.h"
#include "info.h"
#include "out.h"

/*
 * rpma_utils_get_ibv_context_passive -- obtain an RDMA device context by local
 * IP address
 */
static int
rpma_utils_get_ibv_context_passive(const char *addr, struct ibv_context **dev)
{
	ASSERTne(addr, NULL);
	ASSERTne(dev, NULL);

	struct rpma_info *info;
	int ret = rpma_info_new(addr, NULL /* service */, RPMA_INFO_PASSIVE,
			&info);
	if (ret)
		return ret;

	struct rdma_cm_id *temp_id;
	ret = rdma_create_id(NULL, &temp_id, NULL, RDMA_PS_TCP);
	if (ret) {
		Rpma_provider_error = errno;
		ret = RPMA_E_PROVIDER;
		goto err_info_delete;
	}

	ret = rpma_info_bind_addr(info, temp_id);
	if (ret)
		goto err_destroy_id;

	/* obtain the device */
	*dev = temp_id->verbs;

err_destroy_id:
	(void) rdma_destroy_id(temp_id);

err_info_delete:
	(void) rpma_info_delete(&info);
	return ret;
}

/*
 * rpma_utils_get_ibv_context_active - obtain an RDMA device context by remote
 * IP address
 */
static int
rpma_utils_get_ibv_context_active(const char *addr, struct ibv_context **dev)
{
	ASSERTne(addr, NULL);
	ASSERTne(dev, NULL);

	struct rpma_info *info;
	int ret = rpma_info_new(addr, NULL /* service */, RPMA_INFO_ACTIVE,
			&info);
	if (ret)
		return ret;

	struct rdma_cm_id *temp_id;
	ret = rdma_create_id(NULL, &temp_id, NULL, RDMA_PS_TCP);
	if (ret) {
		Rpma_provider_error = errno;
		ret = RPMA_E_PROVIDER;
		goto err_info_delete;
	}

	ret = rpma_info_resolve_addr(info, temp_id);
	if (ret)
		goto err_destroy_id;

	/* obtain the device */
	*dev = temp_id->verbs;

err_destroy_id:
	(void) rdma_destroy_id(temp_id);

err_info_delete:
	(void) rpma_info_delete(&info);
	return ret;
}

/* public librpma API */

/*
 * rpma_utils_get_ibv_context -- obtain an RDMA device context by IP address
 */
int
rpma_utils_get_ibv_context(const char *addr, struct ibv_context **dev)
{
	if (addr == NULL || dev == NULL)
		return RPMA_E_INVAL;

	/* at first, assume the address is local... */
	int ret = rpma_utils_get_ibv_context_passive(addr, dev);
	if (ret == 0)
		return 0;

	/* ... otherwise, it is remote or neither */
	return rpma_utils_get_ibv_context_active(addr, dev);
}

/*
 * rpma_utils_conn_event_2str -- return const string representation of
 * RPMA_CONN_* enums
 */
const char *
rpma_utils_conn_event_2str(enum rpma_conn_event conn_event)
{
	switch(conn_event) {
	case RPMA_CONN_UNDEFINED:
		return "RPMA_CONN_UNDEFINED";
	case RPMA_CONN_ESTABLISHED:
		return "RPMA_CONN_ESTABLISHED";
	case RPMA_CONN_CLOSED:
		return "RPMA_CONN_CLOSED";
	case RPMA_CONN_LOST:
		return "RPMA_CONN_LOST";
	default:
		return "unknown";
	}
}

/*
 * rpma_mr_reg -- XXX
 */
int
rpma_mr_reg(struct rpma_peer *peer, void *ptr, size_t size, int usage, int plt,
		struct rpma_mr_local **mr)
{
	return RPMA_E_NOSUPP;
}

/*
 * rpma_mr_dereg -- XXX
 */
int
rpma_mr_dereg(struct rpma_mr_local **mr)
{
	return RPMA_E_NOSUPP;
}

/*
 * rpma_read -- XXX
 */
int
rpma_read(struct rpma_conn *conn, void *op_context,
	struct rpma_mr_local *dst, size_t dst_offset,
	struct rpma_mr_remote *src,  size_t src_offset,
	size_t len, int flags)
{
	return RPMA_E_NOSUPP;
}

/*
 * rpma_conn_next_completion -- XXX
 */
int
rpma_conn_next_completion(struct rpma_conn *conn, struct rpma_completion *cmpl)
{
	return RPMA_E_NOSUPP;
}
