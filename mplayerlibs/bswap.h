#ifndef __BSWAP_H__
#define __BSWAP_H__

#define bswap_16(x) (((x) & 0x00ff) << 8 | ((x) & 0xff00) >> 8)
			

// code from bits/byteswap.h (C) 1997, 1998 Free Software Foundation, Inc.
#define bswap_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define bswap_64(x) \
     (__extension__						\
      ({ union { __extension__ unsigned long long int __ll;	\
                 unsigned int __l[2]; } __w, __r;		\
         __w.__ll = (x);					\
         __r.__l[0] = bswap_32 (__w.__l[1]);			\
         __r.__l[1] = bswap_32 (__w.__l[0]);			\
         __r.__ll; }))


#ifdef WORDS_BIGENDIAN
#define be2me_16(x) (x)
#define be2me_32(x) (x)
#define be2me_64(x) (x)
#define le2me_16(x) bswap_16(x)
#define le2me_32(x) bswap_32(x)
#define le2me_64(x) bswap_64(x)
#else
#define be2me_16(x) bswap_16(x)
#define be2me_32(x) bswap_32(x)
#define be2me_64(x) bswap_64(x)
#define le2me_16(x) (x)
#define le2me_32(x) (x)
#define le2me_64(x) (x)
#endif

#endif
