#include "Math.h"

#define _mm_abs_ps(x) _mm_and_ps(x, _absMask)
#define PI 3.14159265358979323846264338327950288f

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
__m128 acosConst_2;
__m128 PI_2;
__m128 PI_4;

__m128 _1111;
__m128 _0011;
__m128 _absMask;
__m128 _conjugate;

__m128 m_pi;
__m128 m_mpi;
__m128 m_2pi;
__m128 m_B;
__m128 m_C;
__m128 m_P;

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
	acosConst_2 = _mm_set_ps(-0.5f, -0.083333333333333f, -0.0041666666666666f, -0.0000992063492f);

	PI_2 = _mm_set_ps(PI / 2.0f, PI / 2.0f, PI / 2.0f, PI / 2.0f);
	PI_4 = _mm_set_ps(PI / 4.0f, PI / 4.0f, PI / 4.0f, PI / 4.0f);

	_1111 = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
	_0011 = _mm_castsi128_ps(_mm_setr_epi32(0, 0, 0xFFFFFFFF, 0xFFFFFFFF));
	_absMask = _mm_castsi128_ps(_mm_setr_epi32(-1, 0x7FFFFFFF, -1, 0x7FFFFFFF));
	_conjugate = _mm_castsi128_ps(_mm_set_epi32(0x80000000, 0x80000000, 0x80000000, 0));

	const float B = 4.f / PI;
	const float C = -4.f / (PI * PI);
	const float P = 0.225f;
	m_pi = _mm_set1_ps(PI);
	m_mpi = _mm_set1_ps(-PI);
	m_2pi = _mm_set1_ps(PI * 2);
	m_B = _mm_set1_ps(B);
	m_C = _mm_set1_ps(C);
	m_P = _mm_set1_ps(P);
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

// Slerp with all trig functions converted to SSE - 2nd slowest
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

// Slerp with only sin converted to SSE - slowest
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

// Slerp with all SSE functions - 2nd fastest
void SSEQuaternion::slerp3(SSEQuaternion& q2, SSEQuaternion& out, float t)
{
	// Calculate the dot product of this quaternion and q2
	__m128 dp = dot(q2);

	// Arc cos to get the angle
	__m128 theta = acos2(dp);
	theta = _mm_abs_ps(theta);

	// Load the scalar
	t *= 0.5f;
	__m128 tm = _mm_set_ps(t, 1 - t, 1, 0);

	// Sin values
	__m128 sins = sin2(_mm_mul_ps(tm, theta));

	// Coefficients
	__m128 s = _mm_replicate_ps(sins, 1);
	__m128 c1 = _mm_div_ps(_mm_replicate_ps(sins, 2), s);
	__m128 c2 = _mm_div_ps(_mm_replicate_ps(sins, 3), s);

	// Final result
	out.data = _mm_add_ps(_mm_mul_ps(c1, data), _mm_mul_ps(c2, q2.data));
}

// Attempted q0(q0* q1)^t formula - BROKEN
void SSEQuaternion::slerp4(SSEQuaternion& q2, SSEQuaternion& out, float t)
{
	// Conjugate of q2
	__m128 qc = _mm_xor_ps(data, _conjugate);

	// this * q2c
	__m128 qcq2 = mulQuat(qc, q2.data);

	// Theta betweem the quaternions
	__m128 dp = dot(q2);
	float theta = acos(_mm_cvtss_f32(dp));
	
	// cos*w + sin*xyz
	__m128 sc = sin2(_mm_set_ps(theta * t, theta * t + PI * 0.5f, 0, 0));
	sc = _mm_shuffle_ps(sc, sc, SHUFFLE_PARAM(3, 3, 3, 2));

	// final multiplication
	out.data = mulQuat(data, _mm_mul_ps(sc, qcq2));
}

// Attempted shared sine calculation - fastest
void SSEQuaternion::slerp5(SSEQuaternion& q2, SSEQuaternion& out, float t)
{
	// Calculate the dot product of this quaternion and q2
	__m128 dp = dot(q2);
	_declspec(align(16))float thetaArray[4];
	_mm_store_ps(thetaArray, dp);
	t = t / 2.0f;

	float theta = acos(thetaArray[0]);
	if (theta < 0.0) theta = -theta;

	// Calculate sine values
	__m128 thetas = _mm_set_ps(0, theta, (1 - t) * theta, t * theta);
	__m128 sins = sin2(thetas);

	// Get the coefficients
	__m128 s = _mm_replicate_ps(sins, 2);
	__m128 c1 = _mm_div_ps(_mm_replicate_ps(sins, 1), s);
	__m128 c2 = _mm_div_ps(_mm_replicate_ps(sins, 0), s);

	// Return the result
	out.data = _mm_add_ps(_mm_mul_ps(c1, data), _mm_mul_ps(c2, q2.data));
}

