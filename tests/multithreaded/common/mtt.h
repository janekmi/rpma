/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright 2021, Intel Corporation */

/*
 * mtt.h -- a multithreaded tests' API
 */

#ifndef MTT
#define MTT

#include <stddef.h>

/* arguments coming from the command line */
struct mtt_args {
	unsigned threads_num;
	char *addr;
};

int mtt_parse_args(int argc, char *argv[], struct mtt_args *args);

#define MTT_ERRMSG_MAX 512

/* a store for any thread error message and the return value */
struct mtt_result {
	int ret;
	char errmsg[MTT_ERRMSG_MAX];
};

/*
 * mtt_base_file_name -- extract the exact file name from a file name with path
 */
static const char *
mtt_base_file_name(const char *file_name)
{
	const char *base_file_name = strrchr(file_name, '/');
	if (!base_file_name)
		base_file_name = file_name;
	else
		/* skip '/' */
		base_file_name++;

	return base_file_name;
}

/* on error populate the result and the error string */
#define MTT_ERR(result, func, err) \
	do { \
		(result)->ret = err; \
		snprintf((result)->errmsg, MTT_ERRMSG_MAX - 1, \
			"%s:%d %s() -> %s() failed: %s", \
			mtt_base_file_name(__FILE__), __LINE__, __func__, \
			func, strerror(err)); \
	} while (0)

/* on librpma error populate the result and the error string */
#define MTT_RPMA_ERR(result, func, err) \
	do { \
		(result)->ret = err; \
		snprintf((result)->errmsg, MTT_ERRMSG_MAX - 1, \
			"%s:%d %s() -> %s() failed: %s", \
			mtt_base_file_name(__FILE__), __LINE__, __func__, \
			func, rpma_err_2str(err)); \
	} while (0)

typedef void (*mtt_thread_init_fini_func)(unsigned id, void *prestate,
		void **state_ptr, struct mtt_result *result);
typedef void (*mtt_thread_func)(unsigned id, void *prestate, void *state,
		struct mtt_result *result);

struct mtt_test {
	/*
	 * a pointer to test-provided data passed on all initialization steps
	 * (both sequential and parallel) and also on thread_func
	 */
	void *prestate;

	/*
	 * a function called for each of threads before spawning it (sequential)
	 */
	mtt_thread_init_fini_func thread_seq_init_func;

	/*
	 * a function called at the beginning of each thread
	 * (parallel but before synchronizing all threads)
	 */
	mtt_thread_init_fini_func thread_init_func;

	/*
	 * a thread main function (parallel and after synchronizing all threads)
	 */
	mtt_thread_func thread_func;

	/* a function called at the end of each thread (parallel) */
	mtt_thread_init_fini_func thread_fini_func;

	/*
	 * a function called for each of threads after its termination
	 * (sequential)
	 */
	mtt_thread_init_fini_func thread_seq_fini_func;
};

int mtt_run(struct mtt_test *test, unsigned threads_num);

#endif /* MTT */
