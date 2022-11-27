#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "Camra.h"
#include "HittableList.hpp"
#include "Material.h"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "color.hpp"
#include "rtweekend.h"
#include "vec3.hpp"

using namespace std;

class RayTracer {
 public:
  RayTracer() {
    world = random_scene();  // Cam
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    cam = new camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture,
                     dist_to_focus);
  }
  double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;

    if (discriminant < 0) {
      return -1.0;
    } else {
      return (-half_b - sqrt(discriminant)) / a;
    }
  }

  color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
      ray scattered;
      color attenuation;
      if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, world, depth - 1);
      return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
  }
  hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    //    for (int a = -5; a < 5; a++) {
    //        for (int b = -5; b < 5; b++) {
    //            auto choose_mat = random_double();
    //            point3 center(a + 0.9*random_double(), 0.2, b +
    //            0.9*random_double());
    //
    //            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
    //                shared_ptr<material> sphere_material;
    //
    //                if (choose_mat < 0.8) {
    //                    // diffuse
    //                    auto albedo = color::random() * color::random();
    //                    sphere_material = make_shared<lambertian>(albedo);
    //                    world.add(make_shared<sphere>(center, 0.2,
    //                    sphere_material));
    //                } else if (choose_mat < 0.95) {
    //                    // metal
    //                    auto albedo = color::random(0.5, 1);
    //                    auto fuzz = random_double(0, 0.5);
    //                    sphere_material = make_shared<metal>(albedo, fuzz);
    //                    world.add(make_shared<sphere>(center, 0.2,
    //                    sphere_material));
    //                } else {
    //                    // glass
    //                    sphere_material = make_shared<dielectric>(1.5);
    //                    world.add(make_shared<sphere>(center, 0.2,
    //                    sphere_material));
    //                }
    //            }
    //        }
    //    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
  }

  // image
  const float aspect_ratio = 1.5;
  const int image_width = 600;
  const int image_height = 400;
  const int samples_per_pixel = 30;
  const int max_depth = 50;

  camera* cam = NULL;
  // World

  hittable_list world;

  void write_color(stringstream& out, color pixel_color,
                   int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma-correct for
    // gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
  }
  void render(int j, int pid, int start, int end,
              unordered_map<int, string>& umap) {
    // insert code here...
    // printf(argv[0]);
    // ofstream myfile("img.ppm");
    stringstream myfile;
    // Image

    // Render

    // myfile << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int i = start; i < end; ++i) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / (image_width - 1);
        auto v = (j + random_double()) / (image_height - 1);
        ray r = cam->get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
      }
      write_color(myfile, pixel_color, samples_per_pixel);
    }

    std::cerr << "\nDone.\n";
    umap[pid] = myfile.str();
  }
};

#endif