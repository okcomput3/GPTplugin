/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Andrew Pliatsikas
 * Copyright (c) 2023 Scott Moreau
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
int xx=0;
#include <map>
#include <fstream> 
#include <wayfire/geometry.hpp>
#include <wayfire/workarea.hpp>
#include <wayfire/opengl.hpp>
#include <wayfire/region.hpp>
#include <wayfire/scene-operations.hpp>
#include <wayfire/signal-provider.hpp>
#include <wayfire/scene.hpp>
#include <wayfire/scene-render.hpp>
#include <wayfire/util.hpp>
#include <wayfire/plugin.hpp>
#include <wayfire/output.hpp>
#include <wayfire/output-layout.hpp>
#include <wayfire/util/duration.hpp>
#include <wayfire/render-manager.hpp>
#include <wayfire/workspace-stream.hpp>
#include <wayfire/workspace-set.hpp>
#include <wayfire/per-output-plugin.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/plugins/common/cairo-util.hpp>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <vector>
#include "GPTscale.hpp"
#include <pthread.h> 
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include "lex.hpp"
#include <linux/input-event-codes.h>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdio>  // For popen, pclose
#include <cstdlib> // For system (optional, if you're using it)
#include <cstring> // For strlen (optional, if you're using it)
#include <string>  // For std::string
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>
#include <gtk/gtk.h>
#include <iostream>
#include <set>
#include <chrono>
#include <thread>
#include <atomic>
#include <png.h>
#include <pango/pangocairo.h>
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkPath.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkStream.h"
#define SK_GANESH
#define SK_GL
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/gl/GrGLAssembleInterface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/gl/egl/GrGLMakeEGLInterface.h"
#include "include/core/SkColorSpace.h"
#include "include/gpu/GrDirectContext.h"
using namespace skgpu::ganesh;
#include "include/gpu/ganesh/SurfaceDrawContext.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
//#include "include/gpu/ganesh/GrGpuResourceRef.h" // For skgpu::RefCntedCallback
#include "src/gpu/ganesh/GrColorSpaceXform.h" // For SkColorTypeToGrColorType, may need to adjust include path based on your setup
#include "include/gpu/gl/egl/GrGLMakeEGLInterface.h"
#include <GL/gl.h> // Make sure you include the appropriate OpenGL headers
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
//#ifdef SK_GL
#include "include/core/SkBitmap.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkScalar.h"
#include "include/core/SkShader.h"
#include "include/core/SkSize.h"
#include "include/core/SkString.h"
#include "include/core/SkTileMode.h"
#include "include/core/SkTypes.h"
#include "include/effects/SkGradientShader.h"
#include "include/gpu/GrTypes.h"
#include "include/gpu/ganesh/SkImageGanesh.h"
#include "src/core/SkAutoPixmapStorage.h"
#include "src/gpu/ganesh/GrDirectContextPriv.h"
#include "src/gpu/ganesh/GrGpu.h"
#include "src/gpu/ganesh/gl/GrGLCaps.h"
#include "src/gpu/ganesh/gl/GrGLDefines.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkFont.h"
#include "include/gpu/gl/GrGLAssembleInterface.h"

#include <EGL/egl.h> 

auto mipmapped = skgpu::Mipmapped::kNo;
namespace wf
{
struct simple_texture_t;
}
std::unique_ptr<SkSurface> skSurface;
GLuint textureID;
sk_sp<SkSurface> surface ;
SkImageInfo imageInfo ;


GrDirectContext* sContext = nullptr;
SkSurface* sSurface = nullptr;

void setup_shader(GLuint *program, const std::string& vertexSource, const std::string& fragmentSource) {
    auto vertexShader = OpenGL::compile_shader(vertexSource.c_str(), GL_VERTEX_SHADER);
    auto fragmentShader = OpenGL::compile_shader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);
    auto shaderProgram = GL_CALL(glCreateProgram());

    // Check if vertex shader compilation succeeded
    GLint compileSuccess = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        char log[1024];
        glGetShaderInfoLog(vertexShader, sizeof(log), NULL, log);
        std::cerr << "Vertex shader compilation failed: " << log << std::endl;
        return; // Exit the function if compilation fails
    }

    // Check if fragment shader compilation succeeded
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        char log[1024];
        glGetShaderInfoLog(fragmentShader, sizeof(log), NULL, log);
        std::cerr << "Fragment shader compilation failed: " << log << std::endl;
        return; // Exit the function if compilation fails
    }

    // Attach shaders to the shader program
    GL_CALL(glAttachShader(shaderProgram, vertexShader));
    GL_CALL(glAttachShader(shaderProgram, fragmentShader));
    
    // Link the shader program
    GL_CALL(glLinkProgram(shaderProgram));

    // Check if linking succeeded
    GLint linkSuccess = GL_FALSE;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        char log[1024];
        glGetProgramInfoLog(shaderProgram, sizeof(log), NULL, log);
        std::cerr << "Shader linking failed: " << log << std::endl;
        return; // Exit the function if linking fails
    }

    // Delete the shader objects (cleanup)
    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));

    *program = shaderProgram;
}
    
GLuint shaderProgram;
void initializeShader() {
    printf("101\n");
    std::string vertexShaderSource = R"glsl(
   #version 320 es

in vec2 position;  // Input position attribute
in vec2 texCoord;  // Input texture coordinate attribute

out vec2 TexCoord; // Output texture coordinate

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0); // Output position
    TexCoord = texCoord; // Pass through texture coordinate
}
    )glsl";
    printf("201\n");
    std::string fragmentShaderSource = R"glsl(
        #version 320 es
precision mediump float;

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}

    )glsl";
    printf("301\n");
    setup_shader(&shaderProgram, vertexShaderSource, fragmentShaderSource);
    printf("401\n");
}


using json = nlohmann::json;
  pthread_t thread_id;

const bool DEBUG_MODE =FALSE;
const bool DEBUG_MODE_BLOCK = false;
const bool DEBUG_WIDTH = false;

bool LOAD_MODE = false;
double minWindowSize = 200;

//glow round syntax windows
 bool glow_title=false;
 bool glow_window=true;
 bool glow_circle=true;
int glow_layers = 5; // Number of layers to simulate the glow
double glow_spread = 1.0; // How much each layer is larger than the previous one
double initial_alpha = 0.05; // Starting opacity for the outermost layer




bool needsUpdate = true;
bool start_up=true;
std::string runId ;
std::string threadId;
std::string request_text="";

bool resetloading=false;
std::vector<wf::geometry_t> textLineBoxes;

using json = nlohmann::json;

std::string response;
std::vector<std::string> lines;
std::vector<std::string> loadings;
bool GPTrequestFinished=false;
bool GPTrequestLoading=false;
bool in_c_syntax_block = false;
bool in_syntax_block = false;
double pointer_posX;
double pointer_posY;
double VisibleTextHeight = 0.0; // Total Visible height required for the text


double initialMouseY = 0.0; // Y position where the drag started

double selectrectX = 0;
double selectrectY = 0;
double selectrectWidth = 0;
double selectrectHeight = 0;
double Yscroller=0;

int MaxTextureHeight ;
double previous_scroll_offset;

std::chrono::time_point<std::chrono::steady_clock> start_time ;
wf::framebuffer_t  target_fb;


struct TextExtentsCompat {
    double width;
    double height;
    double x_advance;
    double y_advance;
    double x_bearing;
    double y_bearing;

    // Constructor for easy initialization
    TextExtentsCompat(double w = 0.0, double h = 0.0, double xAdv = 0.0, double yAdv = 0.0, double xBear = 0.0, double yBear = 0.0)
        : width(w), height(h), x_advance(xAdv), y_advance(yAdv), x_bearing(xBear), y_bearing(yBear) {}
};

TextExtentsCompat getTextExtentsFromPango(PangoLayout* layout) {
    PangoRectangle inkRect, logicalRect;
    pango_layout_get_extents(layout, &inkRect, &logicalRect);

    // Convert Pango units to points (device units)
    double width = static_cast<double>(logicalRect.width) / PANGO_SCALE;
    double height = static_cast<double>(logicalRect.height) / PANGO_SCALE;

    // x_advance can be considered as the width of the logical rectangle
    double x_advance = width;
    // y_advance doesn't have a direct equivalent in Pango but you can set a custom logic
    double y_advance = height;

    // Ink rectangle gives the actual bounds of the text, which can be used for bearing calculations
    double x_bearing = static_cast<double>(inkRect.x) / PANGO_SCALE;
    double y_bearing = static_cast<double>(inkRect.y) / PANGO_SCALE;

    return TextExtentsCompat(width, height, x_advance, y_advance, x_bearing, y_bearing);
}

// Wrapper function that mimics cairo_text_extents but uses Pango and your TextExtentsCompat
void pango_text_extents(cairo_t *cr, const char *text, TextExtentsCompat *extents) {
    // Create a Pango layout for the current context and text
    PangoLayout *layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, text, -1);

    // Use your function to get text dimensions
    *extents = getTextExtentsFromPango(layout);

    // Clean up
    g_object_unref(layout);
}



// Function to render text using Pango and measure its extents
void pango_show_text(cairo_t *cr, const char* text) {
    // Ensure a valid Cairo context and text
    if (cr == nullptr || text == nullptr) return;

    // Create a PangoLayout for this context
    PangoLayout *layout = pango_cairo_create_layout(cr);

    // Set the text into the layout
    pango_layout_set_text(layout, text, -1);

    // Render the PangoLayout on the Cairo context
    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    // Clean up
    g_object_unref(layout);
}

void writePNG(const SkPixmap& pixmap, const char* filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return; // Handle error

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) return; // Handle error

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return; // Handle error
    }

    // Error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return; // Handle error
    }

    png_init_io(png_ptr, fp);

    // Set PNG info. Here we assume 8-bit color depth and RGBA format.
    png_set_IHDR(png_ptr, info_ptr, pixmap.width(), pixmap.height(),
                 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    // Write image data
    auto* addr = static_cast<const png_byte*>(pixmap.addr());
    int stride = pixmap.rowBytes();
    for (int y = 0; y < pixmap.height(); ++y) {
        png_write_row(png_ptr, addr + y * stride);
    }

    // Finish writing
    png_write_end(png_ptr, nullptr);

    // Cleanup
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}






static void skia_surface_upload_to_texture(
    sk_sp<SkSurface> skiaSurface, // Changed from cairo_surface_t to sk_sp<SkSurface>
    wf::simple_texture_t& buffer)
{


    // Ensure the Skia surface is not null
    if (!skiaSurface) {
        std::cerr << "Skia surface is null, cannot upload to texture." << std::endl;
        return;
    }

    // Obtain image snapshot from the Skia surface
    sk_sp<SkImage> image = skiaSurface->makeImageSnapshot();
    if (!image) {
        std::cerr << "Failed to create image snapshot from Skia surface." << std::endl;
        return;
    }


buffer.width = image->width();
buffer.height = image->height();

 if (buffer.tex == (GLuint) - 1)
    {
        GL_CALL(glGenTextures(1, &buffer.tex));
    }

    // Bind the texture
    GL_CALL(glBindTexture(GL_TEXTURE_2D, buffer.tex));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

    // We need to convert the SkImage to a format suitable for OpenGL
    SkPixmap pixmap;
    if (image->peekPixels(&pixmap)) {
        // Assuming the Skia surface is using RGBA_8888 which is compatible with GL_RGBA
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                             pixmap.width(), pixmap.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixmap.addr()));
std::cerr << "peek pixels from Skia image." << std::endl;
        writePNG(pixmap, "outputskia.png");
    } else {
        std::cerr << "Failed to peek pixels from Skia image." << std::endl;
    }



}






struct TextLine {
    std::string text;
    wf::geometry_t box;
};

std::vector<TextLine> textLines;

struct FileWatchParams {
    std::string filename;
    time_t lastModified;
    bool stopThread;
    std::atomic<bool> externallySignaled{false}; // New flag

    FileWatchParams() : lastModified(0), stopThread(false) {}
};


FileWatchParams params;
//////////SCROLL BAR/////////////////////////

bool isDragging = false;
double scrollbarWidth; // Width of the scrollbar
double scrollbarX ; // X position of the scrollbar
double scrollbarY ; // Y position of the scrollbar
double scrollbarHeight;;

double scroll_offset = 0; // Current vertical scroll position
    
bool buttonpush=false;

/////////////////////////////////////////////
//////////DOCK//////////////////////////////////////////////
char* mode="chatGPT";
char* Xmode="chatGPT";
char* previous_mode="chatGPT";
bool isDraggingSwitch=false;

// Outer rectangle (background)
     double dockOuterRectX = 5;

// Inner rectangle (switch background)
    double switchX = dockOuterRectX + 10; // Padding from the left edge of the outer rectangle



    double dockBackgroundRectX; // Starting X position of the inner rectangle
    double dockBackgroundRectY; // Starting Y position of the inner rectangle
    double dockBackgroundRectWidth; // Width of the inner rectangle 
    double dockBackgroundRectHeight; // Width of the height rectangle 

    double indicatorX = switchX + 5;
    double indicatorY; // Start inside the switch
    double indicatorWidth; // One third of the switch width minus padding
    double indicatorHeight; // Height minus padding for top and bottom
    
/////////////////////////////////////////////////////////////////    
////////////HIGHLIGHT COPY////////////////////////

static bool singleLineHighlight = true; // Variable to toggle between single and multiple line highlights

struct HighlightedLine {
    int lineIndex;
    double rectX;
    double rectY;
    double rectWidth;
    double rectHeight;
};
std::vector<HighlightedLine> highlightedLines;

struct BlocklightedLine {
    int lineIndex;
    double rectX;
    double rectY;
    double rectWidth;
    double rectHeight;
};
std::vector<BlocklightedLine> blocklightedLines;


struct Circle {
    double x;
    double y;
    double radius;
    std::string color; // Using string for color
    int syntaxBlockId;
    bool pressed=false;
};

std::vector<Circle> circles;

struct BlockRange {
    int start;
    int end;
};


// Store the previous pressed values
bool pastvalueRed = false;
bool pastvalueGreen = false;
bool pastvalueBlue = false;


bool awaitingSecondClick = false;
int firstClickLineIndex = -1;
int secondClickLineIndex = -1;
bool clickOnTextLine = false;
//////////////COPY//////////////////////////
 std::string selectedText;
 std::string escapedText;

//////////////////////////////////////////// 
 enum GPTOptions {
    GPT_RENDER,
    GPT_LOADING,
    GPT_BASH,
    GPT_OPTION_2,
    // Add more options as needed
};


std::string saveAndParseThreadId(const std::string& response) {
    // Save the JSON response to a file
    std::ofstream file("thread.txt");
    if (file.is_open()) {
        file << response;
        file.close();
    } else {
        // Handle the error if the file cannot be opened
     //   std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return "";
    }

    // Now, parse the saved JSON file
    std::ifstream inputFile("thread.txt");
    if (inputFile.is_open()) {
        // Parse the JSON from the file
        json jsonResponse = json::parse(inputFile);

        // Extract the thread_id
        threadId = jsonResponse["id"];
        printf("%parsethreadIds\n",threadId );
    } else {
        // Handle the error if the file cannot be read
       // std::cerr << "Failed to open file for reading: " << filePath << std::endl;
    }

    return threadId;
}

std::string parseRunId(const std::string& response) {
    // Save the JSON response to a file
    std::ofstream file("runID.txt");
    if (file.is_open()) {
        file << response;
        file.close();
    } else {
        // Handle the error if the file cannot be opened
     //   std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return "";
    }

    // Now, parse the saved JSON file
    std::ifstream inputFile("runID.txt");

    if (inputFile.is_open()) {
        // Parse the JSON from the file
        json jsonResponse = json::parse(inputFile);

        // Extract the thread_id
        runId = jsonResponse["id"];
    } else {
        // Handle the error if the file cannot be read
       // std::cerr << "Failed to open file for reading: " << filePath << std::endl;
    }

    return runId;
}


std::string parseRunStatus(const std::string& jsonResponse) {
    // Check if the JSON response is empty
    if (jsonResponse.empty()) {
        std::cerr << "JSON response is empty.\n";
        return "error";
    }

    // Save JSON response to a file
    std::ofstream outFile("runstatus.txt");
    if (!outFile) {
        std::cerr << "Unable to open file for writing.\n";
        return "error";
    }

    outFile << jsonResponse;
    outFile.close();

    // Now attempt to parse the JSON response
    try {
        auto response = json::parse(jsonResponse);
        std::string status = response.value("status", "unknown");
        return status;
    } catch (const json::parse_error& e) {
        std::cerr << "Parsing error: " << e.what() << '\n';
        return "error";
    }
}

void parseJSONgptResponseAssistant() {
    using json = nlohmann::json;

    if (!response.empty() && GPTrequestFinished == true) {
        lines.clear();
        std::string content; // Initialize an empty string to accumulate content

        try {
            json transcript_json = json::parse(response);

            // Check if "data" array exists in the JSON
            if (transcript_json.contains("data") && transcript_json["data"].is_array()) {
                for (const auto& message : transcript_json["data"]) {
                    // For each message, access the content array
                    if (!message["content"].empty() && message["content"][0].contains("text")) {
                        std::string value = message["content"][0]["text"].value("value", "");
                        if (!value.empty()) {
                            content += value + "\n"; // Concatenate value with a newline
                        }
                    }
                }
            }
            
            // Process the concatenated content
            size_t startPos = 0;
            size_t newlinePos;
            while ((newlinePos = content.find('\n', startPos)) != std::string::npos) {
                lines.push_back(content.substr(startPos, newlinePos - startPos));
                startPos = newlinePos + 1;
            }
            
            // Add the last line if it doesn't end with a newline
            if (startPos < content.size()) {
                lines.push_back(content.substr(startPos));
            }
            
        } catch (json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            request_text = "bashGPT JSON parsing error:"; 
        }

        // Update texture or any other relevant functionality after parsing
       // update_texture();
    }
}

  void parseJSONgptResponseChatGPT()
    {
if (!response.empty() && GPTrequestFinished == true) {
    lines.clear();

    try {
        json transcript_json = json::parse(response);

        if (transcript_json.contains("choices") && transcript_json["choices"].is_array()) {
            const auto& choices = transcript_json["choices"];
            if (!choices.empty() && choices[0].contains("message") && choices[0]["message"].contains("content")) {
                std::string content = choices[0]["message"]["content"].get<std::string>();

                size_t startPos = 0;
                size_t newlinePos;
                while ((newlinePos = content.find('\n', startPos)) != std::string::npos) {
                    lines.push_back(content.substr(startPos, newlinePos - startPos));
                    startPos = newlinePos + 1;
                }

                // Add the last line if it doesn't end with a newline
                if (startPos < content.size()) {
                    lines.push_back(content.substr(startPos));
                }
            }
        }
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << '\n';
        request_text = "chatGPT JSON parsing error:";
    }
}

       } 


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to make HTTP request
void *makeHttpRequest(void *response_ptr) {
    request_text = "Initializing request.";
    GPTrequestFinished=false;
    GPTrequestLoading=true;
    resetloading=false;
    std::string* response = reinterpret_cast<std::string*>(response_ptr);
  
std::string fileName;


//sleep(1);
  if (DEBUG_MODE) {
    request_text = "Debug mode: Reading response from file.";
  //  sleep(10);
        // Read response from file for debugging
        std::ifstream inFile("responseCopy.txt");
        if (inFile.is_open()) {
            std::stringstream buffer;
            buffer << inFile.rdbuf();
            *response = buffer.str();
            inFile.close();
        sleep(1);

          usleep(20000);
        GPTrequestFinished=true;
        GPTrequestLoading=false;
         request_text = "Debug mode: Response read from file.";
         pthread_exit(NULL);
        } else {

            GPTrequestFinished=true;
            GPTrequestLoading=false;
            *response= "Failed to open response.txt for reading.";
            pthread_exit(NULL);
        }
    } else if (LOAD_MODE) {
    request_text = "Load mode: Reading response from file.";
  if (mode == "chatGPT") {
    fileName = "response.txt";
} else if (mode == "bashGPT") {
    fileName = "bashresponse.txt";
} else if (mode == "autoGPT") {
    fileName = "autogptresponse.txt"; // Assuming you want a different name here, otherwise keep it as bashresponse.txt
} else {
    fileName = "null.txt"; // Default or unrecognized mode
}

// Open a file in write mode using the determined file name
  //  sleep(10);
        // Read response from file for debugging
        std::ifstream inFile(fileName);
        if (inFile.is_open()) {
            std::stringstream buffer;
            buffer << inFile.rdbuf();
            *response = buffer.str();
            inFile.close();
       // sleep(1);
//printf("Load Response: %s\n", response->c_str());  
  //        usleep(20000);
        GPTrequestFinished=true;
        GPTrequestLoading=false;
        request_text = "Load: Response read from file.";
        LOAD_MODE=FALSE;
         pthread_exit(NULL);
        } else {

            GPTrequestFinished=true;
            GPTrequestLoading=false;
            *response= "Failed to open response.txt for reading.";
            pthread_exit(NULL);
        }
    } 

    else {

    std::string GPTprompt;

    std::string filename = "chatgpt_prompt.txt";
    std::ifstream file(filename);

    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        GPTprompt = buffer.str();
        file.close();
    } else {
        pthread_exit(NULL);
    }

    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    struct curl_slist *headers = NULL;


if (mode=="chatGPT")
{
      curl = curl_easy_init();
    if(curl) {
        request_text = "Setting up request.";
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer key here");

        std::string data = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"system\",\"content\": \"skilled in explaining complex programming .\"},{\"role\": \"user\",\"content\": \"" + GPTprompt + "\"}]}";
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        request_text = "Sending chatGPT Request ";
        res = curl_easy_perform(curl);
     
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
       request_text="Request failed";
        }

        *response = readBuffer;  // Assign the response to the string pointer

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
        request_text="Request failed.";
    }
}




