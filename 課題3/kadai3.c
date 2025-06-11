#include <stdio.h>           // 標準入出力ライブラリ
#include <stdlib.h>          // exit関数などのための標準ライブラリ
#include <math.h>            // sin, cos, M_PIなどの数学関数を使うため
#include "kadai3_DFT_IDFT.h" // 自作のDFTとIDFT関数が書かれたヘッダファイル

#define PI 3.141592653589793
#define SAMPLING_RATE 16000 // サンプリング周波数（Hz）
#define N 1024              // DFTの長さ（サンプル数）
#define FREQ_SINE 10        // 正弦波の周波数（任意に設定）
#define FREQ_COS 10         // 余弦波の周波数（任意に設定）

// 計算結果をファイルに保存する関数
void save_to_file(const char *filename, double *data, int size)
{
    FILE *fp = fopen(filename, "w"); // 書き込みモードでファイルを開く
    if (fp == NULL)
    {
        perror("ファイルを開けませんでした");
        exit(EXIT_FAILURE); // エラーがあれば終了
    }
    for (int i = 0; i < size; i++)
    {
        fprintf(fp, "%d %f\n", i, data[i]); // 各データをインデックスとともに出力
    }
    fclose(fp); // ファイルを閉じる
}
// RAW（バイナリ）形式で保存する関数（1サンプル = double型）
void save_to_raw(const char *filename, double *data, int size)
{
    FILE *fp = fopen(filename, "wb"); // バイナリ書き込みモードで開く
    if (fp == NULL)
    {
        perror("ファイルを開けませんでした");
        exit(EXIT_FAILURE);
    }
    fwrite(data, sizeof(double), size, fp); // double型でそのまま書き出す
    fclose(fp);
}

int main()
{
    // 正弦波と余弦波の実部・虚部配列の宣言と初期化
    double sine[N], sine_i[N] = {0};     // sin波の虚部は全て0で初期化
    double cosine[N], cosine_i[N] = {0}; // cos波の虚部も全て0で初期化

    // 正弦波と余弦波の生成（DFT長Nに基づいて1周期を構成）
    for (int n = 0; n < N; n++)
    {
        sine[n] = sin(2 * PI * FREQ_SINE * n / N);  // 正弦波（0〜2πの1周期）
        cosine[n] = cos(2 * PI * FREQ_COS * n / N); // 余弦波（0〜2πの1周期）
    }

    // 元の波形（時間領域）をファイルに保存
    save_to_file("sinreal.txt", sine, N);
    save_to_file("cosreal.txt", cosine, N);

    // DFTを行うためにコピー（元データを変更しないように）
    double sine_dft_r[N], sine_dft_i[N];
    double cosine_dft_r[N], cosine_dft_i[N];
    for (int i = 0; i < N; i++)
    {
        sine_dft_r[i] = sine[i];     // 実部のコピー
        sine_dft_i[i] = 0.0;         // 虚部は0
        cosine_dft_r[i] = cosine[i]; // 実部のコピー
        cosine_dft_i[i] = 0.0;       // 虚部は0
    }

    // DFT（離散フーリエ変換）を実行
    DFT(N, sine_dft_r, sine_dft_i);     // sin波の周波数スペクトル
    DFT(N, cosine_dft_r, cosine_dft_i); // cos波の周波数スペクトル

    // DFTの結果（実部・虚部）をファイルに保存
    save_to_file("sin_dft_real.txt", sine_dft_r, N);
    save_to_file("sin_dft_imag.txt", sine_dft_i, N);
    save_to_file("cos_dft_real.txt", cosine_dft_r, N);
    save_to_file("cos_dft_imag.txt", cosine_dft_i, N);

    // IDFT用にDFT結果をコピー（DFTの出力をIDFTの入力とする）
    double sine_idft_r[N], sine_idft_i[N];
    double cosine_idft_r[N], cosine_idft_i[N];
    for (int i = 0; i < N; i++)
    {
        sine_idft_r[i] = sine_dft_r[i];     // 実部をコピー
        sine_idft_i[i] = sine_dft_i[i];     // 虚部をコピー
        cosine_idft_r[i] = cosine_dft_r[i]; // 実部をコピー
        cosine_idft_i[i] = cosine_dft_i[i]; // 虚部をコピー
    }

    // IDFT（逆離散フーリエ変換）を実行
    IDFT(N, sine_idft_r, sine_idft_i);
    IDFT(N, cosine_idft_r, cosine_idft_i);

    // IDFTの結果（時間領域に戻った波形）をファイルに保存
    save_to_file("sin_idft_real.txt", sine_idft_r, N);
    save_to_file("sin_idft_imag.txt", sine_idft_i, N);
    save_to_file("cos_idft_real.txt", cosine_idft_r, N);
    save_to_file("cos_idft_imag.txt", cosine_idft_i, N);
 // RAW形式でも保存（1サンプル = double）
    save_to_raw("sine.raw", sine, N);
    save_to_raw("cosine.raw", cosine, N);
    return 0; // 正常終了
   
}
//位相が異なるからスペクトルが異なる
//DFTとIDFTはエネルギー保存
