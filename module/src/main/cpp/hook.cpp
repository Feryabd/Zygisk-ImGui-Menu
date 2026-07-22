#include <unistd.h>
#include <pthread.h>
#include "hook.h"
#include "dobby.h"
#include "KittyMemory/KittyMemory.h"

// Inisialisasi variabel global
bool g_MenuOpen = true;
bool feature_UnlimitedAmmo = false;

// Penampung fungsi amunisi asli game
int (*orig_get_AmmoInClip)(void *instance) = nullptr;

// Fungsi modifikasi kita untuk membekukan amunisi
int hook_get_AmmoInClip(void *instance) {
    if (instance != nullptr && feature_UnlimitedAmmo) {
        return 9999; // Mengunci amunisi di angka 9999 saat toggle aktif
    }
    return orig_get_AmmoInClip(instance);
}

// Thread yang berjalan di latar belakang mencari libil2cpp.so milik game
void *hack_thread(void *) {
    proc_info_t g_il2cppBaseMap;
    
    // Looping mencari pendaftaran alamat memori libil2cpp.so
    do {
        sleep(1);
        g_il2cppBaseMap = KittyMemory::getElfBaseMap("libil2cpp.so");
    } while (!g_il2cppBaseMap.isValid());

    // Alamat Offset target amunisi Pixel Gun 3D Anda: 0x58C46DC
    uintptr_t ammo_address = g_il2cppBaseMap.startAddress + 0x58C46DC;

    // Menjalankan fungsi pengaitan memori via DobbyHook
    DobbyHook((void *)ammo_address, (void *)hook_get_AmmoInClip, (void **)&orig_get_AmmoInClip);

    return nullptr;
}
