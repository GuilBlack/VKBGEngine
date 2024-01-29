#pragma once

namespace vkbg
{

class Pipeline
{
public:
    Pipeline(const std::string& vertShaderPath, const std::string& fragShaderPath);

private:
    void CreateGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath);

private:
    /// <summary>
    /// Read a binary files and return a vector of bytes
    /// </summary>
    static std::vector<uint8_t> ReadFile(const std::string& filePath);
};

}

