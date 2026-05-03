# Compilador
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -pthread -Iinclude

# Archivos
SRC = src/test.c src/logger.c
OUT = test

# Regla principal
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

# Limpiar binarios
clean:
	rm -f $(OUT)