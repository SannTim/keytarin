CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -MMD -MP
LIBS = -lncursesw -ltinfow
SRCS = main.cpp typing_trainer.cpp ui_manager.cpp text_generator.cpp stats_manager.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
TARGET = typing_trainer

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

.PHONY: clean
