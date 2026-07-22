#include <jni.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "zygisk.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "hook.h" // Terhubung ke file hook.cpp

#define TARGET_PACKAGE "com.pixel.gun3d"

int glWidth = 0, glHeight = 0;
bool isMenuInitialized = false;

// Fungsi menggambar menu visual ImGui
void DrawMenuContent() {
    if (!g_MenuOpen) return;

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Stardust PG3D - Zygisk", &g_MenuOpen);
    ImGui::Text("Target: Pixel Gun 3D");
    ImGui::Separator();
    
    // Checkbox terhubung ke variabel di hook.cpp
    ImGui::Checkbox("Unlimited Ammo (Dobby)", &feature_UnlimitedAmmo);
    
    ImGui::End();
}

// Intersepsi eglSwapBuffers untuk menampilkan ImGui di layar game
EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface) = nullptr;
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!isMenuInitialized) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);
        ImGui_ImplOpenGL3_Init("#version 100");
        isMenuInitialized = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    DrawMenuContent();

    ImGui::Render();
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return orig_eglSwapBuffers(dpy, surface);
}

// Siklus Hidup Zygisk
class StardustZygiskModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        const char *process_name = env->GetStringUTFChars(args->nice_name, nullptr);
        
        if (process_name && strcmp(process_name, TARGET_PACKAGE) == 0) {
            api->setOption(zygisk::Option::FORCE_DENYLIST_UNMOUNT);
            
            // Memicu thread pencarian memori game
            pthread_t ntid;
            pthread_create(&ntid, nullptr, hack_thread, nullptr);
            
            // Mengaitkan ImGui ke eglSwapBuffers sistem
            void *eglSwapBuffers_sym = DobbySymbolResolver("libEGL.so", "eglSwapBuffers");
            DobbyHook(eglSwapBuffers_sym, (void *)hook_eglSwapBuffers, (void **)&orig_eglSwapBuffers);
        }
        
        if (process_name) {
            env->ReleaseStringUTFChars(args->nice_name, process_name);
        }
    }
};

REGISTER_ZYGISK_MODULE(StardustZygiskModule)
