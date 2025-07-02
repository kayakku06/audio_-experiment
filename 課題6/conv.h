#ifndef CONV_H
#define CONV_H

// 畳み込み関数
static inline double conv(double *h, double *x, int tap) {
    double y = 0.0;
    for (int i = 0; i < tap; i++) {
        y += h[i] * x[i];
    }
    return y;
}

#endif // CONV_H