std::string createThreadUrl;


if (mode == "bashGPT") {
    CURL *curl = curl_easy_init();
    if (curl) {
        CURLcode res;
        std::string readBuffer;
        struct curl_slist *headers = nullptr;
        request_text = "Setting up request.";

        // Setup common headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string assistantId = "asst_7Ie89EX2XbFAX0W9wNh1rVet";
        std::string apiKey = "key here"; // Replace with your actual API key
        std::string authHeader = "Authorization: Bearer " + apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "OpenAI-Beta: assistants=v1");

        // Assuming you've already created an assistant and have its ID
        // Create a Thread
if (threadId==""){
        createThreadUrl = "https://api.openai.com/v1/threads";
        }
        else { createThreadUrl ="https://api.openai.com/v1/threads/" + threadId + "/messages";}
        curl_easy_setopt(curl, CURLOPT_URL, createThreadUrl.c_str());

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}"); // Empty payload for thread creation
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to create a thread: %s\n", curl_easy_strerror(res));
            request_text = "Thread creation failed";
        } else {
            // Extract thread ID from response
        if (threadId==""){
            threadId = saveAndParseThreadId(readBuffer); // Implement this function to parse the thread ID from the JSON response
            }

printf("threadID:= %s\n",threadId.c_str() );
            // Clear buffer for the next operation
            readBuffer.clear();
  // Assuming you've already created an assistant and have its ID
           // Use your actual assistant ID

            // Send a message within the created thread
            std::string sendMessageUrl = "https://api.openai.com/v1/threads/" + threadId + "/messages";
            std::string messageData = "{\"role\": \"user\", \"content\": \"" + GPTprompt + "\"}";

            curl_easy_setopt(curl, CURLOPT_URL, sendMessageUrl.c_str());

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, messageData.c_str());

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "Sending message failed: %s\n", curl_easy_strerror(res));
                request_text = "Sending message failed";
            } else {
                // Now, initiate a run to prpocess the conversation with the specified assistant
                request_text = "sending bashGPT request";
                std::string runUrl = "https://api.openai.com/v1/threads/" + threadId + "/runs";
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // Ensue headers are still set correctly
                std::string runData = "{\"assistant_id\": \"" + assistantId + "\"}";



                curl_easy_setopt(curl, CURLOPT_URL, runUrl.c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, runData.c_str());

                readBuffer.clear(); // Clear buffer before the next call
                res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    fprintf(stderr, "Initiating run failed: %s\n", curl_easy_strerror(res));
                    request_text = "Run initiation failed";
                } else {
                    *response = readBuffer; // Process the response
                    request_text = "waiting on request";

  runId = parseRunId(readBuffer); // Implement this function based on your response structure
    readBuffer.clear(); // Clear buffer for polling

    // Polling for Run Completion (Step 3 from the previous explanation)
    bool runCompleted = false;
    std::string runStatusUrl = "https://api.openai.com/v1/threads/" + threadId + "/runs/" + runId;
    
printf("runStatusUrl:= %s\n",runStatusUrl.c_str() );

    while (!runCompleted) {
        readBuffer.clear();
        request_text = "polling request";
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1); // Switch to HTTP GET
        curl_easy_setopt(curl, CURLOPT_URL, runStatusUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
       // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Enable follow redirects
        curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS); // Allow HTTPS only


        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to check run status: %s\n", curl_easy_strerror(res));
            break; // Exit loop on failure
        }

        // Check the run status
        std::string status = parseRunStatus(readBuffer); // Implement this function
        if (status == "completed") {
            runCompleted = true;
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait before polling again
        }
    }
    if (runCompleted) {
        std::string messagesUrl = "https://api.openai.com/v1/threads/" + threadId + "/messages";
        readBuffer.clear();
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_URL, messagesUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        request_text="Request Completed ";

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to retrieve messages: %s\n", curl_easy_strerror(res));
            request_text = "Message retrieval failed";
        } else {
            *response = readBuffer; // Process and display the messages
            request_text = "Message retrieval successful";
        }
    }


                }
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
        request_text = "CURL initialization failed.";
    }
}








if (mode=="autoGPT")
{
    curl = curl_easy_init();
    if(curl) {
        request_text = "Setting up request.";
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer key here");

        std::string data = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"system\",\"content\": \"skilled in explaining complex programming .\"},{\"role\": \"user\",\"content\": \"" + GPTprompt + "\"}]}";
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        request_text = "Sending autoGPT Request ";
        res = curl_easy_perform(curl);
     
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
       request_text="Request failed";
        }

        *response = readBuffer;  // Assign the response to the string pointer

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
        request_text="Request failed.";
    }
}





//    request_text="Request Completed ";
   // sleep(1);
//    usleep(500000); 
 GPTrequestFinished=true;
 GPTrequestLoading=false;
 resetloading=true;
  //  std::cout << "Response: " << readBuffer << std::endl;
   printf("Response: %s\n", response->c_str());  
std::string fileName;

// Determine the file name based on the mode
if (mode == "chatGPT") {
    fileName = "response.txt";
} else if (mode == "bashGPT") {
    fileName = "bashresponse.txt";
} else if (mode == "autoGPT") {
    fileName = "autogptresponse.txt"; // Assuming you want a different name here, otherwise keep it as bashresponse.txt
} else {
    fileName = "null.txt"; // Default or unrecognized mode
}


std::string responseContent = *response;
// Open a file in write mode
std::ofstream outFile(fileName);


// Check if the file is open
if (outFile.is_open()) {
    outFile << responseContent << '\n';
    outFile.close(); // Close the file after writing
} else {
    printf("Response file Unable to open file for writing.");
}

 GPTrequestFinished=true;
 GPTrequestLoading=false;
 resetloading=true;


LOAD_MODE=FALSE;

if (GPTrequestFinished==true)

        {

            if (mode=="chatGPT")   
            {parseJSONgptResponseChatGPT();}
            
            if (mode=="bashGPT")
            {parseJSONgptResponseAssistant();}
            if (mode=="autoGPT")
            {parseJSONgptResponseChatGPT();}

        //    update_texture();
        }



    pthread_exit(NULL);
 }
}

bool fileChanged(const std::string& filename, time_t& lastModified) {
    if (filename.empty()) {
        printf("Filename is empty.\n");
        return false;
    }

    struct stat fileInfo;
    if (stat(filename.c_str(), &fileInfo) != 0) {
        printf("Error getting file info for %s: %s\n", filename.c_str(), strerror(errno));
        return false;
    }

    if (fileInfo.st_mtime > lastModified) {
        lastModified = fileInfo.st_mtime;
        return true;
    }

    return false;
}

typedef struct {
    int x, y;
    int width, height;
} wlr_box2;

// Define a test box
wlr_box2 test_box = {
    .x = 0,      // X position
    .y = 0,      // Y position
    .width = 800, // Width
    .height = 600 // Height
};


void* watchFile(void* args) {
    auto* params = static_cast<FileWatchParams*>(args);
    while (!params->stopThread) {
        // Check if the file has changed OR if an external signal has been received
        if (fileChanged(params->filename, params->lastModified) || params->externallySignaled.load()) {
            printf("%s changed.\n", params->filename.c_str());
            params->externallySignaled = false; // Reset the flag after handling
            // Your existing thread logic...
            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, makeHttpRequest, &response) == 0) {
                pthread_join(thread_id, NULL); // Consider using pthread_detach if appropriate
            } else {
                printf("Error creating thread\n");
            }
        }
        sleep(1); // Pause for a second

    }
    return nullptr;
}


sk_sp<const GrGLInterface>  interface;

#define WIDGET_PADDING 10

struct workspace_name
{
    wf::geometry_t rect;
    std::string name;
    std::unique_ptr<wf::simple_texture_t> texture;
  // std::shared_ptr<wf::simple_texture_t> texture;
    cairo_t *cr = nullptr;
    cairo_surface_t *cairo_surface;
  //  cairo_text_extents_t text_extents;
     TextExtentsCompat text_extents; 
};

namespace wf
{
namespace scene
{
namespace workspace_names
{
class simple_node_render_instance_t : public render_instance_t
{

    wf::signal::connection_t<node_damage_signal> on_node_damaged =
        [=] (node_damage_signal *ev)
    {
        push_to_parent(ev->region);
    };

    node_t *self;
    damage_callback push_to_parent;
    std::shared_ptr<workspace_name> workspace;
    wf::point_t *offset;
    double *alpha_fade;
      wf::framebuffer_t saved_pixels;
    wf::region_t saved_pixels_region;

  public:



    simple_node_render_instance_t(node_t *self, damage_callback push_dmg,
        wf::point_t *offset, double *alpha_fade,
        std::shared_ptr<workspace_name> workspace)
    {
        this->offset = offset;
        this->self   = self;
        this->workspace  = workspace;
        this->alpha_fade = alpha_fade;
        this->push_to_parent = push_dmg;
        self->connect(&on_node_damaged);
    }

    void schedule_instructions(
        std::vector<render_instruction_t>& instructions,
        const wf::render_target_t& target, wf::region_t& damage)
    {
        // We want to render ourselves only, the node does not have children
        instructions.push_back(render_instruction_t{
                        .instance = this,
                        .target   = target,
                        .damage   = damage & self->get_bounding_box(),
                    });
    }

 
 



static GrGLFuncPtr GetGLProcAddressStatic(void* ctx, const char* name) {
    auto proc = eglGetProcAddress(name);
    return reinterpret_cast<GrGLFuncPtr>(proc);
}


 SkCanvas* canvas ;



    void init_skia(int w, int h, wf::framebuffer_t target) {
   /* 
auto interface = GrGLMakeAssembledInterface(nullptr, (GrGLGetProc)*[](void *, const char *p) -> void * {
    return (void *)glfwGetProcAddress(p);
});*/

  #define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_SRGB8_ALPHA8 0x8C43

interface = GrGLMakeAssembledInterface(nullptr, &GetGLProcAddressStatic);


     sContext = GrDirectContexts::MakeGL(interface).release();




    if (!sContext) {
                std::cerr << "Failed to create GrDirectContext." << std::endl;
               
            }else{printf("created grDirectContext\n");}
        


    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = target.fb; // assume default framebuffer
    // We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
    //(replace line below with this one to enable correct color spaces) framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;
    GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(w, h,
        0, // sample count
        0, // stencil bits
        framebufferInfo);

    //(replace line below with this one to enable correct color spaces) sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, SkColorSpace::MakeSRGB(), nullptr).release();
     sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();
    
   if (!sSurface) {
                std::cerr << "Failed to create GPU-accelerated SkSurface." << std::endl;
            
            }else{printf(" create GPU-accelerated SkSurface\n");}



    if (sSurface == nullptr) abort();
}




    void render(const wf::render_target_t& target,
        const wf::region_t& region)
    {

  wf::geometry_t g{target_fb.viewport_width,
            target_fb.viewport_height,
            target_fb.viewport_width, target_fb.viewport_height};


target_fb=target;
//target_fb.allocate(target_fb.viewport_width, target_fb.viewport_height);
//target_fb.bind();
if (xx==0)
{
init_skia(1280, 720,target_fb);
//xx=1;
}



        OpenGL::render_begin(target_fb);

 
//target_fb.allocate(target.viewport_width, target.viewport_height);
//target_fb.bind();
        //saved_pixels.bind();

        for (auto& box : region)
        {
            LOGI("inside");


 OpenGL::render_texture(wf::texture_t{target_fb.tex},
                target, g, glm::vec4(1, 1, 1, 0.5),
                OpenGL::TEXTURE_TRANSFORM_INVERT_Y);
        }
 canvas = sSurface->getCanvas();
SkPaint paint;

//if (xx==0){
        paint.setColor(SK_ColorBLUE);
       canvas->drawRect({100, 200, 300, 500}, paint);
 

 if (sContext) { // Use the existing directContext without re-creating it
        sContext->flushAndSubmit();
    }
 printf("yelp\n");
//xx=1;}
  

        OpenGL::render_end();
  //     delete sSurface;
    delete sContext; 
 
//sSurface.reset();
    }

 
        
    
};


class simple_node_t : public node_t, public pointer_interaction_t
{


    wf::point_t offset;
    double alpha_fade;

  public:



    std::shared_ptr<workspace_name> workspace;
    simple_node_t(wf::point_t offset) : node_t(false)
    {
        this->offset     = offset;
        this->alpha_fade = 0.0;
        workspace = std::make_shared<workspace_name>();
    }

    void gen_render_instances(std::vector<render_instance_uptr>& instances,
        damage_callback push_damage, wf::output_t *shown_on) override
    {
        // push_damage accepts damage in the parent's coordinate system
        // If the node is a transformer, it may transform the damage. However,
        // this simple nodes does not need any transformations, so the push_damage
        // callback is just passed along.
        instances.push_back(std::make_unique<simple_node_render_instance_t>(
            this, push_damage, &offset, &alpha_fade, workspace));
    }

    void do_push_damage(wf::region_t updated_region)
    {
        node_damage_signal ev;
        ev.region = updated_region;
        this->emit(&ev);
    }

    wf::geometry_t get_bounding_box() override
    {
        // Specify whatever geometry your node has
        return {workspace->rect.x + offset.x, workspace->rect.y + offset.y,
            workspace->rect.width, workspace->rect.height};
    }

    void set_offset(int x, int y)
    {
        this->offset.x = x;
        this->offset.y = y;
    }

    void set_alpha(double alpha)
    {
        this->alpha_fade = alpha;
    }


  wf::pointer_interaction_t& pointer_interaction() override
    {
      //   LOGI("pointer interaction");
        return *this;
    }

    // First, a node should specify where it wants to receive input.
    // The find_node_at function takes a coordinate in the node's coordinate system (i.e with the
    // transformation of the parent nodes applied, same as bounding box, etc.) and needs to return which
    // node should get a focus for that region.
    //
    // Note that if a node with higher Z-order obstructs a part of the node, that other node will get pointer
    // input first.
    std::optional<wf::scene::input_node_t> find_node_at(const wf::pointf_t& at) override
    {
        // Simply check whether the mouse is over our bounding box.
        wf::region_t our_region{get_bounding_box()};
        if (our_region.contains_pointf(at))
        {
            wf::scene::input_node_t result;
            result.node = this;
            result.local_coords = at;
            return result;
        }

        return {};
    }



  void handle_pointer_enter(wf::pointf_t position) override
    {
      //  LOGI("in the view");
        (void)position;
    }

    void handle_pointer_leave() override
    { //LOGI("out the view");
    }

    void handle_pointer_button(const wlr_pointer_button_event& event) override
    {
    //      LOGI("handle_pointer_button");
    if (event.button == BTN_LEFT) {
        if (event.state == WLR_BUTTON_PRESSED) {
            buttonpush = true;
      //      printf("Button push: %s\n", buttonpush ? "true" : "false");
        } else if (event.state == WLR_BUTTON_RELEASED) {
            buttonpush = false;
        //    printf("Button push: %s\n", buttonpush ? "true" : "false");
        }
    }
       (void)event;
    }

    void handle_pointer_motion(wf::pointf_t pointer_position, uint32_t time_ms) override
    {
        (void)pointer_position;
        (void)time_ms;
     //   printf("Cursor Position: x = %f, y = %f\n", pointer_position.x, pointer_position.y);
        pointer_posX=pointer_position.x;
        pointer_posY=pointer_position.y;

    }

    void handle_pointer_axis(const wlr_pointer_axis_event& event) override
    {
        (void)event;
    }
};





std::shared_ptr<simple_node_t> add_simple_node(wf::output_t *output,
    wf::point_t offset)
{
    auto subnode = std::make_shared<simple_node_t>(offset);
//    wf::scene::add_front(output->node_for_layer(wf::scene::layer::OVERLAY), subnode);
     wf::scene::add_front(output->node_for_layer(wf::scene::layer::BOTTOM ), subnode);

    return subnode;
}

class wayfire_workspace_names_output : public wf::per_output_plugin_instance_t
{
private:
    //std::unique_ptr<SkSurface> skSurface;
    std::shared_ptr<workspace_name> workspace;
    sk_sp<GrDirectContext> directContext ;
    sk_sp<const GrGLInterface> interface; 
    SkImageInfo imageInfo ;

public:
    
  //   explicit wayfire_workspace_names_output(std::unique_ptr<SkSurface> surface)
    //    : skSurface(std::move(surface)) {}

    // Default constructor
   // wayfire_workspace_names_output() : skSurface(nullptr) {}

    wf::wl_timer<false> timer;
    bool hook_set  = false;
    bool timed_out = false;
    
    std::vector<std::vector<std::shared_ptr<simple_node_t>>> workspaces;
    wf::option_wrapper_t<std::string> font{"workspace-names/font"};
    wf::option_wrapper_t<std::string> position{"workspace-names/position"};
    wf::option_wrapper_t<int> display_duration{"workspace-names/display_duration"};
    wf::option_wrapper_t<int> margin{"workspace-names/margin"};
    wf::option_wrapper_t<double> background_radius{
        "workspace-names/background_radius"};
    wf::option_wrapper_t<wf::color_t> text_color{"workspace-names/text_color"};
    wf::option_wrapper_t<wf::color_t> background_color{
        "workspace-names/background_color"};
    wf::option_wrapper_t<bool> show_option_names{"workspace-names/show_option_names"};
    wf::animation::simple_animation_t alpha_fade{display_duration};

      /*wf::plugin_activation_data_t grab_interface{
        .name = "workspace-names",
        .capabilities = wf::CAPABILITY_MANAGE_COMPOSITOR,
    };*/

