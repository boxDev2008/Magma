#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <vector>
#include <string>
#include <functional>
#include <map>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <spirv_cross/spirv_msl.hpp>

#include "default_resource.h"

enum class ShaderType { Graphics, Compute };

enum
{
    GLSL = 1 << 1,
    GLSLES = 1 << 2,
    HLSL = 1 << 3,
    MSL = 1 << 4,
    SPIRV = 1 << 5
};
typedef uint32_t ShaderLangFlags;

struct ShaderStage
{
    std::vector<uint32_t> spirv;
    std::string source;
    EShLanguage stage;
};

struct ShaderProgram
{
    std::map<EShLanguage, ShaderStage> stages;
    ShaderType type;
};

struct ShaderResources
{
    struct UniformBlock
    {
        std::string name;
        uint32_t binding;
    };
    std::vector<UniformBlock> uniform_blocks;
    std::vector<UniformBlock> sampled_images;
};

class ShaderParser
{
public:
    static bool parse(const std::string &filepath, ShaderProgram &program)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Error: Cannot open file '" << filepath << "'\n";
            return false;
        }

        EShLanguage current_stage = EShLangCount;
        std::stringstream current_source;
        std::string line;

        while (std::getline(file, line))
        {
            if (line.find("@stage") == 0)
            {
                if (current_stage != EShLangCount)
                {
                    save_stage(program, current_stage, current_source.str());
                    current_source.str("");
                    current_source.clear();
                }

                current_stage = parse_stage_directive(line, program);
                if (current_stage == EShLangCount)
                {
                    std::cerr << "Error: Invalid stage directive '" << line << "'\n";
                    return false;
                }
            }
            else if (current_stage != EShLangCount)
                current_source << line << '\n';
        }

        if (current_stage != EShLangCount)
            save_stage(program, current_stage, current_source.str());

        return validate_program(program);
    }

private:
    static EShLanguage parse_stage_directive(const std::string &line, ShaderProgram &program)
    {
        if (line.find("vertex") != std::string::npos)
        {
            program.type = ShaderType::Graphics;
            return EShLangVertex;
        }
        else if (line.find("fragment") != std::string::npos)
        {
            program.type = ShaderType::Graphics;
            return EShLangFragment;
        }
        else if (line.find("compute") != std::string::npos)
        {
            program.type = ShaderType::Compute;
            return EShLangCompute;
        }
        return EShLangCount;
    }

    static void save_stage(ShaderProgram &program, EShLanguage stage, const std::string &source)
    {
        program.stages[stage] = { {}, source, stage };
    }

    static bool validate_program(const ShaderProgram &program)
    {
        if (program.type == ShaderType::Compute)
        {
            if (program.stages.size() != 1 || program.stages.find(EShLangCompute) == program.stages.end())
            {
                std::cerr << "Error: Compute shaders must have exactly one stage\n";
                return false;
            }
        }
        else if (program.type == ShaderType::Graphics)
        {
            if (program.stages.find(EShLangVertex) == program.stages.end())
            {
                std::cerr << "Error: Graphics shaders must have a vertex stage\n";
                return false;
            }
            if (program.stages.find(EShLangFragment) == program.stages.end())
            {
                std::cerr << "Error: Graphics shaders must have a fragment stage\n";
                return false;
            }
            if (program.stages.find(EShLangCompute) != program.stages.end())
            {
                std::cerr << "Error: Graphics shaders cannot have compute stage\n";
                return false;
            }
        }
        return true;
    }
};

class SPIRVCompiler
{
public:
    static bool compile(ShaderStage &stage, ShaderResources &resources)
    {
        if (!compile_glsl_to_spirv(stage.source, stage.stage, stage.spirv) ||
            !remap(stage, resources, stage.spirv, stage.spirv))
            return false;
        return !stage.spirv.empty();
    }

private:
    static bool compile_glsl_to_spirv(
        const std::string& glsl_source,
        EShLanguage shader_stage,
        std::vector<uint32_t>& spirv_out)
    {        
        const char* src_ptr = glsl_source.c_str();
        glslang::TShader shader(shader_stage);
        shader.setStrings(&src_ptr, 1);
        shader.setEnvInput(glslang::EShSourceGlsl, shader_stage, glslang::EShClientVulkan, 450);
        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
        shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);
        shader.setAutoMapLocations(true);
        
