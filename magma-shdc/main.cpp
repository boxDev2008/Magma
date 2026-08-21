#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <spirv_cross/spirv_msl.hpp>

#include "default_resource.h"

enum class ShaderType { Graphics, Compute };

enum
{
    GLSL430 = 1 << 1,
    GLSL300ES = 1 << 2,
    HLSL5 = 1 << 3,
    MSL = 1 << 4,
    SPIRV = 1 << 5
};
typedef uint32_t ShaderLangFlags;

struct CompileOptions
{
    bool flip_vert_y = false;
    bool fixup_clipspace = false;

    bool set(const std::string &flag, std::string &error)
    {
        if (flag == "flip_vert_y")
            flip_vert_y = true;
        else if (flag == "fixup_clipspace")
            fixup_clipspace = true;
        else
        {
            error = "Unknown option '" + flag + "'";
            return false;
        }
        return true;
    }
};

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
    std::string name;

    CompileOptions glsl_options;
    CompileOptions glsles_options;
    CompileOptions hlsl_options;
    CompileOptions msl_options;
    bool glsles_options_set = false;
};

enum class ShaderResourceType : uint8_t
{
    SampledImage,
    StorageImage,
    StorageBuffer
};

struct ShaderResources
{
    struct UniformBlock
    {
        std::string name;
        uint32_t binding;
        uint32_t size;

        bool operator<(const UniformBlock &other) const { return binding < other.binding; }
    };

    struct Resource
    {
        std::string name;
        uint32_t binding;
        ShaderResourceType type;

        bool operator<(const Resource &other) const { return binding < other.binding; }
    };

