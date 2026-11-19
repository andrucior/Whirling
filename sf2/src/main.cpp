#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>

#include "MouseController.h"
#include "SceneRenderer.h"
#include "SceneView.h"
#include "Simulation.h"

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
const char* const kWindowTitle = "sf2";
const char* const kGlslVersion = "#version 330";

constexpr glm::vec3 kCameraEye(2.0f, 2.0f, 3.0f);
constexpr glm::vec3 kCameraTarget(0.0f);
constexpr glm::vec3 kWorldUp(0.0f, 1.0f, 0.0f);

void glfw_error_callback(int error, const char* description);
bool InitGlfw();
GLFWwindow* CreateAppWindow();
void InitImGui(GLFWwindow* window);
void ShutdownImGui();
GLFWwindow* Startup();
void Shutdown(GLFWwindow* window);
void DrawCubeSection(Simulation& sim);
void DrawCubeSection(Simulation& sim);
void DrawControlPanel(Simulation& sim, RenderSettings& renderSettings);
void DrawDisplaySection(RenderSettings& settings);
void DrawSimSection(Simulation& sim);
void DrawFrame(GLFWwindow* window, Simulation& sim, SceneRenderer& renderer, SceneView& view, float dt);

int main()
{
    GLFWwindow* window = Startup();
    if (window == nullptr)
    {
        return 1;
    }

    Simulation simulation;
    SceneRenderer renderer;
    SceneView view(kCameraEye, kCameraTarget, kWorldUp);

    if (!renderer.Initialize())
    {
        std::fprintf(stderr, "Failed to create scene shader\n");
        Shutdown(window);
        return 1;
    }

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        const double now = glfwGetTime();
        DrawFrame(window, simulation, renderer, view, static_cast<float>(now - lastTime));
        lastTime = now;
    }

    Shutdown(window);

    return 0;
}

void glfw_error_callback(int error, const char* description)
{
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool InitGlfw()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

GLFWwindow* CreateAppWindow()
{
    GLFWwindow* window = glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle, nullptr, nullptr);
    if (window == nullptr)
    {
        return nullptr;
    }

    glfwSetCursorPosCallback(window, MouseController::onSetCursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseController::onSetMouseButtonCallback);
    glfwSetScrollCallback(window, MouseController::onScrollCallback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::fprintf(stderr, "Failed to initialize GLEW\n");
        glfwDestroyWindow(window);
        return nullptr;
    }

    return window;
}

void InitImGui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(kGlslVersion);
}

void ShutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

GLFWwindow* Startup()
{
    if (!InitGlfw())
    {
        return nullptr;
    }

    GLFWwindow* window = CreateAppWindow();
    if (window == nullptr)
    {
        glfwTerminate();
        return nullptr;
    }

    InitImGui(window);
    return window;
}

void Shutdown(GLFWwindow* window)
{
    ShutdownImGui();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void DrawCubeSection(Simulation& sim)
{
    CubeParams& params = sim.Params();
    SimSettings& settings = sim.Settings();

    ImGui::SeparatorText("Szescian");
    ImGui::SliderFloat("Bok a", &params.a, 0.1f, 5.0f);
    ImGui::SliderFloat("Gestosc", &params.density, 0.1f, 10.0f);
    ImGui::Checkbox("Grawitacja", &params.gravityOn);

    ImGui::SeparatorText("Warunki poczatkowe");
    bool icChanged = false;
    icChanged |= ImGui::SliderFloat("Wychylenie (deg)", &settings.tiltDegrees, 0.0f, 180.0f);
    icChanged |= ImGui::SliderFloat("Predkosc katowa (rad/s)", &settings.omega0, 0.0f, 50.0f);
    if (ImGui::Button("Reset") || icChanged)
    {
        sim.Reset();
    }
}

void DrawDisplaySection(RenderSettings& settings)
{
    ImGui::SeparatorText("Wyswietlanie");
    ImGui::Checkbox("Szescian", &settings.showCube);
    ImGui::Checkbox("Przekatna", &settings.showDiagonal);
    ImGui::Checkbox("Trajektoria", &settings.showTrajectory);
    ImGui::Checkbox("Kierunek grawitacji", &settings.showGravity);
    ImGui::SliderFloat("Rozmiar plaszczyzny", &settings.gravityPlaneSize, 0.5f, 6.0f);
}

void DrawSimSection(Simulation& sim)
{
    ImGui::SeparatorText("Symulacja");
    ImGui::Checkbox("Uruchomiona", &sim.Settings().running);
    ImGui::SliderFloat("Predkosc czasu", &sim.Settings().simSpeed, 0.0f, 4.0f);

    const Simulation::Energy energy = sim.ComputeEnergy();
    ImGui::Text("|W| = %.3f rad/s", glm::length(sim.AngularVelocity()));
    ImGui::Text("E = %.5f J  (Ek %.4f, Ep %.4f)", energy.Total(), energy.kinetic, energy.potential);
}

void DrawControlPanel(Simulation& sim, RenderSettings& renderSettings)
{
    ImGui::SetNextWindowSize(ImVec2(420.0f, 620.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin(kWindowTitle);

    DrawCubeSection(sim);
    DrawDisplaySection(renderSettings);
    DrawSimSection(sim);

    ImGui::End();
}

void DrawFrame(GLFWwindow* window, Simulation& sim, SceneRenderer& renderer, SceneView& view, float dt)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    DrawControlPanel(sim, renderer.Settings());

    view.HandleMouse(ImGui::GetIO().WantCaptureMouse);
    sim.Advance(dt);
    renderer.Update(sim);

    int displayWidth, displayHeight;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
    const float aspect =
        displayHeight != 0 ? static_cast<float>(displayWidth) / static_cast<float>(displayHeight) : 1.0f;

    renderer.BeginFrame(displayWidth, displayHeight);
    renderer.Draw(sim, view.ViewProjection(aspect));

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}