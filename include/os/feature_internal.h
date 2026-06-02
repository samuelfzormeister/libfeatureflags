/*
 * Copyright (c) 2025-2026 The PureDarwin Project, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __OS_FEATURE_INTERNAL__
#define __OS_FEATURE_INTERNAL__

#define __OS_EXPOSE_INTERNALS__ 1
#include <os/internal/internal_shared.h>

#include <os/alloc_once_private.h>
#include <os/debug_private.h>
#include <os/feature_private.h>
#include <os/lock.h>
#include <os/stdlib.h>

#if __has_include(<xpc/private.h>)
#include <xpc/private.h>
#else
#include <xpc/xpc.h>

extern xpc_object_t xpc_create_from_plist(void *data, size_t size);
#endif

#if __OS_DONT_USE_XPC__
#warning This is a note for the logs that XPC has been disabled for this build.
#endif


__BEGIN_DECLS

#define os_feature_log(msg, ...) os_debug_log("libfeatureflags", msg, ## __VA_ARGS__)

// --- What init_featureflags puts into the shared memory bufffer. --- //
struct _os_feature_table_s {
    uint64_t magic;
    uint32_t count;
    struct {
        uint64_t hash; /**/
        bool enabled;
    } entries[];
} typedef * os_feature_table_t;

struct _os_feature_globals_s {
    os_feature_table_t feature_table;
    os_once_t once_token;
    os_unfair_lock_t flags_lock;
    xpc_object_t xpc_cache;
};

void _os_feature_globals_init(void *ctx);

inline struct _os_feature_globals_s *_os_feature_globals(void)
{
    return (struct os_feature_globals_s *)
            os_alloc_once(OS_ALLOC_ONCE_KEY_LIBSYSTEM_FEATUREFLAGS,
                          sizeof(struct _os_feature_globals_s), _os_feature_globals_init);
}

os_feature_table_t _os_feature_table(void);

bool _os_feature_is_enabled_slow(const char *domain, const char *feature);

#define OS_FEATURE_SYSTEM_PATH "/System/Library/FeatureFlags"
#define OS_FEATURE_LIBRARY_PATH "/Library/Preferences/FeatureFlags"

OS_INLINE
const char **_os_feature_search_paths(void)
{
    static const char *paths[] = {
        OS_FEATURE_SYSTEM_PATH,
        OS_FEATURE_LIBRARY_PATH,
        NULL,
    };

    return paths;
}

#define FEATURE_INTERNAL_CRASH(c, x) __extension__({ \
        _os_set_crash_log_cause_and_message(c, "BUG IN LIBFEATUREFLAGS: " x); \
        __builtin_trap(); \
    })

#define FEATURE_CLIENT_CRASH(c, x) __extension__({ \
        _os_set_crash_log_cause_and_message(c, \
                "BUG IN CLIENT OF LIBFEATUREFLAGS: " x); \
        __builtin_trap(); \
    })
__END_DECLS

#endif /* __OS_FEATURE_INTERNAL__ */
