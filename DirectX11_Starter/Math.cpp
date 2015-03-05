#include "Math.h"
#include <xtgmath.h>

// ----- Normal quaternion implementation ----- //

NormalQuaternion::NormalQuaternion(float xp, float yp, float zp, float wp)
{
	x = xp;
	y = yp;
	z = zp;
	w = wp;
}

inline void NormalQuaternion::slerp(NormalQuaternion q2, NormalQuaternion out, float t)
{
	float dotproduct = x * q2.x + y * q2.y + z * q2.z + w * q2.w;
	
	t = t / 2.0;

	float theta = acos(dotproduct);
	if (theta<0.0) theta = -theta;

	float st = sin(theta);
	float sut = sin(t * theta);
	float sout = sin((1 - t) * theta);
	float c1 = sout / st;
	float c2 = sut / st;

	out.x = c1 * x + c2 * q2.x;
	out.y = c1 * y + c2 * q2.y;
	out.z = c1 * z + c2 * q2.z;
	out.w = c1 * w + c2 * q2.w;

	out.normalize();
}

inline void NormalQuaternion::normalize()
{
	float l = 1 / sqrt(x * x + y * y + z * z + w * w);
	x *= l;
	y *= l;
	z *= l;
	w *= l;
}

// ----- SSE quaternion implementation ----- //

SSEQuaternion::SSEQuaternion(float x, float y, float z, float w)
{
	data = _mm_set_ps(x, y, z, w);
}

SSEQuaternion::SSEQuaternion(float* fArray)
{
	_declspec(align(16)) float fArray;
	data = _mm_load_ps(fArray);
}

SSEQuaternion::SSEQuaternion(__m128 set)
{
	data = set;
}

inline void SSEQuaternion::slerp(SSEQuaternion q2, SSEQuaternion out, float t)
{

}