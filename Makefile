# ---------------------------------------------------------------------------
# Compiler settings
# ---------------------------------------------------------------------------
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude -Iinclude/attacks -I/usr/include/ncursesw -Iunity -MMD -MP
LDFLAGS =


# ---------------------------------------------------------------------------
# Test compilation flags
# ---------------------------------------------------------------------------
TEST_DEFS = -DUNIT_TEST

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
# ECU build rules
# ---------------------------------------------------------------------------
$(BIN_DIR)/monitor: $(CORE_OBJ) $(BUILD_DIR)/main_monitor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lncurses

$(BIN_DIR)/attacker: $(CORE_OBJ) $(ATTACK_OBJ) $(BUILD_DIR)/main_attacker.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

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

-include $(BUILD_DIR)/*.d

# ---------------------------------------------------------------------------
# Cleanup
# ---------------------------------------------------------------------------
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	rm -rf $(BIN_DIR)
	rm -rf build/tests

.PHONY: all clean test


# ---------------------------------------------------------------------------
# Static Test Configuration
# ---------------------------------------------------------------------------
CPPCHECK = cppcheck
CPPCHECK_FLAGS = --enable=all --inconclusive --force --xml --xml-version=2 \
                 -I include \
                 --suppressions-list=tools/cppcheck/suppressions.txt \
                 --platform=tools/cppcheck/platform.rules

cppcheck:
	mkdir -p reports
	$(CPPCHECK) $(CPPCHECK_FLAGS) src 2> reports/cppcheck.xml

# ---------------------------------------------------------------------------
# Unit Test Configuration
# ---------------------------------------------------------------------------
TEST_DIR     = tests
UNITY_DIR    = unity
TEST_BUILD   = build/tests

$(shell mkdir -p $(TEST_BUILD))

TEST_BINS = \
    $(TEST_BUILD)/test_bus \
    $(TEST_BUILD)/test_ecu \
    $(TEST_BUILD)/test_hsm \
    $(TEST_BUILD)/test_ids \
    $(TEST_BUILD)/test_security_checks \
    $(TEST_BUILD)/test_protected_memory \
    $(TEST_BUILD)/test_util \
    $(TEST_BUILD)/test_network

# ---------------------------------------------------------------------------
# Explicit test build rules (NO PATTERN RULES)
# ---------------------------------------------------------------------------

$(TEST_BUILD)/test_bus: \
    tests/test_bus.c \
    tests/mock_network.c \
    tests/mock_log.c \
    src/core/bus.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
$(TEST_BUILD)/test_ecu: \
    tests/test_ecu.c \
    tests/mock_network.c \
    tests/mock_hsm.c \
    tests/mock_ids.c \
    tests/mock_log.c \
    src/core/ecu.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
$(TEST_BUILD)/test_protected_memory: \
    tests/test_protected_memory.c \
    tests/mock_log.c \
    tests/mock_sha256.c \
    src/core/protected_memory.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
$(TEST_BUILD)/test_util: \
    tests/test_util.c \
    tests/mock_util_lowlevel.c \
    tests/mock_log.c \
    src/core/util.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
$(TEST_BUILD)/test_hsm: \
    tests/test_hsm.c \
    tests/mock_log.c \
    src/core/hsm.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
$(TEST_BUILD)/test_ids: \
    tests/test_ids.c \
    src/core/ids.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
$(TEST_BUILD)/test_security_checks: \
    tests/test_main_security_checks.c \
    tests/mock_hsm_verify.c \
    src/core/main_security_checks.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
$(TEST_BUILD)/test_network: \
    tests/test_network.c \
    tests/mock_net_lowlevel.c \
    tests/mock_log.c \
    src/core/network.c \
    unity/unity.c
#	$(CC) $(CFLAGS) -o $@ $^
	$(CC) $(CFLAGS) $(TEST_DEFS) -o $@ $^
# ---------------------------------------------------------------------------
# Run all tests
# ---------------------------------------------------------------------------
#test: $(TEST_BINS)
#	@echo "Running unit tests..."
#	@for t in $(TEST_BINS); do echo "==> $$t"; $$t || exit 1; done
#	@echo "All tests passed."
test: $(TEST_BINS)
#	@set -e; \
	echo "Running unit tests..."; \
	for t in $(TEST_BINS); do \
		echo "==> $$t"; \
		$$t; \
	done; \
	echo "All tests passed."
