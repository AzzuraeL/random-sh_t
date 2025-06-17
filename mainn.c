#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>
#include <stdbool.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#define DEBUG 1
#if DEBUG
#define debug_print(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif

static bool trap_triggered = false;
const char *trigger_file = "/tmp/troll_trigger";

static bool is_daintontas() {
    struct fuse_context *ctx = fuse_get_context();
    struct passwd *pw = getpwuid(ctx->uid);
    return pw && strcmp(pw->pw_name, "DainTontas") == 0;
}

static void load_trap_status() {
    FILE *f = fopen(trigger_file, "r");
    trap_triggered = false;
    if (f) {
        int status = 0;
        if (fscanf(f, "%d", &status) == 1) {
            trap_triggered = (status != 0);
        }
        fclose(f);
    }
}

static void save_trap_status() {
    FILE *f = fopen(trigger_file, "w");
    if (f) {
        fprintf(f, "%d", trap_triggered);
        fclose(f);
    }
}

// Troll ASCII art
static const char *troll_art =
"     ______     ____   ____              _ __                       _                                           __\n"
"    / ____/__  / / /  / __/___  _____   (_) /_   ____ _____ _____ _(_)___     ________ _      ______ __________/ /\n"
"   / /_  / _ \\/ / /  / /_/ __ \\/ ___/  / / __/  / __ `/ __ `/ __ `/ / __ \\   / ___/ _ \\ | /| / / __ `/ ___/ __  / \n"
"  / __/ /  __/ / /  / __/ /_/ / /     / / /_   / /_/ / /_/ / /_/ / / / / /  / /  /  __/ |/ |/ / /_/ / /  / /_/ /  \n"
" /_/    \\___/_/_/  /_/  \\____/_/     /_/\\__/   \\__,_/\\__, /\\__,_/_/_/ /_/  /_/   \\___/|__/|__/\\__,_/_/   \\__,_/  \n"
"                                                    /____/                                                        \n";




static int xmp_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 2;
    } 
    else if (strcmp(path, "/very_spicy_info.txt") == 0 || 
             strcmp(path, "/upload.txt") == 0) {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;
        stbuf->st_size = 1024;
    }
    else {
        return -ENOENT;
    }
    
    stbuf->st_uid = geteuid();
    stbuf->st_gid = getegid();
    
    return 0;
}

static int xmp_access(const char *path, int mask) {
    debug_print("access: %s, mask: %d", path, mask);
    int res = access(path, mask);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi) {
    debug_print("readdir: %s", path);
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "very_spicy_info.txt", NULL, 0);
    filler(buf, "upload.txt", NULL, 0);

    return 0;
}	

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
    debug_print("mknod: %s", path);
    return -EPERM;
}

static int xmp_mkdir(const char *path, mode_t mode) {
    debug_print("mkdir: %s", path);
    return -EPERM;
}

static int xmp_unlink(const char *path) {
    debug_print("unlink: %s", path);
    return -EPERM;
}

static int xmp_rmdir(const char *path) {
    debug_print("rmdir: %s", path);
    return -EPERM;
}

static int xmp_symlink(const char *from, const char *to) {
    debug_print("symlink: %s -> %s", from, to);
    return -EPERM;
}

static int xmp_rename(const char *from, const char *to) {
    debug_print("rename: %s -> %s", from, to);
    return -EPERM;
}

static int xmp_link(const char *from, const char *to) {
    debug_print("link: %s -> %s", from, to);
    return -EPERM;
}

static int xmp_chmod(const char *path, mode_t mode) {
    debug_print("chmod: %s, mode: %o", path, mode);
    return -EPERM;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid) {
    debug_print("chown: %s, uid: %d, gid: %d", path, uid, gid);
    return -EPERM;
}

static int xmp_truncate(const char *path, off_t size)
{
    debug_print("truncate: %s", path);
    if (strcmp(path, "/upload.txt") == 0 && is_daintontas()) {
        return 0; 
    }

    return -EPERM;
}

