.PHONY: all clean

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix build/, $(SRCS:.cpp=.o))
LDFLAGS = -lfltk_images -lfltk_png -lfltk_z -lfltk_jpeg \
-lfltk_forms -lfltk -lcomctl32 -lwsock32 -lole32 -luuid -lgdi32 \
-lgdiplus -lwinspool -lcomdlg32 -lopengl32 -lglu32 -lwinmm \
-ladvapi32 -loleaut32 -lodbc32 -lkernel32 -luser32 \
-static

all: $(OBJS)
	g++ -o build/main.exe $(OBJS) $(LDFLAGS) -mwindows

build/%.o: %.cpp | build
	g++ -c $< -o $@ -std=c++14 -Wall -Wextra -O2  

build:
	mkdir build

clean:
	rm -rf build