
/*
 * Copyright 2008-2018 Douglas Wikstrom
 *
 * This file is part of Verificatum Elliptic Curve library (VEC).
 *
 * VEC is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * VEC is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with VEC. If not, see
 * <http://www.gnu.org/licenses/>.
 */

package com.verificatum.vecj;

import java.math.BigInteger;

// We use C style to name things in this file, since it should
// correspond to the native code.

//CHECKSTYLE.OFF: LocalVariableName
//CHECKSTYLE.OFF: LocalFinalVariableName
//CHECKSTYLE.OFF: MethodName
//CHECKSTYLE.OFF: ParameterName

/**
 * Testing routines for VEC. These tests are not meant to be a
 * complete test of the arithmetic. This is handled by the more
 * comprehensive tests in the underlying native code. Here we merely
 * run basic sanity checks to make sure that the native code is called
 * correctly from Java.
 *
 * @author Douglas Wikstrom
 */
@SuppressWarnings({"PMD.MethodNamingConventions",
                   "PMD.VariableNamingConventions"})
public final class TestVEC {

    /**
     * Avoid accidental instantiation.
     */
    private TestVEC() {
    }

    /**
     * Default number of milliseconds used for a single test during
     * testing of a curve.
     */
    static final int DEFAULT_TEST_TIME = 2000;

    /**
     * Convenience method for bounding the execution time of a test.
     *
     * @param t Time when the test started.
     * @param milliSecs Milliseconds the should proceed.
     * @return True if the deadline has passed and false otherwise.
     */
    private static boolean done(final long t, final long milliSecs) {
        return System.currentTimeMillis() > t + milliSecs;
    }

    /**
     * Tests addition during the given number of milliseconds.
     *
     * @param curve_ptr Native pointer to curve.
     * @param milliSecs Duration of the tests.
     */
    public static void test_add(final byte[] curve_ptr, final long milliSecs) {

        final BigInteger[] curveParams = VEC.getCurveParams(curve_ptr);

        final BigInteger[] g = new BigInteger[2];
        g[0] = curveParams[3];
        g[1] = curveParams[4];

        BigInteger[] r = new BigInteger[2];
        r[0] = g[0];
        r[1] = g[1];

        BigInteger[] q = new BigInteger[2];
        q[0] = g[0];
        q[1] = g[1];

        BigInteger[] s = new BigInteger[2];
        s[0] = g[0];
        s[1] = g[1];

        final long t = System.currentTimeMillis();
        while (!done(t, milliSecs)) {

            // Generate somewhat different points.
            q = VEC.add(curve_ptr, q[0], q[1], g[0], g[1]);

            r = VEC.add(curve_ptr, r[0], r[1], g[0], g[1]);
            r = VEC.add(curve_ptr, r[0], r[1], g[0], g[1]);

            s = VEC.add(curve_ptr, s[0], s[1], g[0], g[1]);
            s = VEC.add(curve_ptr, s[0], s[1], g[0], g[1]);
            s = VEC.add(curve_ptr, s[0], s[1], g[0], g[1]);

            // Check associativity.
            final BigInteger[] a = VEC.add(curve_ptr, q[0], q[1], r[0], r[1]);
            final BigInteger[] b = VEC.add(curve_ptr, a[0], a[1], s[0], s[1]);

            final BigInteger[] c = VEC.add(curve_ptr, r[0], r[1], s[0], s[1]);
            final BigInteger[] d = VEC.add(curve_ptr, q[0], q[1], c[0], c[1]);

            assert b[0].equals(d[0]) && b[1].equals(d[1]) : "Failed to add!";
        }
    }

    /**
     * Tests multiplication during the given number of milliseconds.
     *
     * @param curve_ptr Native pointer to curve.
     * @param milliSecs Duration of the tests.
     */
    public static void test_mul(final byte[] curve_ptr,
                                final long milliSecs) {

        final BigInteger[] curveParams = VEC.getCurveParams(curve_ptr);

        BigInteger[] r = new BigInteger[2];
        r[0] = curveParams[3];
        r[1] = curveParams[4];

        final BigInteger n = curveParams[5];

        BigInteger a = BigInteger.ONE.shiftLeft(10000);
        a = a.mod(n);

        BigInteger b = BigInteger.ONE.shiftLeft(10012);
        b = b.mod(n);

        final long t = System.currentTimeMillis();
        while (!done(t, milliSecs)) {
            final BigInteger[] A = VEC.mul(curve_ptr, r[0], r[1], a);
            final BigInteger[] B = VEC.mul(curve_ptr, r[0], r[1], b);

            final BigInteger c = a.add(b).mod(n);
            final BigInteger[] C = VEC.mul(curve_ptr, r[0], r[1], c);

            final BigInteger[] AB = VEC.add(curve_ptr, A[0], A[1], B[0], B[1]);

            a = a.shiftLeft(1).mod(n);
            b = b.shiftLeft(3).mod(n);

            assert C[0].equals(C[0]) && AB[1].equals(AB[1]) : "Failed to mul!";
        }
    }

