SRC_DIR := src
OUT_DIR := out
TEST_SRC := $(SRC_DIR)/test_all.c
TEST_OUT := $(OUT_DIR)/test_all.out

CC := gcc
CFLAGS := -g -I ../../../install/usr/local/include
LDFLAGS := -L ../../../install/usr/local/lib -lmetal

# Default target: build test_all.c
all: $(TEST_OUT)

# Rule to build test_all.c into out/test_all.out
$(TEST_OUT): $(TEST_SRC) | $(OUT_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Ensure output directory exists
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Clean build outputs
.PHONY: clean
clean:
	rm -rf $(OUT_DIR)

