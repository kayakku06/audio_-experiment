#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../課題3/kadai3_DFT_IDFT.h"  // 課題3のDFT関数

#define PI 3.14159265358979323846
#define MAX_N 1024  // 十分なゼロパディングを行う

#define OMEGA_C 0.4  // 正規化カットオフ周波数

// sinc関数（n = N/2のときの0除算対策）
double sinc(double x) {
    if (fabs(x) < 1e-8) return 1.0;
    return sin(PI * x) / (PI * x);
}

// FIRフィルタ係数を計算してファイル出力
void generate_fir_coeff(int N, double *h, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("File open failed");
        exit(EXIT_FAILURE);
    }

    for (int n = 0; n <= N; n++) {
        if (n == N / 2) {
            h[n] = OMEGA_C;
        } else {
            h[n] = sin((n - N / 2.0) * PI * OMEGA_C) / ((n - N / 2.0) * PI);
        }
        fprintf(fp, "%d %.8f\n", n, h[n]);
    }

    // ゼロパディング
    for (int n = N + 1; n < MAX_N; n++) {
        h[n] = 0.0;
    }

    fclose(fp);
    printf("FIR coefficients saved to %s\n", filename);
}

// DFTの振幅スペクトルをdBでファイルに書き出す（横軸は0～1の正規化角周波数）
void output_amplitude_spectrum(double *xr, double *xi, int N, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Cannot write DFT output");
        exit(EXIT_FAILURE);
    }

    for (int k = 0; k <= N / 2; k++) {
        double magnitude = sqrt(xr[k] * xr[k] + xi[k] * xi[k]);
        double amp_db = 20.0 * log10(magnitude + 1e-12); // dB変換（0除算回避のため微小値加算）
        double normalized_angular_freq = 2.0 * ((double)k / N);  // ナイキスト周波数0.5を1に拡大
        fprintf(fp, "%.6f %.8f\n", normalized_angular_freq, amp_db);
    }

    fclose(fp);
    printf("Amplitude spectrum saved to %s\n", filename);
}

int main() {
    int Ns[] = {100, 500, 1000};
    char coeff_filename[64];
    char amp_filename[64];

    for (int i = 0; i < 3; i++) {
        int N = Ns[i];

        double h[MAX_N];
        double xi[MAX_N] = {0};  // 虚部は全て0に初期化

        // FIR係数とファイル名設定
        snprintf(coeff_filename, sizeof(coeff_filename), "fir_coeff_N%d.txt", N);
        generate_fir_coeff(N, h, coeff_filename);

        // DFT実行
        // xrは実部としてh、xiは虚部（0）で入力
        DFT(MAX_N, h, xi);

        // 振幅スペクトルをファイル出力
        snprintf(amp_filename, sizeof(amp_filename), "amp_spectrum_N%d.txt", N);
        output_amplitude_spectrum(h, xi, MAX_N, amp_filename);
    }

    return 0;
}
