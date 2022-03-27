#include<stdio.h>
int main() {
	FILE *fp = fopen("a.txt", "r+");
	while(1) {
		char c = fgetc(fp);
		if(c == 'h') {
			fputc('H', fp);
		}
		if(c == EOF) {
			break;
		}
	}
	fclose(fp);
	return 0;
}
