#include <zukou.h>

#include "sphere-bump.h"
#include "tiff-texture.h"

class CelestialBody final : public zukou::IBoundedDelegate,
                            public zukou::ISystemDelegate
{
 public:
  CelestialBody(const char* color_texture_path, const char* height_texture_path)
      : system_(this),
        bounded_(&system_, this),
        color_texture_path_(color_texture_path),
        height_texture_path_(height_texture_path),
        sphere_(&system_, &bounded_, 8)
  {}

  bool Init(float radius)
  {
    if (!system_.Init()) return false;
    if (!bounded_.Init(glm::vec3(radius))) return false;

    auto tiff_texture0 = std::make_unique<TiffTexture>(&system_);

    if (!tiff_texture0->Init()) return false;
    if (!tiff_texture0->Load(color_texture_path_)) return false;

    auto tiff_texture1 = std::make_unique<TiffTexture>(&system_);

    if (!tiff_texture1->Init()) return false;
    if (!tiff_texture1->Load(height_texture_path_)) return false;

    sphere_.Bind(std::move(tiff_texture0), std::move(tiff_texture1));

    return true;
  }

  int Run() { return system_.Run(); }

  void Configure(glm::vec3 half_size, uint32_t serial) override
  {
    radius_ = glm::min(half_size.x, glm::min(half_size.y, half_size.z));
    sphere_.Render(radius_, glm::mat4(1));

    bounded_.SetTitle("Zukou Celestial Body");
    bounded_.AckConfigure(serial);
    bounded_.NextFrame();
    bounded_.Commit();
  }

  void Frame(uint32_t time) override
  {
    sphere_.Render(radius_, glm::mat4(1), time);
    bounded_.NextFrame();
    bounded_.Commit();
  };

 private:
  zukou::System system_;
  zukou::Bounded bounded_;
  float radius_ = 0.f;

  const char* color_texture_path_;
  const char* height_texture_path_;

  Sphere sphere_;
};

const char* usage =
    "Usage: %s <color texture> <hight texture>\n"
    "\n"
    "    texture: Surface texture in TIFF format"
    "\n";

int
main(int argc, char const* argv[])
{
  if (argc != 3) {
    fprintf(stderr, "tiff file is not specified\n\n");
    fprintf(stderr, usage, argv[0]);
    return EXIT_FAILURE;
  }

  CelestialBody celestial_body(argv[1], argv[2]);

  if (!celestial_body.Init(0.1)) return EXIT_FAILURE;

  return celestial_body.Run();
}