        if (!shader.parse(&DefaultTBuiltInResource, 450, false, EShMsgDefault))
        {
            std::cerr << "GLSL Parsing Failed:\n" << shader.getInfoLog() << "\n";
            return false;
        }
        
        glslang::TProgram program;
        program.addShader(&shader);
        
        if (!program.link(EShMsgDefault) || !program.mapIO())
        {
            std::cerr << "GLSL Linking Failed:\n" << program.getInfoLog() << "\n";
            return false;
        }
        
        glslang::SpvOptions spv_options = {
            .disableOptimizer = true
        };
        
        spirv_out.clear();
        glslang::GlslangToSpv(*program.getIntermediate(shader_stage), spirv_out, &spv_options);
        
        return !spirv_out.empty();
    }

    static bool remap(
        ShaderStage &stage,
        ShaderResources &resources_out,
        const std::vector<uint32_t>& spirv_in,
        std::vector<uint32_t>& spirv_out)
    {
        using namespace spirv_cross;
        
        CompilerGLSL comp(spirv_in);
        spirv_cross::ShaderResources resources = comp.get_shader_resources();

        for (const auto &ub : resources.uniform_buffers)
        {
            comp.set_decoration(ub.id, spv::DecorationDescriptorSet, 0);

            const uint32_t binding = comp.get_decoration(ub.id, spv::DecorationBinding);
            resources_out.uniform_blocks.push_back({ ub.name, binding });
        }

        for (const auto &smp : resources.sampled_images)
        {
            comp.set_decoration(smp.id, spv::DecorationDescriptorSet, 1);

            const uint32_t binding = comp.get_decoration(smp.id, spv::DecorationBinding);
            resources_out.sampled_images.push_back({ smp.name, binding });
        }
        
        CompilerGLSL::Options glsl_opts;
        glsl_opts.vulkan_semantics = true;
        comp.set_common_options(glsl_opts);
        
        const std::string glsl_source = comp.compile();
        
        return compile_glsl_to_spirv(glsl_source, stage.stage, spirv_out);
    }
};

class CrossCompiler
{
public:
    static std::string to_glsl(const std::vector<uint32_t> &spirv)
    {
        spirv_cross::CompilerGLSL compiler(spirv);
        spirv_cross::CompilerGLSL::Options options;
        options.version = 450;
        options.vulkan_semantics = false;
        options.es = false;
        set_entry_point(compiler);
        compiler.set_common_options(options);
        return compiler.compile();
    }

    static std::string to_glsles(const std::vector<uint32_t> &spirv)
    {
        spirv_cross::CompilerGLSL compiler(spirv);
        spirv_cross::CompilerGLSL::Options options;
        options.version = 300;
        options.vulkan_semantics = false;
        options.es = true;
        set_entry_point(compiler);
        compiler.set_common_options(options);
        return compiler.compile();
    }

    static std::string to_hlsl(const std::vector<uint32_t> &spirv)
    {
        spirv_cross::CompilerHLSL compiler(spirv);
        
        spirv_cross::CompilerGLSL::Options glsl_options;
        glsl_options.emit_line_directives = false;
        glsl_options.vertex.flip_vert_y = true;
        glsl_options.vertex.fixup_clipspace = true;
        set_entry_point(compiler);
        compiler.set_common_options(glsl_options);

        spirv_cross::CompilerHLSL::Options hlsl_options;
        hlsl_options.shader_model = 50;
        compiler.set_hlsl_options(hlsl_options);

        return compiler.compile();
    }

    static std::string to_msl(const std::vector<uint32_t> &spirv)
    {
        spirv_cross::CompilerMSL compiler(spirv);
        set_entry_point(compiler);
        return compiler.compile();
    }
private:
    static void set_entry_point(spirv_cross::Compiler &compiler)
    {
        if (compiler.get_execution_model() == spv::ExecutionModelGLCompute)
            compiler.set_entry_point("main", spv::ExecutionModelGLCompute);
    }
};

