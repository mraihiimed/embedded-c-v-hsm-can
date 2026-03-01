CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude -MMD -MP
LDFLAGS = 

# ---------------------------------------------------------------------------
# Common source files shared by all ECUs
# ---------------------------------------------------------------------------
SRC_COMMON = \
    src/bus.c \
    src/network.c \
    src/hsm.c \
    src/ecu.c \
    src/ids.c \
    src/util.c \
    src/protected_memory.c

OBJ_COMMON = $(SRC_COMMON:.c=.o)

# ---------------------------------------------------------------------------
# ECU binaries
# ---------------------------------------------------------------------------
ECU_BINS = \
    bus_server \
    monitor \
    wheel_fl \
    wheel_fr \
    wheel_rl \
    wheel_rr \
    steering_sensor \
    steering_controller \
    brake_controller \
    autonomous_controller \
    engine_ecu \


# ---------------------------------------------------------------------------
# Build rules
# ---------------------------------------------------------------------------
all: $(ECU_BINS)

bus_server: $(OBJ_COMMON) src/main_bus_server.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

monitor: $(OBJ_COMMON) src/main_monitor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lncurses

wheel_fl: $(OBJ_COMMON) src/main_wheel_fl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

wheel_fr: $(OBJ_COMMON) src/main_wheel_fr.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

wheel_rl: $(OBJ_COMMON) src/main_wheel_rl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

wheel_rr: $(OBJ_COMMON) src/main_wheel_rr.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

steering_sensor: $(OBJ_COMMON) src/main_steering_sensor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

steering_controller: $(OBJ_COMMON) src/main_steering_controller.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

brake_controller: $(OBJ_COMMON) src/main_brake_controller.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

autonomous_controller: $(OBJ_COMMON) src/main_autonomous_controller.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

engine_ecu: $(OBJ_COMMON) src/main_engine_ecu.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ---------------------------------------------------------------------------
# Generic compilation rule for src/*.c
# ---------------------------------------------------------------------------
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---------------------------------------------------------------------------
# Include auto-generated dependency files
# ---------------------------------------------------------------------------
-include $(OBJ_COMMON:.o=.d)
-include $(wildcard src/*.d)

# ---------------------------------------------------------------------------
# Cleanup
# ---------------------------------------------------------------------------
clean:
	rm -f src/*.o src/*.d $(OBJ_COMMON) $(OBJ_COMMON:.o=.d) $(ECU_BINS)

.PHONY: all clean