  public:


void initializeGraphics() {

auto size = output->get_screen_size();


auto interface = GrGLMakeNativeInterface();
//interface->unref();
        if (!directContext) { // Initialize directContext if not already initialized
            directContext = GrDirectContexts::MakeGL(interface);


      //  if (!directContext) { // Initialize directContext if not already initialized
        //    directContext = GrDirectContexts::MakeGL();
            std::cerr << "created GrDirectContext." << std::endl;
            if (!directContext) {
                std::cerr << "Failed to create GrDirectContext." << std::endl;
                return;
            }
        }
/*
        if (!skSurface) { // Initialize skSurface if not already initialized
            imageInfo = SkImageInfo::Make(size.width, size.height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
            skSurface = SkSurfaces::RenderTarget(directContext.get(), skgpu::Budgeted::kNo, imageInfo);
            std::cerr << " created GPU-accelerated SkSurface." << std::endl;
            if (!skSurface) {
                std::cerr << "Failed to create GPU-accelerated SkSurface." << std::endl;
                return;
            }
        }*/
    }

void init() override
    {    
 auto size = output->get_screen_size();
//initializeGraphics();

 // We don't manage this pointer's lifetime.

//output->ender->add_effect(&damage_hook, wf::OUTPUT_EFFECT_DAMAGE);
//output->render->add_effect(&overlay_hook, wf::OUTPUT_EFFECT_OVERLAY);

           // GLuint textureID = CreateOpenGLTexture(textureWidth, textureHeight);
 //   if (textureID == 0) {
        // Texture creation failed, handle the error
  //      std::cerr << "Failed to generate OpenGL texture." << std::endl;
  //      return;
  //  }


        printf("checkin init\n");
//initializeShader();
printf("after initialise checkin init\n");

//setupframebuffer();

start_time = std::chrono::steady_clock::now(); 
 // std::string filename = "chatgpt_prompt.txt";
    params.filename = "chatgpt_prompt.txt";
    params.lastModified = 0;

      // Get the current modification time of the file to initialize lastModified
    struct stat fileInfo;
    if (stat(params.filename.c_str(), &fileInfo) == 0) {
        params.lastModified = fileInfo.st_mtime;
    } else {
        printf("Could not get file info for %s\n", params.filename.c_str());
        params.lastModified = 0;  // Fallback if file info couldn't be retrieved
    }

    pthread_t watcherThread;
    if (pthread_create(&watcherThread, nullptr, watchFile, &params)) {
        printf("Error creating thread\n");
    }

//GPTrequestLoading=true;

        if (!response.empty()) 
        {
            printf("Response: %s\n", response.c_str());
        } else {
            if (GPTrequestFinished==true)
            {LOGI("Error: Failed to make HTTP request or received an empty response or waiting.");}
        else if (GPTrequestFinished==false)
            {LOGI("Still waiting on HTTP request.......");}

        }

        alpha_fade.set(0, 0);
        timed_out = false;

        auto wsize = output->wset()->get_workspace_grid_size();
        workspaces.resize(wsize.width);
        for (int x = 0; x < wsize.width; x++)
        {
            workspaces[x].resize(wsize.height);
        }

        auto og = output->get_relative_geometry();
        for (int x = 0; x < wsize.width; x++)
        {
            for (int y = 0; y < wsize.height; y++)
            {
                workspaces[x][y] = add_simple_node(output, {x *og.width,
                    y * og.height});
            }
        }

        output->connect(&workarea_changed);
        output->connect(&viewport_changed);
        font.set_callback(option_changed);
        position.set_callback(option_changed);
        background_color.set_callback(option_changed);
        text_color.set_callback(option_changed);
        show_option_names.set_callback(show_options_changed);

      
            show_options_changed();
            update_texture();
    }


    wf::signal::connection_t<wf::reload_config_signal> reload_config{[this] (wf::reload_config_signal *ev)
        {
     //       parseJSONgptResponse();
        }
    };

    wf::config::option_base_t::updated_callback_t show_options_changed = [=] ()
    {
      //  parseJSONgptResponse();

        viewport_changed.emit(nullptr);

        if (show_option_names)
        {
            timer.disconnect();
            output->render->rem_effect(&post_hook);
        } else
        {
            output->connect(&viewport_changed);
            output->render->add_effect(&post_hook, wf::OUTPUT_EFFECT_POST);
        }

        alpha_fade.animate(alpha_fade, 1.0);
        output->render->damage_whole();
    };



    
    void update_texture()
    {
     

        update_texture_position();
        if (GPTrequestLoading==true)
            {loading_cairo_GPT();}


        if (GPTrequestLoading==false)
            {  render_cairo_GPT();}
            
     
    
//if (GPTrequestFinished==false && GPTrequestLoading==false)
//{printf("ERROR BOTH ARE FALSE or just the start\n");}
      output->render->damage_whole();

    }



/*
void configurePangoContext(cairo_t *cr, wf::output_t* output, GPTOptions option) {
    auto outputGeometry = output->get_relative_geometry();
    PangoFontDescription *font_desc;

    // Create a PangoLayout for this context
    PangoLayout *layout = pango_cairo_create_layout(cr);

    switch (option) {
        case GPT_RENDER:
            font_desc = pango_font_description_from_string("FontName 15"); // Example font description
            break;
        case GPT_LOADING:
            font_desc = pango_font_description_from_string("MathJax_SansSerif-Regular 200"); // Adjust the font size as needed
            break;
        case GPT_BASH:
            font_desc = pango_font_description_from_string("FontName 25"); // Adjust the font size as needed
            break;
        case GPT_OPTION_2:
            // Do specific configurations for GPT_OPTION_2
            // ...
            font_desc = pango_font_description_from_string("FontName 15"); // Default or specific case
            break;
        // Add more cases for additional options
        default:
            // Handle invalid option or provide a default behavior
            font_desc = pango_font_description_from_string("FontName 15"); // Default font description
            break;
    }

    // Apply the font description to the layout
    pango_layout_set_font_description(layout, font_desc);

    // No need to set font size directly if it's included in the PangoFontDescription string
    // Use the layout for text measurement and rendering

    // Clean up
    pango_font_description_free(font_desc);
    g_object_unref(layout);
}
*/
PangoLayout* createConfiguredPangoLayout(cairo_t *cr, GPTOptions option) {
    PangoFontDescription *font_desc = nullptr;

    switch (option) {
        case GPT_RENDER:
            font_desc = pango_font_description_from_string("FontName 150");
            break;
        case GPT_LOADING:
            font_desc = pango_font_description_from_string("MathJax_SansSerif-Regular 200");
            break;
        case GPT_BASH:
            font_desc = pango_font_description_from_string("FontName 25");
            break;
        case GPT_OPTION_2:
            font_desc = pango_font_description_from_string("FontName 15");
            break;
        default:
            font_desc = pango_font_description_from_string("FontName 15");
            break;
    }

    PangoLayout *layout = pango_cairo_create_layout(cr);
    pango_layout_set_font_description(layout, font_desc);
    pango_font_description_free(font_desc); // Cleanup font description immediately after use

    return layout; // Caller is responsible for freeing the layout
}



    void cairo_recreate()
    {

        auto cws = output->wset()->get_current_workspace();
        auto wsn     = workspaces[cws.x][cws.y]->workspace;
        auto size = output->get_screen_size();
  if (!GPTrequestFinished) {
        // HTTP request not finished yet, skip rendering
        return;
    }
        auto og = output->get_relative_geometry();
 auto workarea = output->workarea->get_workarea();
      //  auto font_size = og.height * 0.05;
        cairo_t *cr    = wsn->cr;
        cairo_surface_t *cairo_surface = wsn->cairo_surface;

        if (!cr)
        {
            /* Setup dummy context to get initial font size */
            if (mode=="chatGPT")
            {    
            PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_RENDER);
            }

            if (mode=="bashGPT")
            {    
            PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_BASH);
            }

            cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
            cr = cairo_create(cairo_surface);
            wsn->texture = std::make_unique<wf::simple_texture_t>();
        }

      // PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_RENDER);

        const char *name = wsn->name.c_str();
        pango_text_extents(cr, name, &wsn->text_extents);

        wsn->rect.width  = wsn->text_extents.width + WIDGET_PADDING * 2;
        wsn->rect.height = size.height- wsn->text_extents.height + WIDGET_PADDING * 2;

        /* Recreate surface based on font size */
        cairo_destroy(cr);
        cairo_surface_destroy(cairo_surface);

        cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
            wsn->rect.width, wsn->rect.height);
        cr = cairo_create(cairo_surface);

     //   PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_RENDER);

        wsn->cr = cr;
        wsn->cairo_surface = cairo_surface;
    }

    wf::config::option_base_t::updated_callback_t option_changed = [=] ()
    {
       if (!GPTrequestFinished) {
        // HTTP request not finished yet, skip rendering
        return;
    }
        update_texture();
    };

    void update_texture_position()
    {
//LOGI("update_texture_position");
        auto cws = output->wset()->get_current_workspace();
        auto wsn     = workspaces[cws.x][cws.y]->workspace;

        auto workarea = output->workarea->get_workarea();

        cairo_recreate();

        if ((std::string)position == "top_left")
        {
            wsn->rect.x = workarea.x + margin;
            wsn->rect.y = workarea.y + margin;
        } else if ((std::string)position == "top_center")
        {
            wsn->rect.x = workarea.x + (workarea.width / 2 - wsn->rect.width / 2);
            wsn->rect.y = workarea.y + margin;
        } else if ((std::string)position == "top_right")
        {
            wsn->rect.x = workarea.x + (workarea.width - wsn->rect.width) - margin;
            wsn->rect.y = workarea.y + margin;
        } else if ((std::string)position == "center_left")
        {
            wsn->rect.x = workarea.x + margin;
            wsn->rect.y = workarea.y + (workarea.height / 2 - wsn->rect.height / 2);
        } else if ((std::string)position == "center")
        {
            wsn->rect.x = workarea.x + (workarea.width / 2 - wsn->rect.width / 2);
            wsn->rect.y = workarea.y + (workarea.height / 2 - wsn->rect.height / 2);
        } else if ((std::string)position == "center_right")
        {
            wsn->rect.x = workarea.x + (workarea.width - wsn->rect.width) - margin;
            wsn->rect.y = workarea.y + (workarea.height / 2 - wsn->rect.height / 2);
        } else if ((std::string)position == "bottom_left")
        {
            wsn->rect.x = workarea.x + margin;
            wsn->rect.y = workarea.y + (workarea.height - wsn->rect.height) - margin;
        } else if ((std::string)position == "bottom_center")
        {
            wsn->rect.x = workarea.x + (workarea.width / 2 - wsn->rect.width / 2);
            wsn->rect.y = workarea.y + (workarea.height - wsn->rect.height) - margin;
        } else if ((std::string)position == "bottom_right")
        {
            wsn->rect.x = workarea.x + (workarea.width - wsn->rect.width) - margin;
            wsn->rect.y = workarea.y + (workarea.height - wsn->rect.height) - margin;
        } else
        {
            wsn->rect.x = workarea.x;
            wsn->rect.y = workarea.y;
        }
    }

    wf::signal::connection_t<wf::workarea_changed_signal> workarea_changed{[this] (wf::workarea_changed_signal
                                                                                   *ev)
        {
            update_texture();
        }
    };

    void cairo_clear(cairo_t *cr)
    {
    
if (cr){
        cairo_set_source_rgba(cr, 0, 0, 0, 0);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);
    }
    }


double VisibleTextHeight = 0;
double totalTextHeight = 0;

void calculate_text_dimensions(cairo_t *cr, const std::vector<std::string>& lines, double &VisibleTextHeight, double &totalTextHeight, double &maxWidth) {
    VisibleTextHeight = 0;
    totalTextHeight = 0;

    for (const auto& line : lines) {
        TextExtentsCompat text_extents;
        pango_text_extents(cr, line.c_str(), &text_extents);
        VisibleTextHeight += text_extents.height + WIDGET_PADDING;
        totalTextHeight += text_extents.height + WIDGET_PADDING;
        maxWidth = std::max(maxWidth, text_extents.width);
    }
}


struct CodeBlock {
    double startY;
    double endY;
};
// Temporary variable to store the start of the current code block

// Define a struct to represent a block of highlighted lines
struct HighlightBlock {
    double startY;
    double endY;
    double lineWidth;
};


struct SyntaxBlock {
    size_t startIndex;
    size_t endIndex;
    size_t blockNumber;
    size_t blockLineStartIndex;
    size_t blockLineEndIndex;
    std::string headerName; 
};


