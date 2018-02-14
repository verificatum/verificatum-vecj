
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
 * Benchmarking routines for VEC.
 *
 * @author Douglas Wikstrom
 */
@SuppressWarnings({"PMD.MethodNamingConventions",
                   "PMD.VariableNamingConventions"})
public final class BenchVEC {

    /**
     * Avoid accidental instantiation.
     */
    private BenchVEC() {
    }

    /**
     * Default number of milliseconds used for a single test during
     * testing of a curve.
     */
    static final int DEFAULT_SPEED_TIME = 2000;

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
     * Returns the number of additions performed during the given
     * number of milliseconds.
     *
     * @param curve_ptr Native pointer to curve.
     * @param milliSecs Duration of the timing.
     * @return Number of operations.
     */
    public static long time_add(final byte[] curve_ptr, final long milliSecs) {

        final BigInteger[] curveParams = VEC.getCurveParams(curve_ptr);

        final BigInteger[] g = new BigInteger[2];
        g[0] = curveParams[3];
        g[1] = curveParams[4];

        BigInteger[] r = new BigInteger[2];
        r[0] = g[0];
        r[1] = g[1];

        final long t = System.currentTimeMillis();
        long i = 0;
        while (!done(t, milliSecs)) {
            r = VEC.add(curve_ptr, r[0], r[1], g[0], g[1]);
            i++;
        }
        return i;
    }

    /**
     * Returns the number of multiplications performed during the
     * given number of milliseconds.
     *
     * @param curve_ptr Native pointer to curve.
     * @param milliSecs Duration of the timing.
     * @return Number of operations.
     */
    public static long time_mul(final byte[] curve_ptr,
                                final long milliSecs) {

        final BigInteger[] curveParams = VEC.getCurveParams(curve_ptr);

        BigInteger[] r = new BigInteger[2];
        r[0] = curveParams[3];
        r[1] = curveParams[4];

        final BigInteger n = curveParams[5];

        BigInteger scalar = BigInteger.ONE.shiftLeft(10000);
        scalar = scalar.mod(n);

        final long t = System.currentTimeMillis();
        long i = 0;
        while (!done(t, milliSecs)) {
            r = VEC.mul(curve_ptr, r[0], r[1], scalar);

            scalar = scalar.shiftLeft(1);
            scalar = scalar.mod(n);

            i++;
        }
        return i;
    }

    /**
     * Returns the number of fixed-basis multiplications performed
     * during the given number of milliseconds.
     *
     * @param curve_ptr Native pointer to curve.
     * @param milliSecs Duration of the timing.
     * @return Number of operations.
     */
    public static long time_fmul(final byte[] curve_ptr, final long milliSecs) {

        final BigInteger[] curveParams = VEC.getCurveParams(curve_ptr);

        final BigInteger n = curveParams[5];

        BigInteger scalar = BigInteger.ONE.shiftLeft(100000);
        scalar = scalar.mod(n);

        final long t = System.currentTimeMillis();

        final byte[] table_ptr = VEC.fmul_precompute(curve_ptr,
                                                     curveParams[3],
                                                     curveParams[4],
                                                     1000);

        long i = 0;
        while (!done(t, milliSecs)) {

            final BigInteger[] r = VEC.fmul(curve_ptr, table_ptr, scalar);

            scalar = scalar.shiftLeft(1);
            scalar = scalar.mod(n);

            i++;
        }
        return i;
    }

    /**
     * Times some of the routines of the named curve. For each routine
     * the timing proceeds the given number of milliseconds.
     *
     * @param name Name of standard curve.
     * @param milliSecs Duration of the timing.
     */
    public static void time_curve(final String name, final long milliSecs) {

        final byte[] curve_ptr = VEC.getCurve(name);

        final String h = String.format("%nTiming curve: %s (%d ms/function)",
                                       name, milliSecs);
        System.out.println(h);
        System.out.println(
             "----------------------------------------------------");

        String r;

        r = String.format("%12d additions",
                          time_add(curve_ptr, milliSecs));
        System.out.println(r);

        r = String.format("%12d multiplications",
                          time_mul(curve_ptr, milliSecs));
        System.out.println(r);

        r = String.format("%12d fixed-base multiplications",
                          time_fmul(curve_ptr, milliSecs));
        System.out.println(r);
    }

    /**
     * Executes the timing routines of curves.
     *
     * @param args Command line arguments.
     */
    public static void main(final String[] args) {

        String[] names = null;

        if (args.length > 0) {
            names = args;
        } else {
            names = VEC.getCurveNames();
        }

        final String s =
"\n================================================================\n"
+ "\n          BENCHMARKS FOR VECJ\n\n"
+ "You need to consult the code understand exactly what is\n"
+ "measured before drawing any conclusions, but the benchmarks\n"
+ "are fairly self explanatory."
+ "\n\n"
+ "The code makes calls to Verificatum Elliptic Curve library (VEC)\n"
+ "which is implemented on top of GNU Multiple Precision Arithmetic\n"
+ "library (GMP).\n\n"
+ "================================================================";

        System.out.println(s);

        for (int i = 0; i < names.length; i++) {
            time_curve(names[i], DEFAULT_SPEED_TIME);
        }

        System.out.println("");
    }
}
//CHECKSTYLE.ON: LocalVariableName
//CHECKSTYLE.ON: LocalFinalVariableName
//CHECKSTYLE.ON: MethodName
//CHECKSTYLE.ON: ParameterName