    std::set<UniformBlock> uniform_blocks;
    std::set<Resource> resources;
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
            else if (line.find("@name") == 0)
            {
                if (!parse_name_directive(line, program))
                    return false;
            }
            else if (line.find("@glsl_options") == 0)
            {
                if (!parse_options_directive(line, "@glsl_options", program.glsl_options))
                    return false;
            }
            else if (line.find("@glsles_options") == 0)
            {
                if (!parse_options_directive(line, "@glsles_options", program.glsles_options))
                    return false;
                program.glsles_options_set = true;
            }
            else if (line.find("@hlsl_options") == 0)
            {
                if (!parse_options_directive(line, "@hlsl_options", program.hlsl_options))
                    return false;
            }
            else if (line.find("@msl_options") == 0)
            {
                if (!parse_options_directive(line, "@msl_options", program.msl_options))
                    return false;
            }
            else if (current_stage != EShLangCount)
                current_source << line << '\n';
        }

        if (current_stage != EShLangCount)
            save_stage(program, current_stage, current_source.str());

        if (!program.glsles_options_set)
            program.glsles_options = program.glsl_options;

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

    static bool parse_name_directive(const std::string &line, ShaderProgram &program)
    {
        std::istringstream iss(line);
        std::string tag, name;
        iss >> tag >> name;

        if (name.empty())
        {
            std::cerr << "Error: '@name' directive requires a name, e.g. '@name mesh'\n";
            return false;
        }

        program.name = name;
        return true;
    }

    static bool parse_options_directive(const std::string &line, const std::string &tag, CompileOptions &options)
    {
        std::string rest = line.substr(tag.size());
        std::replace(rest.begin(), rest.end(), ',', ' ');

        std::istringstream iss(rest);
        std::string flag;
        bool any = false;

        while (iss >> flag)
        {
            any = true;
            std::string error;
            if (!options.set(flag, error))
            {
                std::cerr << "Error: " << error << " in '" << tag << "' directive\n";
                return false;
            }
        }

        if (!any)
        {
            std::cerr << "Error: '" << tag << "' directive requires at least one flag\n";
            return false;
        }

        return true;
    }

    static void save_stage(ShaderProgram &program, EShLanguage stage, const std::string &source)
    {
        program.stages[stage] = { {}, source, stage };
    }

    static bool validate_program(const ShaderProgram &program)
    {
        if (program.name.empty())
        {
            std::cerr << "Error: Shader is missing a '@name <name>' directive\n";
            return false;
        }

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
            const uint32_t binding = comp.get_decoration(ub.id, spv::DecorationBinding);

            const spirv_cross::SPIRType &type = comp.get_type(ub.base_type_id);
            const uint32_t size = static_cast<uint32_t>(comp.get_declared_struct_size(type));

            if (!resources_out.uniform_blocks.insert({ ub.name, binding, size }).second)
            {
                std::cerr << "Error: Uniform block '" << ub.name
                           << "' has binding " << binding
                           << " which collides with another uniform block\n";
                return false;
            }

            comp.set_decoration(ub.id, spv::DecorationDescriptorSet, 0);
        }

        auto insert_resource = [&](uint32_t id, const std::string &name, uint32_t binding, ShaderResourceType type) -> bool
        {
            if (!resources_out.resources.insert({ name, binding, type }).second)
            {
                std::cerr << "Error: Resource '" << name
                           << "' has binding " << binding
                           << " which collides with another resource in set 1\n";
                return false;
            }

            comp.set_decoration(id, spv::DecorationDescriptorSet, 1);
            return true;
        };

        for (const auto &smp : resources.sampled_images)
        {
            const uint32_t binding = comp.get_decoration(smp.id, spv::DecorationBinding);
            if (!insert_resource(smp.id, smp.name, binding, ShaderResourceType::SampledImage))
                return false;
        }

        for (const auto &img : resources.storage_images)
        {
            const uint32_t binding = comp.get_decoration(img.id, spv::DecorationBinding);
            if (!insert_resource(img.id, img.name, binding, ShaderResourceType::StorageImage))
                return false;
        }

        for (const auto &storage : resources.storage_buffers)
        {
            const uint32_t binding = comp.get_decoration(storage.id, spv::DecorationBinding);
            if (!insert_resource(storage.id, storage.name, binding, ShaderResourceType::StorageBuffer))
                return false;
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
    static std::string to_glsl430(const std::vector<uint32_t> &spirv, const CompileOptions &opts)
    {
        spirv_cross::CompilerGLSL compiler(spirv);
        spirv_cross::CompilerGLSL::Options options;
        options.version = 430;
        options.vulkan_semantics = false;
        options.es = false;
        options.vertex.flip_vert_y = opts.flip_vert_y;
        options.vertex.fixup_clipspace = opts.fixup_clipspace;
        set_entry_point(compiler);
        compiler.set_common_options(options);
        compiler.add_header_line("#define MAGMA_GLSL 1");
        return compiler.compile();
    }

    static std::string to_glsl300es(const std::vector<uint32_t> &spirv, const CompileOptions &opts)
    {
        spirv_cross::CompilerGLSL compiler(spirv);
        spirv_cross::CompilerGLSL::Options options;
        options.version = 300;
        options.vulkan_semantics = false;
        options.es = true;
        options.vertex.flip_vert_y = opts.flip_vert_y;
        options.vertex.fixup_clipspace = opts.fixup_clipspace;
        set_entry_point(compiler);
        compiler.set_common_options(options);
        compiler.add_header_line("#define MAGMA_GLSL 1");
        return compiler.compile();
    }

    static std::string to_hlsl5(const std::vector<uint32_t> &spirv, const CompileOptions &opts)
    {
        spirv_cross::CompilerHLSL compiler(spirv);
        
        spirv_cross::CompilerGLSL::Options glsl_options;
        glsl_options.emit_line_directives = false;
        glsl_options.vertex.flip_vert_y = opts.flip_vert_y;
        glsl_options.vertex.fixup_clipspace = opts.fixup_clipspace;
        set_entry_point(compiler);
        compiler.set_common_options(glsl_options);

        spirv_cross::CompilerHLSL::Options hlsl_options;
        hlsl_options.shader_model = 50;
        compiler.set_hlsl_options(hlsl_options);
        compiler.add_header_line("#define MAGMA_HLSL 1");

        return compiler.compile();
    }

    static std::string to_msl(const std::vector<uint32_t> &spirv, const CompileOptions &opts)
    {
        spirv_cross::CompilerMSL compiler(spirv);

        spirv_cross::CompilerGLSL::Options common_opts;
        common_opts.vertex.flip_vert_y = opts.flip_vert_y;
        common_opts.vertex.fixup_clipspace = opts.fixup_clipspace;
        compiler.set_common_options(common_opts);
        compiler.add_header_line("#define MAGMA_MSL 1");

        spirv_cross::ShaderResources resources = compiler.get_shader_resources();
        for (const auto &ub : resources.uniform_buffers)
        {
            uint32_t binding = compiler.get_decoration(ub.id, spv::DecorationBinding);
            spirv_cross::MSLResourceBinding rb;
            rb.stage = compiler.get_execution_model();
            rb.desc_set = compiler.get_decoration(ub.id, spv::DecorationDescriptorSet);
            rb.binding = binding;
            rb.msl_buffer = binding + 1;
            compiler.add_msl_resource_binding(rb);
        }

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
    static void write_spirv_array(std::ostream &out, const std::vector<uint32_t> &spirv,
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

    static void write_source_array(std::ostream &out, const std::string &source,
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
        std::ostream &out, const std::string &name,
        ShaderType type, uint32_t lang_flags,
        const ShaderResources &resources
    )
    {
        out << std::format("static inline const mgfx_shader get_{}_shader(mgfx_shader_lang lang) {{\n", name);
        out << "    mgfx_shader shader = {{{0}}};\n";
        out << "    switch (lang) {\n";

        if (type == ShaderType::Compute)
            write_compute_cases(out, name, lang_flags);
        else
            write_graphics_cases(out, name, lang_flags);

        out << "    }\n\n";

        uint32_t i = 0;
        for (const auto &ub : resources.uniform_blocks)
        {
            out << std::format("    shader.uniform_blocks[{}].name = \"{}\";\n",    i, ub.name);
            out << std::format("    shader.uniform_blocks[{}].binding = {};\n",     i, ub.binding);
            out << std::format("    shader.uniform_blocks[{}].size = {};\n",        i, ub.size);
            ++i;
        }

        i = 0;
        for (const auto &res : resources.resources)
        {
            out << std::format("    shader.resources[{}].name = \"{}\";\n", i, res.name);
            out << std::format("    shader.resources[{}].binding = {};\n",  i, res.binding);

            std::string type;
            switch (res.type)
            {
            case ShaderResourceType::SampledImage: type = "MGFX_SHADER_RESOURCE_TYPE_SAMPLED_IMAGE"; break;
            case ShaderResourceType::StorageImage: type = "MGFX_SHADER_RESOURCE_TYPE_STORAGE_IMAGE"; break;
            case ShaderResourceType::StorageBuffer: type = "MGFX_SHADER_RESOURCE_TYPE_STORAGE_BUFFER"; break;
            }
            out << std::format("    shader.resources[{}].type = {};\n", i, type);
            ++i;
        }

        out << "    return shader;\n";
        out << "}\n\n";
    }

private:
    static void write_graphics_cases(std::ostream &out, const std::string &name, uint32_t lang_flags)
    {
        auto write_case = [&](const char *api, const char *renderer) {
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

        if (lang_flags & SPIRV)  write_case("spirv", "MGFX_SHADER_LANG_SPIRV");
        if (lang_flags & HLSL5)   write_case("hlsl5", "MGFX_SHADER_LANG_HLSL5");
        if (lang_flags & GLSL430)   write_case("glsl430", "MGFX_SHADER_LANG_GLSL430");
        if (lang_flags & GLSL300ES) write_case("glsl300es", "MGFX_SHADER_LANG_GLSL300ES");
        if (lang_flags & MSL)    write_case("msl", "MGFX_SHADER_LANG_MSL");
    }

    static void write_compute_cases(std::ostream &out, const std::string &name, uint32_t lang_flags)
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

        if (lang_flags & SPIRV)  write_case("spirv", "MGFX_SHADER_LANG_SPIRV");
        if (lang_flags & HLSL5)   write_case("hlsl5", "MGFX_SHADER_LANG_HLSL5");
        if (lang_flags & GLSL430)   write_case("glsl430", "MGFX_SHADER_LANG_GLSL430");
        if (lang_flags & GLSL300ES) write_case("glsl300es", "MGFX_SHADER_LANG_GLSL300ES");
        if (lang_flags & MSL)    write_case("msl", "MGFX_SHADER_LANG_MSL");
    }
};

int main(int argc, char **argv)
{
    std::string input_file, output_file, lang_string;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "--input" || arg == "-i") && i + 1 < argc)
            input_file = argv[++i];
        else if ((arg == "--output" || arg == "-o") && i + 1 < argc)
            output_file = argv[++i];
        else if ((arg == "--lang" || arg == "-l") && i + 1 < argc)
            lang_string = argv[++i];
    }

    if (input_file.empty() || output_file.empty() || lang_string.empty())
    {
        std::cerr << "Usage: " << argv[0] << " -i <input> -o <output> -l <langs>\n";
        std::cerr << "  Languages: spirv,glsl430,glsl300es,hlsl5,msl\n";
        return 1;
    }

    uint32_t lang_flags = 0;
    std::istringstream lang_stream(lang_string);
    std::string token;
    while (std::getline(lang_stream, token, ','))
    {
        if      (token == "spirv")  lang_flags |= SPIRV;
        else if (token == "glsl430")   lang_flags |= GLSL430;
        else if (token == "glsl300es") lang_flags |= GLSL300ES;
        else if (token == "hlsl5")   lang_flags |= HLSL5;
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

    std::ostringstream header;
    bool ok = true;

    try
    {
        ShaderResources resources;
        for (auto &[stage_type, stage] : program.stages)
        {
            if (!SPIRVCompiler::compile(stage, resources))
                throw std::runtime_error("SPIR-V compilation failed");
        }

        header << "#pragma once\n\n#include <stdint.h>\n\n";

        const std::string &shader_name = program.name;

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

        struct LangInfo
        {
            std::string name;
            std::function<std::string(const std::vector<uint32_t>&)> compile;
        };
        std::vector<LangInfo> langs;

        if (lang_flags & GLSL430)
            langs.push_back({"glsl430", [&](const std::vector<uint32_t> &spirv) {
                return CrossCompiler::to_glsl430(spirv, program.glsl_options);
            }});
        if (lang_flags & GLSL300ES)
            langs.push_back({"glsl300es", [&](const std::vector<uint32_t> &spirv) {
                return CrossCompiler::to_glsl300es(spirv, program.glsles_options);
            }});
        if (lang_flags & HLSL5)
            langs.push_back({"hlsl5", [&](const std::vector<uint32_t> &spirv) {
                return CrossCompiler::to_hlsl5(spirv, program.hlsl_options);
            }});
        if (lang_flags & MSL)
            langs.push_back({"msl", [&](const std::vector<uint32_t> &spirv) {
                return CrossCompiler::to_msl(spirv, program.msl_options);
            }});

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
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        ok = false;
    }

    glslang::FinalizeProcess();

    if (!ok)
        return 1;

    std::ofstream out_file(output_file);
    if (!out_file.is_open())
    {
        std::cerr << "Error: Cannot open output file '" << output_file << "' for writing\n";
        return 1;
    }

    out_file << header.str();

    return 0;
}