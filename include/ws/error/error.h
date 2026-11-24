/**
 * @file error.h
 * @brief Typed error values for embedded C — the ws-error-embedded-c base library.
 *
 * Realizes embedded_c_tech_spec.md §6: failures are typed struct VALUES
 * returned by value (never bare enums, never bools, never errno, never
 * exceptions). Every module declares its own code enum with WS_<NAME>_OK = 0
 * first, then generates its typed error struct via WS_ERROR_TYPE().
 *
 * Heap-free and zero-dependency (freestanding C17 core only): message and
 * source point at static storage (string literals in flash/rodata) or NULL.
 *
 * Consumers include this header as <ws/error/error.h>.
 */
#ifndef WS_WS_ERROR_ERROR_H
#define WS_WS_ERROR_ERROR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fixed severity classes shared across every firmware error value.
 */
typedef enum {
    WS_SEVERITY_RECOVERABLE = 0, /**< Retry or fall back. */
    WS_SEVERITY_DEGRADED = 1, /**< Continue with reduced capability. */
    WS_SEVERITY_FATAL = 2, /**< System cannot continue. */
} ws_severity_t;

/**
 * @brief Define a module-specific typed error value over @p code_enum.
 *
 * Every base library and every system defines its error type with this macro
 * so WS_OK(), WS_FAILED(), WS_ERR_OK(), and WS_ERR() work uniformly without
 * losing static type safety across modules.
 *
 * Example:
 * @code
 * typedef enum {
 *     WS_SENSOR_OK = 0,
 *     WS_SENSOR_TIMEOUT,
 *     WS_SENSOR_BAD_CONFIG,
 * } ws_sensor_err_t;
 * WS_ERROR_TYPE(ws_sensor_err_t, ws_sensor_error_t);
 * @endcode
 */
#define WS_ERROR_TYPE(code_enum, name)                                                             \
    typedef struct {                                                                               \
        code_enum code; /**< Module-specific code; 0 means success. */                             \
        ws_severity_t severity; /**< Severity class (meaningful on failure). */                    \
        const char *message; /**< Optional static string literal, or NULL. */                      \
        const char *source; /**< Optional static origin literal, or NULL. */                       \
    } name

/**
 * @brief Non-zero when @p err represents success (code == 0).
 * Evaluates @p err exactly once.
 */
#define WS_OK(err) ((err).code == 0)

/**
 * @brief Non-zero when @p err represents failure (code != 0).
 * Evaluates @p err exactly once.
 */
#define WS_FAILED(err) ((err).code != 0)

/**
 * @brief Canonical success value for any WS_ERROR_TYPE() struct @p err_type.
 */
#define WS_ERR_OK(err_type)                                                                        \
    ((err_type){ .code = 0, .severity = WS_SEVERITY_RECOVERABLE, .message = NULL, .source = NULL })

/**
 * @brief Construct a failure value of @p err_type with static message/source.
 */
#define WS_ERR(err_type, code_val, sev, msg, src)                                                  \
    ((err_type){ .code = (code_val), .severity = (sev), .message = (msg), .source = (src) })

/**
 * @brief Static human-readable label for a severity class (never NULL).
 */
const char *ws_severity_str(ws_severity_t severity);

#ifdef __cplusplus
}
#endif

#endif /* WS_WS_ERROR_ERROR_H */
