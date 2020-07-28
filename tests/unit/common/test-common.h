/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2020, Intel Corporation
 */

/*
 * test-common.h -- a test's common header
 */

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#define MOCK_IP_ADDRESS		"127.0.0.1"
#define MOCK_SERVICE		"1234" /* a random port number */

extern struct ibv_context Ibv_context;	/* mock IBV context */
extern struct ibv_cq Ibv_cq;		/* mock IBV CQ */

/* random values */
#define MOCK_VERBS		(struct ibv_context *)0x4E4B
#define MOCK_IBV_CQ		(struct ibv_cq *)&Ibv_cq
#define MOCK_QP			(struct ibv_qp *)0xC41D
#define MOCK_PEER		(struct rpma_peer *)0xFEEF
#define MOCK_INFO		(struct rpma_info *)0xE6B2
#define MOCK_CONN		(struct rpma_conn *)0xC004
#define MOCK_PRIVATE_DATA	((void *)"Random data")
#define MOCK_PDATA_LEN		(strlen(MOCK_PRIVATE_DATA) + 1)
#define MOCK_PRIVATE_DATA_2	((void *)"Another random data")
#define MOCK_PDATA_LEN_2	(strlen(MOCK_PRIVATE_DATA_2) + 1)
#define MOCK_LEN		(size_t)0xC415
#define MOCK_OP_CONTEXT		(void *)0xC417

#define MOCK_OK 0

#endif /* TEST_COMMON_H */
