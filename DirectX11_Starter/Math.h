#pragma once

#include <xmmintrin.h>
#include <ctgmath>
#include <DirectXMath.h>

/**
 * Quick note: the __m128 type has 2 underscores
 */
/**
* Useful SSE functions from the packet
*
* -- Initization --
* _declspec(align(16)) a - aligns normal float array before loading into m128 register
* _mm_set_ps(x, y, z, w) - creates an _m128 register object with the float values
* _mm_load_ps(a) - loads a float array into an _m128 register assuming its aligned
* _mm_store_ps(a, b) - stores _m128 register "b" into the float array "a"
*
* -- Math functions --
* _mm_add_ps(a, b) - adds a and b
* _mm_mul_ps(a, b) - multiplies a  and b
* _mm_shuffle_ps(a, a, i) - shuffles the values from "a" around into a new _m128 register 
*						    using "i" as the pattern where "i" is from SHUFFLE_PARAM below
*
* -- Macros below --
*/

// Replicating one value across an m128 register
#define _mm_replicate_ps(v, i) \
	_mm_shuffle_ps(v, v, SHUFFLE_PARAM(i, i, i, i))

// Getting the value for shuffling the values around in an m128 register
#define SHUFFLE_PARAM(x, y, z, w) \
	(x | (y << 2) | (z << 4) | (w << 6))

// Multiplies the first two m128 registers and adds them to the third
#define _mm_madd_ps(a, b, c) \
	_mm_add_ps(_mm_mul_ps(a, b), c))

class NormalQuaternion
{
public:
	NormalQuaternion();
	NormalQuaternion(float x, float y, float z, float w);
	inline float dot(NormalQuaternion q2);
	void slerp(NormalQuaternion& q2, NormalQuaternion& out, float t);
	inline void normalize();

	float x, y, z, w;
};

class SSEQuaternion
{

public:
	SSEQuaternion();
	SSEQuaternion(float, float, float, float);
	SSEQuaternion(float*);
	SSEQuaternion(__m128);
	static void Initialize(void);
	inline __m128 dot(SSEQuaternion&);
	void slerp(SSEQuaternion&, SSEQuaternion&, float);
	void slerp2(SSEQuaternion&, SSEQuaternion&, float);
	inline __m128 aCos(float x);
	inline __m128 sin(float);

	float* getData(void);

private:
	__m128 data;
};

