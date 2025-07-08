#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define SAMPLING_RATE 16000 // 16 kHz
#define AMPLIFY3 3
#define AMPLIFY30 30

void save_text_data(const short *wave, int samples, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        perror("fopen for text output");
        exit(1);
    }

    for (int i = 0; i < samples; i++)
    {
        double time = (double)i*1000 / SAMPLING_RATE;
        fprintf(fp, "%f\t%d\n", time, wave[i]);
    }

    fclose(fp);
}

void save_raw_data(const short *wave, int samples, const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        perror("fopen for raw output");
        exit(1);
    }

    fwrite(wave, sizeof(short), samples, fp);
    fclose(fp);
}

short saturate(int value)
{
    if (value > 32767)
        return 32767;
    if (value < -32768)
        return -32768;
    return (short)value;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("使い方: %s 入力.raw 出力.txt 出力3倍.raw 出力30倍.raw\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *text_file = argv[2];
    const char *loud3_file = argv[3];
    const char *loud30_file = argv[4];
    char text3_file[256], text30_file[256];
    snprintf(text3_file, sizeof(text3_file), "%s.txt", loud3_file);
    snprintf(text30_file, sizeof(text30_file), "%s.txt", loud30_file);

    // ファイルサイズ取得
    struct stat st;
    if (stat(input_file, &st) != 0)
    {
        perror("stat");
        return 1;
    }

    int file_size = st.st_size;
    int num_samples = file_size / sizeof(short);

    short *wave = (short *)calloc(num_samples, sizeof(short));
    short *wave3 = (short *)calloc(num_samples, sizeof(short));
    short *wave30 = (short *)calloc(num_samples, sizeof(short));

    if (!wave || !wave3 || !wave30)
    {
        fprintf(stderr, "メモリ確保に失敗しました\n");
        return 1;
    }

    // データ読み込み
    FILE *fp = fopen(input_file, "rb");
    if (!fp)
    {
        perror("fopen for input");
        return 1;
    }

    fread(wave, sizeof(short), num_samples, fp);
    fclose(fp);

    // 3倍 & 30倍振幅波形作成
    for (int i = 0; i < num_samples; i++)
    {
        wave3[i] = saturate(wave[i] * AMPLIFY3);
        wave30[i] = saturate(wave[i] * AMPLIFY30);
    }

    // 出力
    save_text_data(wave, num_samples, text_file);
    save_text_data(wave3, num_samples, text3_file);
    save_text_data(wave30, num_samples, text30_file);
    save_raw_data(wave3, num_samples, loud3_file);
    save_raw_data(wave30, num_samples, loud30_file);

    free(wave);
    free(wave3);
    free(wave30);

    printf("処理が完了しました。\n");
    return 0;
}
