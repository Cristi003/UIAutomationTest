using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UIAutomationLibraryClientCSharp
{
    public class CubicBezier
    {

        private int NEWTON_ITERATIONS = 4;
        private double NEWTON_MIN_SLOPE = 0.02;
        private double SUBDIVISION_PRECISION = 0.0000001;
        private int SUBDIVISION_MAX_ITERATIONS = 10;

        private const int SPLINE_TABLE_SIZE = 11;
        private double[] mSampleValues = new double[SPLINE_TABLE_SIZE];
        private const double SAMPLE_STEP_SIZE = 1.0 / (double)(SPLINE_TABLE_SIZE - 1);

        private double mX1, mY1, mX2, mY2;

        public CubicBezier() { /* caller must call Init later */ }
        
        public CubicBezier(double aX1, double aY1, double aX2, double aY2)
        {
            Init(aX1, aY1, aX2, aY2);
        }

        public void Init(double aX1, double aY1, double aX2, double aY2)
        {
            mX1 = aX1;
            mY1 = aY1;
            mX2 = aX2;
            mY2 = aY2;

            if (mX1 != mY1 || mX2 != mY2)
                CalcSampleValues();
        }

        public double GetSplineValue(double aX)
        {
            if (mX1 == mY1 && mX2 == mY2)
                return aX;

            return CalcBezier(GetTForX(aX), mY1, mY2);
        }

        private void GetSplineDerivativeValues(double aX, out double aDX, out double aDY)
        {
            double t = GetTForX(aX);
            aDX = GetSlope(t, mX1, mX2);
            aDY = GetSlope(t, mY1, mY2);
        }

        private void CalcSampleValues()
        {
            for (int i = 0; i < SPLINE_TABLE_SIZE; ++i)
            {
                mSampleValues[i] = CalcBezier((double)i * SAMPLE_STEP_SIZE, mX1, mX2);
            }
        }

        private static double CalcBezier(double aT, double aA1, double aA2)
        {
            // use Horner's scheme to evaluate the Bezier polynomial
            return ((A(aA1, aA2) * aT + B(aA1, aA2)) * aT + C(aA1)) * aT;
        }

        private static double A(double aA1, double aA2) { return 1.0 - 3.0 * aA2 + 3.0 * aA1; }
        private static double B(double aA1, double aA2) { return 3.0 * aA2 - 6.0 * aA1; }
        private static double C(double aA1) { return 3.0 * aA1; }

        private static double GetSlope(double aT, double aA1, double aA2)
        {
            return 3.0 * A(aA1, aA2) * aT * aT + 2.0 * B(aA1, aA2) * aT + C(aA1);
        }

        private double GetTForX(double aX)
        {
            // Find interval where t lies
            double intervalStart = 0.0;
            double currentSample = mSampleValues[1];
            double lastSample = mSampleValues[SPLINE_TABLE_SIZE - 1];
            int i;
            for (i = 1; i < (SPLINE_TABLE_SIZE - 1) && currentSample <= aX; ++i, currentSample = mSampleValues[i])
            {
                intervalStart += SAMPLE_STEP_SIZE;
            }
            currentSample = mSampleValues[i - 1]; // t now lies between sample i and sample i+1
            double nextSample = mSampleValues[i];

            // Interpolate to provide an initial guess for t
            double distance = (aX - currentSample) / (nextSample - currentSample);
            double guessForT = intervalStart + distance * SAMPLE_STEP_SIZE;

            // Check the slope to see what strategy to use. If the slope is too small
            // Newton-Raphson iteration won't converge on a root so we use bisection
            // instead.
            double initialSlope = GetSlope(guessForT, mX1, mX2);
            if (initialSlope >= NEWTON_MIN_SLOPE)
            {
                return NewtonRaphsonIterate(aX, guessForT);
            }
            else if (initialSlope == 0.0)
            {
                return guessForT;
            }
            else
            {
                return BinarySubdivide(aX, intervalStart, intervalStart + SAMPLE_STEP_SIZE);
            }
        }
        double NewtonRaphsonIterate(double aX, double aGuessT)
        {
            // Refine guess with Newton-Raphson iteration
            for (int i = 0; i < NEWTON_ITERATIONS; ++i)
            {
                double currentSlope = GetSlope(aGuessT, mX1, mX2);

                if (currentSlope == 0.0)
                    return aGuessT;

                // We're trying to find where f(t) = aX,
                // so we're actually looking for a root for: CalcBezier(t) - aX
                double currentX = CalcBezier(aGuessT, mX1, mX2) - aX;

                aGuessT -= currentX / currentSlope;
            }

            return aGuessT;
        }

        double BinarySubdivide(double aX, double aA, double aB)
        {
            double currentX;
            double currentT;
            int i = 0;

            do
            {
                currentT = aA + (aB - aA) / 2.0;
                currentX = CalcBezier(currentT, mX1, mX2) - aX;

                if (currentX > 0.0)
                {
                    aB = currentT;
                }
                else
                {
                    aA = currentT;
                }
            } while (Math.Abs(currentX) > SUBDIVISION_PRECISION
                && ++i < SUBDIVISION_MAX_ITERATIONS);

            return currentT;
        }
    }
}