    /**
     * Testing fixed-basis multiplications during the given number of
     * milliseconds.
     *
     * @param curve_ptr Native pointer to curve.
     * @param milliSecs Duration of the tests.
     */
    public static void test_fmul(final byte[] curve_ptr, final long milliSecs) {

        final BigInteger[] curveParams = VEC.getCurveParams(curve_ptr);

        final BigInteger n = curveParams[5];

        BigInteger a = BigInteger.ONE.shiftLeft(10000);
        a = a.mod(n);

        BigInteger b = BigInteger.ONE.shiftLeft(10012);
        b = b.mod(n);

        final long t = System.currentTimeMillis();

        final byte[] table_ptr = VEC.fmul_precompute(curve_ptr,
                                                     curveParams[3],
                                                     curveParams[4],
                                                     1000);
        while (!done(t, milliSecs)) {

            final BigInteger[] A = VEC.fmul(curve_ptr, table_ptr, a);
            final BigInteger[] B = VEC.fmul(curve_ptr, table_ptr, b);

            final BigInteger c = a.add(b).mod(n);
            final BigInteger[] C = VEC.fmul(curve_ptr, table_ptr, c);

            final BigInteger[] AB = VEC.add(curve_ptr, A[0], A[1], B[0], B[1]);

            a = a.shiftLeft(1).mod(n);
            b = b.shiftLeft(3).mod(n);

            assert C[0].equals(C[0]) && AB[1].equals(AB[1]) : "Failed to fmul!";
        }
    }

    /**
     * Testing taking square roots during the given number of
     * milliseconds.
     *
     * @param curve_ptr Native pointer to curve.
     * @param milliSecs Duration of the tests.
     */
    public static void test_sqrt(final byte[] curve_ptr, final long milliSecs) {

        final BigInteger[] curveParams = VEC.getCurveParams(curve_ptr);

        final BigInteger n = curveParams[5];

        BigInteger a = BigInteger.ONE.shiftLeft(10000);
        a = a.multiply(a).mod(n);

        final long t = System.currentTimeMillis();
        while (!done(t, milliSecs)) {

            final BigInteger b = VEC.sqrt(a, n);

            assert b.multiply(b).mod(n).equals(a)
                : "Failed to take square root!";

            a = a.shiftLeft(10).add(BigInteger.ONE).mod(n);
            a = a.multiply(a).mod(n);
        }
    }

    /**
     * Times some of the routines of the named curve. For each routine
     * the timing proceeds the given number of milliseconds.
     *
     * @param name Name of standard curve.
     * @param milliSecs Duration of the tests.
     */
    public static void test_curve(final String name, final long milliSecs) {

        final byte[] curve_ptr = VEC.getCurve(name);

        final String h = String.format("%nTesting curve: %s (%d ms/function)",
                                       name, milliSecs);
        System.out.println(h);
        System.out.println(
             "----------------------------------------------------");
        System.out.println("add (point addition)");
        test_add(curve_ptr, milliSecs);

        System.out.println("mul (scalar multiplication)");
        test_mul(curve_ptr, milliSecs);

        System.out.println("fmul (fixed-basis scalar multiplication)");
        test_fmul(curve_ptr, milliSecs);

        System.out.println("sqrt (square root modulo curve order)");
        test_sqrt(curve_ptr, milliSecs);
    }

    /**
     * Executes the testing routines.
     *
     * @param args Command line arguments.
     */
    public static void main(final String[] args) {

        // This is part of sanity testing, even if we only run the
        // tests for some curves.
        String[] names = VEC.getCurveNames();

        if (args.length > 0) {
            names = args;
        }

        final String s =
"\n================================================================\n"
+ "\n           TEST com.verificatum.vecj.VEC \n\n"
+ " Although these tests technically give full coverage, they are\n"
+ " not sufficient to verify the correctness of the arithmetic.\n"
+ " The correctness of the arithmetic is guaranteed by the tests\n"
+ " of the native code. The tests run here merely verify that\n"
+ " native calls are handled correctly.\n\n"
+ "================================================================";

        System.out.println(s);

        for (int i = 0; i < names.length; i++) {
            test_curve(names[i], DEFAULT_TEST_TIME);
        }

        System.out.println("");
    }
}
//CHECKSTYLE.ON: LocalVariableName
//CHECKSTYLE.ON: LocalFinalVariableName
//CHECKSTYLE.ON: MethodName
//CHECKSTYLE.ON: ParameterName
