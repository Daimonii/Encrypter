all:
	gcc Encrypter.c -o encrypter -fsanitize="address"
