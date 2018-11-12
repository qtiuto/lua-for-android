
#define chdir_error    strerror(errno)

#define LFS_VERSION "1.7.0"
#define LFS_LIBNAME "lfs"

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_lfs(lua_State *L);

#ifdef __cplusplus
}
#endif
