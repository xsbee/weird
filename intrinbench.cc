#include <chrono>
#include <vector>
#include <random>
#include <numeric>
#include <iostream>

#include <immintrin.h>

using namespace std::chrono;

void intrinsic_binop (const __m128 *u, __m128 *v, size_t N)
{
    __m128 one = _mm_set1_ps(1);
    __m128 two = _mm_set1_ps(2);

    for (size_t i = 0; i < N; ++i)
    {
        __m128 x = u[i];
        __m128 k = _mm_sub_ps(one, _mm_mul_ps(x, x)); // 1 - x^2
        __m128 l = _mm_mul_ps(two, x); // 2x

        v[i] =_mm_div_ps(
            _mm_sub_ps(
                _mm_mul_ps(
                    _mm_mul_ps(two, x), 
                    _mm_sqrt_ps(k)),
                _mm_mul_ps(l, x)),
            _mm_sub_ps(l, one));

        // v[i] = _mm_mul_ps(_mm_mul_ps(two, x), _mm_sqrt_ps(k)); // (2x√k
        // v[i] = _mm_sub_ps(v[i], _mm_mul_ps(l, x)); // - lx)
        // v[i] = _mm_div_ps(v[i], _mm_sub_ps(l, one)); // / (l - 1)
    }
}

void normal_binop (const float *u, float *v, size_t N)
{
    for (size_t i = 0; i < N; ++i)
    {
        float k = 1 - u[i]*u[i];
        float l = 2 * u[i];

        v[i] = 2.0*u[i]*sqrtf(k);
        v[i] -= l * u[i];
        v[i] /= l - 1.0;
    }
}

int main()
{

    std::ios::sync_with_stdio(false);

    std::vector<float> v0, v1;
    double s0 = 0, s1 = 0;

    std::random_device dev;
    std::uniform_real_distribution<float> dist(
        -0.62637549352853167132671953,
        +0.62637549352853167132671953);

    v0.reserve(16777216);
    v1.reserve(16777216);

    std::cerr << "reserved place for data" << std::endl;

    for (size_t i = 0; i < 16777216; ++i)
        v0[i] = dist(dev);

    std::cerr << "filled in random data" << std::endl;

    for (size_t i = 0; i < 1024; ++i) {
        auto t0 = system_clock::now();
        intrinsic_binop(
            reinterpret_cast<const __m128*>(v0.data()),
            reinterpret_cast<__m128*>(v1.data()),
            4194304);
        auto t1 = system_clock::now();
        s0 += (t1-t0).count() / 1024.0f;

        auto t2 = system_clock::now();
        normal_binop(
            v0.data(), 
            v1.data(), 
            16777216);
        
        auto t3 = system_clock::now();
        s1 += (t3-t2).count() / 1024.0f;

        std::cerr << '\r' << "iteration = " << i;
    }

    std::cerr << '\n';
    std::cerr << "------------" << std::endl;
    std::cout << "simd op (avg)     " << (uint64_t)s0 << " ns" << '\n'
              << "normal op (avg)   " << (uint64_t)s1 << " ns" << std::endl;
}