void render_lines(cairo_t *cr, const std::vector<std::string>& lines, double &startY,  const int MaxTextureHeight, bool &in_c_syntax_block, const wf::color_t &text_color, const wf::color_t &background_color, std::vector<TextLine> &textLines,double &maxWidth) {
//   LOGI("render_lines_GPT");
  
  double codeBlockStartY = 0, codeBlockEndY = 0;

    bool codeBlockFound = false;
    double tempStartY = startY;
    double paddingX = 5.0;
    double paddingY = 5.0;
   

//double blockMaxWidth =0.0;
//std::vector<CodeBlock> codeBlocks;

std::vector<CodeBlock> codeBlocks;

bool in_syntax_block = false;
double tempCodeBlockStartY; 




size_t token_arr_capacity = 5000; // Initial capacity, adjust as needed
auto token_deleter = [](Token* ptr) { free(ptr); };
std::unique_ptr<Token[], decltype(token_deleter)> token_arr(
    (Token*)malloc(sizeof(Token) * token_arr_capacity), 
    token_deleter
);




size_t highlightskippedLineIndex = 0; 
size_t syntaxblockskippedLineIndex = 0; 
std::vector<HighlightBlock> highlightBlocks;
//int highlightskippedLineIndex = 0;
//bool in_c_syntax_block = false;
std::set<int> highlightedIndices;
HighlightBlock currentBlock;
bool isBlockStarted = false;
double BlockLinemaxWidth = 0.0; 
double lineWidth;
double currentBlockMaxWidth = 0.0;
cairo_clear(cr);


/////////////////READ BLOCKS////////////////////////////////////////////////////////////////////////////////
std::vector<SyntaxBlock> syntaxBlocks;
bool inSyntaxBlock = false;
SyntaxBlock syntaxCurrentBlock;
size_t blockNumber = 0; // Counter for block numbers

syntaxCurrentBlock.blockLineStartIndex = 0;
size_t lineInBlockCounter = 0;

 std::map<int, BlockRange> massivesyntaxblockRanges; 



for (size_t ii = 0; ii < lines.size(); ++ii) {
    const std::string& line = lines[ii];



if (line.find_first_not_of(" \t\n\v\f\r") == std::string::npos) {
        continue;

    }

    // Toggle block state at block delimiters
    if (line.substr(0, 3) == "```" || line.substr(0, 6) == "   ```") {
 
        if (in_c_syntax_block) {
            // End of the current block
            syntaxCurrentBlock.endIndex = ii - 1; // -1 to exclude the ending delimiter line
            syntaxCurrentBlock.blockLineEndIndex = lineInBlockCounter+blockNumber-1;
            syntaxBlocks.push_back(syntaxCurrentBlock);
            in_c_syntax_block = false;
        } else {
            // Start of a new block
         //   syntaxCurrentBlock = SyntaxBlock{}; // Reset for the new block
            syntaxCurrentBlock.startIndex = ii + 1; // +1 to exclude the starting delimiter line
            syntaxCurrentBlock.blockNumber = blockNumber++;
            syntaxCurrentBlock.blockLineStartIndex = lineInBlockCounter+blockNumber-1;
            syntaxCurrentBlock.headerName = line.substr(line.find("'''") + 4); // Extracting the header name

                // Check if the resulting string starts with backticks and remove them if present
                if (syntaxCurrentBlock.headerName.rfind("```", 0) == 0) {
                    syntaxCurrentBlock.headerName.erase(0, 3); // Remove the first three backticks
                }

          //  syntaxCurrentBlock.blockLineStartIndex = 0;
          //  lineInBlockCounter = 0;
            in_c_syntax_block = true;
        }
    } else if (in_c_syntax_block) {
        // Increment line count within the block
        lineInBlockCounter++;

    }
}

// Handle the case where the file ends while still in a syntax block
if (in_c_syntax_block) {
    syntaxCurrentBlock.endIndex = lines.size() - 1;
    syntaxCurrentBlock.blockLineEndIndex = lineInBlockCounter - 1;
    syntaxBlocks.push_back(syntaxCurrentBlock);
}
/*
// Printing each syntax block's number, start, end indices, and line count
for (const auto& block : syntaxBlocks) {
    printf("Syntax Block #%zu: Start at line %zu, End at line %zu, Lines in block: %zu to %zu\n Header: %s\n", 
           block.blockNumber, block.startIndex, block.endIndex, block.blockLineStartIndex, block.blockLineEndIndex,block.headerName.c_str());
}
*/




// Optional: Print this if no blocks were found
//if (syntaxBlocks.empty()) {
  //  printf("No syntax blocks found.\n");

//}
/*
//////////////getting information fron syntax block
// Printing the lines of Syntax Block 0/1/2/3/4etc
for (const auto& block : syntaxBlocks) {
    if (block.blockNumber == 1) { // Check for the first syntax block
        std::cout << "Lines from Syntax Block 0:" << std::endl;
        for (size_t i = block.startIndex; i <= block.endIndex; ++i) {
            std::cout << lines[i] << std::endl; // Print each line in the block
        }
        break; // Break after printing the first block
    }
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////


for (size_t ii = 0; ii < lines.size(); ++ii) {
        const auto& line = lines[ii]; 
     
    TextExtentsCompat text_extents;
    pango_text_extents(cr, line.c_str(), &text_extents);

    double textX = WIDGET_PADDING - text_extents.x_bearing; // Left-aligned
    double textY = startY - text_extents.height; //changes to negative for pango

    const char* text = line.c_str();


    // Calculate the end position of the line
    double endX = textX +  text_extents.x_advance;
    double endY = textY +  text_extents.y_advance;


std::string lineStr = line; // Convert from C-style string to std::string
if (lineStr.substr(0, 3)=="```" || lineStr.substr(0, 6)=="   ```") {
    // Code to execute if the line is a code block marker
}else{
    
    double radius = 10.0;  // Adjust the radius for the rounded corners

    double rectX = textX - paddingX;
    double rectY = textY - text_extents.height - paddingY;
    double rectWidth = endX - rectX + paddingX;
    double rectHeight = text_extents.height + 2 * paddingY;


  



    if (!line.empty() && line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
      
  



 std::set<int> blocklightedIndices; 
 


bool skipLine = line == "```" ||  line == "   ```" || line.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
if (skipLine) {
    highlightskippedLineIndex++;
    
    if (line.substr(0, 3)=="```" || line.substr(0, 6)=="   ```") {  // C++20 feature, for earlier versions use line.substr(0, 3) == "```"
        in_c_syntax_block = !in_c_syntax_block; // Toggle the code block state
    }
    
    continue; // Skip further processing for this line
}

    double textY = startY + text_extents.height;
    bool isHighlighted = std::find_if(blocklightedLines.begin(), blocklightedLines.end(),
                                      [ii, &highlightskippedLineIndex](const BlocklightedLine& hl) { return hl.lineIndex == (ii - highlightskippedLineIndex); }) != blocklightedLines.end();

//printf("Line %zu: '%s', width: %f\n", ii, line.c_str(), text_extents.x_advance);

    lineWidth = text_extents.x_advance;

    if (isHighlighted || in_c_syntax_block) {
        if (!isBlockStarted) {

          //  lineWidth = text_extents.x_advance;
            currentBlock.startY = startY;
             currentBlockMaxWidth = lineWidth;
            isBlockStarted = true;
        }
       // currentBlock.endY = textY;
        currentBlock.endY = startY + text_extents.height;
        BlockLinemaxWidth  = std::max(maxWidth, lineWidth); 
        if (isHighlighted) {
            blocklightedIndices.insert(ii - highlightskippedLineIndex);
        }else {
            // Update the max width within the block
            currentBlockMaxWidth = std::max(currentBlockMaxWidth, lineWidth);
        }
        currentBlock.endY = textY;
    } else if (isBlockStarted) {
        currentBlock.lineWidth = currentBlockMaxWidth;
        highlightBlocks.push_back(currentBlock);
        isBlockStarted = false;
        currentBlockMaxWidth = 0.0; // Reset for the next block
  
    }



// If a block is still open at the end, close it
if (isBlockStarted) {
    currentBlock.lineWidth = currentBlockMaxWidth;
    highlightBlocks.push_back(currentBlock);
}





if (in_c_syntax_block) {
 // Render the leading spaces (if any)
    size_t numLeadingSpaces = 0;

// Count the number of leading spaces
while (numLeadingSpaces < line.length() && line[numLeadingSpaces] == ' ') {
    numLeadingSpaces++;
}

if (numLeadingSpaces > 0) {
    std::string leading_spaces = line.substr(0, numLeadingSpaces);
   // cairo_move_to(cr, textX, textY);
   // cairo_set_source_rgb(cr, 1, 1, 1); // Set color for leading spaces
   // pango_show_text(cr, leading_spaces.c_str());
    pango_text_extents(cr, leading_spaces.c_str(), &text_extents);
    textX += text_extents.x_advance;
}  



    TextExtentsCompat text_extents;
    pango_text_extents(cr, line.c_str(), &text_extents);
    // Prepare text line for storage
    TextLine textLine;
    textLine.text = line; // Use original line
    textLine.box.x = textX ;
    textLine.box.y = textY;//+(WIDGET_PADDING*2)  ;
    textLine.box.width = rectWidth;
    textLine.box.height = rectHeight;

    textLines.push_back(textLine);
}else{

 TextExtentsCompat text_extents;
    pango_text_extents(cr, line.c_str(), &text_extents);
    // Prepare text line for storage
    TextLine textLine;
    textLine.text = line; // Use original line
    textLine.box.x = textX ;
    textLine.box.y = textY;//+(WIDGET_PADDING*2)  ;
    textLine.box.width = rectWidth;
    textLine.box.height = rectHeight;

    textLines.push_back(textLine);



}
  
    }
   }

    //  TextExtentsCompat text_extents;
    pango_text_extents(cr, line.c_str(), &text_extents);

   // textX = WIDGET_PADDING - text_extents.x_bearing; // Left-aligned
   // textY = startY + text_extents.height;


    //TextExtentsCompat extents;
    // char* text = line.c_str();
    // Measure the text
    pango_text_extents(cr, text, &text_extents);
    // Now extents.x_advance and extents.y_advance contain the information
    // about how far the current point will move when the text is drawn

    // Calculate the end position of the line
  //  endX = textX + text_extents.x_advance;
  //  endY = textY + text_extents.y_advance;

    //cairo_set_source_rgba(cr, wf::color_t(text_color).r, wf::color_t(text_color).g, 



                         // / wf::color_t(text_color).b, wf::color_t(text_color).a);


   // cairo_move_to(cr, textX, textY);
   // pango_show_text(cr, line.c_str());
  size_t last_position = 0; // Track the position after the last rendered text
size_t token_arr_s = generate_tokens((char*)line.c_str(), line.length(), token_arr.get(), &token_arr_capacity);


 //printf("Current line:%s\n", lineStr.c_str()); // Print the current line for debugging




//std::string lineStr = line; // Convert from C-style string to std::string
if (line.substr(0, 3)=="```" || line.substr(0, 6)=="   ```") {  // Check if the line starts with "```"
    if (in_c_syntax_block) {
        // If already in a code block, this line marks the end
        in_c_syntax_block = false;
        codeBlockEndY = startY;
        codeBlockFound = false;
        // LOGI("FOUND CODE BLOCK END");
    } else {
        // If not in a code block, this line marks the beginning
        in_c_syntax_block = true;
        codeBlockStartY = startY;
        codeBlockFound = true;
        // LOGI("FOUND CODE BLOCK START");
    }
    continue; // Skip further processing for this line
}


if (in_c_syntax_block) {
 // Render the leading spaces (if any)
    size_t numLeadingSpaces = 0;

// Count the number of leading spaces
while (numLeadingSpaces < line.length() && line[numLeadingSpaces] == ' ') {
    numLeadingSpaces++;
}

if (numLeadingSpaces > 0) {
    std::string leading_spaces = line.substr(0, numLeadingSpaces);
   // cairo_move_to(cr, textX, textY);
   // cairo_set_source_rgb(cr, 1, 1, 1); // Set color for leading spaces
   // pango_show_text(cr, leading_spaces.c_str());
    pango_text_extents(cr, leading_spaces.c_str(), &text_extents);
    textX += text_extents.x_advance;
}  

for (size_t i = 0; i < token_arr_s; ++i) {
    Token& token = token_arr[i];

    // Render text between last token and current token
    if (token.index > last_position) {
        std::string non_token_str = line.substr(last_position, token.index - last_position);
        cairo_move_to(cr, textX, textY);
        cairo_set_source_rgb(cr, 1, 1, 1); // Set color for non-token text
     //   pango_show_text(cr, non_token_str.c_str());
        pango_text_extents(cr, non_token_str.c_str(), &text_extents);
        textX += text_extents.x_advance;
    }

    // Render the current token
    std::string token_str = line.substr(token.index, token.size);
 switch (token.type) {
    case Type_Keyword: 
        cairo_set_source_rgb(cr, 0.4, 0.4, 1); // Red for keywords
        break;

    case Type_Type: 
        cairo_set_source_rgb(cr, 0.6, 0, 0.6); //magenta for types
      
        break;

    case Type_Preprocessor: 
        cairo_set_source_rgb(cr, 0, 1, 0); // Green for preprocessor directives
        break;

    case Type_String: 
         cairo_set_source_rgb(cr, 0.8, 0.8, 1); // light Blue for strings
        break;

    case Type_Comment: 
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5); // Grey for comments
        break;

    case Type_Word: 
       cairo_set_source_rgb(cr, 1, 0, 0); // Red 
        break;

    default: 
        cairo_set_source_rgb(cr, 1, 1, 1); // White for any other text
        break;
}

 //   cairo_move_to(cr, textX, textY);
 //   pango_show_text(cr, token_str.c_str());
    pango_text_extents(cr, token_str.c_str(), &text_extents);
    textX += text_extents.x_advance;

    last_position = token.index + token.size; // Update the last position
}

// Render any remaining text after the last token
if (last_position < line.length()) {
    std::string remaining_str = line.substr(last_position);
 //   cairo_move_to(cr, textX, textY);
  //  cairo_set_source_rgb(cr, 1, 1, 1); // Set color for remaining non-token text
  //  pango_show_text(cr, remaining_str.c_str());
    pango_text_extents(cr, remaining_str.c_str(), &text_extents);
    textX += text_extents.x_advance;
//printf("in_c_syntax_block is %s\n", in_c_syntax_block ? "true" : "false");
}


}
    else
{  
//printf("in_c_syntax_block is %s\n", in_c_syntax_block ? "true" : "false");

    

    cairo_set_source_rgba(cr, wf::color_t(text_color).r, wf::color_t(text_color).g, 
                          wf::color_t(text_color).b, wf::color_t(text_color).a);


//    cairo_move_to(cr, textX, textY);
 //   pango_show_text(cr, line.c_str());
}


    startY += text_extents.height + WIDGET_PADDING;

    if (startY > MaxTextureHeight) {break;  }
    }

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Draw the highlight blocks
    
// Map to store current widths for each massivesyntaxblock
std::map<int, double> currentWidths;

// Other necessary variables
double incrementStep = 10.0; // Adjust this value as needed for the speed of width change
int block_end=-1;
int block_start=-1;
    int blockLineCounter = 0; // Counter for the number of blocks
    int massivesyntaxblock = -1;
const double textPaddingLeft = 10.0; // Padding from the left edge of the title bar



//int massivesyntaxblock = 0;  // Your variable
//    int blockLineCounter = 0;    // Line counter
    int lastChangeOfBlockLineCounter = 0;  // To store the last change
    int previousMassiveSyntaxBlock ;  // To store previous value


std::vector<int> lastLineOfMassiveSyntaxBlock; 




//circles.clear();//clear all apart from pressed
circles.erase(std::remove_if(circles.begin(), circles.end(), 
                                 [](const Circle& circle) { return !circle.pressed; }), 
                  circles.end());


for (const auto& block : highlightBlocks) {


 
   previousMassiveSyntaxBlock = massivesyntaxblock; 



for (const auto& syntaxBlock : syntaxBlocks) {
            if (blockLineCounter == syntaxBlock.blockLineStartIndex) {
                massivesyntaxblock++;
                // Update the range for this new massivesyntaxblock
                massivesyntaxblockRanges[massivesyntaxblock].start = blockLineCounter;
            }
             massivesyntaxblockRanges[massivesyntaxblock].end = blockLineCounter-1;
            if (blockLineCounter == syntaxBlock.blockLineEndIndex) {
                massivesyntaxblockRanges[massivesyntaxblock].end = blockLineCounter;
            }


        }
 blockLineCounter++;
}

massivesyntaxblock=-1;
blockLineCounter=0;

 auto size = output->get_screen_size();

for (const auto& block : highlightBlocks) {


 
   previousMassiveSyntaxBlock = massivesyntaxblock; 



// Check if the current line counter is at the start of a new syntax block
    for (const auto& syntaxBlock : syntaxBlocks) {
        if (blockLineCounter == syntaxBlock.blockLineStartIndex) {
            // Increment massivesyntaxblock if a new syntax block starts
            massivesyntaxblock++;
            break;
        }
    }

    double rectWidth = std::max(static_cast<double>(minWindowSize), std::min(static_cast<double>(size.width)-WIDGET_PADDING , block.lineWidth + 2 * paddingX));

    double rectX = WIDGET_PADDING - paddingX;
    double rectY = block.startY - paddingY;
    double rectHeight = block.endY - block.startY + 2 * paddingY;
    double radius = 10.0;
    double titleBarHeight = 30; // Height of the title bar

    // Adjust the Y position for the title bar and main rectangle
    double titleBarY = rectY - titleBarHeight; // Position the title bar above the main rectangle
    double mainRectY = rectY; // Start the main rectangle right below the title bar

const double headerY = titleBarY + titleBarHeight / 2; // Vertically center in the title bar


// Constants for the circles
const double circleRadius = 5.0;  // Radius of each circle
const double circleSpacing = 15.0; // Space between the circles
const double rightMargin = 15.0; // Space at the right end of the title bar
const double circleY = titleBarY + titleBarHeight / 2; // Vertically center in the title bar

// Calculate the starting X position for the first circle
const double firstCircleX = rectX + rectWidth - circleRadius - rightMargin;




if (massivesyntaxblock == previousMassiveSyntaxBlock ) {
                    // If massivesyntaxblock doesnt changes, remove the last three circles
                  /*  if (circles.size() >= 3) {
                        circles.pop_back(); // Removes the last circle
                        circles.pop_back(); // Removes the second last circle
                        circles.pop_back(); // Removes the third last circle
                    }*/
    // Remove the last three circles (red, green, blue) if they exist

if (circles.size() >= 3) {
    pastvalueBlue = circles[circles.size() - 1].pressed; // Store the pressed state of the last circle (Blue)
    circles.pop_back(); // Removes the last circle (Blue)

    pastvalueGreen = circles[circles.size() - 1].pressed; // Store the pressed state of the second last circle (Green)
    circles.pop_back(); // Removes the second last circle (Green)

    pastvalueRed = circles[circles.size() - 1].pressed; // Store the pressed state of the third last circle (Red)
    circles.pop_back(); // Removes the third last circle (Red)
}
}

Circle newCircleRed;
newCircleRed.x = firstCircleX;
newCircleRed.y = circleY;
newCircleRed.radius = circleRadius;
newCircleRed.color = "red";
newCircleRed.syntaxBlockId = massivesyntaxblock;
newCircleRed.pressed = pastvalueRed;
circles.push_back(newCircleRed);


Circle newCircleGreen;
newCircleGreen.x = firstCircleX - circleRadius - circleSpacing;
newCircleGreen.y = circleY;
newCircleGreen.radius = circleRadius;
newCircleGreen.color = "green";
newCircleGreen.syntaxBlockId = massivesyntaxblock;
newCircleGreen.pressed = pastvalueGreen;
circles.push_back(newCircleGreen);


Circle newCircleBlue;
newCircleBlue.x = firstCircleX - 2 * circleRadius - 2 * circleSpacing;
newCircleBlue.y = circleY;
newCircleBlue.radius = circleRadius;
newCircleBlue.color = "blue";
newCircleBlue.syntaxBlockId = massivesyntaxblock;
newCircleBlue.pressed = pastvalueBlue;;
circles.push_back(newCircleBlue);
/*

Circle newCircleRed;
newCircleRed.x = firstCircleX;
newCircleRed.y = circleY;
newCircleRed.radius = circleRadius;
newCircleRed.color = "red";
newCircleRed.syntaxBlockId = massivesyntaxblock;
circles.push_back(newCircleRed);


Circle newCircleGreen;
newCircleGreen.x = firstCircleX - circleRadius - circleSpacing;
newCircleGreen.y = circleY;
newCircleGreen.radius = circleRadius;
newCircleGreen.color = "green";
newCircleGreen.syntaxBlockId = massivesyntaxblock;
circles.push_back(newCircleGreen);


Circle newCircleBlue;
newCircleBlue.x = firstCircleX - 2 * circleRadius - 2 * circleSpacing;
newCircleBlue.y = circleY;
newCircleBlue.radius = circleRadius;
newCircleBlue.color = "blue";
newCircleBlue.syntaxBlockId = massivesyntaxblock;
circles.push_back(newCircleBlue);
*/
if (!DEBUG_WIDTH)
{
        // Check if the block should be skipped
        bool skipBlock = false;
        for (const auto& range : massivesyntaxblockRanges) {
            if (blockLineCounter >= range.second.start && blockLineCounter < range.second.end) {
                skipBlock = true;
                break;
            }
        }

        if (skipBlock) {
            blockLineCounter++; // Move to the next block
            continue; // Skip the rest of the loop body
        }
}


  // printf("Block #%d - rectWidth: %f\n", blockCounter, rectWidth);

if (glow_window==true)
{
cairo_save(cr); // Save the current state of cairo

// Draw the glow focused at the bottom
for (int i = glow_layers; i >= 1; --i) {
    double expand = i * glow_spread; // Calculate expansion for the current layer
    double alpha = initial_alpha * (glow_layers - i + 1); // Calculate alpha for the current layer

    cairo_set_source_rgba(cr, 1, 1, 1, alpha); // Set source to white with calculated alpha
/*

if glow_window_all_sides=true
    // Start a new path for the current glow layer
    cairo_new_path(cr);
    // Move to the start position just above the bottom-left arc
    cairo_move_to(cr, rectX - expand, rectY + rectHeight - radius);

    // Arc for the bottom left corner expanded
    cairo_arc_negative(cr, rectX + radius, rectY + rectHeight - radius, radius + expand, M_PI, M_PI / 2.0);

    // Line to the bottom right corner expanded
    cairo_line_to(cr, rectX + rectWidth - radius - expand, rectY + rectHeight + expand);

    // Arc for the bottom right corner expanded
    cairo_arc_negative(cr, rectX + rectWidth - radius, rectY + rectHeight - radius, radius + expand, M_PI / 2.0, 0);

    // Line up to close the path, but not drawing the top part
    cairo_line_to(cr, rectX + rectWidth + expand, rectY - expand); // Extend to the right and above to avoid drawing the top
    cairo_line_to(cr, rectX - expand, rectY - expand); // Extend to the left and above to complete the path without the top

    cairo_close_path(cr);
    cairo_fill(cr);*/
////////////////////////////////////////////////////////////////////////////////
/*  cairo_new_path(cr);
    // Move to the start position just above the bottom-left arc
    cairo_move_to(cr, rectX, rectY + rectHeight - radius);

    // Arc for the bottom left corner expanded
    cairo_arc_negative(cr, rectX + radius, rectY + rectHeight - radius, radius , M_PI, M_PI / 2.0);

    // Line to the bottom right corner expanded
    cairo_line_to(cr, rectX + rectWidth - radius - expand, rectY + rectHeight + expand);

    // Arc for the bottom right corner expanded
    cairo_arc_negative(cr, rectX + rectWidth - radius, rectY + rectHeight - radius, radius + expand, M_PI / 2.0, 0);
    
    // Line up to close the path, but not drawing the top part
    cairo_line_to(cr, rectX + rectWidth , rectY - expand); // Extend to the right and above to avoid drawing the top
    cairo_line_to(cr, rectX, rectY - expand); // Extend to the left and above to complete the path without the top

    cairo_close_path(cr);
    cairo_fill(cr);*/
//////////////////////////////////////////////////////////////////////////////////////    

 cairo_new_path(cr);
    // Move to the start position just above the bottom-left arc
    cairo_move_to(cr, rectX - expand, rectY + rectHeight - radius);

    // Arc for the bottom left corner expanded
    cairo_arc_negative(cr, rectX + radius, rectY + rectHeight - radius, radius + expand, M_PI, M_PI / 2.0);

    // Line to the bottom right corner expanded
    cairo_line_to(cr, rectX + rectWidth - radius - expand, rectY + rectHeight + expand);

    // Arc for the bottom right corner expanded
    cairo_arc_negative(cr, rectX + rectWidth - radius, rectY + rectHeight - radius, radius + expand, M_PI / 2.0, 0);

    // Line up to close the path, but not drawing the top part
    cairo_line_to(cr, rectX + rectWidth + expand/3, rectY - expand); // Extend to the right and above to avoid drawing the top
    cairo_line_to(cr, rectX - expand/3, rectY - expand); // Extend to the left and above to complete the path without the top

    cairo_close_path(cr);
    cairo_fill(cr);

}

cairo_restore(cr); // Restore cairo state to draw the rectangle on top

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   cairo_set_source_rgba(cr,
            wf::color_t(background_color).r,
            wf::color_t(background_color).g,
            wf::color_t(background_color).b,
            wf::color_t(background_color).a);
   cairo_new_path(cr);
// draw window background window
// Start from the top-left corner
cairo_move_to(cr, rectX, rectY);

// Line to the top-right corner
cairo_line_to(cr, rectX + rectWidth, rectY);

// Line down to the bottom right corner
cairo_line_to(cr, rectX + rectWidth, rectY + rectHeight - radius);

// Arc for the bottom right corner
cairo_arc(cr, rectX + rectWidth - radius, rectY + rectHeight - radius, radius, 0, M_PI / 2.0);

// Line to the bottom left corner
cairo_line_to(cr, rectX + radius, rectY + rectHeight);

// Arc for the bottom left corner
cairo_arc(cr, rectX + radius, rectY + rectHeight - radius, radius, M_PI / 2.0, M_PI);

// Line up to the top left corner
cairo_line_to(cr, rectX, rectY);

// Close the path
cairo_close_path(cr);

cairo_fill(cr);



if (glow_title==true)
{

cairo_save(cr); // Save the current state of cairo

// Draw the glow, excluding the bottom
for (int i = glow_layers; i >= 1; --i) {
    double expand = i * glow_spread; // Calculate expansion for the current layer
    double alpha = initial_alpha * (glow_layers - i + 1); // Calculate alpha for the current layer

    cairo_set_source_rgba(cr, 1, 1, 1, alpha); // Set source to white with calculated alpha

    // Start a new path for the current glow layer
    cairo_new_path(cr);
    cairo_move_to(cr, rectX - expand, titleBarY + titleBarHeight); // Start from the bottom-left corner, moved left and up by 'expand'
    cairo_line_to(cr, rectX - expand, titleBarY + radius); // Line up to the start of the top-left arc
    cairo_arc(cr, rectX + radius, titleBarY + radius, radius + expand, M_PI, 3.0 * M_PI / 2.0); // Top-left corner arc
    cairo_line_to(cr, rectX + rectWidth - radius - expand, titleBarY - expand); // Line to the start of the top-right arc
    cairo_arc(cr, rectX + rectWidth - radius, titleBarY + radius, radius + expand, 3.0 * M_PI / 2.0, 2 * M_PI); // Top-right corner arc
    cairo_line_to(cr, rectX + rectWidth + expand, titleBarY + titleBarHeight); // Line down to the bottom-right corner

    // Fill the path with the current color and alpha
    cairo_fill(cr);
}

cairo_restore(cr); 
}
// Draw the title bar with rounded corners at the top
   cairo_set_source_rgba(cr, 0.5, 0.5, 0.5,wf::color_t(background_color).a); // Grey color for the title bar

    cairo_new_path(cr);
    cairo_arc(cr, rectX + radius, titleBarY + radius, radius, M_PI, 3.0 * M_PI / 2.0);
    cairo_line_to(cr, rectX + rectWidth - radius, titleBarY);
    cairo_arc(cr, rectX + rectWidth - radius, titleBarY + radius, radius, 3.0 * M_PI / 2.0, 0);
    cairo_line_to(cr, rectX + rectWidth, titleBarY + titleBarHeight);
    cairo_line_to(cr, rectX, titleBarY + titleBarHeight);
    cairo_close_path(cr);
    cairo_fill(cr);

 std::string headerName = "";
   for (const auto& block : syntaxBlocks) {
    if (block.blockNumber == massivesyntaxblock) { // Check for the first syntax block   
            headerName = block.headerName;
            break;
        }
    }



 // Draw the header text in the title bar
if (!headerName.empty()) {
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, wf::color_t(background_color).a); // White color for text

    TextExtentsCompat extents;
    pango_text_extents(cr, headerName.c_str(), &extents);

    // Calculate the vertical position considering the font metrics
    double textY = titleBarY + (titleBarHeight - extents.height) / 2;

    cairo_move_to(cr, rectX + textPaddingLeft, textY); // Move to adjusted Y position
    pango_show_text(cr, headerName.c_str());
}



const double grey_r = 0.5, grey_g = 0.5, grey_b = 0.5; // Target grey color components
double circle_initial_alpha = wf::color_t(background_color).a+0.2;
int circle_glow_layers=20;
const double glow_radius = 5.0; // Radius of the glow effect beyond the circle

