#ifndef KADAI3_DFT_IDFT_H
#define KADAI3_DFT_IDFT_H

#include <math.h>  // sin, cos など三角関数を使うためにインクルード

// M_PI が未定義の場合に定義（円周率）
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 離散フーリエ変換（DFT）を行う関数
 *
 * @param DFT_SIZE 変換するデータの長さ（サンプル数）
 * @param xr 実部の配列ポインタ（入力波形の実部。変換後もここに実部が上書きされる）
 * @param xi 虚部の配列ポインタ（入力波形の虚部。変換後もここに虚部が上書きされる）
 *
 * 入力信号が実数信号の場合、xi（虚部）はすべて0であることが想定されている。
 * 計算には一時的な配列 temp_r, temp_i を用い、最後に元の配列に上書き保存する。
 */
static inline void DFT(int DFT_SIZE, double *xr, double *xi)
{
    double temp_r[DFT_SIZE];  // 実部の一時保存用配列
    double temp_i[DFT_SIZE];  // 虚部の一時保存用配列

    for (int k = 0; k < DFT_SIZE; k++) {
        temp_r[k] = 0.0;
        temp_i[k] = 0.0;
        // 各 k に対して n 全体をループして変換を行う
        for (int n = 0; n < DFT_SIZE; n++) {
            double angle = -2.0 * M_PI * k * n / DFT_SIZE;
            double cos_val = cos(angle);  // 実部用の重み
            double sin_val = sin(angle);  // 虚部用の重み

            // 実部のDFT計算: Re = xr * cos + xi * sin
            temp_r[k] += xr[n] * cos_val - xi[n] * sin_val;

            // 虚部のDFT計算: Im = xr * sin + xi * cos
            temp_i[k] += xr[n] * sin_val + xi[n] * cos_val;
        }
    }

    // 結果を元の配列にコピー（破壊的更新）
    for (int k = 0; k < DFT_SIZE; k++) {
        xr[k] = temp_r[k];
        xi[k] = temp_i[k];
    }
}

/**
 * @brief 逆離散フーリエ変換（IDFT）を行う関数
 *
 * @param DFT_SIZE データの長さ（サンプル数）
 * @param Xr 実部の配列ポインタ（周波数スペクトルの実部。逆変換後、時間信号の実部として上書きされる）
 * @param Xi 虚部の配列ポインタ（周波数スペクトルの虚部。逆変換後、時間信号の虚部として上書きされる）
 *
 * DFT同様、一時配列を使って計算し、最後に元の配列に上書きする。
 * IDFTの式では角度が正になり、最後に全体を N で割ることでスケーリングする。
 */
static inline void IDFT(int DFT_SIZE, double *Xr, double *Xi)
{
    double temp_r[DFT_SIZE];  // 実部の一時保存用配列
    double temp_i[DFT_SIZE];  // 虚部の一時保存用配列

    for (int n = 0; n < DFT_SIZE; n++) {
        temp_r[n] = 0.0;
        temp_i[n] = 0.0;
        // 各 n に対して k 全体をループして逆変換を行う
        for (int k = 0; k < DFT_SIZE; k++) {
            double angle = 2.0 * M_PI * k * n / DFT_SIZE;
            double cos_val = cos(angle);
            double sin_val = sin(angle);

            // 実部のIDFT計算: Re = Xr * cos - Xi * sin
            temp_r[n] += Xr[k] * cos_val - Xi[k] * sin_val;

            // 虚部のIDFT計算: Im = Xr * sin + Xi * cos
            temp_i[n] += Xr[k] * sin_val + Xi[k] * cos_val;
        }
        // Nで割ってスケーリング
        temp_r[n] /= DFT_SIZE;
        temp_i[n] /= DFT_SIZE;
    }

    // 結果を元の配列にコピー（破壊的更新）
    for (int n = 0; n < DFT_SIZE; n++) {
        Xr[n] = temp_r[n];
        Xi[n] = temp_i[n];
    }
}

#endif // KADAI3_DFT_IDFT_H
