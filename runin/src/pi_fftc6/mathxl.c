/*  ******** xlong double math functions ********  */

#include "config.h"


xlfloat cosxl(xlfloat);
xlfloat sinxl(xlfloat);
xlfloat atanxl(xlfloat);
xlfloat expxl(xlfloat);
xlfloat logxl(xlfloat);
xlfloat sqrtxl(xlfloat);


#ifndef XLC_PI_2
#define XLC_PI_2 ((xlfloat) 1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314L)
#endif
#ifndef XLC_LOG2
#define XLC_LOG2 ((xlfloat) 0.693147180559945309417232121458176568075500134360255254120680009493393621969694715605L)
#endif


xlfloat cosxl(xlfloat x)
{
    xlfloat cosxl_taylor(int, xlfloat);
    xlfloat xt;
    xlint i;
    
    if (x < 0) x = -x;
    xt = x * (1 / XLC_PI_2) + (xlfloat) 1 / 2;
    if (xt > XLINT_MAX) return XLFLOAT_NAN;
    i = (xlint) xt;
    return cosxl_taylor(i & 3, x - i * XLC_PI_2);
}


xlfloat sinxl(xlfloat x)
{
    xlfloat cosxl_taylor(int, xlfloat);
    xlfloat xt;
    xlint i;
    
    xt = x * (1 / XLC_PI_2) + (xlfloat) 1 / 2;
    if (xt > XLINT_MAX || xt < XLINT_MIN + 1) return XLFLOAT_NAN;
    i = (xlint) xt;
    if (xt < 0) i--;
    return cosxl_taylor((i & 3) + 3, x - i * XLC_PI_2);
}


xlfloat atanxl(xlfloat x)
{
    xlfloat atanhxl_taylor(int, xlfloat);
    xlfloat y0, ys;
    
    ys = 1;
    if (x < 0) {
        ys = -1;
        x = -x;
    }
    y0 = 0;
    if (x > 1) {
        y0 = ys * XLC_PI_2;
        ys = -ys;
        x = 1 / x;
    }
    if (x * (x + 2) > 1) {
        y0 += ys * (XLC_PI_2 / 2);
        x = (x - 1) / (x + 1);
    }
    return y0 + ys * atanhxl_taylor(-1, x);
}


xlfloat expxl(xlfloat x)
{
    xlfloat expxl_taylor(xlfloat);
    xlfloat xt, y, yt;
    xlint i, j;
    
    xt = x * (1 / XLC_LOG2) + (xlfloat) 1 / 2;
    if (xt > XLINT_MAX || xt < XLINT_MIN + 1 || -xt > XLINT_MAX - 1) return XLFLOAT_NAN;
    i = (xlint) xt;
    if (xt < 0) i--;
    y = expxl_taylor(x - i * XLC_LOG2);
    yt = 2;
    if (i < 0) {
        i = -i;
        yt = (xlfloat) 1 / 2;
    }
    for (j = 0; i >> j; j++) {
        if ((i >> j) & 1) y *= yt;
        yt *= yt;
    }
    return y;
}


xlfloat logxl(xlfloat x)
{
    xlfloat atanhxl_taylor(int, xlfloat);
    xlfloat x0, xt;
    xlint i, j;
    
    if (x <= 0) return XLFLOAT_NAN;
    i = 0;
    if (x > 2) {
        do {
            x0 = (xlfloat) 1 / 2;
            j = 1;
            for (xt = 4; x >= xt; xt *= xt) {
                x0 *= x0;
                j <<= 1;
            }
            x *= x0;
            i += j;
        } while (x > 2);
    } else if (x < 1) {
        do {
            x0 = 2;
            j = 1;
            for (xt = (xlfloat) 1 / 4; x <= xt; xt *= xt) {
                x0 *= x0;
                j <<= 1;
            }
            x *= x0;
            i -= j;
        } while (x < 1);
    }
    if (x * x > 2) {
        x *= (xlfloat) 1 / 2;
        i++;
    }
    return i * XLC_LOG2 + 2 * atanhxl_taylor(1, (x - 1) / (x + 1));
}


xlfloat sqrtxl(xlfloat x)
{
    xlfloat x0, xt, y0, ys, e, y;
    
    if (x < 0) return XLFLOAT_NAN;
    if (x == 0) return 0;
    ys = 1;
    if (x > 4) {
        do {
            x0 = (xlfloat) 1 / 4;
            y0 = 2;
            for (xt = 16; x >= xt; xt *= xt) {
                x0 *= x0;
                y0 *= y0;
            }
            x *= x0;
            ys *= y0;
        } while (x > 4);
    } else if (x < 1) {
        do {
            x0 = 4;
            y0 = (xlfloat) 1 / 2;
            for (xt = (xlfloat) 1 / 16; x <= xt; xt *= xt) {
                x0 *= x0;
                y0 *= y0;
            }
            x *= x0;
            ys *= y0;
        } while (x < 1);
    }
    y = 1 - x * ((xlfloat) 1 / 4);
    y = y * y + (xlfloat) 1 / 2;
    do {
        e = 1 - y * y * x;
        y += e * y * ((xlfloat) 1 / 2);
    } while (e * e > XLFLOAT_EPSILON / 2);
    return x * y * ys;
}


/*  ******** Taylor Expansion ********  */


xlfloat cosxl_taylor(int cs_mode, xlfloat x)
{
    /* ---- cos(x + cs_mode * pi / 2) ---- */
    xlfloat jp, x2, xp, y;
    int j, j2;
    
    x2 = x * x;
    xp = x2;
    j = 4 * (cs_mode & 1) + 2;
    y = j - x2;
    jp = j;
    j2 = j;
    do {
        j += 8;
        j2 += j;
        xp *= x2;
        y = y * j2 + xp;
        jp *= j2;
        j += 8;
        j2 += j;
        xp *= x2;
        y = y * j2 - xp;
        jp *= j2;
    } while (xp > XLFLOAT_EPSILON * jp);
    if (cs_mode & 1) y *= x;
    if ((cs_mode + 1) & 2) y = -y;
    return y / jp;
}


xlfloat atanhxl_taylor(int hyp_mode, xlfloat x)
{
    xlfloat jp, x2, xp, y;
    int j;
    
    x2 = hyp_mode * x * x;
    xp = x2 * x2;
    y = 15 + 5 * x2 + 3 * xp;
    jp = 15;
    j = 5;
    do {
        j += 2;
        xp *= x2;
        y = y * j + xp * jp;
        jp *= j;
        j += 2;
        xp *= x2;
        y = y * j + xp * jp;
        jp *= j;
        if (jp * XLFLOAT_EPSILON > 1) {
            jp *= XLFLOAT_EPSILON;
            y *= XLFLOAT_EPSILON;
        }
    } while (xp > XLFLOAT_EPSILON);
    return x * y / jp;
}


xlfloat expxl_taylor(xlfloat x)
{
    xlfloat jp, x2, xp, y;
    int j, j2;
    
    x2 = x * x;
    xp = x2;
    y = 6 + (6 + x2) * x + 3 * x2;
    jp = 6;
    j = 3;
    j2 = 6;
    do {
        j += 2;
        j2 += 4 * j - 6;
        xp *= x2;
        y = y * j2 + xp * (x + j);
        jp *= j2;
    } while (xp > XLFLOAT_EPSILON * jp);
    return y / jp;
}

