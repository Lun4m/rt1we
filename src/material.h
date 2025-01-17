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

class Dielectric : public Material {
public:
  Dielectric(double index_of_refraction) : ir(index_of_refraction){};

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    attenuation = Color(1.0, 1.0, 1.0);
    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    Vec3 unit_direction = unit_vector(r_in.direction());
    auto cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    auto sin_theta = sqrt(1 - cos_theta * cos_theta); // Should use fabs?
    //
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;

    if (cannot_refract ||
        reflectance(cos_theta, refraction_ratio) > random_double()) {
      direction = reflect(unit_direction, rec.normal);
    } else {
      direction = refract(unit_direction, rec.normal, refraction_ratio);
    }

    scattered = Ray(rec.p, direction);
    return true;
  }

private:
  double ir; // Index of refraction

  static double reflectance(double cosine, double ir_ratio) {
    // Schlink's approximation
    auto r0 = (1 - ir_ratio) / (1 + ir_ratio);
    r0 *= r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};

#endif // !MATERIAL_H