class HeaderWriter
{
public:
    static void write_spirv_array(std::ofstream &out, const std::vector<uint32_t> &spirv,
        const std::string &name, const std::string &api, const std::string &stage)
    {
        std::string array_name = name + "_" + api + "_" + stage;
        out << "static const uint32_t " << array_name << "[] = {";

        for (size_t i = 0; i < spirv.size(); ++i)
        {
            if (i % 8 == 0) out << "\n\t";
            out << "0x" << std::hex << std::setw(8) << std::setfill('0') << spirv[i];
            if (i < spirv.size() - 1) out << ",";
        }

        out << "\n};\n\n";
    }

    static void write_source_array(std::ofstream &out, const std::string &source,
        const std::string &name, const std::string &api, const std::string &stage)
    {
        out << "/*\n" << source << "*/\n";

        std::string array_name = name + "_" + api + "_" + stage;
        out << "static const uint8_t " << array_name << "[] = {";

        for (size_t i = 0; i < source.size(); ++i)
        {
            if (i % 16 == 0) out << "\n\t";
            out << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                << static_cast<int>(static_cast<uint8_t>(source[i]));
            if (i < source.size() - 1) out << ",";
        }

        out << "\n};\n\n";
    }

    static void write_shader_getter(
        std::ofstream &out, const std::string &name,
        ShaderType type, uint32_t lang_flags,
        const ShaderResources &resources
    )
    {
        out << std::format("static inline const mg_shader get_{}_shader(mg_renderer_type type) {{\n", name);
        out << "    mg_shader shader = {0};\n";
        out << "    switch (type) {\n";

        if (type == ShaderType::Compute)
            write_compute_cases(out, name, lang_flags);
        else
            write_graphics_cases(out, name, lang_flags);

        out << "    }\n\n";

        for (size_t i = 0; i < resources.uniform_blocks.size(); ++i)
        {
            out << std::format("    shader.uniform_blocks[{}].name = \"{}\";\n",
                i, resources.uniform_blocks[i].name);
            out << std::format("    shader.uniform_blocks[{}].binding = {};\n",
                i, resources.uniform_blocks[i].binding);
        }

        for (size_t i = 0; i < resources.sampled_images.size(); ++i)
        {
            out << std::format("    shader.sampled_images[{}].name = \"{}\";\n",
                i, resources.sampled_images[i].name);
            out << std::format("    shader.sampled_images[{}].binding = {};\n",
                i, resources.sampled_images[i].binding);
        }

        out << "    return shader;\n";
        out << "}\n\n";
    }

private:
    static void write_graphics_cases(std::ofstream &out, const std::string &name, uint32_t lang_flags)
    {
        auto write_case = [&](const std::string &api, const std::string &renderer) {
            out << std::format(
                "        case {}:\n"
                "            shader.vertex.code = (void*){}_{}_vert;\n"
                "            shader.vertex.size = sizeof({}_{}_vert);\n"
                "            shader.fragment.code = (void*){}_{}_frag;\n"
                "            shader.fragment.size = sizeof({}_{}_frag);\n"
                "            break;\n",
                renderer, name, api, name, api, name, api, name, api
            );
        };

        if (lang_flags & SPIRV)  write_case("spirv", "MG_RENDERER_TYPE_VULKAN");
        if (lang_flags & HLSL)   write_case("hlsl", "MG_RENDERER_TYPE_DIRECT3D11");
        if (lang_flags & GLSL)   write_case("glsl", "MG_RENDERER_TYPE_OPENGL");
        if (lang_flags & GLSLES) write_case("glsles", "MG_RENDERER_TYPE_OPENGLES");
        if (lang_flags & MSL)    write_case("msl", "MG_RENDERER_TYPE_METAL");
    }

