#include <cstring>
#include <dlfcn.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "PXM_Loader", __VA_ARGS__)

using zygisk::Api;
using zygisk::AppSpecializeArgs;

class PXMModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process != nullptr) {
            if (strcmp(process, "com.pubg.imobile") == 0) {
                should_load = true;
            }
            env->ReleaseStringUTFChars(args->nice_name, process);
        }
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        if (should_load) {
            void *handle = dlopen("/system/lib64/libPXM.so", RTLD_NOW);
            if (handle) {
                LOGD("libPXM.so successfully loaded!");
            } else {
                LOGD("Failed to load libPXM.so: %s", dlerror());
            }
        }
    }

private:
    Api *api = nullptr;
    JNIEnv *env = nullptr;
    bool should_load = false;
};

REGISTER_ZYGISK_MODULE(PXMModule)