// Calculating multiple sine values at once
__m128 SSEQuaternion::sin2(__m128 theta) {
	__m128 theta2 = _mm_mul_ps(theta, theta);
	__m128 sin = _mm_mul_ps(theta2, _mm_replicate_ps(sinConst, 0));           // dx^2
	sin = _mm_mul_ps(theta2, _mm_add_ps(sin, _mm_replicate_ps(sinConst, 1))); // cx^2 + dx^4
	sin = _mm_mul_ps(theta2, _mm_add_ps(sin, _mm_replicate_ps(sinConst, 2))); // bx^2 + cx^4 + dx^6
	return _mm_mul_ps(theta, _mm_add_ps(sin, _mm_replicate_ps(sinConst, 3))); // ax + bx^3 + cx^5 + dx^7
}

// Returns have the acos of the dot product value - BROKEN
__m128 SSEQuaternion::acos2(__m128 dot) {

	// Mask xxxx to 1, 1, x^2, x^2
	__m128 mask = _mm_mul_ps(dot, dot); // x^2, x^2, x^2, x^2
	mask = _mm_sub_ps(mask, _1111);      // x^2-1, x^2-1, x^2-1, x^2-1
	mask = _mm_and_ps(mask, _0011);     // 0, 0, x^2-1, x^2-1
	mask = _mm_add_ps(mask, _1111);     // 1, 1, x^2, x^2

	// Multiply further to get x, x, x^5, x^5
	__m128 factor = _mm_mul_ps(mask, mask); // 1, 1, x^4, x^4
	factor = _mm_mul_ps(dot, factor);       // x, x, x^5, x^5

	// Get the inside terms
	__m128 terms = _mm_shuffle_ps(mask, mask, SHUFFLE_PARAM(2, 0, 3, 1)); // x^2, 1, x^2, 1
	terms = _mm_mul_ps(terms, acosConst);                                // dx^2, c, bx^2, a
	terms = _mm_shuffle_ps(terms, terms, SHUFFLE_PARAM(3, 2, 1, 0));    // a, bx^2, c, dx^2

	// Multiply by the factor to get each final term
	terms = _mm_mul_ps(terms, factor); // ax bx^3 cx^5 dx^7

	// Sum the results together
	__m128 shuffle = _mm_shuffle_ps(terms, terms, SHUFFLE_PARAM(3, 2, 1, 0));
	terms = _mm_add_ps(terms, shuffle);
	shuffle = _mm_shuffle_ps(terms, terms, SHUFFLE_PARAM(2, 3, 0, 1));

	// Return acos(x) as m128
	return _mm_add_ps(_mm_add_ps(terms, shuffle), PI_2);
}

// Multiplies two quaternions
// http://stackoverflow.com/questions/18542894/how-to-multiply-two-quaternions-with-minimal-instructions
__m128 SSEQuaternion::mulQuat(__m128 q1, __m128 q2)
{
	__m128 wzyx = _mm_shuffle_ps(q1, q1, SHUFFLE_PARAM(0, 1, 2, 3));
	__m128 baba = _mm_shuffle_ps(q2, q2, SHUFFLE_PARAM(0, 1, 0, 1));
	__m128 dcdc = _mm_shuffle_ps(q2, q2, SHUFFLE_PARAM(2, 3, 2, 3));

	__m128 ZnXWY = _mm_hsub_ps(_mm_mul_ps(q1, baba), _mm_mul_ps(wzyx, dcdc));
	__m128 XZYnW = _mm_hadd_ps(_mm_mul_ps(q1, dcdc), _mm_mul_ps(wzyx, baba));

	__m128 XZWY = _mm_addsub_ps(_mm_shuffle_ps(XZYnW, ZnXWY, SHUFFLE_PARAM(3, 2, 1, 0)),
								_mm_shuffle_ps(ZnXWY, XZYnW, SHUFFLE_PARAM(2, 3, 0, 1)));

	return _mm_shuffle_ps(XZWY, XZWY, SHUFFLE_PARAM(2, 1, 3, 0));
}

//float* SSEQuaternion::getData()
//{
//	_declspec(align(16))float array[4];
//	_mm_store_ps(array, data);
//
//	float x = array[0];
//	float y = array[1];
//	float z = array[2];
//	float w = array[3];
//
//	float l = 1 / sqrt(x * x + y * y + z * z + w * w);
//	x *= l;
//	y *= l;
//	z *= l;
//	w *= l;
//
//	array[0] = x;
//	array[1] = y;
//	array[2] = z;
//	array[3] = w;
//
//	return array;
//}