#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLE_SIZE 1024      // 読み込むサンプル数
#define NUM_VOWELS 5          // 母音の数

// 母音とファイル名の対応
const char* vowel_labels[NUM_VOWELS] = {"a", "i", "u", "e", "o"};
const char* template_files[NUM_VOWELS] = {
    "../data/a00.raw", "../data/i00.raw", "../data/u00.raw", "../data/e00.raw", "../data/o00.raw"
};

// テンプレート用スペクトル配列
double templates[NUM_VOWELS][SAMPLE_SIZE];

// fft.o に含まれるFFT関数の宣言（実装は.oにある）
void fft(int n, double* x_real, double* x_imag);

// 音声データ（16bit PCM）の読み込み
void read_raw(const char* filename, double* buffer) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "ファイル %s が開けません\n", filename);
        exit(1);
    }
    short temp;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        if (fread(&temp, sizeof(short), 1, fp) != 1) {
            fprintf(stderr, "ファイル %s の読み込みエラー\n", filename);
            exit(1);
        }
        buffer[i] = (double)temp;
    }
    fclose(fp);
}

// 対数パワースペクトルの計算
void compute_log_power_spectrum(double* signal, double* log_power) {
    double real[SAMPLE_SIZE], imag[SAMPLE_SIZE];
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        real[i] = signal[i];
        imag[i] = 0.0;
    }

    fft(SAMPLE_SIZE, real, imag);

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        double power = real[i]*real[i] + imag[i]*imag[i];
        log_power[i] = log(power + 1e-10);  // log(0)防止
    }
}

// ユークリッド距離を計算
double euclidean_distance(double* a, double* b) {
    double sum = 0.0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

// メイン関数
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("使い方: %s 入力ファイル名\n", argv[0]);
        return 1;
    }

    // 各母音テンプレートの読み込みと特徴量抽出
    for (int i = 0; i < NUM_VOWELS; i++) {
        double signal[SAMPLE_SIZE];
        double log_power[SAMPLE_SIZE];
        read_raw(template_files[i], signal);
        compute_log_power_spectrum(signal, log_power);
        memcpy(templates[i], log_power, sizeof(double) * SAMPLE_SIZE);
    }

    // 入力音声の処理
    double input_signal[SAMPLE_SIZE];
    double input_log_power[SAMPLE_SIZE];
    read_raw(argv[1], input_signal);
    compute_log_power_spectrum(input_signal, input_log_power);

    // 各テンプレートとの距離を比較
    int recognized_index = 0;
    double min_distance = euclidean_distance(input_log_power, templates[0]);

    for (int i = 1; i < NUM_VOWELS; i++) {
        double dist = euclidean_distance(input_log_power, templates[i]);
        if (dist < min_distance) {
            min_distance = dist;
            recognized_index = i;
        }
    }

    printf("認識結果: /%s/ （ユークリッド距離: %.2f）\n",
           vowel_labels[recognized_index], min_distance);

    return 0;
}
