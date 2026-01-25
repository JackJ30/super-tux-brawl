#include "gpu_pipeline.h"

#include "logger.h"
#include "platform.h"
#include "renderer.h"
#include "resource_loader.h"

#include "cjson/cJSON.h"
#include <string.h>

SDL_GPUShaderFormat get_availale_shader_formats() {

    SDL_GPUShaderFormat format = 0;

#ifdef SHADER_FORMAT_SPV
    format |= SDL_GPU_SHADERFORMAT_SPIRV;
#endif
#ifdef SHADER_FORMAT_MSL
    format |= SDL_GPU_SHADERFORMAT_MSL;
#endif
#ifdef SHADER_FORMAT_DXIL
    format |= SDL_GPU_SHADERFORMAT_DXIL;
#endif

    return format;
}

SDL_GPUShader* load_shader(char* path) {
    // get tmp
    ArenaMark s = get_scratch_arena(NULL, 0);

    // get shader stage
    SDL_GPUShaderStage stage;
    if (strstr(path, "vert") != NULL) stage = SDL_GPU_SHADERSTAGE_VERTEX;
    else if (strstr(path, "frag") != NULL) stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    else {
        log_err("Could not determine shader type for: %s", path);
        goto err;
    }

    // get format properties
    SDL_GPUShaderFormat format = SDL_GetGPUShaderFormats(renderer.gpu);
    char* format_name;
    char* format_entrypoint;
    if (format & SDL_GPU_SHADERFORMAT_SPIRV) {
        format_name = "spv";
        format_entrypoint = "main";
    } else if (format & SDL_GPU_SHADERFORMAT_DXIL) {
        format_name = "dxil";
        format_entrypoint = "main";
    } else if (format & SDL_GPU_SHADERFORMAT_MSL) {
        format_name = "msl";
        format_entrypoint = "main0";
    } else {
        log_err("Shader format (%d) fucked sdl for shader: %s", format, path);
        goto err;
    }

    // load shader code
    byte* shader_code = NULL;
    size shader_code_size;
    if (!load_resource_file(tprintf("shaders/%s.%s", path, format_name), s.arena, &shader_code, &shader_code_size)) {
        goto err;
    }

    // load reflection metadata
    byte* metadata = NULL;
    size metadata_size;
    if (!load_resource_file(tprintf("shaders/%s.json", path), s.arena, &metadata, &metadata_size)) {
        goto err;
    }
    cJSON* metadata_json = cJSON_ParseWithLength((char*)metadata, metadata_size);
    if (metadata_json == NULL) {
        log_err("Could not parse shader metadata for: %s", path);
        cJSON_Delete(metadata_json);
        goto err;
    }
    u32 num_samplers = cJSON_GetObjectItemCaseSensitive(metadata_json, "samplers")->valueint;
    u32 num_uniform_buffers = cJSON_GetObjectItemCaseSensitive(metadata_json, "uniform_buffers")->valueint;
    u32 num_storage_buffers = cJSON_GetObjectItemCaseSensitive(metadata_json, "storage_buffers")->valueint;
    u32 num_storage_textures = cJSON_GetObjectItemCaseSensitive(metadata_json, "storage_textures")->valueint;
    cJSON_Delete(metadata_json);

    // create shader
    SDL_GPUShaderCreateInfo info = {
        .code = (Uint8*)shader_code,
        .code_size = shader_code_size,
        .entrypoint = format_entrypoint,
        .format = format,
        .stage = stage,
        .num_samplers = num_samplers,
        .num_uniform_buffers = num_uniform_buffers,
        .num_storage_buffers = num_storage_buffers,
        .num_storage_textures = num_storage_textures,
    };

    // create shader
    SDL_GPUShader* shader = SDL_CreateGPUShader(renderer.gpu, &info);
    if (shader == NULL) {
        log_err("Failed create GPU shader: %s, error: %s", path, SDL_GetError());
        goto err;
    }

    release_scratch_arena(s);
    return shader;

err:
    release_scratch_arena(s);
    return NULL;
}

SDL_GPUGraphicsPipeline* graphics_pipeline_load(char* vertex_path, char* fragment_path, b8 blend) {

    // load shaders
    SDL_GPUShader* vertex = load_shader(vertex_path);
    if (vertex == NULL) {
        return NULL;
    }
    SDL_GPUShader* fragment = load_shader(fragment_path);
    if (fragment == NULL) {
        return NULL;
    }

    // create pipeline
    SDL_GPUGraphicsPipelineCreateInfo info = {
        .target_info = (SDL_GPUGraphicsPipelineTargetInfo){
            .num_color_targets = 1,
            .color_target_descriptions = &(SDL_GPUColorTargetDescription){
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.gpu, platform.window)
            },
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = (SDL_GPURasterizerState){
            .fill_mode = SDL_GPU_FILLMODE_FILL,
        },
        .vertex_shader = vertex,
        .fragment_shader = fragment,
    };
    if (blend) {
        ((SDL_GPUColorTargetDescription*)info.target_info.color_target_descriptions)[0].blend_state = (SDL_GPUColorTargetBlendState){
            .enable_blend = true,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        };
    }

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(renderer.gpu, &info);
    if (pipeline == NULL) {
        log_err("Failed to create graphics pipeline: %s", SDL_GetError());
        return NULL;
    }

    SDL_ReleaseGPUShader(renderer.gpu, vertex);
    SDL_ReleaseGPUShader(renderer.gpu, fragment);
    return pipeline;
}

void graphics_pipeline_destroy(SDL_GPUGraphicsPipeline* pipeline) {
    SDL_ReleaseGPUGraphicsPipeline(renderer.gpu, pipeline);
}
