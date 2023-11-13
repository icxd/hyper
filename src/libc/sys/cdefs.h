#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H

#define __CONCAT(x, y) x##y
#define __STRING(x) #x

#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#endif // _SYS_CDEFS_H