    static void write_compute_cases(std::ofstream &out, const std::string &name, uint32_t lang_flags)
    {
        auto write_case = [&](const std::string &api, const std::string &renderer) {
            out << std::format(
                "        case {}:\n"
                "            shader.compute.code = (void*){}_{}_comp;\n"
                "            shader.compute.size = sizeof({}_{}_comp);\n"
                "            break;\n",
                renderer, name, api, name, api
            );
        };

        if (lang_flags & SPIRV)  write_case("spirv", "MG_RENDERER_TYPE_VULKAN");
        if (lang_flags & HLSL)   write_case("hlsl", "MG_RENDERER_TYPE_DIRECT3D11");
        if (lang_flags & GLSL)   write_case("glsl", "MG_RENDERER_TYPE_OPENGL");
        if (lang_flags & GLSLES) write_case("glsles", "MG_RENDERER_TYPE_OPENGLES");
        if (lang_flags & MSL)    write_case("msl", "MG_RENDERER_TYPE_METAL");
    }
};

int main(int argc, char **argv)
{
    std::string input_file, output_file, shader_name, lang_string;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "--input" || arg == "-i") && i + 1 < argc)
            input_file = argv[++i];
        else if ((arg == "--output" || arg == "-o") && i + 1 < argc)
            output_file = argv[++i];
        else if ((arg == "--name" || arg == "-n") && i + 1 < argc)
            shader_name = argv[++i];
        else if ((arg == "--lang" || arg == "-l") && i + 1 < argc)
            lang_string = argv[++i];
    }

    if (input_file.empty() || output_file.empty() || shader_name.empty() || lang_string.empty())
    {
        std::cerr << "Usage: " << argv[0] << " -i <input> -o <output> -n <name> -l <langs>\n";
        std::cerr << "  Languages: spirv,glsl,glsles,hlsl,msl\n";
        return 1;
    }

    uint32_t lang_flags = 0;
    std::istringstream lang_stream(lang_string);
    std::string token;
    while (std::getline(lang_stream, token, ','))
    {
        if      (token == "spirv")  lang_flags |= SPIRV;
        else if (token == "glsl")   lang_flags |= GLSL;
        else if (token == "glsles") lang_flags |= GLSLES;
        else if (token == "hlsl")   lang_flags |= HLSL;
        else if (token == "msl")    lang_flags |= MSL;
        else
        {
            std::cerr << "Unknown language: " << token << "\n";
            return 1;
        }
    }

    ShaderProgram program;
    if (!ShaderParser::parse(input_file, program))
        return 1;

    glslang::InitializeProcess();

    ShaderResources resources;
    for (auto &[stage_type, stage] : program.stages)
    {
        if (!SPIRVCompiler::compile(stage, resources))
            return 1;
    }

    std::ofstream header(output_file);
    header << "#pragma once\n\n#include <stdint.h>\n\n";

    if (lang_flags & SPIRV)
    {
        for (const auto &[stage_type, stage] : program.stages)
        {
            std::string stage_name =
                (stage_type == EShLangVertex) ? "vert" :
                (stage_type == EShLangFragment) ? "frag" : "comp";
            HeaderWriter::write_spirv_array(header, stage.spirv, shader_name, "spirv", stage_name);
        }
    }

    struct LangInfo { std::string name; std::function<std::string(const std::vector<uint32_t>&)> compile; };
    std::vector<LangInfo> langs;
    
    if (lang_flags & GLSL)   langs.push_back({"glsl", CrossCompiler::to_glsl});
    if (lang_flags & GLSLES) langs.push_back({"glsles", CrossCompiler::to_glsles});
    if (lang_flags & HLSL)   langs.push_back({"hlsl", CrossCompiler::to_hlsl});
    if (lang_flags & MSL)    langs.push_back({"msl", CrossCompiler::to_msl});

    for (const auto &lang : langs)
    {
        for (const auto &[stage_type, stage] : program.stages)
        {
            std::string stage_name =
                (stage_type == EShLangVertex) ? "vert" :
                (stage_type == EShLangFragment) ? "frag" : "comp";
            std::string compiled = lang.compile(stage.spirv);
            HeaderWriter::write_source_array(header, compiled, shader_name, lang.name, stage_name);
        }
    }

    HeaderWriter::write_shader_getter(header, shader_name, program.type, lang_flags, resources);
    glslang::FinalizeProcess();

    return 0;
}