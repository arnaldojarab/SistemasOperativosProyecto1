CC     = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude

# Main binary
SRC_MAIN = src/main.c src/truck.c src/scheduler.c src/logger.c
OUT_MAIN = terminal

# Test binary
SRC_TEST = src/test.c src/logger.c
OUT_TEST = test

all: $(OUT_MAIN)

$(OUT_MAIN): $(SRC_MAIN)
	$(CC) $(CFLAGS) $(SRC_MAIN) -o $(OUT_MAIN)

test: $(SRC_TEST)
	$(CC) $(CFLAGS) $(SRC_TEST) -o $(OUT_TEST)

clean:
	rm -f $(OUT_MAIN) $(OUT_TEST)