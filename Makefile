TARGET_EXEC ?= GAS.out

CC = clang

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./lib/*

SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name *.cpp -or -name *.c ! -name test.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


INC_DIRS := $(shell find $(SRC_DIRS) -type d)
#For libpq
LIBPQ_INCLUDE_DIR = $(shell pg_config --includedir)
INC_DIRS += $(LIBPQ_INCLUDE_DIR)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
LDFLAGS ?= -pthread -lpq


#$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -Wall -Wextra -g

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
.PHONY: run

clean:
	$(RM) -r $(BUILD_DIR)

run:
	./run.sh

-include $(DEPS)

MKDIR_P ?= mkdir -p