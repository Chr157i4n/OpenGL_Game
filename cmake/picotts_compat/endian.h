/* Minimal endian.h shim for MSVC.
 *
 * SVOX PicoTTS includes <endian.h> (POSIX/glibc) from picopltf.h to detect
 * the host byte order.  MSVC does not provide that header, so this shim
 * supplies the few macros the engine needs.
 */
#ifndef OG_PICOTTS_COMPAT_ENDIAN_H
#define OG_PICOTTS_COMPAT_ENDIAN_H

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#ifndef BYTE_ORDER
#if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#endif

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN BIG_ENDIAN
#endif

#ifndef __BYTE_ORDER
#define __BYTE_ORDER BYTE_ORDER
#endif

#endif /* OG_PICOTTS_COMPAT_ENDIAN_H */
