/**
 * @file error_test.c
 * @brief Standalone host tests for ws-error-embedded-c.
 */
#include <stdio.h>
#include <string.h>

#include <ws/error/error.h>

typedef enum {
    WS_DEMO_OK = 0,
    WS_DEMO_TIMEOUT,
    WS_DEMO_BAD_CONFIG,
    WS_DEMO_HW_FAULT,
} ws_demo_err_t;
WS_ERROR_TYPE(ws_demo_err_t, ws_demo_error_t);

typedef enum {
    WS_OTHER_OK = 0,
    WS_OTHER_EMPTY,
} ws_other_err_t;
WS_ERROR_TYPE(ws_other_err_t, ws_other_error_t);

static int s_checks = 0;
static int s_failures = 0;

static void check(int cond, const char *desc)
{
    s_checks++;
    if (!cond) {
        s_failures++;
        fprintf(stderr, "FAIL: %s\n", desc);
    }
}

static ws_demo_error_t demo_read(int fail_mode, uint32_t *out)
{
    if (out == NULL) {
        return WS_ERR(ws_demo_error_t, WS_DEMO_BAD_CONFIG, WS_SEVERITY_RECOVERABLE, "NULL out",
                      "demo.read");
    }
    if (fail_mode == 1) {
        return WS_ERR(ws_demo_error_t, WS_DEMO_TIMEOUT, WS_SEVERITY_DEGRADED, "bus timed out",
                      "demo.read");
    }
    if (fail_mode == 2) {
        return WS_ERR(ws_demo_error_t, WS_DEMO_HW_FAULT, WS_SEVERITY_FATAL, NULL, NULL);
    }
    *out = 42u;
    return WS_ERR_OK(ws_demo_error_t);
}

int main(void)
{
    /* Canonical OK value. */
    ws_demo_error_t ok = WS_ERR_OK(ws_demo_error_t);
    check(ok.code == WS_DEMO_OK, "WS_ERR_OK code is 0");
    check(ok.severity == WS_SEVERITY_RECOVERABLE, "WS_ERR_OK default severity");
    check(ok.message == NULL, "WS_ERR_OK message NULL");
    check(ok.source == NULL, "WS_ERR_OK source NULL");
    check(WS_OK(ok), "WS_OK true on ok");
    check(!WS_FAILED(ok), "WS_FAILED false on ok");

    /* Out-param success path. */
    uint32_t val = 0u;
    ws_demo_error_t e0 = demo_read(0, &val);
    check(WS_OK(e0), "demo_read success WS_OK");
    check(val == 42u, "demo_read populates out-param");

    /* Failure with message + source + severity. */
    ws_demo_error_t e1 = demo_read(1, &val);
    check(WS_FAILED(e1), "timeout is WS_FAILED");
    check(!WS_OK(e1), "timeout is not WS_OK");
    check(e1.code == WS_DEMO_TIMEOUT, "timeout code preserved");
    check(e1.severity == WS_SEVERITY_DEGRADED, "degraded severity preserved");
    check(e1.message != NULL && strcmp(e1.message, "bus timed out") == 0, "message preserved");
    check(e1.source != NULL && strcmp(e1.source, "demo.read") == 0, "source preserved");

    /* Failure with optional NULL message/source. */
    ws_demo_error_t e2 = demo_read(2, &val);
    check(WS_FAILED(e2), "hw fault is WS_FAILED");
    check(e2.code == WS_DEMO_HW_FAULT, "hw fault code preserved");
    check(e2.severity == WS_SEVERITY_FATAL, "fatal severity preserved");
    check(e2.message == NULL, "optional message NULL allowed");
    check(e2.source == NULL, "optional source NULL allowed");

    /* Independent module error types coexist cleanly. */
    ws_other_error_t oe = WS_ERR(ws_other_error_t, WS_OTHER_EMPTY, WS_SEVERITY_RECOVERABLE,
                                 "queue empty", "other.pop");
    check(WS_FAILED(oe), "second error type works with WS_FAILED");
    check(oe.code == WS_OTHER_EMPTY, "second error type code preserved");

    /* Value-copy independence. */
    ws_demo_error_t copy = e1;
    check(copy.code == e1.code && copy.severity == e1.severity && copy.message == e1.message &&
              copy.source == e1.source,
          "struct copy preserves all fields");

    /* Severity labels. */
    check(strcmp(ws_severity_str(WS_SEVERITY_RECOVERABLE), "recoverable") == 0,
          "recoverable label");
    check(strcmp(ws_severity_str(WS_SEVERITY_DEGRADED), "degraded") == 0, "degraded label");
    check(strcmp(ws_severity_str(WS_SEVERITY_FATAL), "fatal") == 0, "fatal label");
    check(strcmp(ws_severity_str((ws_severity_t)99), "unknown") == 0, "unknown severity label");

    printf("error_test: %d checks, %d failures: %s\n", s_checks, s_failures,
           s_failures == 0 ? "PASS" : "FAIL");
    return s_failures == 0 ? 0 : 1;
}
