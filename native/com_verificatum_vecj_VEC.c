
/* Copyright 2008-2019 Douglas Wikstrom
 *
 * This file is part of Verificatum Elliptic Curve library (VEC).
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gmp.h>
#include "convert.h"
#include "vec.h"

/*
 * We use compiler flags that enforce that unused variables are
 * flagged as errors. Here we are forced to use a given API, so we
 * need to explicitly trick the compiler to not issue an error for
 * those parameters that we do not use.
 */
#define VECJ_UNUSED(x) ((void)(x))

#ifdef __cplusplus
extern "C" {
#endif

  /***********************************************************************
   * The following macros and methods are helper functionality.          *
   ***********************************************************************/

  /*
   * Allocate a byte[] in Java space and copy len bytes from cBytes (a C
   * jbyte array) to the allocated array.
   */
#define copyBytesCtoJ(javaBytes, cBytes, len)                           \
  javaBytes = (*env)->NewByteArray(env, len);                           \
  (*env)->SetByteArrayRegion(env, javaBytes, 0, len, (jbyte*)cBytes)

  /*
   * Copy len bytes from javaBytes (a Java byte[]) to cBytes (a C jbyte
   * array). This does not allocate any memory in C space.
   */
#define copyBytesJtoC(cBytes, javaBytes, len)                           \
  (*env)->GetByteArrayRegion(env, javaBytes, 0, len, (jbyte*)cBytes)

  /*
   * The length of javaArray.
   */
#define length(javaArray)                       \
  (*env)->GetArrayLength(env, javaArray)

  /*
   * Allocate an array of byte[] in Java space.
   */
#define newByteArrayArray(len)                                          \
  (*env)->NewObjectArray(env, (jsize)len, (*env)->FindClass(env, "[B"), NULL)

  /*
   * Assign object to position i in objectArray.
   */
#define setElement(objectArray, i, object)                          \
  (*env)->SetObjectArrayElement(env, objectArray, (jsize)i, object)

  /*
   * Converts a pair of mpz_t's into a newly allocated array of length 2
   * of byte[]'s representing the mpz_t's.
   */
  void mpz_t_pair_to_2DjbyteArray(JNIEnv *env, jobjectArray* res,
                                  mpz_t first, mpz_t second)
  {

    jbyteArray firstArray;
    jbyteArray secondArray;

    *res = newByteArrayArray(2);

    mpz_t_to_jbyteArray(env, &firstArray, first);
    mpz_t_to_jbyteArray(env, &secondArray, second);

    setElement(*res, 0, firstArray);
    setElement(*res, 1, secondArray);
  }

  /*
   * Converts a string in C to a new jbyteArray in Java space.
   */
  jbyteArray string_to_jbyteArray(JNIEnv *env, char *str)
  {
    jbyteArray javaBytes;
    copyBytesCtoJ(javaBytes, str, strlen(str));
    return javaBytes;
  }

  /*
   * Converts a pointer to a curve into a new jbyteArray in Java space.
   */
  jbyteArray curve_to_jbyteArray(JNIEnv *env, vec_curve *curve)
  {
    jbyteArray javaArray;
    copyBytesCtoJ(javaArray, &curve, sizeof(vec_curve*));
    return javaArray;
  }

  /*
   * Converts a jbyteArray in Java into a pointer to a curve.
   */
  vec_curve *
  jbyteArray_to_curve(JNIEnv *env, jbyteArray javaArray)
  {
    vec_curve* curve;
    copyBytesJtoC(&curve, javaArray, sizeof(vec_curve*));
    return curve;
  }

  /*
   * Converts a pointer struct to a table for simultaneous
   * multiplication into a new jbyteArray in Java space.
   */
  jbyteArray table_to_jbyteArray(JNIEnv *env, vec_jfmul_tab_ptr table_ptr)
  {
    jbyteArray javaArray;
    copyBytesCtoJ(javaArray, &table_ptr, sizeof(vec_jfmul_tab_ptr));
    return javaArray;
  }

  /*
   * Converts a jbyteArray into a pointer struct to a table for
   * simultaneous multiplication.
   */
  vec_jfmul_tab_ptr
  jbyteArray_to_table(JNIEnv *env, jbyteArray javaArray)
  {
    vec_jfmul_tab_ptr table_ptr;
    copyBytesJtoC(&table_ptr, javaArray, sizeof(vec_jfmul_tab_ptr));
    return table_ptr;
  }


  /***********************************************************************
   * The following methods are documented in the source of vecj.VEC. *
   ***********************************************************************/

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    curve_get_names
   * Signature: ()[[B
   */
  JNIEXPORT jobjectArray JNICALL Java_com_verificatum_vecj_VEC_curve_1get_1names
  (JNIEnv *env, jclass clazz)
  {
    char *name;
    int i;
    int len = vec_curve_number_of_names();

    jobjectArray javaResult = newByteArrayArray(len);

    VECJ_UNUSED(clazz);

    for (i = 0; i < len; i++)
      {
        name = vec_curve_get_name(i);
        setElement(javaResult, i, string_to_jbyteArray(env, name));
      }
    return javaResult;
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    curve_get_named
   * Signature: ([B)[B
   */
  JNIEXPORT jbyteArray JNICALL Java_com_verificatum_vecj_VEC_curve_1get_1named
  (JNIEnv *env, jclass clazz, jbyteArray javaName)
  {
    jbyte *name;
    vec_curve *curve;

    VECJ_UNUSED(clazz);

    /* Fetch a pointer to the jbyteArray, viewed as a jbyte[]. The NULL
       parameter indicates that we do not need to know if the JVM copies
       the bytes for us to a new array or not. */
    name = (*env)->GetByteArrayElements(env, javaName, NULL);

    /* Fetch pointer to curve. */
    curve = vec_curve_get_named_len((char*) name, length(javaName), 1);

    /* Return pointer converted to a byte[]. */
    return curve_to_jbyteArray(env, curve);
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    curve_free
   * Signature: ([B)V
   */
  JNIEXPORT void JNICALL Java_com_verificatum_vecj_VEC_curve_1free
  (JNIEnv *env, jclass clazz, jbyteArray javaCurve)
  {
    vec_curve *curve = jbyteArray_to_curve(env, javaCurve);
    VECJ_UNUSED(clazz);
    vec_curve_free(curve);
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    get_curve_params
   * Signature: ([B)[[B
   */
  JNIEXPORT jobjectArray JNICALL Java_com_verificatum_vecj_VEC_get_1curve_1params
  (JNIEnv *env, jclass clazz, jbyteArray javaCurve)
  {
    vec_curve *curve = jbyteArray_to_curve(env, javaCurve);

    jbyteArray modulusArray;
    jbyteArray aArray;
    jbyteArray bArray;
    jbyteArray gxArray;
    jbyteArray gyArray;
    jbyteArray nArray;

    jobjectArray javaResult = newByteArrayArray(6);

    VECJ_UNUSED(clazz);

    mpz_t_to_jbyteArray(env, &modulusArray, curve->modulus);
    mpz_t_to_jbyteArray(env, &aArray, curve->a);
    mpz_t_to_jbyteArray(env, &bArray, curve->b);
    mpz_t_to_jbyteArray(env, &gxArray, curve->gx);
    mpz_t_to_jbyteArray(env, &gyArray, curve->gy);
    mpz_t_to_jbyteArray(env, &nArray, curve->n);

    setElement(javaResult, 0, modulusArray);
    setElement(javaResult, 1, aArray);
    setElement(javaResult, 2, bArray);
    setElement(javaResult, 3, gxArray);
    setElement(javaResult, 4, gyArray);
    setElement(javaResult, 5, nArray);

    return javaResult;
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    sqrt
   * Signature: ([B[B)[B
   */
  JNIEXPORT jbyteArray JNICALL Java_com_verificatum_vecj_VEC_sqrt
  (JNIEnv *env, jclass clazz, jbyteArray javaa, jbyteArray javaModulus) {

    jbyteArray javaResult;

    mpz_t a;
    mpz_t modulus;
    mpz_t result;

    VECJ_UNUSED(clazz);

    mpz_init(result);

    jbyteArray_to_mpz_t(env, &a, javaa);
    jbyteArray_to_mpz_t(env, &modulus, javaModulus);

    vec_sqrt(result, a, modulus);

    mpz_t_to_jbyteArray(env, &javaResult, result);

    mpz_clear(result);
    mpz_clear(a);
    mpz_clear(modulus);

    return javaResult;
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    add
   * Signature: ([B[B[B[B[B)[[B
   */
  JNIEXPORT jobjectArray JNICALL Java_com_verificatum_vecj_VEC_add
  (JNIEnv *env, jclass clazz, jbyteArray javaCurve,
   jbyteArray javax1, jbyteArray javay1,
   jbyteArray javax2, jbyteArray javay2) {

    vec_curve *curve = jbyteArray_to_curve(env, javaCurve);

    jobjectArray javaResult;

    mpz_t x1;
    mpz_t y1;
    mpz_t x2;
    mpz_t y2;
    mpz_t rx;
    mpz_t ry;
    vec_scratch_mpz_t scratch;

    VECJ_UNUSED(clazz);

    mpz_init(rx);
    mpz_init(ry);

    /* Convert point coordinates as byte[] to mpz_t */
    jbyteArray_to_mpz_t(env, &x1, javax1);
    jbyteArray_to_mpz_t(env, &y1, javay1);
    jbyteArray_to_mpz_t(env, &x2, javax2);
    jbyteArray_to_mpz_t(env, &y2, javay2);

    vec_scratch_init_mpz_t(scratch);

    vec_jadd_aff(scratch,
                 rx, ry,
                 curve,
                 x1, y1,
                 x2, y2);

    vec_scratch_clear_mpz_t(scratch);

    mpz_t_pair_to_2DjbyteArray(env, &javaResult, rx, ry);

    mpz_clear(y2);
    mpz_clear(x2);
    mpz_clear(y1);
    mpz_clear(x1);

    mpz_clear(ry);
    mpz_clear(rx);

    return javaResult;
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    mul
   * Signature: ([B[B[B[B)[[B
   */
  JNIEXPORT jobjectArray JNICALL Java_com_verificatum_vecj_VEC_mul
  (JNIEnv *env, jclass clazz, jbyteArray javaCurve,
   jbyteArray javaX, jbyteArray javaY, jbyteArray javaScalar) {

    vec_curve *curve = jbyteArray_to_curve(env, javaCurve);

    mpz_t x;
    mpz_t y;
    mpz_t scalar;

    mpz_t rx;
    mpz_t ry;

    jobjectArray javaResult;

    VECJ_UNUSED(clazz);

    /* Translate jbyteArray-parameters to their corresponding GMP
       mpz_t-elements. */
    jbyteArray_to_mpz_t(env, &x, javaX);
    jbyteArray_to_mpz_t(env, &y, javaY);
    jbyteArray_to_mpz_t(env, &scalar, javaScalar);

    /* Compute exponentiation. */
    mpz_init(rx);
    mpz_init(ry);

    vec_jmul_aff(rx, ry,
                 curve,
                 x, y,
                 scalar);

    mpz_t_pair_to_2DjbyteArray(env, &javaResult, rx, ry);

    /* Deallocate resources. */
    mpz_clear(ry);
    mpz_clear(rx);

    mpz_clear(scalar);
    mpz_clear(y);
    mpz_clear(x);

    return javaResult;
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    smul
   * Signature: ([B[[B[[B[[B)[[B
   */
  JNIEXPORT jobjectArray JNICALL Java_com_verificatum_vecj_VEC_smul
  (JNIEnv *env, jclass clazz,
   jbyteArray javaCurve,
   jobjectArray javaBasesx,
   jobjectArray javaBasesy,
   jobjectArray javaScalars) {

    vec_curve *curve = jbyteArray_to_curve(env, javaCurve);

    int i;
    mpz_t *basesx;
    mpz_t *basesy;
    mpz_t *scalars;
    mpz_t resultx;
    mpz_t resulty;

    jbyteArray javaBasex;
    jbyteArray javaBasey;

    /* Extract number of bases/scalars. */
    jsize numberOfBases = (*env)->GetArrayLength(env, javaBasesx);

    jobjectArray javaResult;

    VECJ_UNUSED(clazz);

    /* Convert bases represented arrays of byte[] to arrays of mpz_t. */
    basesx = vec_array_alloc(numberOfBases);
    basesy = vec_array_alloc(numberOfBases);
    for (i = 0; i < numberOfBases; i++)
      {
        javaBasex =
          (jbyteArray)(*env)->GetObjectArrayElement(env, javaBasesx, i);
        jbyteArray_to_mpz_t(env, &(basesx[i]), javaBasex);

        javaBasey =
          (jbyteArray)(*env)->GetObjectArrayElement(env, javaBasesy, i);
        jbyteArray_to_mpz_t(env, &(basesy[i]), javaBasey);
      }

    /* Convert scalars represented as array of byte[] to an array of
       mpz_t. */
    scalars = vec_array_alloc(numberOfBases);
    for (i = 0; i < numberOfBases; i++)
      {
        jbyteArray javaScalar =
          (jbyteArray)(*env)->GetObjectArrayElement(env, javaScalars, i);
        jbyteArray_to_mpz_t(env, &(scalars[i]), javaScalar);
      }

    /* Call GMPMEE's scalariated product function. */
    mpz_init(resultx);
    mpz_init(resulty);

    vec_jsmul_aff(resultx, resulty,
                  curve,
                  basesx, basesy,
                  scalars,
                  numberOfBases);

    mpz_t_pair_to_2DjbyteArray(env, &javaResult, resultx, resulty);

    /* Deallocate resources. */
    mpz_clear(resultx);
    mpz_clear(resulty);
    vec_array_clear_free(scalars, numberOfBases);
    vec_array_clear_free(basesy, numberOfBases);
    vec_array_clear_free(basesx, numberOfBases);

    return javaResult;
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    fmul_precompute
   * Signature: ([B[B[BI)[B
   */
  JNIEXPORT jbyteArray JNICALL Java_com_verificatum_vecj_VEC_fmul_1precompute
  (JNIEnv *env, jclass clazz, jbyteArray javaCurve,
   jbyteArray javaBasisx, jbyteArray javaBasisy, jint size)
  {
    vec_jfmul_tab_ptr table_ptr;
    vec_curve *curve = jbyteArray_to_curve(env, javaCurve);
    mpz_t x;
    mpz_t y;

    VECJ_UNUSED(clazz);

    jbyteArray_to_mpz_t(env, &x, javaBasisx);
    jbyteArray_to_mpz_t(env, &y, javaBasisy);

    table_ptr = vec_jfmul_precomp_aff(curve, x, y, (int)size);

    mpz_clear(y);
    mpz_clear(x);

    return table_to_jbyteArray(env, table_ptr);
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    fmul
   * Signature: ([B[B[B)[[B
   */
  JNIEXPORT jobjectArray JNICALL Java_com_verificatum_vecj_VEC_fmul
  (JNIEnv *env, jclass clazz, jbyteArray javaCurve, jbyteArray javaTable,
   jbyteArray javaScalar)
  {
    jobjectArray javaResult;
    vec_curve *curve = jbyteArray_to_curve(env, javaCurve);
    vec_jfmul_tab_ptr table_ptr = jbyteArray_to_table(env, javaTable);
    mpz_t scalar;
    mpz_t rx;
    mpz_t ry;

    VECJ_UNUSED(clazz);

    mpz_init(rx);
    mpz_init(ry);

    jbyteArray_to_mpz_t(env, &scalar, javaScalar);

    vec_jfmul_aff(rx, ry, curve, table_ptr, scalar);

    mpz_t_pair_to_2DjbyteArray(env, &javaResult, rx, ry);

    mpz_clear(scalar);

    mpz_clear(ry);
    mpz_clear(rx);

    return javaResult;
  }

  /*
   * Class:     com_verificatum_vecj_VEC
   * Method:    fmul_free
   * Signature: ([B[B)V
   */
  JNIEXPORT void JNICALL Java_com_verificatum_vecj_VEC_fmul_1free
  (JNIEnv *env, jclass clazz, jbyteArray javaCurve, jbyteArray javaTable)
  {
    VECJ_UNUSED(clazz);
    vec_jfmul_free_aff(jbyteArray_to_curve(env, javaCurve),
                       jbyteArray_to_table(env, javaTable));
  }

#ifdef __cplusplus
}
#endif
