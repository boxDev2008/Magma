<img src="content/logo.png" width=800>
<br>
Magma is yet another set of tools for game development in C.

# Compilation
To generate the `Sandbox` project we can use premake in the `tools` folder or you can install it yourself.

Then you can run ```.\tools\premake5\premake5.exe vs2022``` on Windows to generate a `Visual Studio 2022` project.

# How to use:

# Platform layer

Basic game loop example using the Magma platform layer
```c
mg_platform_init_info_t platform_init_info = {
	.title = "Magma Sandbox",
    .position_x = 200,
	.position_y = 100,
	.width = 1280,
	.height = 720
};

mg_event_connect(MG_EVENT_CODE_APPLICATION_QUIT, (PFN_on_event)on_application_quit);

mg_platform_t *platform = mg_platform_initialize(&platform_init_info);

while (is_running)
{
    mg_platform_poll_messages(platform);
}

mg_platform_shutdown(platform);
```

# Core layer
- ***Important***: The core functions can only work if the platform layer has been initialized successfully.

### Events
```c
void on_resize(mg_resized_event_data_t *data)
{

}

mg_event_connect(MG_EVENT_CODE_RESIZED, (PFN_on_event)on_resize);
```

- **Event System Setup**: The event system is a mechanism for handling various types of events, such as window resize events, keyboard input, mouse movements, etc. In this case, the framework provides an event system, and `mg_event_connect` is a function to connect a callback function to a specific event.
    
- **Event Code**: Each type of event is identified by a unique event code. In this example, `MG_EVENT_CODE_RESIZED` is the event code for a window resize event.
    
- **Callback Function**: The `on_resize` function is a callback function that will be called whenever a resize event occurs. This function takes a pointer to `mg_resized_event_data_t` structure as an argument, which contains information about the new width and height of the window after resizing.

- **Event Connection**: The `mg_event_connect` function is then used to connect the `on_resize` function to the resize event. Whenever a window resize event occurs, the platform layer will automatically invoke the `on_resize` function.

- **Note**: You can also define custom events.
### Input
- **Enumeration Definitions**:
    - `mg_mouse_buttons`: Defines the different mouse buttons, including left, right, and middle buttons.
    - `mg_keys`: Defines various keyboard keys using their hexadecimal values.

- **Functionality**:
    - `mg_input_is_key_down(key)`: Checks if a specific keyboard key is currently pressed.
    - `mg_input_is_mouse_button_down(button)`: Checks if a specific mouse button is currently pressed.
    - `mg_input_get_mouse_delta()`: Retrieves the change in mouse position since the last frame.
    - `mg_input_get_mouse_position(x, y)`: Retrieves the current position of the mouse cursor.

- **Usage**:
    - To check if a key is pressed, you can use `mg_input_is_key_down()` function by passing the desired key from `mg_keys_t`.
    - Similarly, to check if a mouse button is pressed, use `mg_input_is_mouse_button_down()` by passing the desired button from `mg_mouse_buttons_t`.
    - If you need to know how much the mouse moved since the last frame, you can use `mg_input_get_mouse_delta()`.
    - To get the current position of the mouse cursor, you can use `mg_input_get_mouse_position()` by providing pointers to variables where the x and y coordinates will be stored.

- **Example Usage**:
```c
// Check if the 'A' key is currently pressed
if (mg_input_is_key_down(MG_KEY_A))
{
    // Perform action when 'A' key is pressed
}

// Check if the left mouse button is currently pressed
if (mg_input_is_mouse_button_down(MG_MOUSE_BUTTON_LEFT))
{
    // Perform action when left mouse button is pressed
}

// Get the change in mouse scrolling
int8_t mouse_delta = mg_input_get_mouse_delta();

// Get the current position of the mouse cursor
int32_t mouse_x, mouse_y;
mg_input_get_mouse_position(&mouse_x, &mouse_y);
```

# Low level renderer

Magma provides a Render Hardware Interface (RHI) to allow easy access to different graphics APIs (Vulkan, OpenGL, etc.)

