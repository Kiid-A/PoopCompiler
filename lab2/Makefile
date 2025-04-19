# 编译器和编译选项
CXX = g++
# 通用编译选项
CXXFLAGS = -Wall -Wextra
# 调试编译选项
DEBUG_FLAGS = -g -O0
# 发布编译选项
RELEASE_FLAGS = -O2

# 源文件和头文件路径
SRC_DIR = src
TEST_DIR = test

# 可执行文件名称前缀
EXECUTABLE_PREFIX = ""

# 源文件和目标文件
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(SRC_DIR)/%.o, $(SRC_FILES))

# 测试文件和目标文件
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_EXECUTABLES = $(patsubst $(TEST_DIR)/%.cpp, $(EXECUTABLE_PREFIX)%, $(TEST_FILES))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(TEST_DIR)/%.o, $(TEST_FILES))

# 自动生成的依赖文件
DEPS = $(patsubst $(SRC_DIR)/%.cpp, $(SRC_DIR)/%.d, $(SRC_FILES)) \
       $(patsubst $(TEST_DIR)/%.cpp, $(TEST_DIR)/%.d, $(TEST_FILES))

# 默认目标，生成发布版本
all: $(TEST_EXECUTABLES)

# 链接生成可执行文件
$(EXECUTABLE_PREFIX)%: $(TEST_DIR)/%.o $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -o $@ $^

# 编译源文件
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -MMD -MP -c -o $@ $<

# 编译测试文件
$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -MMD -MP -c -o $@ $<

# 调试目标
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TEST_EXECUTABLES)

# 清理目标
clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(TEST_EXECUTABLES) $(DEPS)

# 包含自动生成的依赖文件
-include $(DEPS)