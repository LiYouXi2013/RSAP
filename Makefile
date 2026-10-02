.PHONY: all clean run

SRCS := main.cpp $(shell powershell -Command "Get-ChildItem -Recurse src -Filter *.cpp | ForEach-Object { $$_.FullName }")

OBJS = $(addprefix build/, $(notdir $(SRCS:.cpp=.o)))

CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./include

LDFLAGS = -lfltk_images -lfltk_png -lfltk_z -lfltk_jpeg \
-lfltk_forms -lfltk -lcomctl32 -lws2_32 -lole32 -luuid -lgdi32 \
-lgdiplus -lwinspool -lcomdlg32 -lopengl32 -lglu32 -lwinmm \
-ladvapi32 -loleaut32 -lodbc32 -lkernel32 -luser32 \
-lwinhttp \
-static

all: $(OBJS)
	g++ -o build/main.exe $(OBJS) $(LDFLAGS)

run: all
	cd build && main.exe

build/%.o: %.cpp | build
	g++ $(CXXFLAGS) -c $< -o $@

build/%.o: src/%.cpp | build
	g++ $(CXXFLAGS) -c $< -o $@

build/%.o: src/dialogs/%.cpp | build
	g++ $(CXXFLAGS) -c $< -o $@

build:
	mkdir build