##### Currently supported APIs
- Vulkan (1.0+)
- OpenGL (4.2+)
- DirectX11 - Coming Soon

### Usage

#### Initializing the renderer
```c
mg_swapchain_config_info_t swapchain_config_info = {
    .format = MG_PIXEL_FORMAT_R8G8B8A8_UNORM,
    .present_mode = MG_PRESENT_MODE_FIFO,
    .width = 1280,
    .height = 720
};
mg_renderer_init_info_t init_info = {
    .platform = &platform,
    .type = MG_RENDERER_TYPE_VULKAN, // Or MG_RENDERER_TYPE_OPENGL
    .swapchain_config_info = &swapchain_config_info
};

mg_rhi_renderer_initialize(&init_info);

... // Platform loop

mg_rhi_renderer_shutdown();
```

#### Buffers
- Buffers are regions of memory used to store data required for rendering, such as vertex data, index data, and uniform data.
- There are various types of buffers, including vertex buffers, index buffers, dynamic vertex buffers, dynamic index buffers, and uniform buffers.
- Vertex buffers hold vertex data used to define geometry, while index buffers hold indices that reference vertices, enabling efficient vertex reuse.
- Dynamic buffers are often used for frequently changing data, such as dynamic vertex and index buffers.
- Uniform buffers store data (such as matrices, textures, or other parameters) that remain constant for multiple rendering frames.

Example usage:
```c
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};
size_t vertex_buffer_size = sizeof(vertices);
mg_vertex_buffer_t vertex_buffer = mg_rhi_renderer_create_vertex_buffer(vertex_buffer_size, vertices);

// To bind the vertex buffer
// NOTE: Buffers should be bound after a pipeline
mg_rhi_renderer_bind_vertex_buffer(vertex_buffer);
```

#### Render passes
- A render pass represents a collection of attachments (such as color and depth buffers) used during rendering.
- It defines the layout and usage of these attachments, specifying how they are used as input and output during rendering operations.
- Render passes are essential for organizing rendering operations efficiently and enabling optimizations like framebuffer compression and tile-based rendering.

Example usage:
```c
mg_attachment_info_t color_attachment = {
    .format = MG_PIXEL_FORMAT_R8G8B8A8_UNORM
};
mg_attachment_info_t depth_stencil_attachment = {
    .format = MG_PIXEL_FORMAT_D32_SFLOAT
};
mg_render_pass_create_info_t render_pass_create_info = {
    .color_attachment = color_attachment,
    .depth_stencil_attachment = depth_stencil_attachment,
    .has_depth_stencil_attachment = true
};
mg_render_pass_t render_pass = mg_rhi_renderer_create_render_pass(&render_pass_create_info);
```

#### Samplers
- Samples can be taken using various filtering methods, such as nearest-neighbor filtering or bilinear/trilinear filtering, depending on the sampling mode and texture configuration.

Example usage:
```c
// Declare sampler create info
mg_sampler_create_info_t sampler_create_info = {
    .mag_filter = MG_SAMPLER_FILTER_LINEAR,
    .min_filter = MG_SAMPLER_FILTER_LINEAR,
    .address_mode_u = MG_SAMPLER_ADDRESS_MODE_REPEAT,
    .address_mode_v = MG_SAMPLER_ADDRESS_MODE_REPEAT,
    .address_mode_w = MG_SAMPLER_ADDRESS_MODE_REPEAT
};

// Create sampler
mg_sampler_t sampler = mg_rhi_renderer_create_sampler(&sampler_create_info);
```

#### Images
- Images represent two-dimensional arrays of texels (texture elements) used for rendering and texturing.
- They can be used as color attachments, depth attachments, or sampled within shaders for texturing.
- Images are often associated with specific formats (e.g., RGBA8, depth/stencil formats) that define how texel data is stored and interpreted.

