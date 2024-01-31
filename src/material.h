#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "ray.h"
#include "vec3.h"

class HitRecord; // avoids circular import

class Material {
public:
  virtual ~Material() = default;
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material {
public:
  Lambertian(const Color &a) : albedo(a) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    auto scatter_direction = rec.normal + random_unit_vector();

    // Prevent degenerate scatter direction
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;

    scattered = Ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }

private:
  Color albedo;
};

class Metal : public Material {
public:
  Metal(const Color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz * random_unit_vector());
    attenuation = albedo;
    return true;
  }

private:
  Color albedo;
  double fuzz;
};

#endif // !MATERIAL_H
