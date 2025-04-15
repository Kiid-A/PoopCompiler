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

# 可执行文件名称
EXECUTABLE = ll1_test

# 源文件和目标文件
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(SRC_DIR)/%.o, $(SRC_FILES))
TEST_FILE = $(TEST_DIR)/ll1_test.cpp
TEST_OBJ = $(TEST_DIR)/ll1_test.o

# 默认目标，生成发布版本
all: $(EXECUTABLE)

# 链接生成可执行文件
$(EXECUTABLE): $(OBJ_FILES) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -o $@ $^

# 编译源文件
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c -o $@ $<

# 编译测试文件
$(TEST_OBJ): $(TEST_FILE)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c -o $@ $<

# 调试目标
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(EXECUTABLE)

# 清理目标
clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(EXECUTABLE)