#include "Pipeline.h"

namespace vkbg
{

    Pipeline::Pipeline(const std::string& vertShaderPath, const std::string& fragShaderPath)
    {
        CreateGraphicsPipeline(vertShaderPath, fragShaderPath);
    }

    void Pipeline::CreateGraphicsPipeline(const std::string & vertShaderPath, const std::string & fragShaderPath)
    {
        auto vertShaderCode = ReadFile(vertShaderPath);
        auto fragShaderCode = ReadFile(fragShaderPath);

        LOG("vertShaderCode size: " << vertShaderCode.size() << std::endl);
        LOG("fragShaderCode size: " << fragShaderCode.size() << std::endl);
    }

    std::vector<uint8_t> Pipeline::ReadFile(const std::string & filePath)
    {
        // ate: start reading at the end of the file
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<uint8_t> buffer(fileSize);

        file.seekg(0);
        file.read((char*)buffer.data(), fileSize);
        file.close();

        return buffer;
    }

}
