/**
 * Copyright (c) 2017 Melown Technologies SE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * *  Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @file math.hpp
 * @author Ondrej Prochazka <ondrej.prochazka@citationtech.net>
 *
 * Low level math helper functions
 */
      
#ifndef MATH_MATH_HPP
#define MATH_MATH_HPP

#include <algorithm>
#include <stdexcept>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "dbglog/dbglog.hpp"


namespace math {

namespace ublas = boost::numeric::ublas;

/* square function */

template <typename T>
T sqr( T val ) { return val * val; }

/** even and odd */

template <typename T>
bool even( T val ) { return ( val >> 1 << 1 == val ); }

template <typename T>
bool odd( T val ) { return ( val >> 1 << 1 != val ); }


/** interval check */

template<typename T>
bool ccinterval( const T & lb, const T  & ub, const T & value ) {
    return ( lb <= value && value <= ub );
}
    

/**
  * Signum function
  */
  
template <typename Value_t>
int sgn( const Value_t & value ) {
   if ( value > 0 ) return 1;
   if ( value < 0 ) return -1;
   return 0;
}

/** Clamp value to given range.
 */
template <typename T>
inline T clamp(T value, T min, T max)
{
    return std::max(min, std::min(value, max));
}

template <typename T>
inline bool isInteger(T value, T tolerance = T(0))
{
    T integer;
    return (std::abs(std::modf(value, &integer)) <= tolerance);
}

/**
  * Matrix inversion
  */
                       
template <typename T, typename L, typename C>
ublas::matrix<T,L,C> matrixInvert( const ublas::matrix<T,L,C> & input ) {
                       
    typedef ublas::permutation_matrix<std::size_t> pmatrix;
                               
    // create a working copy of the input
    ublas::matrix<T,L,C> A(input);
                                       
    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());
                                               
    // perform LU-factorization
    int res = lu_factorize(A,pm);
    if( res != 0 ) {
        LOGTHROW(warn1, std::runtime_error)
            << "Singular matrix in math::matrixInvert. Aborting.";
    }
                                                           
    // create identity matrix of "inverse"
    ublas::matrix<T,L,C> inverse = ublas::identity_matrix<T>(A.size1());
                                                         
    // backsubstitute to get the inverse
    lu_substitute(A, pm, inverse);
    return inverse;
}
                                                                               
/**
 * Matrix determinant
 */

template <class matrix_T>
double determinant(ublas::matrix_expression<matrix_T> const& mat_r)
{
  double det = 1.0;

  matrix_T mLu(mat_r() );
  ublas::permutation_matrix<std::size_t> pivots(mat_r().size1() );

  int is_singular = lu_factorize(mLu, pivots);

  if (!is_singular)
  {
    for (std::size_t i=0; i < pivots.size(); ++i)
    {
      if (pivots(i) != i)
        det *= -1.0;

      det *= mLu(i,i);
    }
  }
  else
    det = 0.0;

  return det;
} 


/**
 * Solve a 2x2 linear system
 */
template<typename MatrixType, typename VectorType>
void solve2x2(const MatrixType &mat, const VectorType &rhs, VectorType &result)
{
    double det = mat(0,0)*mat(1,1) - mat(0,1)*mat(1,0);

    // check the determinant
    if (std::abs(det) < 1e-14) {
        LOGTHROW(err1, std::runtime_error) << "Singular matrix in solve2x2.";
    }

    result(0) = (rhs(0)*mat(1,1) - mat(0,1)*rhs(1)) / det;
    result(1) = (mat(0,0)*rhs(1) - rhs(0)*mat(1,0)) / det;
}

                                                                               
} // namespace math

#endif // MATH_MATH_HPP
      
