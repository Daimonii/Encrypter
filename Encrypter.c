#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

void gamma_generator()
{
	srand(time(NULL));
	while (1) {
		char c = rand() % 256;
		putchar(c);
	}
}

void error(int n)
{
	printf("error - %d\n", n);
	exit(1);
}

void main(int argc, char *argv[])
{
	int key_pipes[2];
	int file_pipes[2];
	char key;
	char data;
	char result = 0;
	size_t n1, n2;
	FILE *key_file;
	FILE *result_file = fopen(argv[3], "w");
	
	if (strcmp(argv[1], "encrypt") == 0)
		key_file = fopen("key", "w");

	if (pipe(key_pipes) != 0)
		error(1);

	if (pipe(file_pipes) != 0)
		error(2);

	switch (fork())
	{
		case -1:
			error(3);
		case 0:
			close(1);
			dup(key_pipes[1]);
			close(key_pipes[0]);
			close(key_pipes[1]);
			close(file_pipes[0]);
			close(file_pipes[1]);
			
			if (strcmp(argv[1], "encrypt") == 0)
				gamma_generator();
			else if (strcmp(argv[1], "decrypt") == 0)
				execlp("cat", "cat", "key", NULL);
		default:
			switch(fork())
			{
				case -1:
					error(4);
				case 0:
					close(1);
					dup(file_pipes[1]);
					close(file_pipes[0]);
					close(file_pipes[1]);
					close(key_pipes[0]);
					close(key_pipes[1]);
					execlp("cat", "cat", argv[2], NULL);
				default:
					close(file_pipes[1]);
					close(key_pipes[1]);
					while ((n2 = read(file_pipes[0], &data, 1)) > 0) {
						n1 = read(key_pipes[0], &key, 1);
						result = data ^ key;
						fputc(result, result_file);
						if (strcmp(argv[1], "encrypt") == 0)
							fputc(key, key_file);
					}
					if (strcmp(argv[1], "encrypt") == 0) {
						fclose(key_file);
					}
					fclose(result_file);
			}
	}
}
