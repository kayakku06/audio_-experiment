#ifndef SHIFT_H
#define SHIFT_H

// 時間シフト関数
static inline void shift(double xnew, double *x, int tap) {
    for (int i = tap - 1; i > 0; i--) {
        x[i] = x[i - 1];
    }
    x[0] = xnew;
}

#endif // SHIFT_H