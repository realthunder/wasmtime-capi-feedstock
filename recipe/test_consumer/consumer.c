/* Does this package give an embedder a usable runtime?
 *
 * The discriminator is at COMPILE time: a wasmtime too old for the
 * WebAssembly exception-handling proposal does not declare
 * wasmtime_config_wasm_exceptions_set, so this file does not build against
 * it. That proposal is what lets a guest compiled from C++ throw at all,
 * and it is the same declaration FreeCAD's FindWasmtime.cmake greps for.
 *
 * The run then compiles a module that uses a tag and a throw. Measured on
 * 48.0.1: the proposal is ON BY DEFAULT there, so turning it on explicitly
 * changes nothing and this part does not discriminate by itself -- it is a
 * smoke test that the packaged library loads, resolves and works, which a
 * relocation or SONAME mistake in build.sh would break.
 */
#include <stdio.h>
#include <string.h>

#include <wasmtime.h>

/* (module (tag $e) (func (export "boom") (throw $e))) -- hand-assembled so
 * the test needs no toolchain beyond a C compiler. */
static const unsigned char MODULE_WAT[] =
    "(module\n"
    "  (tag $e)\n"
    "  (func (export \"boom\") (throw $e))\n"
    ")\n";

int main(void)
{
    wasm_config_t *config = wasm_config_new();
    if (!config) {
        fprintf(stderr, "wasm_config_new failed\n");
        return 1;
    }
    wasmtime_config_wasm_exceptions_set(config, true);

    wasm_engine_t *engine = wasm_engine_new_with_config(config);
    if (!engine) {
        fprintf(stderr, "engine creation failed\n");
        return 1;
    }

    wasm_byte_vec_t wasm;
    wasmtime_error_t *error =
        wasmtime_wat2wasm((const char *)MODULE_WAT, strlen((const char *)MODULE_WAT), &wasm);
    if (error) {
        fprintf(stderr, "wat2wasm failed: the exception proposal is not "
                        "understood by this build\n");
        wasmtime_error_delete(error);
        wasm_engine_delete(engine);
        return 1;
    }

    wasmtime_module_t *module = NULL;
    error = wasmtime_module_new(engine, (const uint8_t *)wasm.data, wasm.size, &module);
    wasm_byte_vec_delete(&wasm);
    if (error) {
        fprintf(stderr, "module compilation failed: exception handling is "
                        "not enabled in this build\n");
        wasmtime_error_delete(error);
        wasm_engine_delete(engine);
        return 1;
    }

    printf("wasmtime compiled a module using the exception-handling "
           "proposal\n");
    wasmtime_module_delete(module);
    wasm_engine_delete(engine);
    return 0;
}
