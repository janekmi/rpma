/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2020, Intel Corporation
 */

/*
 * private_data.h -- a store for connections private data (definitions)
 */

#ifndef LIBRPMA_PRIVATE_DATA_H
#define LIBRPMA_PRIVATE_DATA_H

#include <rdma/rdma_cma.h>

/*
 * ERRORS
 * rpma_pdata_new() can fail with the following errors:
 *
 * - XXX
 */
int rpma_private_data_new(struct rdma_cm_event *edata,
		struct rpma_conn_private_data **pdata_ptr);

/*
 * ERRORS
 * rpma_pdata_delete() can fail with the following errors:
 *
 * - XXX
 */
int rpma_private_data_delete(struct rpma_conn_private_data **pdata_ptr);

#endif /* LIBRPMA_PRIVATE_DATA_H */
