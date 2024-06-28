module;

#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.ShaderIncluder;
import <fstream>;
import <sstream>;
import <string>;
import <format>;
import <filesystem>;

namespace Engine
{
    export class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        virtual shaderc_include_result *GetInclude(
            const char *const requested_source,
            const shaderc_include_type includeType,
            const char *const requesting_source,
            const size_t include_depth) override;

        virtual void ReleaseInclude(shaderc_include_result *const data) override;

    private:
        struct IncludeResultPlaceholder
        {
        public:
            std::string sourceName;
            std::string content;
        };

        [[nodiscard]]
        static std::filesystem::path __resolveTargetAbsolutePath(
            const shaderc_include_type includeType,
            const std::string_view targetSource,
            const std::string_view requestingSource) noexcept;
    };
}

module: private;

namespace Engine
{
    shaderc_include_result *ShaderIncluder::GetInclude(
        const char *const requested_source,
        const shaderc_include_type includeType,
        const char *const requesting_source,
        const size_t include_depth)
    {
        auto *const pPlaceholder{ new IncludeResultPlaceholder };
        auto &sourceName{ pPlaceholder->sourceName };
        auto &content{ pPlaceholder->content };

        const auto absolutePath{ __resolveTargetAbsolutePath(includeType, requested_source, requesting_source) };
        std::ifstream fin{ absolutePath };
        if (fin)
        {
            std::ostringstream oss;
            oss << fin.rdbuf();

            sourceName = absolutePath.string();
            content = oss.str();
        }
        else
            content = std::format("Cannot open file: {}", requested_source);

        auto *const pRetVal{ new shaderc_include_result };
        pRetVal->source_name = pPlaceholder->sourceName.c_str();
        pRetVal->source_name_length = pPlaceholder->sourceName.size();
        pRetVal->content = pPlaceholder->content.c_str();
        pRetVal->content_length = pPlaceholder->content.size();
        pRetVal->user_data = pPlaceholder;

        return pRetVal;
    }

    void ShaderIncluder::ReleaseInclude(shaderc_include_result *const data)
    {
        delete data->user_data;
        delete data;
    }

    std::filesystem::path ShaderIncluder::__resolveTargetAbsolutePath(
        const shaderc_include_type includeType,
        const std::string_view targetSource,
        const std::string_view requestingSource) noexcept
    {
        if (includeType == shaderc_include_type::shaderc_include_type_standard)
            return targetSource;

        const auto parentPath{ std::filesystem::path{ requestingSource }.parent_path() };
        return (parentPath / targetSource);
    }
}