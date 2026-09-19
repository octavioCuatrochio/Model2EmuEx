/* Folder listing for the launcher's folder browser and ROM scan
   (POSIX opendir or Windows FindFirstFile). */
#ifndef M2_DIRLIST_H
#define M2_DIRLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Calls cb for each entry of `path` except "." and "..". Returns -1 if the
   folder can't be read. */
int  m2dir_list(const char *path, void (*cb)(void *user, const char *name, int is_dir), void *user);
int  m2dir_exists(const char *path);
/* The user's home folder. */
void m2dir_home(char *out, int size);
/* The parent of `path` in place; 0 if it has none (a root). */
int  m2dir_parent(char *path);
/* path + separator + name. */
void m2dir_join(char *out, int size, const char *path, const char *name);
/* Windows: the drive letters present ("C:\\", ...), as a string of letters;
   elsewhere empty. */
void m2dir_drives(char *letters, int size);

#ifdef __cplusplus
}
#endif

#endif
