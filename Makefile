all: ext2analyzer

ext2analyzer: ext2analyzer.o imgui_draw.o imgui_impl_glfw.o imgui_impl_opengl3.o imgui_tables.o imgui_widgets.o imgui.o
	g++ `pkg-config --cflags glfw3` -o ext2analyzer ext2analyzer.o \
	imgui_draw.o imgui_impl_glfw.o imgui_impl_opengl3.o imgui_tables.o \
	imgui_widgets.o imgui.o -lext2fs -lGL `pkg-config --static --libs glfw3`

ext2analyzer.o: ext2analyzer.cpp imgui.h imgui_impl_glfw.h imgui_impl_opengl3.h
	g++ -c ext2analyzer.cpp

imgui_draw.o: imgui_draw.cpp imgui.h imgui_internal.h imstb_rectpack.h imstb_truetype.h
	g++ -c imgui_draw.cpp

imgui_impl_glfw.o: imgui_impl_glfw.cpp imgui.h imgui_impl_glfw.h
	g++ -c imgui_impl_glfw.cpp

imgui_impl_opengl3.o: imgui_impl_opengl3.cpp imgui.h imgui_impl_opengl3.h
	g++ -c imgui_impl_opengl3.cpp

imgui_tables.o: imgui_tables.cpp imgui.h imgui_internal.h
	g++ -c imgui_tables.cpp

imgui_widgets.o: imgui_widgets.cpp imgui.h imgui_internal.h imstb_textedit.h
	g++ -c imgui_widgets.cpp

imgui.o: imgui.cpp imgui.h imgui_internal.h
	g++ -c imgui.cpp
