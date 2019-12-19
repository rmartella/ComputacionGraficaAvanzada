/*
 * mathUtil.h
 *
 *  Created on: 02/12/2016
 *      Author: rey
 */

#ifndef MATHUTIL_H_
#define MATHUTIL_H_

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

template<typename RM, typename CM>

void DLL_PUBLIC CopyMat(const RM& from, CM& to) {
	/*to[0][0] = from.a1;
	 to[1][0] = from.a2;
	 to[2][0] = from.a3;
	 to[3][0] = from.a4;
	 to[0][1] = from.b1;
	 to[1][1] = from.b2;
	 to[2][1] = from.b3;
	 to[3][1] = from.b4;
	 to[0][2] = from.c1;
	 to[1][2] = from.c2;
	 to[2][2] = from.c3;
	 to[3][2] = from.c4;
	 to[0][3] = from.d1;
	 to[1][3] = from.d2;
	 to[2][3] = from.d3;
	 to[3][3] = from.d4;*/
	/*to[0][0] = from.a1;
	 to[0][1] = from.a2;
	 to[0][2] = from.a3;
	 to[0][3] = from.a4;
	 to[1][0] = from.b1;
	 to[1][1] = from.b2;
	 to[1][2] = from.b3;
	 to[1][3] = from.b4;
	 to[2][0] = from.c1;
	 to[2][1] = from.c2;
	 to[2][2] = from.c3;
	 to[2][3] = from.c4;
	 to[3][0] = from.d1;
	 to[3][1] = from.d2;
	 to[3][2] = from.d3;
	 to[3][3] = from.d4;*/
	to[0][0] = from.a1;
	to[1][0] = from.a2;
	to[2][0] = from.a3;
	to[3][0] = from.a4;
	to[0][1] = from.b1;
	to[1][1] = from.b2;
	to[2][1] = from.b3;
	to[3][1] = from.b4;
	to[0][2] = from.c1;
	to[1][2] = from.c2;
	to[2][2] = from.c3;
	to[3][2] = from.c4;
	to[0][3] = from.d1;
	to[1][3] = from.d2;
	to[2][3] = from.d3;
	to[3][3] = from.d4;
}

#endif /* MATHUTIL_H_ */
