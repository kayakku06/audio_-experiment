#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "shift.h"
#include "conv.h"
#define TAP 101 // フィルタ長
#define INPUT_FILE "../data/mix.raw"
#define OUTPUT_FILE "output.raw"
#define COEFF_FILE "../課題５/fir_coeff_N100.txt"
#define TXT_ORIG_FILE "mix.txt"
#define TXT_OUT_FILE "filtered.txt"
#define FS 16000 // サンプリング周波数（例: 16kHz）


// フィルタ係数読み込み
int load_coefficients(const char *filename, double *h, int max_tap)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("係数ファイル読み込み失敗");
        exit(1);
    }
    int n = 0;
    while (fscanf(fp, "%*d %lf", &h[n]) == 1 && n < max_tap)
    {
        n++;
    }
    fclose(fp);
    return n;
}

int main()
{
    FILE *fp_in = fopen(INPUT_FILE, "rb");
    if (!fp_in)
    {
        perror("入力ファイルを開けません");
        return 1;
    }

    FILE *fp_out = fopen(OUTPUT_FILE, "wb");
    FILE *fp_txt_orig = fopen(TXT_ORIG_FILE, "w");
    FILE *fp_txt_filt = fopen(TXT_OUT_FILE, "w");

    if (!fp_out || !fp_txt_orig || !fp_txt_filt)
    {
        perror("出力ファイルを開けません");
        return 1;
    }

    double h[TAP];
    double x[TAP] = {0};
    load_coefficients(COEFF_FILE, h, TAP);

    int16_t sample_in, sample_out;
    int n = 0;

    while (fread(&sample_in, sizeof(int16_t), 1, fp_in) == 1)
    {
        double xn = sample_in / 32768.0;

        // 時間（秒）をX軸に、正規化した元データを出力
        fprintf(fp_txt_orig, "%.6f\t%.6f\n", (double)n * 1000 / FS, xn);

        shift(xn, x, TAP);
        double yn = conv(h, x, TAP);

        if (yn > 1.0)
            yn = 1.0;
        if (yn < -1.0)
            yn = -1.0;

        sample_out = (int16_t)(yn * 32767.0);
        fwrite(&sample_out, sizeof(int16_t), 1, fp_out);

        // 正規化後の出力データを書き出し
        fprintf(fp_txt_filt, "%.6f\t%.6f\n", (double)n *1000/ FS, yn);

        n++;
    }

    fclose(fp_in);
    fclose(fp_out);
    fclose(fp_txt_orig);
    fclose(fp_txt_filt);

    printf("Filtering complete. Output written to '%s'\n", OUTPUT_FILE);
    return 0;
}
