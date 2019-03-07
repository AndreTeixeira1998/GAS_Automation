TARGET_EXEC ?= GAS.out

CC = clang

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./lib/*

<<<<<<< HEAD
<<<<<<< HEAD
SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name *.cpp -or -name *.c ! -name test.c -or -name *.s)
=======
SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name *.cpp -or -name *.c -or -name *.s)
>>>>>>> Read and obtain a minified JSON string from a file
=======
SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name *.cpp -or -name *.c ! -name test.c -or -name *.s)
>>>>>>> Import configuration from JSON file
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

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
<<<<<<< HEAD
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -Wall -Wextra -g
=======
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	# -g
>>>>>>> Import configuration from JSON file

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p