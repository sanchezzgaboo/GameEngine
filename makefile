CXX = g++
CXXFLAGS = -I ~/SFML-3.0.2/include -L ~/SFML-3.0.2/lib -lsfml-graphics -lsfml-window -lsfml-system -lGL

# Object files to compile
OBJECTS = build/imgui.o build/imgui_widgets.o build/imgui_draw.o build/imgui_tables.o build/imgui_sfml.o
TARGET = sfml-app-test

# The main target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): main.cpp $(OBJECTS)
	$(CXX) main.cpp $(OBJECTS) -o $@ $(CXXFLAGS)

# Compile imgui source files into object files in the build directory
build/imgui.o: ./imgui/imgui.cpp
	$(CXX) -c $< -o $@ -I ~/SFML-3.0.2/include

build/imgui_widgets.o: ./imgui/imgui_widgets.cpp
	$(CXX) -c $< -o $@ -I ~/SFML-3.0.2/include

build/imgui_draw.o: ./imgui/imgui_draw.cpp
	$(CXX) -c $< -o $@ -I ~/SFML-3.0.2/include

build/imgui_tables.o: ./imgui/imgui_tables.cpp
	$(CXX) -c $< -o $@ -I ~/SFML-3.0.2/include

build/imgui_sfml.o: ./imgui/imgui-SFML.cpp
	$(CXX) -c $< -o $@ -I ~/SFML-3.0.2/include

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJECTS)
