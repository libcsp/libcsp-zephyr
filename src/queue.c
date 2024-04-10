/*
 * Copyright (c) 2024 Space Cubics, LLC.
 *
 * SPDX - License - Identifier: Apache - 2.0
 */

#include <zephyr/ztest.h>
#include <csp/csp.h>

/* test enqueue, dequeue */
ZTEST(queue, test_queue_enqueue_dequeue_one)
{
	char item1[] = "abc";
	char item2[sizeof(item1)];

	int qlength = 10;
	int buf_size = qlength * sizeof(item1);
	char buf[buf_size];

	csp_queue_handle_t qh;
	csp_static_queue_t q;

	/* zero clear */
	memset(buf, 0, buf_size);
	memset(item2, 0, sizeof(item2));

	csp_init();

	/* create */
	qh = csp_queue_create_static(qlength, sizeof(item1), buf, &q);

	/* enqueue, dequeue */
	zassert_equal(csp_queue_enqueue(qh, item1, 1000), CSP_QUEUE_OK);
	zassert_equal(csp_queue_dequeue(qh, item2, 1000), CSP_QUEUE_OK);

	/* compare item1 & item2 */
	zassert_equal(memcmp(item1, item2, sizeof(item1)), 0);
}

/* test enqueue, dequeue when full */
ZTEST(queue, test_queue_enqueue_dequeue_full)
{
	char item1[] = "abc";
	char item2[sizeof(item1)];

	int qlength = 10;
	int buf_size = qlength * sizeof(item1);
	char buf[buf_size];

	csp_queue_handle_t qh;
	csp_static_queue_t q;

	/* zero clear */
	memset(buf, 0, buf_size);
	memset(item2, 0, sizeof(item2));

	csp_init();

	/* create */
	qh = csp_queue_create_static(qlength, sizeof(item1), buf, &q);

	/* enqueue to the limit */
	for (int i = 0; i < qlength; i++) {
		zassert_equal(csp_queue_enqueue(qh, item1, 1000), CSP_QUEUE_OK);
	}
	/* dequeue to the limit */
	for (int i = 0; i < qlength; i++) {
		zassert_equal(csp_queue_dequeue(qh, buf, 1000), CSP_QUEUE_OK);
	}
}

/* test when full, enqueue additionally */
ZTEST(queue, test_queue_enqueue_overflow)
{
	char item1[] = "abc";
	char item2[sizeof(item1)];

	int qlength = 10;
	int buf_size = qlength * sizeof(item1);
	char buf[buf_size];

	csp_queue_handle_t qh;
	csp_static_queue_t q;

	/* zero clear */
	memset(buf, 0, buf_size);
	memset(item2, 0, sizeof(item2));

	csp_init();

	/* create */
	qh = csp_queue_create_static(qlength, sizeof(item1), buf, &q);

	/* enqueue to the limit */
	for (int i = 0; i < qlength; i++) {
		zassert_equal(csp_queue_enqueue(qh, item1, 1000), CSP_QUEUE_OK);
	}

	/* The queue is full, so enqueue operation throws an error */
	zassert_equal(csp_queue_enqueue(qh, item1, 1000), CSP_QUEUE_ERROR);
}

/* test when empty, dequeue one more */
ZTEST(queue, test_queue_dequeue_underflow)
{
	char item1[] = "abc";
	char item2[sizeof(item1)];

	int qlength = 10;
	int buf_size = qlength * sizeof(item1);
	char buf[buf_size];

	csp_queue_handle_t qh;
	csp_static_queue_t q;

	/* zero clear */
	memset(buf, 0, buf_size);
	memset(item2, 0, sizeof(item2));

	csp_init();

	/* create */
	qh = csp_queue_create_static(qlength, sizeof(item1), buf, &q);

	/* The queue is empty, so dequeue operation throws an error */
	zassert_equal(csp_queue_dequeue(qh, item2, 1000), CSP_QUEUE_ERROR);
}

/* test queue_free for queue size */
ZTEST(queue, test_queue_size)
{
	char item1[] = "abc";
	char item2[sizeof(item1)];

	int qlength = 10;
	int buf_size = qlength * sizeof(item1);
	char buf[buf_size];

	csp_queue_handle_t qh;
	csp_static_queue_t q;
	int qsize;

	/* zero clear */
	memset(buf, 0, buf_size);
	memset(item2, 0, sizeof(item2));

	csp_init();

	/* create */
	qh = csp_queue_create_static(qlength, sizeof(item1), buf, &q);

	/* Before enqueue, size is 0 */
	qsize = csp_queue_size(qh);
	zassert_equal(qsize, 0);

	/* After enqueue, size is 1 */
	csp_queue_enqueue(qh, item1, 1000);
	zassert_equal(csp_queue_size(qh), qsize + 1);

	/* After dequeue, size is 1 */
	qsize = csp_queue_size(qh);
	csp_queue_dequeue(qh, item2, 1000);
	zassert_equal(csp_queue_size(qh), qsize - 1);
}

/* test queue_free for available size */
ZTEST(queue, test_queue_free)
{
	char item1[] = "abc";
	char item2[sizeof(item1)];

	int qlength = 10;
	int buf_size = qlength * sizeof(item1);
	char buf[buf_size];

	csp_queue_handle_t qh;
	csp_static_queue_t q;

	/* zero clear */
	memset(buf, 0, buf_size);
	memset(item2, 0, sizeof(item2));

	csp_init();

	/* create */
	qh = csp_queue_create_static(qlength, sizeof(item1), buf, &q);

	/* Before enqueue, the available size is qlength */
	zassert_equal(csp_queue_free(qh), qlength);

	/* After enqueue, the available size is qlength - 1. */
	csp_queue_enqueue(qh, item1, 1000);
	zassert_equal(csp_queue_free(qh), qlength - 1);

	/* After dequeue, the available size is qlength */
	csp_queue_dequeue(qh, item2, 1000);
	zassert_equal(csp_queue_free(qh), qlength);
}

ZTEST_SUITE(queue, NULL, NULL, NULL, NULL, NULL);
