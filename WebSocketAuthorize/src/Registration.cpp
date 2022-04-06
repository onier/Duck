#include "rttr/registration.h"
#include "AuthorizeHandler.h"

RTTR_PLUGIN_REGISTRATION {
    rttr::registration::class_<AuthorizeHandler>("AuthorizeHandler")
            (
                    rttr::metadata("key", "WebSocketServerHandler")
            )
            .constructor<>()
                    (
                            rttr::policy::ctor::as_std_shared_ptr
                    );
};