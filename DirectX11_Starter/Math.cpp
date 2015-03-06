#include "Math.h"

// ----- Normal quaternion implementation ----- //

NormalQuaternion::NormalQuaternion()
{
	x = y = z = 0;
	w = 1;
}

NormalQuaternion::NormalQuaternion(float xp, float yp, float zp, float wp)
{
	x = xp;
	y = yp;
	z = zp;
	w = wp;
}

void NormalQuaternion::slerp(NormalQuaternion q2, NormalQuaternion out, float t)
{
	float dotproduct = dot(q2);
	
	t = t / 2.0f;

	float theta = acos(dotproduct);
	if (theta<0.0) theta = -theta;

	float st = (float)sin(theta);
	float sut = (float)sin(t * theta);
	float sout = (float)sin((1 - t) * theta);
	float c1 = sout / st;
	float c2 = sut / st;

	out.x = c1 * x + c2 * q2.x;
	out.y = c1 * y + c2 * q2.y;
	out.z = c1 * z + c2 * q2.z;
	out.w = c1 * w + c2 * q2.w;

	out.normalize();
}

inline float NormalQuaternion::dot(NormalQuaternion q2)
{
	return x * q2.x + y * q2.y + z * q2.z + w * q2.w;
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

/*
SSEQuaternion::SSEQuaternion(float x, float y, float z, float w)
{
	data = _mm_set_ps(x, y, z, w);
}

SSEQuaternion::SSEQuaternion(float* fArray)
{
	data = _mm_set_ps(fArray[0], fArray[1], fArray[2], fArray[3]);
}

SSEQuaternion::SSEQuaternion(__m128 set)
{
	data = set;
}

inline __m128 SSEQuaternion::dot(SSEQuaternion q2)
{
	__m128 mul = _mm_mul_ps(data, q2.data);
	__m128 shuffle = _mm_shuffle_ps(mul, mul, SHUFFLE_PARAM(3, 2, 1, 0));
	__m128 sum = _mm_add_ps(mul, shuffle);
	shuffle = _mm_shuffle_ps(sum, sum, SHUFFLE_PARAM(2, 3, 0, 1));
	return _mm_add_ps(sum, shuffle);
}

inline void SSEQuaternion::slerp(SSEQuaternion q2, SSEQuaternion out, float t)
{
	__m128 dp = dot(q2);

	t = t / 2.0f;
	/*
	float theta = acos(dot);
	if (theta<0.0) theta = -theta;

	float st = (float)sin(theta);
	float sut = (float)sin(t * theta);
	float sout = (float)sin((1 - t) * theta);
	float c1 = sout / st;
	float c2 = sut / st;

	out.data = _mm_add_ps(_mm_mul_ps(c1, data), _mm_mul_ps(c2, data));
	
}

*/