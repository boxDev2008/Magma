#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <pystring/pystring.h>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <spirv-tools/optimizer.hpp>
#include <spirv-tools/libspirv.hpp>

#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <spirv_cross/spirv_msl.hpp>

#include "default_resource.h"

struct ShaderResources
{
    struct UniformBlock
    {
        std::string name;
        uint32_t binding;
    };
    std::vector<UniformBlock> uniform_blocks;
    std::string sampled_image_name;
};

struct ShaderSource
{
    std::string vertex;
    std::string fragment;
};

enum ShaderLangFlags
{
    GLSL = 1 << 1,
    GLSLES = 1 << 2,
    HLSL = 1 << 3,
    MSL = 1 << 4,
    SPIRV = 1 << 5
};

void parse_shader(const std::string &file, ShaderSource &out_source)
{
    std::ifstream input_file(file);
    if (!input_file.is_open())
    {
        std::cerr << "Error opening file: " << file << std::endl;
        input_file.close();
        return;
    }

    char line[1024];
    std::stringstream output_vert, output_frag;

    EShLanguage stage = EShLanguage::EShLangVertex;

    std::vector<std::string> tokens;
    while (input_file.getline(line, sizeof(line)))
    {
        pystring::split(line, tokens);
        if (tokens[0] == "@stage")
        {
            if (tokens[1] == "vertex")
                stage = EShLanguage::EShLangVertex;
            else if (tokens[1] == "fragment")
                stage = EShLanguage::EShLangFragment;
        }
        else if (stage == EShLanguage::EShLangVertex)
            output_vert << line << '\n';
        else
            output_frag << line << '\n';
    }

    out_source.vertex = output_vert.str();
    out_source.fragment = output_frag.str();

    input_file.close();
}

void compile_shader_to_spirv(std::string &source, EShLanguage stage, std::vector<uint32_t> &out_spirv)
{
    glslang::InitializeProcess();

    const char* sources[1] = { source.c_str() };
    glslang::TShader shader(stage);
    shader.setStrings(sources, 1);

    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 450);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);
    shader.setAutoMapLocations(true);
    //shader.setAutoMapBindings(true);

    bool parse_success = shader.parse(&DefaultTBuiltInResource, 450, false, EShMsgDefault);
    if (!parse_success)
    {
        std::cerr << "Parsing Failed: " << shader.getInfoLog() << "\n";
        return;
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault) || !program.mapIO())
    {
        std::cerr << "Linking Failed: " << program.getInfoLog() << "\n";
        return;
    }

    glslang::SpvOptions spv_options;
    spv_options.generateDebugInfo = false;
    spv_options.stripDebugInfo = false;
    spv_options.disableOptimizer = true;
    spv_options.optimizeSize = false;
    spv_options.disassemble = false;
    spv_options.validate = false;
    spv_options.emitNonSemanticShaderDebugInfo = false;
    spv_options.emitNonSemanticShaderDebugSource = false;

    glslang::GlslangToSpv(*program.getIntermediate(stage), out_spirv, &spv_options);

    glslang::FinalizeProcess();
}

void compile_spirv_to_glsl(const std::vector<uint32_t> &spirv, std::string &output_source)
{
	spirv_cross::CompilerGLSL glsl(spirv);
    //spirv_cross::ShaderResources resources = glsl.get_shader_resources();

	spirv_cross::CompilerGLSL::Options options;
	options.version = 450;
    options.vulkan_semantics = false;
	options.es = false;
    //options.emit_push_constant_as_uniform_buffer = true;
	glsl.set_common_options(options);
	output_source = glsl.compile();
}

void compile_spirv_to_glsles(const std::vector<uint32_t> &spirv, std::string &output_source)
{
	spirv_cross::CompilerGLSL glsl(spirv);
	spirv_cross::CompilerGLSL::Options options;
	options.version = 300;
    options.vulkan_semantics = false;
	options.es = true;
	glsl.set_common_options(options);
	output_source = glsl.compile();
}

void compile_spirv_to_hlsl(const std::vector<uint32_t> &spirv, std::string &output_source)
{
	spirv_cross::CompilerHLSL hlsl(spirv);
    //spirv_cross::ShaderResources resources = hlsl.get_shader_resources();

    spirv_cross::CompilerGLSL::Options glsl_options;
    glsl_options.emit_line_directives = false;
    glsl_options.vertex.flip_vert_y = true;
    glsl_options.vertex.fixup_clipspace = true;
    hlsl.set_common_options(glsl_options);

    spirv_cross::CompilerHLSL::Options hlsl_options;
    hlsl_options.shader_model = 50;
    hlsl.set_hlsl_options(hlsl_options);

	output_source = hlsl.compile();
}

