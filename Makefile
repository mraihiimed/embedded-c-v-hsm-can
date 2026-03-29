# ---------------------------------------------------------------------------
# Compiler settings
# ---------------------------------------------------------------------------
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude -Iinclude/attacks -MMD -MP
LDFLAGS = 

# ---------------------------------------------------------------------------
# Directory structure
# ---------------------------------------------------------------------------
CORE_DIR     = src/core
ECU_DIR      = src/ecu
ATTACK_DIR   = src/attacks
BUILD_DIR    = src/build
BIN_DIR      = build/bin

$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
CORE_SRC     = $(wildcard $(CORE_DIR)/*.c)
ECU_SRC      = $(wildcard $(ECU_DIR)/main_*.c)
ATTACK_SRC   = $(wildcard $(ATTACK_DIR)/*.c)

CORE_OBJ     = $(CORE_SRC:$(CORE_DIR)/%.c=$(BUILD_DIR)/%.o)
ECU_OBJ      = $(ECU_SRC:$(ECU_DIR)/%.c=$(BUILD_DIR)/%.o)
ATTACK_OBJ   = $(ATTACK_SRC:$(ATTACK_DIR)/%.c=$(BUILD_DIR)/%.o)

ECU_BINS     = $(ECU_SRC:$(ECU_DIR)/main_%.c=%)
ECU_BIN_PATHS = $(addprefix $(BIN_DIR)/,$(ECU_BINS))

# ---------------------------------------------------------------------------
# Default target
# ---------------------------------------------------------------------------
all: $(ECU_BIN_PATHS)

# ---------------------------------------------------------------------------
# Special case: monitor (ncurses)
# ---------------------------------------------------------------------------
$(BIN_DIR)/monitor: $(CORE_OBJ) $(BUILD_DIR)/main_monitor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lncurses

# ---------------------------------------------------------------------------
# Attacker ECU (links attack modules)
# ---------------------------------------------------------------------------
$(BIN_DIR)/attacker: $(CORE_OBJ) $(ATTACK_OBJ) $(BUILD_DIR)/main_attacker.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ---------------------------------------------------------------------------
# Generic ECU rule
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

$(BUILD_DIR)/%.o: $(ATTACK_DIR)/%.c
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
