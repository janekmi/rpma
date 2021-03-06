// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2021, Intel Corporation */

/*
 * rpma_utils_ibv_context_is_odp_capable.c -- 'check odp' multithreaded test
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <librpma.h>

struct thread_args {
	int thread_num;
	struct ibv_context *dev;
	int is_odp_supported_exp;
};

static const char *api_name = "rpma_utils_ibv_context_is_odp_capable";

static void *thread_main(void *arg)
{
	int ret;
	int is_odp_supported = 0;

	/* parameters */
	struct thread_args *p_thread_args = (struct thread_args *)arg;

	/* check if the device context's capability supports On-Demand Paging */
	ret = rpma_utils_ibv_context_is_odp_capable(p_thread_args->dev,
				&is_odp_supported);

	if (ret) {
		fprintf(stderr, "[thread #%d] %s failed: %s\n",
			p_thread_args->thread_num, api_name,
			rpma_err_2str(ret));
		exit(-1);
	} else if (p_thread_args->is_odp_supported_exp != is_odp_supported) {
		fprintf(stderr,
				"[thread #%d] %s returned an unexpected value: is_odp_supported = %d, is_odp_supported_exp = %d\n",
			p_thread_args->thread_num, api_name,
			is_odp_supported, p_thread_args->is_odp_supported_exp);
		exit(-1);
	}

	return NULL;
}

int
main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "usage: %s <thread_num> <addr>\n", argv[0]);
		return -1;
	}

	/* configure logging thresholds to see more details */
	rpma_log_set_threshold(RPMA_LOG_THRESHOLD, RPMA_LOG_LEVEL_INFO);
	rpma_log_set_threshold(RPMA_LOG_THRESHOLD_AUX, RPMA_LOG_LEVEL_INFO);

	/* parameters */
	int ret = 0;
	int i;
	int thread_num = (int)strtoul(argv[1], NULL, 10);
	char *addr = argv[2];

	/* resources */
	struct ibv_context *dev = NULL;
	/* obtain an IBV context for a local IP address */
	ret = rpma_utils_get_ibv_context(addr, RPMA_UTIL_IBV_CONTEXT_LOCAL,
				&dev);
	if (ret) {
		fprintf(stderr, "rpma_utils_get_ibv_context() failed");
		return -1;
	}

	/* check if the device context's capability supports On-Demand Paging */
	int is_odp_supported = 0;
	ret = rpma_utils_ibv_context_is_odp_capable(dev, &is_odp_supported);
	if (ret) {
		fprintf(stderr,
			"rpma_utils_ibv_context_is_odp_capable() failed");
		return -1;
	}

	pthread_t *p_threads;
	p_threads = malloc(sizeof(pthread_t) * (unsigned int)thread_num);
	if (p_threads == NULL) {
		fprintf(stderr, "malloc() failed");
		return -1;
	}

	struct thread_args *threads_args = malloc(sizeof(struct thread_args)
				* (unsigned int)thread_num);
	if (threads_args == NULL) {
		fprintf(stderr, "malloc() failed");
		ret = -1;
		goto err_free_p_threads;
	}

	for (i = 0; i < thread_num; i++) {
		threads_args[i].thread_num = i;
		threads_args[i].dev = dev;
		threads_args[i].is_odp_supported_exp = is_odp_supported;
	}

	for (i = 0; i < thread_num; i++) {
		if ((ret = pthread_create(&p_threads[i], NULL, thread_main,
				&threads_args[i])) != 0) {
			fprintf(stderr, "Cannot start the thread #%d: %s\n",
				i, strerror(ret));
			/*
			 * Set thread_num to the number
			 * of already created threads
			 * to join them below.
			 */
			thread_num = i;
			/* return -1 on error */
			ret = -1;
			break;
		}
	}

	for (i = thread_num - 1; i >= 0; i--)
		pthread_join(p_threads[i], NULL);

	free(threads_args);

err_free_p_threads:
	free(p_threads);

	return ret;
}