void compile_spirv_to_metal(const std::vector<uint32_t> &spirv, std::string &output_source)
{
    spirv_cross::CompilerMSL msl(spirv);
    output_source = msl.compile();
}

void get_spirv_resources(const std::vector<uint32_t>& spirv, ShaderResources &output_resources)
{
	spirv_cross::CompilerGLSL glsl(spirv);
    spirv_cross::ShaderResources resources = glsl.get_shader_resources();
    for (auto &resource : resources.uniform_buffers)
    {
        const uint32_t binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
        output_resources.uniform_blocks.push_back({ resource.name, binding });
    }
    if (!resources.sampled_images.empty())
        output_resources.sampled_image_name = resources.sampled_images[0].name;
}

void write_spirv_to_header(const std::vector<uint32_t>& spirv, const std::string& name, const std::string& api, const std::string& stage, std::ofstream& output)
{
    std::string array_name = name + "_" + api + "_" + stage;
    output << "static const uint32_t " << array_name << "[] = {";

    for (size_t i = 0; i < spirv.size(); ++i)
    {
        if (i % 8 == 0) output << "\n\t";
        output << "0x" << std::hex << std::setw(8) << std::setfill('0') << spirv[i];
        if (i < spirv.size() - 1) output << ",";
    }

    output << "\n};\n\n";
}

void write_shader_source_to_header(const std::string& shader_source, const std::string& name, const std::string& api, const std::string& stage, std::ofstream& output)
{
    output << "/*\n" << shader_source << "*/\n";

    std::string array_name = name + "_" + api + "_" + stage;
    output << "static const uint8_t " << array_name << "[] = {";

    for (size_t i = 0; i < shader_source.size(); ++i)
    {
        if (i % 16 == 0) output << "\n\t";
        output << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(shader_source[i]);
        if (i < shader_source.size() - 1) output << ",";
    }

    output << "\n};\n\n";
}

void write_get_shader_function(const std::string& name, std::ofstream& output, uint32_t lang_flags, const ShaderResources &resources)
{
    output << std::format("static inline const mg_shader get_{}_shader(mg_renderer_type type) {{\n", name);
    output << "    mg_shader shader = {0};\n";
    output << "    switch (type) {\n";

    auto write_shader_case = [&](const std::string& shader_type, const std::string& renderer_case) {
        output << std::format(
            "        case {}:\n"
            "            shader.vertex = (mg_shader_source){{.code = {}_{}_vert, .size = sizeof({}_{}_vert)}};\n"
            "            shader.fragment = (mg_shader_source){{.code = {}_{}_frag, .size = sizeof({}_{}_frag)}};\n"
            "            break;\n",
            renderer_case,
            name, shader_type,
            name, shader_type,
            name, shader_type,
            name, shader_type
        );
    };

    if (lang_flags & ShaderLangFlags::SPIRV)
        write_shader_case("spirv", "MG_RENDERER_TYPE_VULKAN");
    if (lang_flags & ShaderLangFlags::HLSL)
        write_shader_case("hlsl", "MG_RENDERER_TYPE_DIRECT3D11");
    if (lang_flags & ShaderLangFlags::GLSL)
        write_shader_case("glsl", "MG_RENDERER_TYPE_OPENGL");
    if (lang_flags & ShaderLangFlags::GLSLES)
        write_shader_case("glsles", "MG_RENDERER_TYPE_OPENGLES");

    output << "    }\n\n";

    for (uint32_t i = 0; i < resources.uniform_blocks.size(); ++i)
    {
        output << std::format("    shader.uniform_blocks[{}].name = \"{}\";\n",
            i,
            resources.uniform_blocks[i].name);
        output << std::format("    shader.uniform_blocks[{}].binding = {};\n",
            i,
            resources.uniform_blocks[i].binding);
    }

    if (!resources.sampled_image_name.empty())
        output << "    shader.sampled_image_name = \"" << resources.sampled_image_name << "\";\n";

    output << "    return shader;\n";
    output << "}\n\n";
}

