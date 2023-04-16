#include "demo_procs.h"

procs_t procs;

#ifdef DEMO_RTDL
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void procs_deinit(void) {
    if (procs.module) {
        dlclose(procs.module);
    }
}

int procs_reload(void) {
    procs_deinit();

    // generate temporary filename
    char *tmp_file_path = tmpnam(NULL);

    // copy module file to temporary file
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/cp", "cp", MODULE_PATH, tmp_file_path, (char *)NULL);
    } else if (pid < 0) {
        fprintf(stderr, "Calling execl /bin/cp failed\n");
        return EXIT_FAILURE;
    } else {
        wait(NULL);
    }

    // load scene module from temporary file
    procs.module = dlopen(tmp_file_path, RTLD_LAZY);
    if (!procs.module) {
        fprintf(stderr, "%s\n", dlerror());
        return EXIT_FAILURE;
    }

    // unlink (remove) temporary file
    unlink(tmp_file_path);

    // load scene api
    *(void **)(&procs.scene_init) = dlsym(procs.module, "scene_init");
    *(void **)(&procs.scene_deinit) = dlsym(procs.module, "scene_deinit");
    *(void **)(&procs.scene_render) = dlsym(procs.module, "scene_render");
    if (!procs.scene_init || !procs.scene_deinit || !procs.scene_render) {
        fprintf(stderr, "Can't load symbols from module\n");
        dlclose(procs.module);
        return EXIT_FAILURE;
    }

    printf("Scene module loaded\n");
    return EXIT_SUCCESS;
}

#else

void procs_deinit(void) {}
int procs_reload(void) { return 0; }

#endif
