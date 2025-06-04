#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SAMPLING_RATE 16000 // 16 kHz
#define CENTER_DURATION_MS 20
#define FILE_COUNT 5

const char *output_files[FILE_COUNT] = {
    "a00_center.txt", "i00_center.txt", "u00_center.txt", "e00_center.txt", "o00_center.txt"
};

void extract_and_save_center_20ms(const char *infile, const char *outfile)
{
    struct stat st;
    if (stat(infile, &st) != 0)
    {
        fprintf(stderr, "ファイル %s が見つかりません。\n", infile);
        return;
    }

    if (st.st_size < 2)
    {
        fprintf(stderr, "ファイル %s のサイズが小さすぎます。\n", infile);
        return;
    }

    int num_samples = st.st_size / sizeof(short);
    int window_samples = SAMPLING_RATE * CENTER_DURATION_MS / 1000;

    int start;
    if (num_samples % 2 == 0)
    {
        start = num_samples / 2 - window_samples / 2;
    }
    else
    {
        start = (num_samples + 1) / 2 - window_samples / 2;
    }

    if (start < 0 || (start + window_samples) > num_samples)
    {
        fprintf(stderr, "%s の中央20msが範囲外です。\n", infile);
        return;
    }

    short *wave = (short *)malloc(sizeof(short) * window_samples);
    if (!wave)
    {
        fprintf(stderr, "メモリ確保失敗（%s）\n", infile);
        return;
    }

    FILE *fp = fopen(infile, "rb");
    if (!fp)
    {
        perror("fopen");
        free(wave);
        return;
    }

    // バイト位置 M = start * 2
    if (fseek(fp, start * sizeof(short), SEEK_SET) != 0)
    {
        perror("fseek失敗");
        fclose(fp);
        free(wave);
        return;
    }

    size_t read_count = fread(wave, sizeof(short), window_samples, fp);
    fclose(fp);

    if (read_count != window_samples)
    {
        fprintf(stderr, "サンプル読み込み数が不足しました。\n");
        free(wave);
        return;
    }

    FILE *out = fopen(outfile, "w");
    if (!out)
    {
        perror("fopen (output)");
        free(wave);
        return;
    }

    for (int i = 0; i < window_samples; i++)
    {
        double time_ms = (double)(start + i) * 1000.0 / SAMPLING_RATE;
        fprintf(out, "%.3f\t%d\n", time_ms, wave[i]);
    }

    fclose(out);
    free(wave);

    printf("%s → %s に保存しました（開始時刻 %.3f ms）\n",
           infile, outfile, (int)start * 1000.0 / SAMPLING_RATE);
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        fprintf(stderr, "使い方: %s a00.raw i00.raw u00.raw e00.raw o00.raw\n", argv[0]);
        return 1;
    }

    for (int i = 0; i < FILE_COUNT; i++)
    {
        extract_and_save_center_20ms(argv[i + 1], output_files[i]);
    }

    printf("すべての処理が完了しました。\n");
    return 0;
}
