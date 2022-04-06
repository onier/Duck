#include "rttr/registration.h"
#include "WebClientAuthorizeHandler.h"

RTTR_PLUGIN_REGISTRATION {
    rttr::registration::class_<WebClientAuthorizeHandler>("WebClientAuthorizeHandler")
            (
                    rttr::metadata("key", "WebSocketClientHandler")
            )
            .constructor<>()
                    (
                            rttr::policy::ctor::as_std_shared_ptr
                    );
};