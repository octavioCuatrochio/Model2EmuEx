/* Folder listing; see dirlist.h. */
#include "dirlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define SEP '\\'
#else
#include <dirent.h>
#define SEP '/'
#endif

int m2dir_list(const char *path, void (*cb)(void *user, const char *name, int is_dir), void *user)
{
#ifdef _WIN32
    char pattern[1100];
    WIN32_FIND_DATAA fd;
    snprintf(pattern, sizeof pattern, "%s\\*", path);
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return -1;
    do {
        if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
            cb(user, fd.cFileName, (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    return 0;
#else
    DIR *d = opendir(path);
    if (!d)
        return -1;
    struct dirent *e;
    while ((e = readdir(d))) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
            continue;
        char full[1100];
        struct stat st;
        snprintf(full, sizeof full, "%s/%s", path, e->d_name);
        int is_dir = !stat(full, &st) && S_ISDIR(st.st_mode);
        cb(user, e->d_name, is_dir);
    }
    closedir(d);
    return 0;
#endif
}

int m2dir_exists(const char *path)
{
    struct stat st;
    return path[0] && !stat(path, &st) && (st.st_mode & S_IFMT) == S_IFDIR;
}

void m2dir_home(char *out, int size)
{
    const char *h = getenv("HOME");
#ifdef _WIN32
    if (!h) h = getenv("USERPROFILE");
#endif
    snprintf(out, (size_t)size, "%s", h ? h : ".");
}

int m2dir_parent(char *path)
{
    size_t n = strlen(path);
    while (n > 1 && (path[n - 1] == '/' || path[n - 1] == '\\'))
        path[--n] = 0;
    char *s = strrchr(path, '/'), *b = strrchr(path, '\\');
    if (b > s) s = b;
    if (!s)
        return 0;
#ifdef _WIN32
    if (s == path + 2 && path[1] == ':') {   /* "C:\x" -> "C:\" */
        if (!s[1]) return 0;
        s[1] = 0;
        return 1;
    }
#endif
    if (s == path) {   /* "/x" -> "/" */
        if (!s[1]) return 0;
        s[1] = 0;
        return 1;
    }
    *s = 0;
    return 1;
}

void m2dir_join(char *out, int size, const char *path, const char *name)
{
    size_t n = strlen(path);
    int sep = n && (path[n - 1] == '/' || path[n - 1] == '\\');
    snprintf(out, (size_t)size, "%s%s%s", path, sep ? "" : (char[]){ SEP, 0 }, name);
}

void m2dir_drives(char *letters, int size)
{
    int k = 0;
#ifdef _WIN32
    DWORD mask = GetLogicalDrives();
    for (int i = 0; i < 26 && k < size - 1; i++)
        if (mask & (1u << i))
            letters[k++] = (char)('A' + i);
#endif
    if (size > 0)
        letters[k] = 0;
}