int main(int argc, char **argv)
{
    const char *input_file = nullptr;
    const char *output_file = nullptr;
    const char *lang = nullptr;
    const char *name = nullptr;

    for (int32_t i = 1; i + 1 < argc; i++)
        if (!strcmp(argv[i], "--input") || !strcmp(argv[i], "-i"))
            input_file = argv[++i];
        else if (!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o"))
            output_file = argv[++i];
        else if (!strcmp(argv[i], "--lang") || !strcmp(argv[i], "-l"))
            lang = argv[++i];
        else if (!strcmp(argv[i], "--name") || !strcmp(argv[i], "-n"))
            name = argv[++i];

    if (!input_file)
    {
        std::cerr << "No input file specified!\nUse the '--input' flag to specify the input file.\n";
        return 1;
    }

    if (!output_file)
    {
        std::cerr << "No output file specified!\nUse the '--output' flag to specify the output file.\n";
        return 1;
    }

    if (!name)
    {
        std::cerr << "No shader name specified!\nUse the '--name' flag to specify the shader name.\n";
        return 1;
    }

    if (!lang)
    {
        std::cerr << "No language specified!\nUse the '--lang glsl,spirv,...' flag (valid options: glsl, spirv, hlsl, msl) to specify the languages to compile to.\n";
        return 1;
    }

    ShaderSource parsed_source;
    parse_shader(input_file, parsed_source);

    std::vector<uint32_t> spirv_vert, spirv_frag;
    compile_shader_to_spirv(parsed_source.vertex, EShLanguage::EShLangVertex, spirv_vert);
    compile_shader_to_spirv(parsed_source.fragment, EShLanguage::EShLangFragment, spirv_frag);

    if (spirv_vert.empty() || spirv_frag.empty())
        return 1;

    std::istringstream lang_stream(lang);
    std::string token;

    uint32_t lang_flags = 0;
    while (std::getline(lang_stream, token, ','))
    {
        if      (token == "spirv")  lang_flags |= ShaderLangFlags::SPIRV;
        else if (token == "glsl")   lang_flags |= ShaderLangFlags::GLSL;
        else if (token == "glsles") lang_flags |= ShaderLangFlags::GLSLES;
        else if (token == "hlsl")   lang_flags |= ShaderLangFlags::HLSL;
        else if (token == "msl")    lang_flags |= ShaderLangFlags::MSL;
        else
        {
            std::cerr << "Unknown language: \'" << token << "\' (valid options: spirv, glsl, hlsl, msl)\n";
            return 1;
        }
    }

    std::ofstream header_file(output_file);
    header_file << "#pragma once\n\n#include <stdint.h>\n\n";

    ShaderSource source;
    ShaderResources resources;

    get_spirv_resources(spirv_vert, resources);
    get_spirv_resources(spirv_frag, resources);

    if (lang_flags & ShaderLangFlags::SPIRV)
    {
        write_spirv_to_header(spirv_vert, name, "spirv", "vert", header_file);
        write_spirv_to_header(spirv_frag, name, "spirv", "frag", header_file);
    }
    if (lang_flags & ShaderLangFlags::GLSL)
    {
        compile_spirv_to_glsl(spirv_vert, source.vertex);
        compile_spirv_to_glsl(spirv_frag, source.fragment);
        write_shader_source_to_header(source.vertex, name, "glsl", "vert", header_file);
        write_shader_source_to_header(source.fragment, name, "glsl", "frag", header_file);
    }
    if (lang_flags & ShaderLangFlags::GLSLES)
    {
        compile_spirv_to_glsles(spirv_vert, source.vertex);
        compile_spirv_to_glsles(spirv_frag, source.fragment);
        write_shader_source_to_header(source.vertex, name, "glsles", "vert", header_file);
        write_shader_source_to_header(source.fragment, name, "glsles", "frag", header_file);
    }
    if (lang_flags & ShaderLangFlags::HLSL)
    {
        compile_spirv_to_hlsl(spirv_vert, source.vertex);
        compile_spirv_to_hlsl(spirv_frag, source.fragment);
        write_shader_source_to_header(source.vertex, name, "hlsl", "vert", header_file);
        write_shader_source_to_header(source.fragment, name, "hlsl", "frag", header_file);
    }
    if (lang_flags & ShaderLangFlags::MSL)
    {
        compile_spirv_to_metal(spirv_vert, source.vertex);
        compile_spirv_to_metal(spirv_frag, source.fragment);
        write_shader_source_to_header(source.vertex, name, "msl", "vert", header_file);
        write_shader_source_to_header(source.fragment, name, "msl", "frag", header_file);
    }

    write_get_shader_function(name, header_file, lang_flags, resources);

    header_file.close();

    return 0;
}