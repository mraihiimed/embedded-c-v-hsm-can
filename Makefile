# ---------------------------------------------------------------------------
# Compiler settings
# ---------------------------------------------------------------------------
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude -MMD -MP
LDFLAGS = 

# ---------------------------------------------------------------------------
# Directory structure
# ---------------------------------------------------------------------------
CORE_DIR     = src/core
ECU_DIR      = src/ecu
BUILD_DIR    = src/build
BIN_DIR      = build/bin

# Ensure directories exist
$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
CORE_SRC = $(wildcard $(CORE_DIR)/*.c)
ECU_SRC  = $(wildcard $(ECU_DIR)/main_*.c)

CORE_OBJ = $(CORE_SRC:$(CORE_DIR)/%.c=$(BUILD_DIR)/%.o)
ECU_OBJ  = $(ECU_SRC:$(ECU_DIR)/%.c=$(BUILD_DIR)/%.o)

# ECU binary names (main_xxx.c → xxx)
ECU_BINS = $(ECU_SRC:$(ECU_DIR)/main_%.c=%)
ECU_BIN_PATHS = $(addprefix $(BIN_DIR)/,$(ECU_BINS))

# ---------------------------------------------------------------------------
# Default target
# ---------------------------------------------------------------------------
all: $(ECU_BIN_PATHS)

# ---------------------------------------------------------------------------
# Build rule for monitor (special case: needs ncurses)
# ---------------------------------------------------------------------------
$(BIN_DIR)/monitor: $(CORE_OBJ) $(BUILD_DIR)/main_monitor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lncurses

# ---------------------------------------------------------------------------
# Generic build rule for all other ECUs
# ---------------------------------------------------------------------------
$(BIN_DIR)/%: $(CORE_OBJ) $(BUILD_DIR)/main_%.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ---------------------------------------------------------------------------
# Compilation rules
# ---------------------------------------------------------------------------
$(BUILD_DIR)/%.o: $(CORE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(ECU_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---------------------------------------------------------------------------
# Auto-generated dependencies
# ---------------------------------------------------------------------------
-include $(BUILD_DIR)/*.d

# ---------------------------------------------------------------------------
# Cleanup
# ---------------------------------------------------------------------------
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	rm -rf $(BIN_DIR)

.PHONY: all clean
