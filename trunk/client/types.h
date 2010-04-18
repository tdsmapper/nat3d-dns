/* types.h: Type portability between Windows and Linux/BSD */
#ifndef __TYPES_H__
#define __TYPES_H__

/* Integer types */

#ifdef _MSC_VER /* Visual Studio/Microsoft equivalents of UNIX datatypes */
	typedef unsigned __int8 byte;
	typedef unsigned __int8 uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;
	typedef signed __int8 int8_t;
	typedef signed __int16 int16_t;
	typedef signed __int32 int32_t;
	typedef signed __int64 int64_t;

  typedef  unsigned long in_addr_t;
#define sleep(a) Sleep(a*1000);
#endif

#endif  /* __TYPES_H__ */