double glow_spread = 0.5; // How much each layer is larger than the previous one
// Draw the circles based on their pressed state
for (auto& circle : circles) {
///////////GLOW/////////////////////////////////////////////

if (glow_circle==true)
 {  // Draw the glow effect for each circle
  // Determine the circle's base color
 // Determine the circle's base color
    double base_r = 1.0, base_g = 1.0, base_b = 1.0;



    if (circle.color == "red") {
        base_r = 1.0; base_g = 0.0; base_b = 0.0;
    } else if (circle.color == "green") {
        base_r = 0.0; base_g = 1.0; base_b = 0.0;
    } else if (circle.color == "blue") {
        base_r = 0.0; base_g = 0.0; base_b = 1.0;
    }

    // Create a radial gradient from the center of the circle
    cairo_pattern_t* pat = cairo_pattern_create_radial(circle.x, circle.y, circle.radius, circle.x, circle.y, circle.radius + glow_radius);
    cairo_pattern_add_color_stop_rgba(pat, 0, base_r, base_g, base_b, initial_alpha);
    cairo_pattern_add_color_stop_rgba(pat, 1, grey_r, grey_g, grey_b, wf::color_t(background_color).a+0.2); // Fade to grey and transparent

    cairo_set_source(cr, pat);
    cairo_arc(cr, circle.x, circle.y, circle.radius + glow_radius, 0, 2 * M_PI);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

}

/*
cairo_set_source_rgba(cr,
            wf::color_t(background_color).r,
            wf::color_t(background_color).g,
            wf::color_t(background_color).b,
            wf::color_t(background_color).a);
*/




//////////////////////////////////////////////////////////////////
    if (circle.color == "red") {
        cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, wf::color_t(background_color).a); // Red color
    } else if (circle.color == "green") {
        cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, wf::color_t(background_color).a); // Green color
    } else if (circle.color == "blue") {
        cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, wf::color_t(background_color).a); // Blue color
    }

    cairo_new_path(cr);
    cairo_arc(cr, circle.x, circle.y, circle.radius, 0, 2 * M_PI);
    cairo_fill(cr);
}  

    if (DEBUG_MODE_BLOCK) {
        // Logic to print additional information
        char blockNumberText[100];
        // Assuming massivesyntaxblock starts from 0
        sprintf(blockNumberText, "BlockLINE #%d MassiveSyntaxBlock=%d width=%f", blockLineCounter, massivesyntaxblock, rectWidth);
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0); // White color for text
        double textX = rectX + rectWidth + 100; // Adjust as needed
        double textY = rectY + rectHeight; // Adjust as needed
        cairo_move_to(cr, textX, textY);
        pango_show_text(cr, blockNumberText);
    }


    blockLineCounter++; // Increment the line counter
}
/*
  // Optional: Output all recorded block ranges
    for (const auto& item : massivesyntaxblockRanges) {
        std::cout << "MassiveSyntaxBlock " << item.first 
                  << " starts at " << item.second.start 
                  << " and ends at " << item.second.end << std::endl;
    }*/
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

startY=tempStartY;
in_c_syntax_block = false;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

for (size_t ii = 0; ii < lines.size(); ++ii) {
        const auto& line = lines[ii]; 
  

     
    TextExtentsCompat text_extents;
    pango_text_extents(cr, line.c_str(), &text_extents);

    double textX = WIDGET_PADDING - text_extents.x_bearing; // Left-aligned
    double textY = startY /*+ text_extents.height*/;

    const char* text = line.c_str();


    // Calculate the end position of the line
    double endX = textX +  text_extents.x_advance;
    double endY = textY +  text_extents.y_advance;


std::string lineStr = line; // Convert from C-style string to std::string
if (lineStr.substr(0, 3)=="```" || lineStr.substr(0, 6) == "   ```") { // Check if the line starts with "```"
    // Code for handling the start or end of a code block
}else{
    
    double radius = 10.0;  // Adjust the radius for the rounded corners

    double rectX = textX - paddingX;
    double rectY = textY /*- text_extents.height*/ - paddingY;
    double rectWidth = endX - rectX + paddingX;
    double rectHeight = text_extents.height + 2 * paddingY;


  



    if (!line.empty() && line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
      


//draw background curved rectangle
if (!in_c_syntax_block) {
cairo_set_source_rgba(cr,
            wf::color_t(background_color).r,
            wf::color_t(background_color).g,
            wf::color_t(background_color).b,
            wf::color_t(background_color).a);

// Start a new path for each rectangle
    cairo_new_path(cr);

        // Top left corner
    cairo_arc(cr, rectX + radius, rectY + radius, radius, M_PI, 3.0 * M_PI / 2.0);
    // Top side
    cairo_line_to(cr, rectX + rectWidth - radius, rectY);
    // Top right corner
    cairo_arc(cr, rectX + rectWidth - radius, rectY + radius, radius, 3.0 * M_PI / 2.0, 0);
    // Right side
    cairo_line_to(cr, rectX + rectWidth, rectY + rectHeight - radius);
    // Bottom right corner
    cairo_arc(cr, rectX + rectWidth - radius, rectY + rectHeight - radius, radius, 0, M_PI / 2.0);
    // Bottom side
    cairo_line_to(cr, rectX + radius, rectY + rectHeight);
    // Bottom left corner
    cairo_arc(cr, rectX + radius, rectY + rectHeight - radius, radius, M_PI / 2.0, M_PI);
    // Left side
    cairo_close_path(cr);

    cairo_fill(cr);

  }



    TextExtentsCompat text_extents;
    pango_text_extents(cr, line.c_str(), &text_extents);
    // Prepare text line for storage
    TextLine textLine;
    textLine.text = line; // Use original line
    textLine.box.x = textX ;
    textLine.box.y = textY;//+(WIDGET_PADDING*2)  ;
    textLine.box.width = rectWidth;
    textLine.box.height = rectHeight;

    textLines.push_back(textLine);

  
    }
   }

    std::set<int> highlightedIndices; 

  // Check if the line is only whitespace
    // Check if the line is only whitespace or a specific code block delimiter
 bool skipLine = line.find_first_not_of(" \t\n\v\f\r") == std::string::npos ||
                line.substr(0, 3)=="```" || line.substr(0, 6)=="   ```"; // Check if the line is a code block marker or whitespace

// Check if the line is highlighted and should not be skipped
bool isHighlighted = std::find_if(highlightedLines.begin(), highlightedLines.end(),
                                  [ii, &highlightskippedLineIndex](const HighlightedLine& hl) { 
                                      return hl.lineIndex == (ii - highlightskippedLineIndex); 
                                  }) != highlightedLines.end();

if (skipLine) {
    highlightskippedLineIndex++;
    // Logic for skipped lines
}


 if (!skipLine) {
  if (isHighlighted) {


        double textX = WIDGET_PADDING - text_extents.x_bearing;
        double textY = startY + text_extents.height;
        double rectX = textX - paddingX;
        double rectY = textY - text_extents.height - paddingY;
        double rectWidth = text_extents.x_advance + 2 * paddingX;
        double rectHeight = text_extents.height + 2 * paddingY;

if (in_c_syntax_block) {
     // Render the leading spaces (if any)
    size_t numLeadingSpaces = 0;

    // Count the number of leading spaces
    while (numLeadingSpaces < line.length() && line[numLeadingSpaces] == ' ') {
        numLeadingSpaces++;
    }

    if (numLeadingSpaces > 0) {
        std::string leading_spaces = line.substr(0, numLeadingSpaces);
       // cairo_move_to(cr, textX, textY);
  
        pango_text_extents(cr, leading_spaces.c_str(), &text_extents);
        rectX += text_extents.x_advance;
    }  
}
         //   cairo_set_source_rgba(cr, 1.0, 0.5, 0.0, 0.2); // Orange color for highlight
         //   cairo_rectangle(cr, rectX, rectY, rectWidth, rectHeight);


    cairo_set_source_rgba(cr, 0.0, 0.0, 0.8, 0.7); // Example: Orange color for highlight
    cairo_new_path(cr);

    double radius = 10; // Adjust the radius for the curvature of the corners

    // Top left corner
    cairo_arc(cr, rectX + radius, rectY + radius, radius, M_PI, 3.0 * M_PI / 2.0);
    // Top side
    cairo_line_to(cr, rectX + rectWidth - radius, rectY );
    // Top right corner
    cairo_arc(cr, rectX + rectWidth - radius, rectY + radius, radius, 3.0 * M_PI / 2.0, 0);
    // Right side
    cairo_line_to(cr, rectX + rectWidth, rectY  + rectHeight - radius);
    // Bottom right corner
    cairo_arc(cr, rectX + rectWidth - radius, rectY   + rectHeight - radius, radius, 0, M_PI / 2.0);
    // Bottom side
    cairo_line_to(cr, rectX + radius, rectY + rectHeight);
    // Bottom left corner
    cairo_arc(cr, rectX + radius, rectY + rectHeight - radius, radius, M_PI / 2.0, M_PI);
    // Left side
    cairo_line_to(cr, rectX, rectY + radius);



    cairo_close_path(cr);
    cairo_fill(cr);

            cairo_fill(cr);
            highlightedIndices.insert(ii - highlightskippedLineIndex);
           // adjustedIndex++; 
        }
}
    //  TextExtentsCompat text_extents;
    pango_text_extents(cr, line.c_str(), &text_extents);

   // textX = WIDGET_PADDING - text_extents.x_bearing; // Left-aligned
   // textY = startY + text_extents.height;


    //TextExtentsCompat extents;
    // char* text = line.c_str();
    // Measure the text
    pango_text_extents(cr, text, &text_extents);
    // Now extents.x_advance and extents.y_advance contain the information
    // about how far the current point will move when the text is drawn

    // Calculate the end position of the line
  //  endX = textX + text_extents.x_advance;
  //  endY = textY + text_extents.y_advance;

    //cairo_set_source_rgba(cr, wf::color_t(text_color).r, wf::color_t(text_color).g, 



                         // / wf::color_t(text_color).b, wf::color_t(text_color).a);


   // cairo_move_to(cr, textX, textY);
   // pango_show_text(cr, line.c_str());
  size_t last_position = 0; // Track the position after the last rendered text
size_t token_arr_s = generate_tokens((char*)line.c_str(), line.length(), token_arr.get(), &token_arr_capacity);


 //printf("Current line:%s\n", lineStr.c_str()); // Print the current line for debugging


//std::string lineStr = line; // Convert from C-style string to std::string
if (line.substr(0, 3)=="```" || line.substr(0, 6)=="   ```") { // Check if the line starts with "```"
    in_c_syntax_block = !in_c_syntax_block; // Toggle the state of being inside a code block

    if (in_c_syntax_block) {
        // If just entered a code block
        codeBlockStartY = startY;
        codeBlockFound = true;
        // LOGI("FOUND CODE BLOCK START");
    } else {
        // If just exited a code block
        codeBlockEndY = startY;
        codeBlockFound = false;
        // LOGI("FOUND CODE BLOCK END");
    }
    continue; // Skip further processing for this line
}



if (in_c_syntax_block) {

    
     // Render the leading spaces (if any)
    size_t numLeadingSpaces = 0;

    // Count the number of leading spaces
    while (numLeadingSpaces < line.length() && line[numLeadingSpaces] == ' ') {
        numLeadingSpaces++;
    }

    if (numLeadingSpaces > 0) {
        std::string leading_spaces = line.substr(0, numLeadingSpaces);
       // cairo_move_to(cr, textX, textY);
  
        pango_text_extents(cr, leading_spaces.c_str(), &text_extents);
        textX += text_extents.x_advance;
    }  


for (size_t i = 0; i < token_arr_s; ++i) {
    Token& token = token_arr[i];

    // Render text between last token and current token
    if (token.index > last_position) {
        std::string non_token_str = line.substr(last_position, token.index - last_position);
        cairo_move_to(cr, textX, textY);
        cairo_set_source_rgb(cr, 1, 1, 1); // Set color for non-token text
        pango_show_text(cr, non_token_str.c_str());
        pango_text_extents(cr, non_token_str.c_str(), &text_extents);
        textX += text_extents.x_advance;
    }

    // Render the current token
    std::string token_str = line.substr(token.index, token.size);
 switch (token.type) {
    case Type_Keyword: 
        cairo_set_source_rgb(cr, 0.4, 0.4, 1); // Red for keywords
        break;

    case Type_Type: 
        cairo_set_source_rgb(cr, 0.6, 0, 0.6); //magenta for types
      
        break;

    case Type_Preprocessor: 
        cairo_set_source_rgb(cr, 0, 1, 0); // Green for preprocessor directives
        break;

    case Type_String: 
         cairo_set_source_rgb(cr, 0.8, 0.8, 1); // light Blue for strings
        break;

    case Type_Comment: 
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5); // Grey for comments
        break;

    case Type_Word: 
       cairo_set_source_rgb(cr, 1, 0, 0); // Red 
        break;

    default: 
        cairo_set_source_rgb(cr, 1, 1, 1); // White for any other text
        break;
}

    cairo_move_to(cr, textX, textY);
    pango_show_text(cr, token_str.c_str());
    pango_text_extents(cr, token_str.c_str(), &text_extents);
    textX += text_extents.x_advance;

    last_position = token.index + token.size; // Update the last position
}

// Render any remaining text after the last token
if (last_position < line.length()) {
    std::string remaining_str = line.substr(last_position);
    cairo_move_to(cr, textX, textY);
    cairo_set_source_rgb(cr, 1, 1, 1); // Set color for remaining non-token text
    pango_show_text(cr, remaining_str.c_str());
    pango_text_extents(cr, remaining_str.c_str(), &text_extents);
    textX += text_extents.x_advance;
//printf("in_c_syntax_block is %s\n", in_c_syntax_block ? "true" : "false");
}


}
    else
{  
//printf("in_c_syntax_block is %s\n", in_c_syntax_block ? "true" : "false");

    

    cairo_set_source_rgba(cr, wf::color_t(text_color).r, wf::color_t(text_color).g, 
                          wf::color_t(text_color).b, wf::color_t(text_color).a);


    cairo_move_to(cr, textX, textY);
    pango_show_text(cr, line.c_str());}


    startY += text_extents.height + WIDGET_PADDING;

    if (startY > MaxTextureHeight) {break;  }
    }

//reset syntax block if at the end of file
in_c_syntax_block = false;


    cairo_stroke(cr);
////////////CIRCLES///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//render yellow circles if pressed 

    for (auto& circle : circles) {
    if (circle.pressed) { 
     
    
//make the red/green/blue circles yellow when pressed
if (circle.color == "red") {
        cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, wf::color_t(background_color).a); // Red color
    } else if (circle.color == "green") {
        cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, wf::color_t(background_color).a); // Green color
    } else if (circle.color == "blue") {
        cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, wf::color_t(background_color).a); // Blue color
    }

    cairo_new_path(cr);
    cairo_arc(cr, circle.x, circle.y, circle.radius, 0, 2 * M_PI);
    cairo_fill(cr);

    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







}




sk_sp<SkSurface> CreateSurface(int width, int height)
{
   /* m_dc = GetDC(m_WHandle);

    if (!(m_hRC = CreateWGLContext(m_dc)))
        return nullptr;
*/
constexpr int nSampleCount = 1;
    constexpr int nStencilBits = 8;

   

  //auto  m_BackendContext = GrGLMakeNativeInterface();
 // auto  m_Context = GrDirectContexts::MakeGL(m_BackendContext, {});


// Check for context creation failure
   if (!directContext) {
    fprintf(stderr, "Failed to create GrDirectContext.\n");
    return nullptr;
}else{printf("create direct\n");}

     // Create an OpenGL framebuffer object.
    GLuint framebufferId;
    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

    // Create a texture to use as the framebuffer's color attachment.
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Attach the texture to the framebuffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    // Check framebuffer completeness.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Failed to create complete framebuffer.\n");
        glDeleteTextures(1, &textureId);
        glDeleteFramebuffers(1, &framebufferId);
//        return nullptr;
    }



    // Setup GrGLFramebufferInfo.
    GrGLFramebufferInfo framebufferInfo = {};
    framebufferInfo.fFBOID = framebufferId;
    framebufferInfo.fFormat = GL_RGBA8;

    //m_BackendContext->fFunctions.fGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);
    GLuint currentlyBoundFBO;



     interface->fFunctions.fGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&currentlyBoundFBO));
/*
    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = buffer;
    fbInfo.fFormat = GR_GL_RGBA8;
*/
    SkSurfaceProps props(0, kRGB_H_SkPixelGeometry);
//SkSurfaceProps props;
//GrBackendRenderTarget backendRT(width, height, nSampleCount, nStencilBits, fbInfo);

 GrBackendRenderTarget backendRT = GrBackendRenderTargets::MakeGL(width, height, nSampleCount, nStencilBits, framebufferInfo);



    return SkSurfaces::WrapBackendRenderTarget(directContext.get(), backendRT,kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, nullptr, &props);
}

 





    
 


void wrap_text_to_fit_screen(cairo_t *cr, std::vector<std::string>& lines, double maxWidth) {
    std::vector<std::string> wrappedLines;
    bool in_c_syntax_block = false;


    for (const auto& line : lines) {
        if (line.substr(0, 3) == "```" || line.substr(0, 6) == "   ```") {
            in_c_syntax_block = !in_c_syntax_block;

            if (in_c_syntax_block) {
                // If just entered a code block
            //    wrappedLines.push_back(""); 
            //    wrappedLines.push_back(""); 
            //    wrappedLines.push_back(""); 
                wrappedLines.push_back(""); 
                wrappedLines.push_back(line); // Add the line as-is
            } else {
                // If just exited a code block
                wrappedLines.push_back(line); // Add the line as-is
            //    wrappedLines.push_back(""); 
            //    wrappedLines.push_back(""); 
            //    wrappedLines.push_back(""); 
                wrappedLines.push_back(""); 
            }
 
            continue;
        }

        if (in_c_syntax_block) {
            wrappedLines.push_back(line);

        } else {
            std::istringstream wordsStream(line);
            std::string word;
            std::string newLine;

            while (wordsStream >> word) {
                std::string testLine = newLine + (newLine.empty() ? "" : " ") + word;
                TextExtentsCompat extents;
                pango_text_extents(cr, testLine.c_str(), &extents);

                if (extents.width <= maxWidth) {
                    newLine = testLine;
                } else {
                    if (!newLine.empty()) {
                        wrappedLines.push_back(newLine);
                  }
                    newLine = word;
                }
            }

            if (!newLine.empty()) {
                wrappedLines.push_back(newLine);

            }
        }
    }


    lines = std::move(wrappedLines);
    in_c_syntax_block = false; // Reset in_c_syntax_block at the end
}

  // Fixed start time


    void render_cairo_GPT()
    {
//LOGI ("render_cairo_GPT");
        auto cws = output->wset()->get_current_workspace();
        auto wsn     = workspaces[cws.x][cws.y]->workspace;

          if (!GPTrequestFinished) {
                // HTTP request not finished yet, skip rendering
                return;
            }

        auto workarea = output->workarea->get_workarea();
        auto size = output->get_screen_size();
        const int MaxTextureHeight = size.height-workarea.y;  


        in_c_syntax_block = false;
        double maxWidth = 0.0;

        const char *name = wsn->name.c_str();
        double radius = background_radius;
        cairo_t *cr   = wsn->cr;

//        cairo_clear(cr);

        double totalTextHeight=0;

        double currentY = WIDGET_PADDING + workarea.y;
        textLines.clear();

  //  maxWidthScreen = size.width; // Or however you get the max width
    wrap_text_to_fit_screen(cr, lines, size.width/3);



    calculate_text_dimensions(cr, lines, VisibleTextHeight, totalTextHeight, maxWidth);
    VisibleTextHeight = std::max(std::min(VisibleTextHeight + WIDGET_PADDING, static_cast<double>(MaxTextureHeight)), 10.0);
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    wsn->rect.height =  MaxTextureHeight ;//VisibleTextHeight; for just the text height 
    wsn->rect.width = std::max(static_cast<int>(size.width), 10); // Adjust width based on maxWidth
///////////////////////////////////////////////////////////////////////////////////////////////////////////

Yscroller=(scroll_offset/1.2*(totalTextHeight/VisibleTextHeight));
//printf("scroll_offset: %f\n", scroll_offset);
//printf("VisibleTextHeight: %f\n", VisibleTextHeight);
//printf("totalTextHeight : %f\n", totalTextHeight );
//printf("Yscroller: %f\n", Yscroller);

    // Begin rendering the text
    double startY = WIDGET_PADDING -Yscroller; // Start drawing text from this Y position

    // Recreate the Cairo surface with new dimensions
    cairo_clear(cr);
    
    cairo_surface_destroy(wsn->cairo_surface);
    wsn->cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wsn->rect.width, wsn->rect.height);
   

    cr = cairo_create(wsn->cairo_surface);

