#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "constants.h"
#include "hittable_list.h"
#include "material.h"

#include <iostream>

class Camera {
public:
  double aspect_ratio = 1.0;
  int image_width = 100;
  int samples_per_pixel = 10;
  int max_depth = 10; // Max number of ray bounces into scene

  void render(const HittableList &world) {
    initialize();

    // ppm header
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
      std::clog << "\rScanlines remaining: " << (image_height - j) << ' '
                << std::flush;
      for (int i = 0; i < image_width; i++) {
        Color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
          Ray r = get_ray(i, j);
          pixel_color += ray_color(r, max_depth, world);
        }

        write_color(std::cout, pixel_color, samples_per_pixel);
      }
    }

    std::clog << "\rDone.                 \n";
  }

private:
  int image_height;
  Point3 center;
  Point3 pixel00_loc;
  Vec3 pixel_delta_u;
  Vec3 pixel_delta_v;

  void initialize() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = Point3(0, 0, 0);

    // Viewport dimension
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width =
        viewport_height * (static_cast<double>(image_width) / image_height);

    // Viewport edge vectors
    auto viewport_u = Vec3(viewport_width, 0, 0);
    auto viewport_v = Vec3(0, -viewport_height, 0);

    // Pixel spacings
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Upper left pixel location
    auto viewport_upper_left =
        center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  Ray get_ray(int i, int j) const {
    // Random sample camera ray for pixel (i,j)
    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square();

    auto ray_origin = center;
    auto ray_direction = pixel_sample - ray_origin;
    return Ray(ray_origin, ray_direction);
  }

  Vec3 pixel_sample_square() const {
    // Random point surrounding a pixel (at the origin)
    auto px = -0.5 + random_double();
    auto py = -0.5 + random_double();
    return (px * pixel_delta_u) + (py * pixel_delta_v);
  }

  Color ray_color(const Ray &r, int depth, const HittableList &world) const {
    HitRecord rec;

    if (depth <= 0) {
      return Color(0, 0, 0);
    }
    if (world.hit(r, Interval(0.001, infinity), rec)) {
      Ray scattered;
      Color attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered)) {
        return attenuation * ray_color(scattered, depth - 1, world);
      }

      return Color(0, 0, 0);
    }

    Vec3 unit_direction = unit_vector(r.direction());
    // LERP transformation
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
  };
};

#endif // !CAMERA_H