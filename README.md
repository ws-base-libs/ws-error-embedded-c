# ws-error-embedded-c

Typed error values for Embedded C (`embedded_c_tech_spec.md` §6).

## Purpose

Every fallible operation in this firmware family returns a **typed struct by value** declared via `WS_ERROR_TYPE(code_enum, name)`, carrying:

1. `code` — a module-specific enum whose first enumerator is `WS_<NAME>_OK = 0`,
2. `severity` — one of `WS_SEVERITY_RECOVERABLE`, `WS_SEVERITY_DEGRADED`, `WS_SEVERITY_FATAL`,
3. `message` — optional static string literal (or `NULL`),
4. `source` — optional static origin string literal (or `NULL`).

Zero dynamic allocation, zero external dependencies, no global `errno`.

## Quick start

```c
#include <ws/error/error.h>

typedef enum {
    WS_SENSOR_OK = 0,
    WS_SENSOR_TIMEOUT,
    WS_SENSOR_BAD_CONFIG,
} ws_sensor_err_t;
WS_ERROR_TYPE(ws_sensor_err_t, ws_sensor_error_t);

ws_sensor_error_t ws_sensor_read(uint32_t *out)
{
    if (out == NULL) {
        return WS_ERR(ws_sensor_error_t, WS_SENSOR_BAD_CONFIG,
                      WS_SEVERITY_RECOVERABLE, "NULL out", "sensor.read");
    }
    *out = 25000u;
    return WS_ERR_OK(ws_sensor_error_t);
}
```

See [examples/basic_error.c](examples/basic_error.c).

## API reference

Public header: `<ws/error/error.h>` (guard `WS_WS_ERROR_ERROR_H`).

| Symbol | Meaning |
|---|---|
| `ws_severity_t` | `WS_SEVERITY_RECOVERABLE = 0`, `WS_SEVERITY_DEGRADED = 1`, `WS_SEVERITY_FATAL = 2` |
| `WS_ERROR_TYPE(code_enum, name)` | Defines `typedef struct { code_enum code; ws_severity_t severity; const char *message; const char *source; } name` |
| `WS_OK(err)` / `WS_FAILED(err)` | Generic success (`code == 0`) / failure (`code != 0`) predicates on any `WS_ERROR_TYPE` value |
| `WS_ERR_OK(err_type)` | Canonical success initializer (`code = 0`, `severity = WS_SEVERITY_RECOVERABLE`, `NULL`, `NULL`) |
| `WS_ERR(err_type, code, sev, msg, src)` | Failure value initializer |
| `ws_severity_str(ws_severity_t)` | Static human-readable severity string (never `NULL`) |

## Config reference

None. No compile-time configuration macros or runtime initialization.

## Build & test (standalone)

```sh
cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure
```

## Quirks

- `message` and `source` MUST point at static storage (string literals) or `NULL`; pointers are stored verbatim and never copied.
- `WS_OK(err)` and `WS_FAILED(err)` take struct values, not pointers, and evaluate their argument once.
- Never typedef a bare enum as `ws_<name>_error_t` — always use `WS_ERROR_TYPE`.