if (mode=="chatGPT")
{    
PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_RENDER);
}

if (mode=="bashGPT")
{    
PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_BASH);
}

// Clear the existing content
 /*   cairo_clear(cr);

    // Set the source color to green (R, G, B, A)
    cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, 0.5); // Green color

    // Paint the entire surface with the current source color (green)
    cairo_paint(cr);
*/
    // Set the source color for text rendering
    
    cairo_set_source_rgba(cr, wf::color_t(text_color).r, wf::color_t(text_color).g, 
                          wf::color_t(text_color).b, wf::color_t(text_color).a);

bool in_c_syntax_block = false; // Initialize this as needed


render_lines(cr, lines, startY, MaxTextureHeight, in_c_syntax_block, text_color, background_color, textLines,maxWidth);

dock_cairo_GPT(cr);
in_c_syntax_block = false;

/*
// Iterate through the circles to draw their detection rectangles
for (const auto& circle : circles) {
    // Calculate the bounds of the detection area
    double left = circle.x - circle.radius;
    double top = circle.y - circle.radius;
    double width = circle.radius * 2;
    double height = circle.radius * 2;

    // Set color for the detection rectangle (e.g., semi-transparent black)
    cairo_set_source_rgba(cr, 1, 1, 1, 0.5); 
    // Draw the rectangle
    cairo_rectangle(cr, left, top, width, height);
    cairo_stroke(cr); // Only outline the rectangle, do not fill
}
*/





//scroll bar
///////////////////////////////////////////////////////////////////
// Scrollbar dimensions
    scrollbarWidth = 5.0;  // Width of the scrollbar
    const double scrollbarMargin = 0.0;  // Space between text and scrollbar

    // Calculate the scrollbar thumb properties
//    double thumbHeight = std::max(visibleHeight * (visibleHeight / VisibleTextHeight), 20.0);  // Minimum thumb height

    double thumbHeight = 100;;  // Minimum thumb height

  //  double thumbPosition = (visibleHeight - thumbHeight) * (scroll_offset / (double)(VisibleTextHeight - visibleHeight));
    double  thumbPosition = scroll_offset;

// printf("thumbPosition=%f\n",thumbPosition );
    // Draw scrollbar
    cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.8);  // Scrollbar color (grey, semi-transparent)
  //  cairo_rectangle(cr, wsn->rect.width - scrollbarWidth - scrollbarMargin, thumbPosition,
    //                scrollbarWidth, thumbHeight);
  cairo_rectangle(cr, scrollbarMargin, thumbPosition, scrollbarWidth, thumbHeight);


    cairo_fill(cr);


//scrollbarX = wsn->rect.width - scrollbarWidth; // X position of the scrollbar
scrollbarY = workarea.y+thumbPosition ; // Y position of the scrollbar
scrollbarHeight =thumbHeight;


// Adjusted calculation for scrollbarX to match the rendering position
scrollbarX = scrollbarMargin; // This sets the X position of the scrollbar to the left margin

// Draw scrollbar
cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.8);  // Scrollbar color (grey, semi-transparent)
cairo_rectangle(cr, scrollbarX, thumbPosition, scrollbarWidth, thumbHeight);


 // Print the scrollbar dimensions
 //   printf("Scrollbar Dimensions - X: %f, Y: %f, Width: %f, Height: %f\n", 
   //        scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight);


//////////////////////////////////////////////////////////////////////////
        OpenGL::render_begin();
        cairo_surface_upload_to_texture(wsn->cairo_surface, *wsn->texture);
        OpenGL::render_end();
  //      GPTrequestFinished=false;
        cairo_destroy(cr);
}


std::string scrollText(const std::string& text, size_t max_length) {
    static size_t pos = 0;
    std::string scrolled_text = text;
    
    // Ensure the text is at least as long as max_length by padding with spaces if needed
    if (scrolled_text.length() < max_length) {
        scrolled_text += std::string(max_length - scrolled_text.length(), ' ');
    }

    // Create the scrolling effect
    scrolled_text = scrolled_text.substr(1, max_length - 1) + scrolled_text[0];

    // Update the position for the next call
    pos = (pos + 1) % max_length;

    return scrolled_text;
}
size_t offset = -1;
size_t max_length =100;

/*
std::string scrollText(const std::string& text, size_t max_length, size_t offset) {
    std::string scrolled_text = text;
    
    // Ensure the text is at least as long as max_length by padding with spaces if needed
    if (scrolled_text.length() < max_length) {
        scrolled_text += std::string(max_length - scrolled_text.length(), ' ');
    }

    // Apply the offset to create the scrolling effect
    size_t actual_offset = offset % max_length;
    scrolled_text = scrolled_text.substr(actual_offset) + scrolled_text.substr(0, actual_offset);

    return scrolled_text;
}*/




std::string scrollText(const std::string& text, size_t max_length, size_t offset) {
    std::string scrolled_text = text;
    
    // Ensure the text is at least as long as max_length by padding with spaces if needed
    if (scrolled_text.length() < max_length) {
        scrolled_text += std::string(max_length - scrolled_text.length(), ' ');
    }

    // Apply the offset to create the scrolling effect in the opposite direction
    size_t actual_offset = offset % max_length;
    scrolled_text = scrolled_text.substr(max_length - actual_offset) + scrolled_text.substr(0, max_length - actual_offset);

    return scrolled_text;
}
size_t time_offset = 30;



GLuint CreateOpenGLTexture(int width, int height) {
    
    glGenTextures(1, &textureID); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture as a 2D texture

    // Set up texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Allocate texture storage
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  //  glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

// Check for errors
GLenum err = glGetError();
if (err != GL_NO_ERROR) {
    std::cerr << "Error during texture setup: " << std::hex << err << std::endl;
    // Handle error appropriately
}

    return textureID; // Return the texture ID
}


std::unique_ptr<SkSurface> InitializeSkiaAndCreateSurface() {
    auto size = output->get_screen_size();
    auto glInterface = GrGLMakeNativeInterface();
    auto context = GrDirectContexts::MakeGL(glInterface);
    if (!context) {
        // Handle error: Direct context creation failed
        return nullptr;
    }

    // Assuming you have created an OpenGL texture as before
   
    GLuint textureID = CreateOpenGLTexture(size.width, size.height);

if (textureID == 0) {
    // Texture creation failed, handle the error
    std::cerr << "Failed to create OpenGL texture." << std::endl;
} else {
    // Success, you can proceed
}


GLenum err = glGetError();
if (err != GL_NO_ERROR) {
    // Handle the error. You can convert err to a string for more readable output
    std::cerr << "OpenGL error: " << std::hex << err << std::endl;
}
    SkColorType colorType = kRGBA_8888_SkColorType; // Example color type

    GrGLTextureInfo textureInfo = {GL_TEXTURE_2D, textureID, GL_RGBA8};
    GrBackendFormat backendFormat = context->defaultBackendFormat(colorType, GrRenderable::kYes);

    // Create the backend texture
    GrBackendTexture backendTexture = context->createBackendTexture(size.width, size.height, backendFormat, skgpu::Mipmapped::kNo, GrRenderable::kNo);

  

if (!backendTexture.isValid()) {
    std::cerr << "Failed to create Skia backend texture." << std::endl;
    return nullptr;
}
    GrColorType grColorType = SkColorTypeToGrColorType(kRGBA_8888_SkColorType);
    SkSurfaceProps surfaceProps; // Default surface properties

    // Use SkSurface instead of skgpu::ganesh::SurfaceDrawContext
    auto surfaceDrawContext = SurfaceDrawContext::MakeFromBackendTexture(
        context.get(),
        grColorType,
        SkColorSpace::MakeSRGB(),
        backendTexture,
        1, // Sample count for MSAA; use 1 for no MSAA
        kTopLeft_GrSurfaceOrigin,
        surfaceProps,
        nullptr // You can pass nullptr for the releaseHelper if you don't need it
    );

    if (!surfaceDrawContext) {
        // Handle error: SurfaceDrawContext creation failed
        return nullptr;
    }

 // Ge    // Create an SkSurface from the GrBackendRenderTarget
 //    GrBackendRenderTarget backendRenderTarget(textureWidth, textureHeight, 0, skgpu::Mipmapped::kNo, textureInfo);
 //  GrBackendRenderTarget backendRenderTarget(textureWidth, textureHeight, 0, GrMipMapped::kNo, textureInfo);

//GrGLFramebufferInfo framebufferInfo(/* provide the necessary parameters for GrGLFramebufferInfo */);
//GrGLFramebufferInfo framebufferInfo; 

    // Retrieving the current OpenGL framebuffer ID
    GrGLuint framebufferId;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&target_fb.fb));

    GrGLFramebufferInfo framebufferInfo = {target_fb.fb, GL_RGBA8};


/*
glGenFramebuffers(1, &framebufferId);
glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

// Attach the texture as a color attachment to the framebuffer
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

// Check if the framebuffer is complete
if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
 printf("gothere\n");   // Handle framebuffer setup failure
}

*/




    int sampleCount = 1; // Typically 1 if you're not using multisampling
    int stencilBits = 0; // Assuming no stencil buffer is required

    // Use GrBackendRenderTargets::MakeGL to create a GrBackendRenderTarget
    GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(
        size.width, size.height, sampleCount, stencilBits, framebufferInfo);
/*
    // Create a backend render target using the framebuffer info
    GrBackendRenderTarget backendRenderTarget(
        textureWidth,
        textureHeight,
        0, // Sample count for multisampling, use 1 if you don't need multisampling
        0, // Stencil bits, use 0 if you don't need a stencil buffer
        framebufferInfo);
*/
    // Create an SkSurface from the SurfaceDrawContext
    auto skSurface = SkSurfaces::WrapBackendRenderTarget(
        context.get(),
        backendRenderTarget,
        kTopLeft_GrSurfaceOrigin,
        colorType,
        SkColorSpace::MakeSRGB(),
        &surfaceProps
    );

  if (!skSurface) {
    std::cerr << "Failed to create SkSurface from backend render target." << std::endl;
   // context->deleteBackendRenderTarget(backendRenderTarget); // Clean up the backend render target
   // context->deleteBackendTexture(backendTexture); // Clean up the backend texture
    return nullptr;
}

    return std::unique_ptr<SkSurface>(skSurface.release());

}











  bool setupDone = false;
sk_sp<SkSurface> skSurface ;

void setupframebuffer(){ {
auto size = output->get_screen_size();

    auto glInterface = GrGLMakeNativeInterface();
    auto context = GrDirectContexts::MakeGL(glInterface);
  

    // Assuming you have created an OpenGL texture as before
    
 //   GLuint textureID = CreateOpenGLTexture(textureWidth, textureHeight);

 // Generate OpenGL texture using the CreateOpenGLTexture function


GLenum error = glGetError();
if (error != GL_NO_ERROR) {
    switch (error) {
        case GL_INVALID_ENUM:
            std::cerr << "OpenGL ES Error: GL_INVALID_ENUM - An unacceptable value is specified for an enumerated argument." << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cerr << "OpenGL ES Error: GL_INVALID_VALUE - A numeric argument is out of range." << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cerr << "OpenGL ES Error: GL_INVALID_OPERATION - The specified operation is not allowed in the current state." << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "OpenGL ES Error: GL_OUT_OF_MEMORY - There is not enough memory left to execute the command." << std::endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << "OpenGL ES Error: GL_INVALID_FRAMEBUFFER_OPERATION - The framebuffer object is not complete." << std::endl;
            break;
        // Include other cases as necessary for OpenGL ES
        default:
            std::cerr << "OpenGL ES Error: Unknown error code " << error << std::endl;
    }
} else {
    std::cerr << "No OpenGL ES Error detected." << std::endl;
}

const char* versionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
if (versionStr != nullptr) {
    std::cout << "OpenGL ES Version: " << versionStr << std::endl;
} else {
    std::cerr << "Failed to retrieve OpenGL ES version information." << std::endl;
}

;

    // Retrieving the current OpenGL framebuffer ID
  
// Create the framebuffer

// OpenGL context should be ready here, but verify this is the case
        // Create the framebuffer
// Create the framebuffer object (FBO)
        glGenFramebuffers(1, &framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

        // Attach the texture as a color attachment to the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
//sleep(2);
   glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&framebufferId));

        // Check for framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete." << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer on failure
         //   return;
        }

        //glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer until needed
        






    GrGLFramebufferInfo framebufferInfo = {framebufferId, GL_RGBA8};
// Bind the framebuffer
// Bind the framebuffer
glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

// Query attachment parameters for GL_COLOR_ATTACHMENT0
GLint attachmentType, attachmentObject;
glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &attachmentType);
glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attachmentObject);

// Check if the attachment type is correct
if (attachmentType != GL_TEXTURE) {
    // Handle incorrect attachment type
    std::cerr << "Framebuffer attachment is not a texture." << std::endl;
}

// Check if the attachment object name matches the texture ID
if (attachmentObject != textureID) {
    // Handle incorrect attachment object
    std::cerr << "Framebuffer attachment is not the expected texture." << std::endl;
}

// Unbind the framebuffer
//glBindFramebuffer(GL_FRAMEBUFFER, 0);


glViewport(0, 0,  size.width,  size.height);

// Clear the screen
glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);

GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
switch (status) {
    case GL_FRAMEBUFFER_COMPLETE:
        // Framebuffer is complete.
        std::cout << "Framebuffer is complete." << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cerr << "Framebuffer incomplete: Attachment is incomplete." << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cerr << "Framebuffer incomplete: No images are attached to the framebuffer." << std::endl;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cerr << "Framebuffer incomplete: Unsupported by framebuffer attachment." << std::endl;
        break;
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        std::cerr << "Framebuffer incomplete: Attached images must have the same dimensions." << std::endl;
        break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
        std::cerr << "Framebuffer incomplete: Attached images must have the same format." << std::endl;
        break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        std::cerr << "Framebuffer incomplete: All attachments must have the same number of multisample samples." << std::endl;
        break;
#endif
    // Additional cases for OpenGL ES 3.0 and above
#ifdef GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        std::cerr << "Framebuffer incomplete: If any framebuffer attachment is layered, all attachments must be layered." << std::endl;
        break;
#endif
    default:
        std::cerr << "Framebuffer incomplete: Unknown reason." << std::endl;
}



/*
if (context->backend() != GrBackendApi::kOpenGL_GrBackend ||
            !static_cast<const GrGLCaps*>(context->priv().caps())->rectangleTextureSupport()) {
            printf(" This GM requires an OpenGL context that supports texture rectangles.\n");
         //   return DrawResult::kSkip;
        }
*/
    SkColorType colorType = kRGBA_8888_SkColorType; // Example color type

    GrGLTextureInfo textureInfo = {GL_TEXTURE_2D, textureID, GL_RGBA8};
  //  GrBackendFormat backendFormat = context->defaultBackendFormat(kRGBA_8888_SkColorType,
                                                                    //  GrRenderable::kYes);//context->defaultBackendFormat(colorType, GrRenderable::kYes);
 GrBackendFormat backendFormat = GrBackendFormats::MakeGL(colorType, GR_GL_TEXTURE_RECTANGLE);
    // Create the backend texture
    GrBackendTexture backendTexture = context->createBackendTexture(size.width, size.height, backendFormat , skgpu::Mipmapped::kYes, GrRenderable::kYes, GrProtected::kNo, "CreateRectangleTextureImage");

/*  auto bet = dContext->createBackendTexture(content.width(),
                                                  content.height(),
                                                  format,
                                                  skgpu::Mipmapped::kNo,n
                                                  GrRenderable::kNo,
                                                  GrProtected::kNo,
                                                  "CreateRectangleTextureImage");
        if (!bet.isValid()) {*/

    GrColorType grColorType = SkColorTypeToGrColorType(kRGBA_8888_SkColorType);
    SkSurfaceProps surfaceProps; // Default surface properties

    // Use SkSurface instead of skgpu::ganesh::SurfaceDrawContext
    auto surfaceDrawContext = SurfaceDrawContext::MakeFromBackendTexture(
        context.get(),
        grColorType,
        SkColorSpace::MakeSRGB(),
        backendTexture,
        1, // Sample count for MSAA; use 1 for no MSAA
        kTopLeft_GrSurfaceOrigin,
        surfaceProps,
        nullptr // You can pass nullptr for the releaseHelper if you don't need it
    );

 
 // Ge    // Create an SkSurface from the GrBackendRenderTarget
 //    GrBackendRenderTarget backendRenderTarget(textureWidth, textureHeight, 0, skgpu::Mipmapped::kNo, textureInfo);
 //  GrBackendRenderTarget backendRenderTarget(textureWidth, textureHeight, 0, GrMipMapped::kNo, textureInfo);

//GrGLFramebufferInfo framebufferInfo(/* provide the necessary parameters for GrGLFramebufferInfo */);
//GrGLFramebufferInfo framebufferInfo; 







    int sampleCount = 1; // Typically 1 if you're not using multisampling
    int stencilBits = 0; // Assuming no stencil buffer is required

    // Use GrBackendRenderTargets::MakeGL to create a GrBackendRenderTarget
    GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(
        size.width, size.height, sampleCount, stencilBits, framebufferInfo);
/*
    // Create a backend render target using the framebuffer info
    GrBackendRenderTarget backendRenderTarget(
        textureWidth,
        textureHeight,
        0, // Sample count for multisampling, use 1 if you don't need multisampling
        0, // Stencil bits, use 0 if you don't need a stencil buffer
        framebufferInfo);
*/
    // Create an SkSurface from the SurfaceDrawContext
    skSurface = SkSurfaces::WrapBackendRenderTarget(
        context.get(),
        backendRenderTarget,
        kTopLeft_GrSurfaceOrigin,
        colorType,
        SkColorSpace::MakeSRGB(),
        &surfaceProps
    );

//    setupDone=true;
   
    // Attempt to initialize Skia and create a GPU-accelerated surface

  //  auto skSurface = InitializeSkiaAndCreateSurface();
   
   


// Example Skia setup for a raster surface


// Your Skia drawing commands here
// ...

// Ensure all Skia commands are flushed
//canvas->flush();
    // Additional drawing operations can follow here...

    // Flush the drawing operations to the surface
   // canvas->flush();

    // Note: Depending on your application, additional steps may be required to present
    // the drawn content to the screen, such as swapping buffers for an OpenGL context.
}
}




void writePNG(const SkPixmap& pixmap, const char* filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return; // Handle error

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) return; // Handle error

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return; // Handle error
    }

    // Error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return; // Handle error
    }

    png_init_io(png_ptr, fp);

    // Set PNG info. Here we assume 8-bit color depth and RGBA format.
    png_set_IHDR(png_ptr, info_ptr, pixmap.width(), pixmap.height(),
                 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    // Write image data
    auto* addr = static_cast<const png_byte*>(pixmap.addr());
    int stride = pixmap.rowBytes();
    for (int y = 0; y < pixmap.height(); ++y) {
        png_write_row(png_ptr, addr + y * stride);
    }

    // Finish writing
    png_write_end(png_ptr, nullptr);

    // Cleanup
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}



//sk_sp<SkSurface> rasterSurface;
GLuint framebufferId;;

bool set_up_complete=false;

GLuint tex;
wf::texture_t wfTexture;
 wf::simple_texture_t bench_tex;
//GLuint textureID; 

void loading_cairo_GPT() {
    /*
SkCanvas* canvas = sSurface->getCanvas(); // Get the SkCanvas to draw on.
canvas->clear(SK_ColorWHITE); // Clear the surface.
canvas->clear(SK_ColorWHITE); 
canvas->clear(SK_ColorWHITE); 
 // if (sContext) { // Use the existing directContext without re-creating it
        sContext->flush();
   // }
auto size = output->get_screen_size();
     //   workspace->texture = std::make_unique<wf::simple_texture_t>();
 auto cws = output->wset()->get_current_workspace();
 auto wsn     = workspaces[cws.x][cws.y]->workspace;

wsn->texture = std::make_unique<wf::simple_texture_t>();
wsn->rect.width  =size.width;
wsn->rect.height = size.height;*/
} 