Example usage:
```c
mg_image_create_info_t image_create_info = {
    .format = MG_PIXEL_FORMAT_R8G8B8A8_UNORM,
    .type = MG_IMAGE_TYPE_2D,
    .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
    .width = image_width,
    .height = image_height
};
mg_image_t image = mg_rhi_renderer_create_image(&image_create_info);

// Write image data
mg_image_write_info_t image_write_info = {
    .format = MG_PIXEL_FORMAT_R8G8B8A8_UNORM,
    .width = image_width,
    .height = image_height,
    .data = image_data
};
mg_rhi_renderer_write_image(image, sampler, 0, &image_write_info);

// Update the image to send the new one to the shader
// NOTE: You must update the image after applying any changes to it if you want to see results
mg_rhi_renderer_update_image(image);
```

#### Pipeline
- A pipeline encapsulates the configuration of the rendering pipeline, including vertex and fragment processing stages, rasterization settings, blending options, and input assembly.
- It defines how vertices and primitives are processed and transformed into fragments, and how those fragments are shaded and combined to produce final pixel values.
- Pipelines are crucial for defining the behavior of the GPU during rendering, ensuring consistency across different rendering operations and enabling efficient parallel processing.

Example usage:
```c
// Declare pipeline layout info
mg_vertex_attribute_info_t vertex_attributes[2] = {
    {0, MG_VERTEX_FORMAT_FLOAT3, 0}, // Position
    {1, MG_VERTEX_FORMAT_FLOAT2, 3 * sizeof(float)} // Texture coordinates
};

mg_vertex_layout_info_t vertex_layout_info = {
    .stride = sizeof(float) * 5,
    .attributes = vertex_attributes,
    .attribute_count = 2
};

// Declare pipeline create info
mg_pipeline_create_info_t pipeline_create_info = {
    .vertex_shader = /* Load your vertex shader */,
    .fragment_shader = /* Load your fragment shader */,
    .vertex_layout = vertex_layout_info,
    .polygon_mode = MG_POLYGON_MODE_FILL,
    .cull_mode = MG_CULL_MODE_NONE,
    .front_face = MG_FRONT_FACE_CW,
    .color_blend = /* Define your color blending settings */,
    .depth_stencil = /* Define your depth stencil settings */,
    .render_pass = /* Create or retrieve your render pass */ /*NOTE: If you don't define a render pass, it will use the default one*/,
    .has_push_constants = false,
    .push_constants_size = 0
};

// Create pipeline
mg_pipeline_t pipeline = mg_rhi_renderer_create_pipeline(&pipeline_create_info);
```

#### Framebuffers
- Framebuffers represent a collection of attachments (usually images) that define the rendering target for a particular render pass.
- They provide the render targets where the output of rendering operations (such as color, depth, and stencil data) is written.
- Framebuffers typically consist of one or more attachments, including color attachments for rendering color data and depth/stencil attachments for depth and stencil testing.

Example usage:
```c
mg_framebuffer_create_info_t framebuffer_create_info = {
    .render_pass = render_pass,
    .color_attachment = /* your color image */,
    .depth_stencil_attachment = /* your depth stencil image */,
    .width = 1920,
    .height = 1080
};
mg_framebuffer_t framebuffer = mg_rhi_renderer_create_framebuffer(&framebuffer_create_info);
```

#### Drawing in render loop
```c
// Begin renderer
mg_rhi_renderer_begin_frame();

// Bind render pass and framebuffer
mg_render_pass_begin_info_t render_pass_begin_info = {
	.render_area = {0, 0, swapchain_config_info.width, swapchain_config_info.height},
	.clear_value = /* Define your clear color */
};
mg_rhi_renderer_begin_render_pass(render_pass, framebuffer, &render_pass_begin_info);

// Bind pipeline
mg_rhi_renderer_bind_pipeline(pipeline);

// Bind buffers
mg_rhi_renderer_bind_vertex_buffer(vertex_buffer);
mg_rhi_renderer_bind_index_buffer(index_buffer, MG_INDEX_TYPE_UINT32);

// Draw
mg_rhi_renderer_draw_indexed(/* Number of indices */, /* Start index */);

// End renderer
mg_rhi_renderer_end_render_pass();
mg_rhi_renderer_end_frame();

// Present renderer
mg_rhi_renderer_present_frame();
```

# Audio layer
Still WIP no docs yet
