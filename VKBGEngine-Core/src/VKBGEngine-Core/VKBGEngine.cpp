#include "VKBGEngine.h"
#include "Window.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/SwapChain.h"
#include "WindowProps.h"

namespace vkbg
{
void Engine::Init(EngineProps properties)
{
    glfwInit();

    m_Window = new Window(properties.WindowProperties);

    m_RenderContext = new RenderContext(m_Window);

    m_SwapChain = new SwapChain(m_RenderContext, {
            properties.WindowProperties.Width,
            properties.WindowProperties.Height
        });

    PipelineProps pipelineProperties = Pipeline::GetDefaultPipelineProps(
        properties.WindowProperties.Width,
        properties.WindowProperties.Height
    );

    m_Pipeline = new Pipeline(
        m_RenderContext,
        "res/Shaders/Compiled/Simple.vert.spv",
        "res/Shaders/Compiled/Simple.frag.spv",
        pipelineProperties
    );
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
    delete m_Pipeline;
    delete m_SwapChain;
    delete m_RenderContext;
    delete m_Window;
}

}
