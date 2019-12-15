/*
 * mathUtil.h
 *
 *  Created on: 02/12/2016
 *      Author: rey
 */

#ifndef HEADERS_MATHUTIL_H_
#define HEADERS_MATHUTIL_H_

template<typename RM, typename CM>

void CopyMat(const RM& from, CM& to) {
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

#endif /* HEADERS_MATHUTIL_H_ */
