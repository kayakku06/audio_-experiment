#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "../課題3/kadai3_DFT_IDFT.h"  // 課題3のDFT関数

#define DFT_SIZE 1024
#define SAMPLING_RATE 16000

// ハミング窓関数を生成
void apply_hamming_window(double *x, int L) {
    for (int n = 0; n < L; n++) {
        double w = 0.54 - 0.46 * cos(2.0 * M_PI * n / (L - 1));
        x[n] *= w;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "使い方: %s 入力.raw 出力.txt\n", argv[0]);
        return 1;
    }

    const char *infile = argv[1];
    const char *outfile = argv[2];

    // 入力ファイルサイズ確認
    struct stat st;
    if (stat(infile, &st) != 0) {
        perror("ファイルが存在しません");
        return 1;
    }

    int L = st.st_size / sizeof(short);
    short *raw = (short *)calloc(L, sizeof(short));
    if (!raw) {
        perror("メモリ確保失敗");
        return 1;
    }

    FILE *fp = fopen(infile, "rb");
    if (!fp) {
        perror("ファイルオープン失敗");
        free(raw);
        return 1;
    }
    fread(raw, sizeof(short), L, fp);
    fclose(fp);

    // 波形をdouble型に変換してゼロパディング
    double xr[DFT_SIZE] = {0};
    double xi[DFT_SIZE] = {0};

    for (int i = 0; i < L && i < DFT_SIZE; i++) {
        xr[i] = (double)raw[i];
    }

    // ハミング窓適用
    apply_hamming_window(xr, (L < DFT_SIZE ? L : DFT_SIZE));

    // DFT実行
    DFT(DFT_SIZE, xr, xi);

    // 結果出力 (.txt)
    FILE *out = fopen(outfile, "w");
    if (!out) {
        perror("出力ファイル作成失敗");
        free(raw);
        return 1;
    }

    for (int k = 0; k < DFT_SIZE; k++) {
        double power = xr[k] * xr[k] + xi[k] * xi[k];
        double log_power = log10(power + 1e-6); // εでゼロ除算回避
        double freq = (double)k * SAMPLING_RATE / DFT_SIZE;
        fprintf(out, "%.1f\t%.6f\n", freq, log_power);
    }

    fclose(out);
    free(raw);

    printf("出力完了: %s → %s（%d点）\n", infile, outfile, DFT_SIZE);
    return 0;
}
