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

void NormalQuaternion::slerp(NormalQuaternion& q2, NormalQuaternion& out, float t)
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

// Fraction constant for taylor series
__m128 sinConst;
__m128 acosConst;
__m128 PI_2;

SSEQuaternion::SSEQuaternion()
{
}

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

void SSEQuaternion::Initialize()
{
	sinConst = _mm_set_ps(1.0f, -0.166666666f, 0.008333333f, -0.0001984126984f);
	acosConst = _mm_set_ps(-1.0f, -0.166666666f, -0.008333333f, -0.0001984126984f);

	float PI = 3.14159265358979323846264338327950288f;

	PI_2 = _mm_set_ps(PI / 2.0f, PI / 2.0f, PI / 2.0f, PI / 2.0f);
}

inline __m128 SSEQuaternion::dot(SSEQuaternion& q2)
{
	__m128 mul = _mm_mul_ps(q2.data, data);
	__m128 shuffle = _mm_shuffle_ps(mul, mul, SHUFFLE_PARAM(3, 2, 1, 0));
	__m128 sum = _mm_add_ps(mul, shuffle);
	shuffle = _mm_shuffle_ps(sum, sum, SHUFFLE_PARAM(2, 3, 0, 1));
	
	return _mm_add_ps(sum, shuffle);
}

inline __m128 SSEQuaternion::sin(float x)
{
	__m128 result;
	_declspec(align(16))

	// Get exponents of angles
	float x2 = x * x;
	float x3 = x * x2;
	float x5 = x2 * x3;
	float x7 = x2 * x5;

	// Load back into m128 and multiply by constants to get 4 components of taylor series
	result = _mm_set_ps(x, x3, x5, x7);
	result = _mm_mul_ps(result, sinConst);

	// Calculate taylor series for sin and store in all 4 elements of returned m128
	__m128 shuffle = _mm_shuffle_ps(result, result, SHUFFLE_PARAM(3, 2, 1, 0));
	result = _mm_add_ps(result, shuffle);
	shuffle = _mm_shuffle_ps(result, result, SHUFFLE_PARAM(2, 3, 0, 1));

	// Return sin(x) as m128
	return _mm_add_ps(result, shuffle);
}

inline __m128 SSEQuaternion::aCos(float x)
{
	__m128 result;
	_declspec(align(16))

	// Get exponents of angles
	float x2 = x * x;
	float x3 = x * x2;
	float x5 = x2 * x3;
	float x7 = x2 * x5;

	// Load back into m128 and multiply by constants to get 4 components of taylor series
	result = _mm_set_ps(x, x3, x5, x7);
	result = _mm_mul_ps(result, acosConst);

	// Calculate taylor series for sin and store in all 4 elements of returned m128
	__m128 shuffle = _mm_shuffle_ps(result, result, SHUFFLE_PARAM(3, 2, 1, 0));
	result = _mm_add_ps(result, shuffle);
	shuffle = _mm_shuffle_ps(result, result, SHUFFLE_PARAM(2, 3, 0, 1));

	// Return acos(x) as m128
	return _mm_add_ps(_mm_add_ps(result, shuffle), PI_2);
}

// Slerp with all trig functions converted to SSE
void SSEQuaternion::slerp(SSEQuaternion& q2, SSEQuaternion& out, float t)
{
	// Calculate the dot product of this quaternion and q2
	__m128 dp = dot(q2);
	_declspec(align(16))float dpArray[4];
	_mm_store_ps(dpArray, dp);

	t = t / 2.0f;
	
	__m128 theta = aCos(dpArray[0]);
	_declspec(align(16))float thetaArray[4];
	_mm_store_ps(thetaArray, theta);
	if (thetaArray[0] <0.0) thetaArray[0] = -thetaArray[0];
	
	__m128 st = sin(thetaArray[0]);
	__m128 sut = sin(t * thetaArray[0]);
	__m128 sout = sin((1 - t) * thetaArray[0]);

	__m128 c1 = _mm_div_ps(sout, st);
	__m128 c2 = _mm_div_ps(sut, st);

	out.data = _mm_add_ps(_mm_mul_ps(c1, data), _mm_mul_ps(c2, q2.data));
}

// Slerp with only sin converted to SSE
void SSEQuaternion::slerp2(SSEQuaternion& q2, SSEQuaternion& out, float t)
{
	// Calculate the dot product of this quaternion and q2
	__m128 dp = dot(q2);
	_declspec(align(16))float thetaArray[4];
	_mm_store_ps(thetaArray, dp);
	t = t / 2.0f;

	float theta = acos(thetaArray[0]);

	if (theta<0.0) theta = -theta;

	__m128 st = sin(theta);
	__m128 sut = sin(t * theta);
	__m128 sout = sin((1 - t) * theta);

	__m128 c1 = _mm_div_ps(sout, st);
	__m128 c2 = _mm_div_ps(sut, st);

	out.data = _mm_add_ps(_mm_mul_ps(c1, data), _mm_mul_ps(c2, q2.data));
}

float* SSEQuaternion::getData()
{
	_declspec(align(16))float array[4];
	_mm_store_ps(array, data);

	float x = array[0];
	float y = array[1];
	float z = array[2];
	float w = array[3];

	float l = 1 / sqrt(x * x + y * y + z * z + w * w);
	x *= l;
	y *= l;
	z *= l;
	w *= l;

	array[0] = x;
	array[1] = y;
	array[2] = z;
	array[3] = w;

	return array;
}