void loading_cairo_GPT3() {
auto size = output->get_screen_size();

//GLuint textureSKIA = CreateOpenGLTexture(size.width, size.height);

//SkImageInfo imageInfo = SkImageInfo::Make(size.width, size.height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);

//auto rasterSurface = SkSurfaces::Raster(imageInfo);
//auto canvas = rasterSurface->getCanvas();


////////working cpu skia////////////////////////////////////////////////////////////////////////////////////////
//auto imageInfo = SkImageInfo::MakeN32Premul(size.width,size.height);

// Then, create a raster SkSurface using the SkImageInfo
//auto surface = SkSurfaces::Raster(imageInfo);
////////////////////////////////////////////////////////////////////////////////////////////////
// Check if the surface was created successfully

// Create a render target compatible with the GPU context.
// imageInfo = SkImageInfo::MakeN32Premul(size.width, size.height);
// surface = SkSurfaces::RenderTarget(directContext.get(), skgpu::Budgeted::kNo, imageInfo);
//auto surface = SkSurface::MakeFromBackendTexture(directContext.get(), backendTexture, kTopLeft_GrSurfaceOrigin, sampleCount, colorType, nullptr, nullptr);


//sk_sp<SkSurface> surface2 = CreateSurface(size.width, size.height);


/*
//surface = surface2;
if (!sSurface) {
    // Handle surface creation failure
printf("surface not created101\n");
}else{printf("surface created 101\n");}



//auto directContext = GrDirectContexts::MakeGL();
if (!sContext ) {
    // Handle context creation failure
printf("directcontext not created\n");
}else{printf("bingo directcontext is created\n");}
*/
// OpenGL::render_begin(target_fb);
SkCanvas* canvas = sSurface->getCanvas(); // Get the SkCanvas to draw on.
canvas->clear(SK_ColorWHITE); // Clear the surface.


/*
SkPaint paint;
paint.setColor(SK_ColorGREEN); // Set the color of the circle

paint.setAntiAlias(true); // Enable anti-aliasing for smoother edges

// Coordinates for the center of the circle
float centerX = 250;
float centerY = 250;

// Radius of the circle
float radius = 100;
// Draw the circle on the canvas
SkColor colors[] = {SK_ColorWHITE, SK_ColorYELLOW}; // Gradient colors
SkScalar positions[] = {0.0f, 1.0f}; // Position of each color in the gradient

// Create the radial gradient shader
// Create the radial gradient shader
auto shader = SkGradientShader::MakeRadial(
    SkPoint::Make(centerX, centerY), // Gradient center
    radius,                          // Gradient radius
    colors,                          // Array of colors
    positions,                       // Position of each color
    2,                               // Number of colors (manually specified)
    SkTileMode::kClamp               // Tile mode
);

// Set up the paint with the shader
//SkPaint paint;
paint.setShader(shader);           // Set the shader on the paint
paint.setAntiAlias(true);          // Enable anti-aliasing for smoother edges

// Draw the circle with the gradient paint
canvas->drawCircle(centerX, centerY, radius, paint);
paint.setColor(SK_ColorRED);
// Coordinates for the center of the circle
centerX = 250;
centerY = 250;

// Radius of the circle
radius = 50;
// Draw the circle on the canvas
//canvas->drawCircle(centerX, centerY, radius, paint);
*/
// More contrasting colors for the text gradient
//SkColor textColors[] = {SK_ColorBLUE, SK_ColorMAGENTA}; // Example: Blue to Magenta

// Change gradient transition positions for text
//SkScalar textPositions[] = {0.2f, 0.8f}; // Positions for gradient transition
//sk_sp<SkTypeface> typeface = SkTypeface::MakeFromName("Arial", SkFontStyle::Normal());
/*
// Calculate text width and position correctly before creating the gradient
const char* text = "SKIA";
SkFont font(typeface, 10); // Assuming typeface has been defined earlier
font.setEdging(SkFont::Edging::kAntiAlias);

// Measure the text to get bounds
SkRect textBounds;
font.measureText(text, strlen(text), SkTextEncoding::kUTF8, &textBounds);
float textWidth = textBounds.width();

float charWidth = textWidth / strlen(text);
// Adjust xText and yText to position the text as needed
float xText = (canvas->imageInfo().width() - textWidth) / 2-200;
float yText = 250; // Adjust as needed for vertical positioning

// First, ensure typeface is properly declared and initialized
//auto typeface = SkTypeface::MakeFromName("Arial", SkFontStyle::Normal());

SkPaint paintText;
// Now, you can use typeface with SkFont
if (typeface) { // Check if typeface creation was successful
    SkFont font(typeface, 100); // Set the font size and typeface
    font.setEdging(SkFont::Edging::kAntiAlias);

    // Define your text colors and positions for the gradient
//    SkColor textColors[] = {SK_ColorRED, SK_ColorBLUE};
   // SkScalar textPositions[] = {0.3f, 0.7f};

float gradientEndPointX = xText + charWidth * 50.5; // Adjust this calculation based on your needs

SkPoint points[2] = {
    SkPoint::Make(xText, yText), // Start point of the gradient at the beginning of the text
    SkPoint::Make(gradientEndPointX, yText) // End point of the gradient halfway through the 'K'
};

SkColor colors[2] = {SK_ColorGRAY, SK_ColorRED}; // Example colors
SkScalar positions[2] = {0.5f, 1.0f}; // Positions of the gradient stops

// Ensure you have the correct number of colors and positions
auto shaderCount = static_cast<int>(std::size(colors));


// Create the gradient shader
auto textShader = SkGradientShader::MakeLinear(
    points, colors, positions, std::size(colors), SkTileMode::kClamp
);

    // Set up paint for text drawing with the gradient shader
    SkPaint paintText;
    paintText.setShader(textShader);
    paintText.setAntiAlias(true);

    // Draw the text on the canvas
    canvas->drawString(text, xText, yText, font, paintText);
}*/

// Draw the text on the canvas (use SkCanvas::drawString)
//canvas->drawString(text, xText, yText, font, paintText);
  if (sContext) { // Use the existing directContext without re-creating it
        sContext->flushAndSubmit();
    }
else{printf("dcontext not created\n");}
 //OpenGL::render_end();

 //   workspace->texture = std::make_unique<wf::simple_texture_t>();
 auto cws = output->wset()->get_current_workspace();
 auto wsn     = workspaces[cws.x][cws.y]->workspace;

wsn->texture = std::make_unique<wf::simple_texture_t>();
wsn->rect.width  =size.width;
wsn->rect.height = size.height;
/*
 OpenGL::render_begin(target_fb);
skia_surface_upload_to_texture(surface, *wsn->texture);
        OpenGL::render_end();
        */
// glBindFramebuffer(GL_FRAMEBUFFER, 0);
/*

SkBitmap bitmap;
bitmap.allocPixels(skSurface->imageInfo());

if (skSurface->readPixels(bitmap, 0, 0)) {
    // Now you have the bitmap with the pixels read correctly.
    // You can proceed with extracting the SkPixmap and saving it.
    SkPixmap pixmap;
    if (bitmap.peekPixels(&pixmap)) {
        writePNG(pixmap, "output.png");
    }
} else {
    printf("Failed to read pixels directly from surface\n");
}
*/



}




void loading_cairo_GPT2()
    {
  auto cws = output->wset()->get_current_workspace();
 auto wsn     = workspaces[cws.x][cws.y]->workspace;
      
static float current_dots = 0.0f;   // Use a floating-point variable
static const float max_dots = 5.0f; // Maximum number of dots
static const float increment = 0.0f; // Smaller increment value
static bool increasing = true;

// Update the loading text
//std::string loading_text = request_text+ std::string(static_cast<int>(current_dots), '.');


// Update the loading text
  auto last_update_time = std::chrono::steady_clock::now();
  
std::string previous_loading_text;
std::string loading_text;


if (request_text!="Request Completed ")
{
if (request_text != previous_loading_text)
{
//loading_text = request_text+ std::string(static_cast<int>(current_dots), '.');
loading_text = request_text;
}
 //loading_text= request_text;
    size_t max_length = 30; //

     // while (true) {
    
          auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);

        // Check if a new second has passed
        if (elapsed.count() > time_offset ) {
            offset++; // Update the offset to the new number of seconds
         //   std::cout << "Seconds elapsed: " << time_offset<< std::endl;
            time_offset=time_offset+100;
            
            // Here you can call the function that needs to be executed every second
            // ...
        }
loading_text = request_text;
   // }
}

if (request_text!="Request Completed ")
{
 loading_text = scrollText(request_text, max_length, offset);
}
else{loading_text = request_text;
}
  

//usleep(200000);

// Logic to increment or decrement the dots for animation

/*
if (increasing) {
    if (current_dots < max_dots) {
        current_dots += increment; // Increment by a smaller amount
    } else {
        increasing = false;
    }
} else {
    if (current_dots > 0.0f) {
        current_dots -= increment; // Decrement by a smaller amount
    } else {
        increasing = true;
    }
}*/
if (resetloading==false) 
       {LOGI("loading_GPT");
        resetloading=true;}
        
        std::string loading = request_text;
        std::istringstream iss(loading_text);
        std::string line;
        loadings.clear(); // Clear existing contents

        auto og = output->get_relative_geometry();
      //  auto font_size = og.height * 0.1;
        cairo_t *cr    = wsn->cr;
        cairo_surface_t *cairo_surface = wsn->cairo_surface;


        if (!cr)
        {
            // Setup dummy context to get initial font size 
            cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
            cr = cairo_create(cairo_surface);
            wsn->texture = std::make_unique<wf::simple_texture_t>();
        }

        PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_LOADING);
        pango_text_extents(cr, loading.c_str(), &wsn->text_extents);

        wsn->rect.width  = wsn->text_extents.width+ WIDGET_PADDING * 8;
        wsn->rect.height = wsn->text_extents.height*2 + WIDGET_PADDING * 2;

        /* Recreate surface based on font size */
        cairo_destroy(cr);
        cairo_surface_destroy(cairo_surface);

        cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
            wsn->rect.width, wsn->rect.height);
        cr = cairo_create(cairo_surface);

    //  PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_LOADING);

        wsn->cr = cr;
        wsn->cairo_surface = cairo_surface;

        auto workarea = output->workarea->get_workarea();
        
        double maxWidth = 0.0;
        auto size = output->get_screen_size();
        MaxTextureHeight = size.height-workarea.y;    
        double radius = background_radius;
      

        cairo_clear(cr);


while (std::getline(iss, line)) {
    loadings.push_back(line);
}

calculate_text_dimensions(cr, loadings, VisibleTextHeight, totalTextHeight, maxWidth);
 

  
   
    // Adjust the Cairo rectangle to fit the total text height
  //  wsn->rect.height = VisibleTextHeight + WIDGET_PADDING*2; // Add extra padding at the bottom

    // Begin rendering the text
    double startY = WIDGET_PADDING; // Start drawing text from this Y position
    // Recreate the Cairo surface with new dimensions
    cairo_clear(cr);
    cairo_surface_destroy(wsn->cairo_surface);
    wsn->cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wsn->rect.width, wsn->rect.height);
    cr = cairo_create(wsn->cairo_surface);
  //   cairo_select_font_face(cr, std::string(font).c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//cairo_set_font_size(cr, 35);
 //   PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_LOADING);
//Clear the existing content
//    cairo_clear(cr);

    // Set the source color to green (R, G, B, A)
 //   cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, 0.5); // Green color

    // Paint the entire surface with the current source color (green)
 //   cairo_paint(cr);

    // Set the source color for text rendering
    
    cairo_set_source_rgba(cr, wf::color_t(text_color).r, wf::color_t(text_color).g, 
                          wf::color_t(text_color).b, wf::color_t(text_color).a);





    for (const auto& loading : loadings) {
  

    TextExtentsCompat text_extents;
    pango_text_extents(cr, loading.c_str(), &text_extents);

    double textX = WIDGET_PADDING - text_extents.x_bearing; // Left-aligned
    double textY = startY + text_extents.height;


    TextExtentsCompat extents;
    const char* text = request_text.c_str();
   // loading_text = request_text
    // Measure the text
    pango_text_extents(cr, text, &extents);
    // Now extents.x_advance and extents.y_advance contain the information
    // about how far the current point will move when the text is drawn
    // Move to the starting position


    // Calculate the end position of the loading
    double endX = textX + extents.x_advance;
    double endY = textY + extents.y_advance;

    double paddingX = 5.0;
    double paddingY = 5.0;
    double radius = 10.0;  // Adjust the radius for the rounded corners

    double rectX = 0;
    double rectY = textY - extents.height - paddingY;
    double rectWidth = extents.width + paddingX*2;
    double rectHeight = wsn->text_extents.height*1.5+  paddingY;


        cairo_set_source_rgba(cr,
            wf::color_t(background_color).r,
            wf::color_t(background_color).g,
            wf::color_t(background_color).b,
            wf::color_t(background_color).a);
 //   if (!loading.empty() && loading.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
      

    cairo_new_path(cr);  
        // Top left corner
    cairo_arc(cr, rectX + radius, rectY + radius, radius, M_PI, 3.0 * M_PI / 2.0);
    // Top side
    cairo_line_to(cr, rectX + rectWidth - radius, rectY);
    // Top right corner
    cairo_arc(cr, rectX + rectWidth - radius, rectY + radius, radius, 3.0 * M_PI / 2.0, 0);
    // Right side
    cairo_line_to(cr, rectX + rectWidth, rectY + rectHeight - radius);
    // Bottom right corner
    cairo_arc(cr, rectX + rectWidth - radius, rectY + rectHeight - radius, radius, 0, M_PI / 2.0);
    // Bottom side
    cairo_line_to(cr, rectX + radius, rectY + rectHeight);
    // Bottom left corner
    cairo_arc(cr, rectX + radius, rectY + rectHeight - radius, radius, M_PI / 2.0, M_PI);
    // Left side
    cairo_close_path(cr);

    cairo_fill(cr);
  //  }
    //  cairo_rectangle(cr, rectX, rectY, rectWidth, rectHeight);
    //  cairo_fill(cr);
    //  TextExtentsCompat text_extents;
    pango_text_extents(cr, loading.c_str(), &text_extents);

    textX = WIDGET_PADDING - text_extents.x_bearing; // Left-aligned
    textY = startY ;//+ text_extents.height;


    //TextExtentsCompat extents;
    // char* text = loading.c_str();
    // Measure the text
    pango_text_extents(cr, text, &extents);
    // Now extents.x_advance and extents.y_advance contain the information
    // about how far the current point will move when the text is drawn

    // Calculate the end position of the loading
    endX = textX + extents.x_advance;
    endY = textY + extents.y_advance;

  //  cairo_set_source_rgba(cr, wf::color_t(text_color).r, wf::color_t(text_color).g, 
    //                      wf::color_t(text_color).b, wf::color_t(text_color).a);
if (request_text!="Request Completed ")
{
  cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.5); // Red color
}else{ cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, 0.5); // green color
}
    cairo_move_to(cr, textX, textY);
    pango_show_text(cr, loading.c_str());

    startY += text_extents.height + WIDGET_PADDING;

    if (startY > MaxTextureHeight) {break;  }
    }

    cairo_stroke(cr);

        OpenGL::render_begin();
        cairo_surface_upload_to_texture(wsn->cairo_surface, *wsn->texture);
        OpenGL::render_end();
//GPTrequestFinished=false;

cairo_destroy(cr);

}/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////



// Helper function to draw a rounded rectangle
void draw_rounded_rectangle(cairo_t *cr, double x, double y, double width, double height, double radius) {
    double degrees = M_PI / 180.0;

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);
}




void draw_arrow(cairo_t *cr, double x, double y, double width, double height, const char* direction) {
    double arrowWidth = width * 0.6; // Width of the arrow itself
    double arrowHeight = height * 0.4; // Height of the arrow itself

    // Calculate the starting point of the arrow
    double startX = x + (width - arrowWidth) / 2;
    double startY = y + (height - arrowHeight) / 2;

    if (strcmp(direction, "left") == 0) {
        // Draw left arrow
        cairo_move_to(cr, startX + arrowWidth, startY);
        cairo_line_to(cr, startX, startY + arrowHeight / 2);
        cairo_line_to(cr, startX + arrowWidth, startY + arrowHeight);
    } else if (strcmp(direction, "right") == 0) {
        // Draw right arrow
        cairo_move_to(cr, startX, startY);
        cairo_line_to(cr, startX + arrowWidth, startY + arrowHeight / 2);
        cairo_line_to(cr, startX, startY + arrowHeight);
    }

    cairo_close_path(cr); // Close the path to complete the arrow shape
    cairo_fill(cr); // Fill the arrow with the current source color
}


void dock_cairo_GPT(cairo_t *cr) {
    // Assume these are defined and initialized properly
    auto cws = output->wset()->get_current_workspace();
    auto wsn = workspaces[cws.x][cws.y]->workspace;
    auto size = output->get_screen_size();
    char* modeText ;
    

    // Texts to display
 if (Xmode=="chatGPT")
    {modeText = "Mode:Chat-GPT";} 
 if (Xmode=="bashGPT")
    {modeText = "Mode:Bash-GPT";}
 if (Xmode=="autoGPT")
    {modeText = "Mode:Auto-GPT";} 
    
    const char* pageText = "Page";
    // Prepare to draw text
    PangoLayout *layout = createConfiguredPangoLayout(cr,  GPT_BASH);
  // Measure "Hello" text extents
    TextExtentsCompat modeTextExtents;
    pango_text_extents(cr, modeText, &modeTextExtents);
    // Setting up the rectangle area, modify these values as needed
    double dockRectX = 5; // Starting X position of the rectangle
    double dockRectY = size.height - 100; // Starting Y position of the rectangle
    double dockRectWidth = size.width/3 - 20; // Width of the rectangle
    double dockRectHeight = modeTextExtents.height+10; // Height of the rectangle
    double radius = 15; // Radius for rounded corners




    // Setting up the dockBackground rectangle area, modify these values as needed
    dockBackgroundRectX = 5; // Starting X position of the inner rectangle
    dockBackgroundRectY = size.height - 100; // Starting Y position of the inner rectangle
    dockBackgroundRectWidth = size.width/3 - 10; // Width of the inner rectangle, spanning almost full width
    dockBackgroundRectHeight = 100; // Height of the inner rectangle, adjust as needed
//    double radius = 20; // Radius for rounded corners

    // Draw larger background rectangle
    cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.6); // Darker gray for the background
    draw_rounded_rectangle(cr, 0, size.height - dockBackgroundRectHeight, dockBackgroundRectWidth , dockBackgroundRectHeight, radius);
    cairo_fill(cr); // Fill the background of the larger rectangle




    // Set up colors
    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.6); // Light gray background
    draw_rounded_rectangle(cr, dockRectX, dockRectY, dockRectWidth, dockRectHeight, radius);
    cairo_fill(cr); // Fill the background of the rectangle

  cairo_set_source_rgb(cr, 0, 0, 0); // Text color: black

  
    double modeX = dockRectX + 10; // Left padding inside the rectangle
    double modeY = dockRectY + (dockRectHeight - modeTextExtents.height) / 2; // Vertically centered

  
    TextExtentsCompat pageExtents;
    pango_text_extents(cr, pageText, &pageExtents);
    double pageX = dockRectX + dockRectWidth - pageExtents.width - 10; // Right padding inside the rectangle
    double pageY = modeY; 

    // Draw "mode" text
    cairo_move_to(cr, modeX, modeY);
    pango_show_text(cr, modeText);

    // Draw "page" text
    cairo_move_to(cr, pageX, pageY);
    pango_show_text(cr, pageText);


// Outer rectangle (background)
    

    double dockOuterRectY = size.height - 80; // Start a bit higher to fit the entire design
//    double dockOuterRectWidth = size.width/3/3; // Take up nearly the entire width
//    double dockOuterRectHeight = modeTextExtents.height; // Fixed height
 radius = 0; // Radius for rounded corners

// Inner rectangle (switch background)
    double switchY = dockOuterRectY + 10; // Padding from the top edge of the outer rectangle
    double switchWidth = size.width/3/3; // Fixed width for the switch
    double switchHeight = modeTextExtents.height; // Fixed height for the switch
    // Draw switch background
    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.8); // Light gray background for the switch
    draw_rounded_rectangle(cr, switchX, switchY, switchWidth, switchHeight, 10); // Smaller radius for the switch
    cairo_fill(cr);




