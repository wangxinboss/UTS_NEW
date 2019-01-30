#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace ColorSpace
        {
            void RGBInt2Lab(int R, int G, int B, double &L, double &a, double &b)
            {
                RGBDouble2Lab((double)R, (double)G, (double)B, L, a, b);
            }

            void RGBDouble2Lab(double R, double G, double B, double &Ls, double &as, double &bs)
            {
				//RGB -> XYZ -> CIE-L*ab
                double r, g, b, X, Y, Z, fx, fy, fz, xr, yr, zr;
                double eps = 216. / 24389.;
                double k = 24389. / 27.;

                double Xr = 0.964221;  // reference white D50
                double Yr = 1.0;
                double Zr = 0.825211;

                // RGB to XYZ
                r = R / 255.; //R 0..1
                g = G / 255.; //G 0..1
                b = B / 255.; //B 0..1

                // assuming sRGB (D65)
                if (r <= 0.04045)
                    r = r / 12.;
                else
                    r = pow((r + 0.055) / 1.055, 2.4);

                if (g <= 0.04045)
                    g = g / 12.;
                else
                    g = pow((g + 0.055) / 1.055, 2.4);

                if (b <= 0.04045)
                    b = b / 12.;
                else
                    b = pow((b + 0.055) / 1.055, 2.4);


                X = 0.436052025 * r + 0.385081593 * g + 0.143087414 * b;
                Y = 0.222491598 * r + 0.71688606  * g + 0.060621486 * b;
                Z = 0.013929122 * r + 0.097097002 * g + 0.71418547  * b;

                // XYZ to Lab
                xr = X / Xr;
                yr = Y / Yr;
                zr = Z / Zr;

                if (xr > eps)
                    fx = pow(xr, 1 / 3.);
                else
                    fx = (k * xr + 16.) / 116.;

                if (yr > eps)
                    fy = pow(yr, 1 / 3.);
                else
                    fy = (k * yr + 16.) / 116.;

                if (zr > eps)
                    fz = pow(zr, 1 / 3.);
                else
                    fz = (k * zr + 16.) / 116.;

                Ls = (116 * fy) - 16.;
                as = 500. * (fx - fy);
                bs = 200. * (fy - fz);
            }
        }
    }
}
