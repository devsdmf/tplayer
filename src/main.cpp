
#include <iostream>
#include <string>
#include <unistd.h>
#include <mikmod.h>
#include <gui.hpp>

using namespace std;

int main(int argc, char** argv) {
    if (argc > 1) {
        char *audio_file = argv[1];
        MODULE *module;

        MikMod_InitThreads();
        MikMod_RegisterAllDrivers();
        MikMod_RegisterAllLoaders();

        md_mode |= DMODE_SOFT_MUSIC | DMODE_NOISEREDUCTION | DMODE_INTERP;
        if (MikMod_Init("")) {
            cout << "Could not initialize audio driver, reason: ", MikMod_strerror(MikMod_errno);
            return 1;
        }

        module = Player_Load(audio_file, 64, 0);
        if (module) {
            Player_Start(module);

            while (Player_Active()) {
                usleep(10000);
                MikMod_Update();
            }

            Player_Stop();
            Player_Free(module);
        } else {
            cout << "Could not load module file, reason: ", MikMod_strerror(MikMod_errno);
        }

        MikMod_Exit();
    }

    start();

    return 0;
}

