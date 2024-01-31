#include "camera.h"
#include "color.h"
#include "constants.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

int main(int argc, char *argv[]) {
  HittableList world;

  auto radius = cos(0.25 * pi);

  auto material_left = make_shared<Lambertian>(Color(0, 0, 1));
  auto material_right = make_shared<Lambertian>(Color(1, 0, 0));

  world.add(make_shared<Sphere>(Point3(-radius, 0, -1), radius, material_left));
  world.add(make_shared<Sphere>(Point3(radius, 0, -1), radius, material_right));

  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.vfov = 90;

  cam.render(world);
  return 0;
}
