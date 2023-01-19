#include <zukou.h>

#include <sys/mman.h>
#include <unistd.h>

#include <array>
#include <cstring>

#include "ray-marching.fragment.h"
#include "ray-marching.vertex.h"

class App : public zukou::IBoundedDelegate
{
  struct Vertex {
    float x, y;
  };

 public:
  App()
      : bounded_(&system_, this),
        pool_(&system_),
        gl_vertex_buffer_(&system_),
        vertex_array_(&system_),
        vertex_shader_(&system_),
        fragment_shader_(&system_),
        program_(&system_),
        rendering_unit_(&system_),
        base_technique_(&system_)
  {}

  ~App()
  {
    if (fd_ != 0) {
      close(fd_);
    }
  }

  // void Render(glm::vec3 half_size) {}

  int Run() { return system_.Run(); }

  void Configure(glm::vec3 half_size, uint32_t serial) override
  {
    base_technique_.Uniform(0, "half_size", half_size);
    bounded_.AckConfigure(serial);
    bounded_.Commit();
  }

  bool Init(const glm::vec3& half_size)
  {
    if (!system_.Init()) return false;
    if (!bounded_.Init(half_size)) return false;

    fd_ = zukou::Util::CreateAnonymousFile(pool_size());
    if (!pool_.Init(fd_, pool_size())) return false;
    if (!vertex_buffer_.Init(&pool_, 0, vertex_buffer_size())) return false;
    if (!gl_vertex_buffer_.Init()) return false;
    if (!vertex_array_.Init()) return false;
    if (!vertex_shader_.Init(
            GL_VERTEX_SHADER, ray_marching_vertex_shader_source))
      return false;
    if (!fragment_shader_.Init(
            GL_FRAGMENT_SHADER, ray_marching_fragment_shader_source))
      return false;
    if (!program_.Init()) return false;

    if (!rendering_unit_.Init(&bounded_)) return false;
    if (!base_technique_.Init(&rendering_unit_)) return false;

    {
      auto vertex_buffer_data = static_cast<char*>(
          mmap(nullptr, pool_size(), PROT_WRITE, MAP_SHARED, fd_, 0));

      std::memcpy(vertex_buffer_data, vertices_.data(), vertex_buffer_size());

      munmap(vertex_buffer_data, pool_size());
    }

    gl_vertex_buffer_.Data(GL_ARRAY_BUFFER, &vertex_buffer_, GL_STATIC_DRAW);

    program_.AttachShader(&vertex_shader_);
    program_.AttachShader(&fragment_shader_);
    program_.Link();

    vertex_array_.Enable(0);
    vertex_array_.VertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0, &gl_vertex_buffer_);

    base_technique_.Bind(&vertex_array_);
    base_technique_.Bind(&program_);

    base_technique_.DrawArrays(GL_TRIANGLE_FAN, 0, vertices_.size());

    return true;
  }

 private:
  zukou::System system_;
  zukou::Bounded bounded_;

  int fd_ = 0;

  zukou::ShmPool pool_;
  zukou::Buffer vertex_buffer_;

  zukou::GlBuffer gl_vertex_buffer_;
  zukou::GlVertexArray vertex_array_;
  zukou::GlShader vertex_shader_;
  zukou::GlShader fragment_shader_;
  zukou::GlProgram program_;

  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;

  const Vertex A = {-1, +1};
  const Vertex B = {-1, -1};
  const Vertex C = {+1, -1};
  const Vertex D = {+1, +1};

  std::array<Vertex, 8> vertices_ = {A, B, C, D};

  inline size_t vertex_buffer_size()
  {
    return sizeof(decltype(vertices_)::value_type) * vertices_.size();
  }

  inline size_t pool_size() { return vertex_buffer_size(); }
};

int
main(int /*argc*/, char const* /*argv*/[])
{
  /* code */
  App app;

  if (!app.Init(glm::vec3(0.1, 0.1, 0.1))) return 1;

  return app.Run();
}
