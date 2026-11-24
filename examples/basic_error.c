/**
 * @file basic_error.c
 * @brief Minimal example showing WS_ERROR_TYPE() usage in ws-error-embedded-c.
 */
#include <stdint.h>
#include <stdio.h>

#include <ws/error/error.h>

typedef enum {
    WS_SAMPLE_OK = 0,
    WS_SAMPLE_OUT_OF_RANGE,
} ws_sample_err_t;
WS_ERROR_TYPE(ws_sample_err_t, ws_sample_error_t);

static ws_sample_error_t clamp_percent(int32_t raw, uint8_t *out)
{
    if (out == NULL || raw < 0 || raw > 100) {
        return WS_ERR(ws_sample_error_t, WS_SAMPLE_OUT_OF_RANGE, WS_SEVERITY_RECOVERABLE,
                      "percent must be 0..100", "example.clamp_percent");
    }
    *out = (uint8_t)raw;
    return WS_ERR_OK(ws_sample_error_t);
}

int main(void)
{
    uint8_t pct = 0u;
    ws_sample_error_t err = clamp_percent(120, &pct);
    if (WS_FAILED(err)) {
        printf("[%s] %s: %s\n", ws_severity_str(err.severity), err.source, err.message);
    }
    return 0;
}
