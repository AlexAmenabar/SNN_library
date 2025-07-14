#include <stdio.h>

int main() {
    FILE *infile = fopen("optdigits.tra", "r");
    FILE *outfile = fopen("optdigits_train.txt", "w");

    if (!infile || !outfile) {
        perror("File open error");
        return 1;
    }

    int ch;
    while ((ch = fgetc(infile)) != EOF) {
        if (ch != ',') {
            fputc(ch, outfile);
        }
    }

    fclose(infile);
    fclose(outfile);
    return 0;
}