/*
    // Switch indicator (inside the switch)
 if (mode=="chatGPT")
  //  {indicatorX = switchX + 5;} 
 if (mode=="bashGPT")
  //  {indicatorX = switchX + 5 + size.width/3/9;}
 if (mode=="autoGPT")
  //  {indicatorX = switchX + 5 + size.width/3/9 + size.width/3/9;} 
*/

    indicatorY = switchY + 5; // Start inside the switch
    indicatorWidth = (switchWidth / 3) - 10; // One third of the switch width minus padding
    indicatorHeight = switchHeight - 10; // Height minus padding for top and bottom
    // Draw switch indicator for the current mode (assuming first mode is selected)
    cairo_set_source_rgb(cr, 0, 0, 0); // Black indicator
    draw_rounded_rectangle(cr, indicatorX, indicatorY, indicatorWidth, indicatorHeight, 5); // Smaller radius for the indicator
    cairo_fill(cr);

    // Text "Mode" next to the switch
 //   cairo_move_to(cr, switchX + switchWidth + 20, outerRectY + outerRectHeight / 2 + 10); // Adjust for text size
  //  pango_show_text(cr, "Mode");



}



/*
void dock_cairo_GPT(cairo_t *cr) {
    // Assume these are defined and initialized properly
    auto cws = output->wset()->get_current_workspace();
    auto wsn = workspaces[cws.x][cws.y]->workspace;
    auto size = output->get_screen_size();

    // Prepare to draw text
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 24); // Adjust font size as needed

    // Outer rectangle (background)
    double outerRectX = 5;
    double outerRectY = size.height - 120; // Start a bit higher to fit the entire design
    double outerRectWidth = size.width - 10; // Take up nearly the entire width
    double outerRectHeight = 100; // Fixed height
    double radius = 15; // Radius for rounded corners
    // Draw outer rectangle
    cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.6); // Darker gray for the background
    draw_rounded_rectangle(cr, outerRectX, outerRectY, outerRectWidth, outerRectHeight, radius);
    cairo_fill(cr);

    // Inner rectangle (switch background)
    double switchX = outerRectX + 10; // Padding from the left edge of the outer rectangle
    double switchY = outerRectY + 10; // Padding from the top edge of the outer rectangle
    double switchWidth = 150; // Fixed width for the switch
    double switchHeight = 80; // Fixed height for the switch
    // Draw switch background
    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.8); // Light gray background for the switch
    draw_rounded_rectangle(cr, switchX, switchY, switchWidth, switchHeight, 10); // Smaller radius for the switch
    cairo_fill(cr);

    // Switch indicator (inside the switch)
    double indicatorX = switchX + 5; // Start inside the switch
    double indicatorY = switchY + 5; // Start inside the switch
    double indicatorWidth = (switchWidth / 3) - 10; // One third of the switch width minus padding
    double indicatorHeight = switchHeight - 10; // Height minus padding for top and bottom
    // Draw switch indicator for the current mode (assuming first mode is selected)
    cairo_set_source_rgb(cr, 0, 0, 0); // Black indicator
    draw_rounded_rectangle(cr, indicatorX, indicatorY, indicatorWidth, indicatorHeight, 10); // Smaller radius for the indicator
    cairo_fill(cr);

    // Text "Mode" next to the switch
    cairo_move_to(cr, switchX + switchWidth + 20, outerRectY + outerRectHeight / 2 + 10); // Adjust for text size
    pango_show_text(cr, "Mode");

    // Text "Middle" in the center of the outer rectangle
    cairo_move_to(cr, outerRectX + outerRectWidth / 2 - 30, outerRectY + outerRectHeight / 2 + 10); // Roughly center, adjust as needed
    pango_show_text(cr, "Middle");

    // Arrows on the right side
    double arrowBoxWidth = 50; // Width for each arrow box
    double arrowBoxHeight = 80; // Same as switch height for alignment
    double arrowBoxX = outerRectX + outerRectWidth - arrowBoxWidth * 2 - 20; // Position from the right edge
    double arrowBoxY = switchY; // Align with the switch vertically

    // Left Arrow
    cairo_set_source_rgb(cr, 0, 0, 0); // Black arrow
    draw_arrow(cr, arrowBoxX, arrowBoxY + arrowBoxHeight / 2, arrowBoxWidth, arrowBoxHeight, "left");
    
    // Right Arrow
    arrowBoxX += arrowBoxWidth + 10; // Position for the right arrow
    draw_arrow(cr, arrowBoxX, arrowBoxY + arrowBoxHeight / 2, arrowBoxWidth, arrowBoxHeight, "right");
}
*/
    void set_alpha()
    {
        auto wsize = output->wset()->get_workspace_grid_size();
        for (int x = 0; x < wsize.width; x++)
        {
            for (int y = 0; y < wsize.height; y++)
            {
                workspaces[x][y]->set_alpha(alpha_fade);
            }
        }
    }


std::string escapeQuotes(const std::string& input) {
    std::string output;
    for (char c : input) {
        if (c == '\"') {
            output += "\\\"";
        } else {
            output += c;
        }
    }
    return output;
}
/*

std::string escapeQuotes(const std::string& input) {
    std::string output;
    for (char c : input) {
        if (c == '\"') {
            output += "\\\"";
        } else if (c == '\'') {
            output += "\\\'";
        } else {
            output += c;
        }
    }
    return output;
}
*/


    wf::effect_hook_t pre_hook = [=] ()
    {
 output->render->damage_whole();
// wf::pointf_t cursor_position = wf::get_core().get_cursor_position();


auto size = output->get_screen_size();
  double minIndicatorX = switchX + 5;
  double midIndicatorX=  switchX + 5 + size.width/4/9 ;
  double maxIndicatorX = switchX + 5 + size.width/3/9 + size.width/3/9;

  // std::cout << "isDraggingSwitch: " << isDraggingSwitch << std::endl;
  if (buttonpush==false) {
            isDraggingSwitch= false;
        }

if (isDraggingSwitch==false){
    if (indicatorX<midIndicatorX) 
    {
    mode="chatGPT";
    }else if (indicatorX>=midIndicatorX && indicatorX<maxIndicatorX) 
    {mode="bashGPT";
       
    }else if (indicatorX>=minIndicatorX)  
    {
    mode="autoGPT";    
    }
}

if (isDraggingSwitch==true){
    if (indicatorX<midIndicatorX) 
    {
    Xmode="chatGPT";
    }else if (indicatorX>=midIndicatorX && indicatorX<maxIndicatorX) 
    {Xmode="bashGPT";
       
    }else if (indicatorX>=minIndicatorX)  
    {
    Xmode="autoGPT";    
    }
}


//printf("indicatorX=%f\n",indicatorX );

//if (isDraggingSwitch==false)
{
if (previous_mode!=mode)
{LOAD_MODE=TRUE;
params.externallySignaled = true;
update_texture();
}
}


previous_mode=mode;



    auto workarea = output->workarea->get_workarea();





for (auto& circle : circles) {
    // Calculate the bounds of the circle
    double circleLeft = circle.x - circle.radius;
    double circleRight = circle.x + circle.radius;
    double circleTop = circle.y - circle.radius;
    double circleBottom = circle.y + circle.radius;

    // Check if the pointer is within the bounds of the circle
    if (pointer_posX >= circleLeft && pointer_posX <= circleRight &&
        pointer_posY >= circleTop +workarea.y  && pointer_posY <= circleBottom+workarea.y ) {
        
    
        if (buttonpush == true) {
            circle.pressed = true; // Update the pressed state of the circle
            std::cout << "Button pressed on circle associated with massivesyntaxblock " 
                      << circle.syntaxBlockId << ", Color: " << circle.color << std::endl;

            // Additional logic when a circle is pressed
        } else {
            circle.pressed = false; // Reset the pressed state when the button is not pressed
        }

        break; // Assuming only one circle can be pressed at a time
    } else {
        circle.pressed = false; // Optionally reset the pressed state if the circle is not under the pointer
    }
      
       
}



    initialMouseY = pointer_posY + workarea.y ;
    // Check if the cursor is over the scrollbar
    if (pointer_posX >= scrollbarX && pointer_posX <= (scrollbarX + scrollbarWidth) &&
        pointer_posY >= scrollbarY && pointer_posY <= (scrollbarY + scrollbarHeight) || isDragging == true) {

        // On mouse press
        if ( buttonpush==true) {
            isDragging = true;
           
        }

        // On mouse release
        if (buttonpush==false) {
            isDragging = false;
        }

        // Dragging logic
        if (isDragging) {
            scroll_offset = pointer_posY- (workarea.y*2)  ;
            // Clamp scroll_offset to not exceed VisibleTextHeight
            scroll_offset = std::min(scroll_offset, VisibleTextHeight-scrollbarHeight);
         //   printf("visibleTextHeight at drag%f\n",VisibleTextHeight );
            // ensure that scroll_offset does not go below 0
            scroll_offset = std::max(scroll_offset, 0.0);
         //   printf("cursor_position.y%f\n",scroll_offset);
         //   printf("workarea=%d\n",workarea.y);
         //   printf("Dragging - SCROLLBARoffset is %d\n", scroll_offset);
            
         if (static_cast<int>(scroll_offset) != static_cast<int>(previous_scroll_offset)) 
          {//needsUpdate=true;}
            update_texture();}
            //render_cairo_GPT();

            previous_scroll_offset=scroll_offset;
           
        }
    }

    else if (GPTrequestLoading==false)
           {

//for (const auto& textLine : textLines) {
    


 selectedText.clear();
 escapedText.clear();

int clipClick=0; 

if (GPTrequestFinished==true)
   
{
if (buttonpush == true) {
    bool clickInsideTextLine = false;

    // Check each text line for a click
    for (size_t i = 0; i < textLines.size(); ++i) {
        const auto& textLine = textLines[i];
//take away line worth just adding 20 just
         double halfLineHeight = textLine.box.height / 2;

if ((pointer_posX >= dockBackgroundRectX && pointer_posX <= (dockBackgroundRectX + dockBackgroundRectWidth) &&
     pointer_posY >= dockBackgroundRectY +workarea.y  && pointer_posY <= (dockBackgroundRectY + dockBackgroundRectHeight+workarea.y )) /*|| isDraggingDock*/) {
//LOGI("clicking dock");
if ((pointer_posX >= indicatorX && pointer_posX <= (indicatorX + indicatorWidth) &&
     pointer_posY >= indicatorY +workarea.y && pointer_posY <= (indicatorY + indicatorHeight +workarea.y )) || isDraggingSwitch) {
//LOGI("clicking indicator dock");


/*
    // Switch indicator (inside the switch)
 if (mode=="chatGPT")
  //  {indicatorX = switchX + 5;} 
 if (mode=="bashGPT")
  //  {indicatorX = switchX + 5 + size.width/3/9;}
 if (mode=="autoGPT")
  //  {
*/



        // On mouse press
        if ( buttonpush==true) {
            isDraggingSwitch = true;
           
        }

        // On mouse release
        if (buttonpush==false) {
            isDraggingSwitch= false;
        }

 // Dragging logic
        if (isDraggingSwitch==true) {
             indicatorX = pointer_posX  ;
            // Clamp scroll_offset to not exceed VisibleTextHeight
             indicatorX = std::min(indicatorX, maxIndicatorX);
         //   printf("visibleTextHeight at drag%f\n",VisibleTextHeight );
            // ensure that scroll_offset does not go below 0
             indicatorX = std::max(indicatorX, minIndicatorX );











         //   printf("cursor_position.y%f\n",scroll_offset);
         //   printf("workarea=%d\n",workarea.y);
         //   printf("Dragging - SCROLLBARoffset is %d\n", scroll_offset);
            
         if (static_cast<int>(scroll_offset) != static_cast<int>(previous_scroll_offset)) 
          {//needsUpdate=true;}
            update_texture();}
            //render_cairo_GPT();

            previous_scroll_offset=scroll_offset;
           
        }



}

}         
 else if (pointer_posX >= textLine.box.x && pointer_posX <= (textLine.box.x + textLine.box.width) &&
            pointer_posY >= textLine.box.y  +workarea.y - /*halfLineHeight*/ textLine.box.height  && pointer_posY <= (textLine.box.y  +workarea.y  /*+ textLine.box.height*/ /*-  halfLineHeight*/)) {
            clickInsideTextLine = true;

            if (!awaitingSecondClick) {
                // First click
                clipClick =1;
                firstClickLineIndex = i;
                awaitingSecondClick = true;
                printf("First click at line: %d\n", i);
                    
                    HighlightedLine hl;
                    hl.lineIndex = i;
                    hl.rectX = textLines[i].box.x;
                    hl.rectY = textLines[i].box.y + Yscroller;
                    hl.rectWidth = textLines[i].box.width;
                    hl.rectHeight = textLines[i].box.height;
                    highlightedLines.push_back(hl);

                     // Update the rendering after second click

                update_texture();
                render_cairo_GPT();


            } else 
            if (i != firstClickLineIndex){
                // Second click
                clipClick =2;

                secondClickLineIndex = i;
                printf("Second click at line: %d\n", i);

                // Clear previous highlights
               // highlightedLines.clear();

                // Calculate the range for selection
                int start = std::min(firstClickLineIndex, secondClickLineIndex);
                int end = std::max(firstClickLineIndex, secondClickLineIndex);

               

                // Process highlighted lines
                for (int j = start; j <= end; ++j) {
                    HighlightedLine hl;
                    hl.lineIndex = j;
                    hl.rectX = textLines[j].box.x;
                    hl.rectY = textLines[j].box.y + Yscroller;
                    hl.rectWidth = textLines[j].box.width;
                    hl.rectHeight = textLines[j].box.height;
                    highlightedLines.push_back(hl);

                    // Update the rendering after second click
    //            GPTrequestFinished = true;
                update_texture();
                render_cairo_GPT();
  //              GPTrequestFinished = false;

                // Concatenate the text for clipboard
                if (j > start) selectedText += "\n"; // Add newline between lines
                selectedText += textLines[j].text;

                printf("Highlighting line: %d\n", j);
                printf("Selected line: %s\n", textLines[j].text.c_str());
               
                }
                printf("selectedText:%s\n",selectedText );
////////////////////copy_to_clipboard;////////////////////////////////////////////////////////////
             //   std::string command = "wl-copy \"" + selectedText + "\"";
                escapedText = escapeQuotes(selectedText);
                std::string command = "wl-copy \"" + escapedText + "\"";

                // Define a global variable to keep track of the child process ID
                pid_t child_pid = -1;

                // Inside your function where you call fork()
                if (child_pid > 0) {
                    // There's a previously running child process, try to terminate it
                    kill(child_pid, SIGTERM);  // Send termination signal to the child process
                    waitpid(child_pid, NULL, 0);  // Wait for the child process to terminate
                }

                child_pid = fork();
                if (child_pid == -1) {
                    // Fork failed
                    // Handle error
                } else if (child_pid == 0) {
                    // Child process
                    system(command.c_str());
                    _exit(EXIT_SUCCESS);  // Exit child process
                } else {
                    // Parent process
                    // No need to wait for the child process here since it's handled at the start
                }
//////////////////////////////////////////////////////////////////////////////////////////////////////


          
                // Update the rendering after second click
             //   GPTrequestFinished = true;
                update_texture();
                render_cairo_GPT();
             //   GPTrequestFinished = false;

                awaitingSecondClick = false; // Reset awaitingSecondClick
            }

            break; // Found the clicked text line
        }


    }

    // Click is outside any text line
    if (!clickInsideTextLine ) {
        // Reset selection
        clipClick =0;
        highlightedLines.clear();
        awaitingSecondClick = false;
      //  printf("Click outside text line, clearing highlights\n");
    //    GPTrequestFinished = true;
           //     update_texture();
           //     render_cairo_GPT();
      //          GPTrequestFinished = false;
    }
}

}

    }



  if (alpha_fade.running())
        {
            set_alpha();
            output->render->damage_whole();
        }

   if (GPTrequestLoading == true) {
        update_texture(); 
    }
    //    update_texture();   




if (!GPTrequestFinished) {
        // HTTP request not finished yet, skip rendering
      // /  return;
    }



if (GPTrequestFinished==true)

        {

            if (mode=="chatGPT")   
            {parseJSONgptResponseChatGPT();}
            
            if (mode=="bashGPT")
            {parseJSONgptResponseAssistant();}
            if (mode=="autoGPT")
            {parseJSONgptResponseChatGPT();}

            update_texture();
        }

      
    };

    wf::signal::connection_t<wf::workspace_changed_signal> viewport_changed{[this] (wf::
                                                                                    workspace_changed_signal*
                                                                                    ev)
        {
            auto wsize = output->wset()->get_workspace_grid_size();
            auto nvp   = output->wset()->get_current_workspace();
            auto og    = output->get_relative_geometry();

            for (int x = 0; x < wsize.width; x++)
            {
                for (int y = 0; y < wsize.height; y++)
                {
                    workspaces[x][y]->set_offset((x - nvp.x) * og.width,
                        (y - nvp.y) * og.height);
                }
            }

            output->render->damage_whole();

            activate();

            if (show_option_names)
            {
                return;
            }

            if (!alpha_fade.running())
            {
                if (!timer.is_connected())
                {
                    alpha_fade.animate(alpha_fade, 1.0);
                }
            } else if (timed_out)
            {
                timed_out = false;
                alpha_fade.animate(alpha_fade, 1.0);
            }

            if (timer.is_connected())
            {
                timer.disconnect();
                timer.set_timeout((int)display_duration, timeout);
            }
        }
    };

    wf::wl_timer<false>::callback_t timeout = [=] ()
    {
        output->render->damage_whole();
        alpha_fade.animate(1.0, 0.0);
        timed_out = true;
    };


     wf::effect_hook_t damage_hook = [=] ()
    {
       // if (!output->render->get_scheduled_damage().empty())
        {
         //   loading_cairo_GPT();
       output->render->damage_whole();
        }

     //   output->render->damage_whole();
    };

    wf::effect_hook_t overlay_hook = [=] ()
    {


// Make sure this lambda captures `this` correctly or adjust based on actual usage
   // auto fb = output->render->get_target_framebuffer();
 /*     wf::geometry_t new_geometry = {0, 0, 800, 600}; // Define new geometry here
    fb.geometry = new_geometry; // Set new geometry

    // Check if wfTexture is valid
    if (this->wfTexture.tex_id > 0) {


        OpenGL::render_begin(fb);
        
        // Use the prepared wfTexture directly
        OpenGL::render_texture(
            this->wfTexture, // Use the texture from your class
            fb, fb.geometry, glm::vec4(1.0f), 
            OpenGL::TEXTURE_TRANSFORM_INVERT_Y
        );

        OpenGL::render_end();
  
    } else {
        // Handle the case where wfTexture is not valid
        // This could involve logging a message or attempting to initialize the texture
    }
   */ };

    wf::effect_hook_t post_hook = [=] ()
    {
     if (!alpha_fade.running())
        {
            if (timed_out)
            {
             //   deactivate();
                timed_out = false;
                output->render->damage_whole();
            } else if (!timer.is_connected())
            {
                timer.set_timeout((int)display_duration, timeout);
            }
        } else
        {
            set_alpha();
        }
    };

    void activate()
    {



        if (hook_set)
        {
            return;
        }

        output->render->add_effect(&post_hook, wf::OUTPUT_EFFECT_POST);
        output->render->add_effect(&pre_hook, wf::OUTPUT_EFFECT_PRE);
        output->render->damage_whole();
        hook_set = true;
    }

    void deactivate()
    {
        if (!hook_set)
        {
            return;
        }

        output->render->rem_effect(&post_hook);
        output->render->rem_effect(&pre_hook);
        hook_set = false;
    }

    void fini() override
    {
     //   on_raw_pointer_button.disconnect();
        deactivate();
        auto wsize = output->wset()->get_workspace_grid_size();
        for (int x = 0; x < wsize.width; x++)
        {
            for (int y = 0; y < wsize.height; y++)
            {
                auto& wsn = workspaces[x][y]->workspace;
                cairo_surface_destroy(wsn->cairo_surface);
                cairo_destroy(wsn->cr);
                wsn->texture->release();
                wsn->texture.reset();
                wf::scene::remove_child(workspaces[x][y]);
                workspaces[x][y].reset();
            }
        }

        output->render->damage_whole();
    }


};

DECLARE_WAYFIRE_PLUGIN(wf::per_output_plugin_t<wayfire_workspace_names_output>);
}
}
}