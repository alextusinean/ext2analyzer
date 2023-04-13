#include <ext2fs/ext2fs.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define EXT2_IMG_FILE "/home/alextusinean/Desktop/mikrofresh.1.img"

ImFont* ImGui_AddDefaultFont(float pixel_size)
{
    ImFontConfig config;
    config.SizePixels = pixel_size;
    config.OversampleH = config.OversampleV = 1;
    config.PixelSnapH = false;

    return ImGui::GetIO().Fonts->AddFontDefault(&config);
}

int main(void)
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(1366, 768, "ext2 Analyzer", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_AddDefaultFont(0);
    ImFont* zfont = ImGui_AddDefaultFont(1);


    ext2_filsys fs;
	errcode_t error = ext2fs_open(EXT2_IMG_FILE, 0, 0, 4096, unix_io_manager, &fs);
	if (error) {
		printf("open error %ld\n", error);
		return 1;
	}

	blk64_t blockCount = ext2fs_blocks_count(fs->orig_super);
    size_t blockBitmapSize = ceil(blockCount / 8);
	char* blockBitmap = (char*) calloc(blockBitmapSize, sizeof(char));

	error = ext2fs_read_block_bitmap(fs);
	if (error) {
		printf("read_block_bitmap error %ld\n", error);
		return 1;
	}

	error = ext2fs_get_block_bitmap_range(fs->block_map, ext2fs_get_block_bitmap_start(fs->block_map), blockCount, blockBitmap);
	if (error) {
		printf("get_block_bitmap_range error %ld\n", error);
		return 1;
	}

	char* realBlockBitmap = (char*) calloc(blockBitmapSize, sizeof(char));
    std::ifstream infile;
    infile.open(EXT2_IMG_FILE, std::ios_base::binary);
    if (!infile) {
		printf("failed opening fs\n");
		return 1;
    }

    size_t blockIndex = 0;
    size_t index = 0;
    while (true) {
        if (index >= 4096) {
            blockIndex++;
            index = 0;
        }

        unsigned char byte;
        infile >> byte;
        if (infile.eof())
            break;

        index++;
        if (byte != 0) {
            size_t realBlockBitmapIndex = floor(blockIndex / 8);
            realBlockBitmap[realBlockBitmapIndex] |= 1UL << (blockIndex % 8);

            infile.seekg(4096 - index - 1, std::ios::cur);
            index = 4096;
        }
    }

    infile.close();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("blocks");
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
            ImGui::PushFont(zfont);

            int squareSize = std::floor(std::sqrt(blockCount));

            for (int y = 0; y < squareSize + 2; y++)
            {
                ImGui::NewLine();
                for (int x = 0; x < squareSize; x++)
                {
                    blk64_t index = y * squareSize + x;
                    if (index >= blockCount)
                        break;

                    size_t blockBitmapIndex = floor(index / 8);
                    bool isUsedReal = (realBlockBitmap[blockBitmapIndex] >> (index % 8)) & 1U;
                    bool isUsed = (blockBitmap[blockBitmapIndex] >> (index % 8)) & 1U;
                    if (isUsedReal && isUsed) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.0f, 0.0f, 1.0f));
                    }

                    if (isUsedReal && !isUsed) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.95f, 0.0f, 1.0f));
                    }

                    if (!isUsedReal && isUsed) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.75f, 0.75f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.70f, 0.70f, 1.0f));
                    }
                    
                    if (!isUsedReal && !isUsed) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
                    }

                    std::stringstream ss;
                    ss << "##foo" << index;
                    ImGui::SameLine(); ImGui::Button(ss.str().c_str());
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    {
                        ImGui::PopFont();
                        ImGui::SetTooltip("block #%lld", index);
                        ImGui::PushFont(zfont);
                    }

                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                }
            }

            ImGui::PopFont();
            ImGui::PopStyleVar();
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    free(blockBitmap);
    ext2fs_close(fs);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}