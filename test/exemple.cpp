#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

// Include GLAD, GLFW
#include <glad/glad.h>

#include <GLFW/glfw3.h>

// Include C++ standard libraries
#include <stdio.h>

// Include ImGui, ImPlot, ImGui File Dialog
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

// Include spdlog
#include "spdlog/spdlog.h"

// Include curl
#include <curl/curl.h>

// Include gphoto2
#ifdef interface
#undef interface
#endif
#include <gphoto2/gphoto2-camera.h>

// ----------------------------------------------------------------------------

void showFps(GLFWwindow *window) {
  static double prevTime = glfwGetTime();
  static int frameCount = 0;
  double currentTime = glfwGetTime();
  double elapsed = currentTime - prevTime;

  if (elapsed > 0.25) {
    prevTime = currentTime;
    double fps = (double)frameCount / elapsed;
    double msPerFrame = 1000.0 / fps;
    char tmp[128];
    sprintf(tmp, "FPS: %.2f, Frame time: %.2f ms", fps, msPerFrame);
    glfwSetWindowTitle(window, tmp);
    frameCount = 0;
  }
  frameCount++;
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *userp) {
  size_t totalSize = size * nmemb;
  userp->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

std::string performGetRequest(const std::string &url) {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (curl) {
    // Set the URL for the GET request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set the callback function to handle the data received
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request
    res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK)
      spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));

    // Always cleanup
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();
  return readBuffer;
}

void getCameraInfo(const char *port) {
  GPContext *context = gp_context_new();
  Camera *camera;
  gp_camera_new(&camera);

  GPPortInfoList *portInfoList;
  gp_port_info_list_new(&portInfoList);
  gp_port_info_list_load(portInfoList);

  GPPortInfo portInfo;
  int portNumber = gp_port_info_list_lookup_path(portInfoList, port);
  gp_port_info_list_get_info(portInfoList, portNumber, &portInfo);
  gp_camera_set_port_info(camera, portInfo);

  if (gp_camera_init(camera, context) != GP_OK) {
    spdlog::error("Failed to initialize camera on port {}", port);
    gp_camera_free(camera);
    gp_context_unref(context);
    return;
  }

  CameraText text;
  gp_camera_get_summary(camera, &text, context);
  spdlog::info("Camera Summary: {}", text.text);

  gp_camera_exit(camera, context);
  gp_camera_free(camera);
  gp_context_unref(context);
}

void listConnectedCameras() {
  GPContext *context = gp_context_new();
  CameraList *cameraList;
  gp_list_new(&cameraList);

  int numCameras;
  gp_camera_autodetect(cameraList, context);
  numCameras = gp_list_count(cameraList);

  for (int i = 0; i < numCameras; ++i) {
    const char *cameraName;
    const char *cameraPort;
    gp_list_get_name(cameraList, i, &cameraName);
    gp_list_get_value(cameraList, i, &cameraPort);
    spdlog::info("Camera {}: {} at {}", i + 1, cameraName, cameraPort);
  }

  if (numCameras == 0)
    spdlog::info("No cameras found");

  gp_list_free(cameraList);
  gp_context_unref(context);
}

void drawFileDialog() {
  // open Dialog Simple
  if (ImGui::Button("Open File Dialog")) {
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File",
                                            ".cpp,.h,.hpp", config);
  }
  // display
  if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
      // action
    }

    // close
    ImGuiFileDialog::Instance()->Close();
  }
}

void drawSimpleTestWindow() {
  ImGui::Begin("Simple Test Window");

  drawFileDialog();

  if (ImGui::Button("List Connected Cameras")) {
    listConnectedCameras();
  }

  if (ImGui::Button("Get Camera Info")) {
    getCameraInfo("usb:001,050");
  }

  if (ImGui::Button("Perform GET Request")) {
    std::string url = "https://api.github.com";
    std::string response = performGetRequest(url);
    spdlog::info("Response: {}", response);
  }

  ImGui::End();
}

int main(void) {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(1600, 900, "Explorer400D", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    spdlog::error("Failed to initialize OpenGL context");
    return -1;
  }

  // Initialize ImGui with GLFW & OpenGL && ImPlot
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Show FPS
    showFps(window);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Dockspace
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // Demo windows
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();

    // Simple test window
    drawSimpleTestWindow();

    // Render the window
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}
