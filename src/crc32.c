/*
 * Copyright (c) 2024 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/ztest.h>
#include <csp/csp.h>
#include <csp/csp_crc32.h>
#include <csp/csp_id.h>
#include <endian.h>

struct crc_fixture {
	csp_packet_t *buf;
};

static struct crc_fixture crc_fixture;

static void *setup(void)
{
	csp_init();

	return &crc_fixture;
}

static void before(void *data)
{
	struct crc_fixture *fixture = data;

	fixture->buf = csp_buffer_get(0);
	zassert_true(fixture->buf != NULL, NULL);
}

static void after(void *data)
{
	struct crc_fixture *fixture = data;

	csp_buffer_free(fixture->buf);
}

ZTEST_F(crc, test_crc_verify_without_crc)
{
	csp_packet_t *packet = fixture->buf;
	int ret;

	/* Check size error */
	packet->length = 0;
	ret = csp_crc32_verify(packet);
	zassert_equal(ret, CSP_ERR_CRC32, "Expected error on size in csp_crc32_verify");

	/* Check without crc */
	memcpy(packet->data, "Hello", 5);
	packet->length = 5;
	ret = csp_crc32_verify(packet);
	zassert_equal(ret, CSP_ERR_CRC32, "Expected error on crc in csp_crc32_verify");
}

ZTEST_F(crc, test_crc_without_header)
{
	csp_packet_t *packet = fixture->buf;
	int ret;

	memcpy(packet->data, "Hello", 5);
	packet->length = 5;

	/* Check without header */
	csp_crc32_append(packet);
	ret = csp_crc32_verify(packet);
	zassert_equal(CSP_ERR_NONE, ret, "Error on csp_crc32_verify");
}

ZTEST_F(crc, test_crc_with_header)
{
	csp_packet_t *packet = fixture->buf;
	uint32_t crc;
	int ret;

	memcpy(packet->data, "Hello", 5);
	packet->length = 5;

	/* Check with header.
	 * This simulate CSP_21 define on csp_crc32_append().
	 */
	csp_id_prepend(packet);
	crc = csp_crc32_memory(packet->frame_begin, packet->frame_length);
	/* Convert to network byte order */
	crc = htobe32(crc);
	/* Copy checksum to packet */
	memcpy(&packet->data[packet->length], &crc, sizeof(crc));
	packet->length += sizeof(crc);

	ret = csp_crc32_verify(packet);
	zassert_equal(CSP_ERR_NONE, ret, "Error on csp_crc32_verify");
}

ZTEST_SUITE(crc, NULL, setup, before, after, NULL);
