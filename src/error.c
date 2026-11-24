/**
 * @file error.c
 * @brief Implementation of ws-error-embedded-c helpers.
 *
 * Heap-free, zero global state; returns pointers to static string literals.
 */
#include <ws/error/error.h>

const char *ws_severity_str(ws_severity_t severity)
{
    switch (severity) {
    case WS_SEVERITY_RECOVERABLE:
        return "recoverable";
    case WS_SEVERITY_DEGRADED:
        return "degraded";
    case WS_SEVERITY_FATAL:
        return "fatal";
    default:
        return "unknown";
    }
}
