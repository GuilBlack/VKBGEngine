#include "VKBGEngine.h"
#include "Window.h"
#include "Helper.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"

namespace vkbg
{
void Engine::Init(WindowProps properties)
{
    Helper::Print("Hello World!\n");
    glfwInit();

    m_Window = new Window(properties);

    m_RenderContext = new RenderContext(m_Window);

    m_Pipeline = new Pipeline("res/Shaders/Compiled/Simple.vert.spv", "res/Shaders/Compiled/Simple.frag.spv");
}

void Engine::Run()
{
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
    }
}

void Engine::Shutdown()
{
    delete m_Window;
    delete m_Pipeline;
    delete m_RenderContext;
}

}