#ifdef HAVE_UTIMENSAT
static int xmp_utimens(const char *path, const struct timespec ts[2]) {
    debug_print("utimens: %s", path);
    return 0;
}
#endif

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    debug_print("open: %s", path);
    
    if (strcmp(path, "/very_spicy_info.txt") != 0 && 
        strcmp(path, "/upload.txt") != 0 &&
        strcmp(path, "/") != 0)
        return -ENOENT;

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
    debug_print("read: %s, size: %zu, offset: %lld", path, size, (long long)offset);
    load_trap_status();

    if (trap_triggered && strstr(path, ".txt")) {
        size_t len = strlen(troll_art);
        if (offset >= len)
            return 0;
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, troll_art + offset, size);
        return size;
    }

    if (strcmp(path, "/very_spicy_info.txt") == 0) {
        const char *content = is_daintontas()
            ? "Very spicy internal developer information: leaked roadmap.docx\n"
            : "DainTontas' personal secret!!.txt\n";

        size_t len = strlen(content);
        if (offset >= len)
            return 0;
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, content + offset, size);
        return size;
    }

    if (strcmp(path, "/upload.txt") == 0) {
        return 0; // empty
    }

    return -ENOENT;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi) {
    debug_print("write: %s, size=%zu", path, size);

    if (strcmp(path, "/upload.txt") == 0 && is_daintontas()) {
        if (strstr(buf, "upload")) {
            trap_triggered = true;
            save_trap_status();
            debug_print("Trap triggered by DainTontas!");
        }
        return size;
    }

    return -EACCES;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf) {
    debug_print("statfs: %s", path);
    return 0;
}

static int xmp_release(const char *path, struct fuse_file_info *fi) {
    debug_print("release: %s", path);
    return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi) {
    debug_print("fsync: %s", path);
    return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int xmp_fallocate(const char *path, int mode,
                        off_t offset, off_t length, struct fuse_file_info *fi) {
    debug_print("fallocate: %s", path);
    return -EOPNOTSUPP;
}
#endif

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value,
                        size_t size, int flags) {
    debug_print("setxattr: %s, name: %s", path, name);
    return -EPERM; 
}

static int xmp_getxattr(const char *path, const char *name, char *value,
                        size_t size) {
    debug_print("getxattr: %s, name: %s", path, name);
    return -ENODATA; 
}

static int xmp_listxattr(const char *path, char *list, size_t size) {
    debug_print("listxattr: %s", path);
    return 0; 
}

static int xmp_removexattr(const char *path, const char *name) {
    debug_print("removexattr: %s, name: %s", path, name);
    return -EPERM;
}
#endif

static struct fuse_operations xmp_oper = {
    .getattr    = xmp_getattr,
    .access     = xmp_access,
    .readdir    = xmp_readdir,
    .mknod      = xmp_mknod,
    .mkdir      = xmp_mkdir,
    .symlink    = xmp_symlink,
    .unlink     = xmp_unlink,
    .rmdir      = xmp_rmdir,
    .rename     = xmp_rename,
    .link       = xmp_link,
    .chmod      = xmp_chmod,
    .chown      = xmp_chown,
    .truncate   = xmp_truncate,
#ifdef HAVE_UTIMENSAT
    .utimens    = xmp_utimens,
#endif
    .open       = xmp_open,
    .read       = xmp_read,
    .write      = xmp_write,
    .statfs     = xmp_statfs,
    .release    = xmp_release,
    .fsync      = xmp_fsync,
#ifdef HAVE_POSIX_FALLOCATE
    .fallocate  = xmp_fallocate,
#endif
#ifdef HAVE_SETXATTR
    .setxattr   = xmp_setxattr,
    .getxattr   = xmp_getxattr,
    .listxattr  = xmp_listxattr,
    .removexattr = xmp_removexattr,
#endif
};

int main(int argc, char *argv[]) {
    debug_print("Starting Drama Troll Filesystem");

    FILE *f = fopen(trigger_file, "a+");
    if (f) fclose(f);
    load_trap_status();

    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
