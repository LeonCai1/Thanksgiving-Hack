#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "color.hpp"
#include "vec3.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "rtweekend.h"
#include "HittableList.hpp"
#include "Camra.h"
#include "Material.h"
#include <fstream>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;


class RayTracer {
    public:
    RayTracer() {
//        world = random_scene();//Cam
//        point3 lookfrom(13,2,3);
//        point3 lookat(0,0,0);
//        vec3 vup(0,1,0);
//        auto dist_to_focus = 10.0;
//        auto aperture = 0.1;
//        cam = new camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
         initializer("config.json");
    }

    vec3 list2vec3(json list){
      return vec3(list[0], list[1], list[2]);
    }

    auto get_material(json sph){
      shared_ptr<material> result;
      json mat = sph["material"];
      json col = mat["color"];
      std::string m = mat["mat"];
      if(m == "lambertian"){
        result = make_shared<lambertian>(color(list2vec3(col)));
      }else if(m == "dielectric"){
        result = make_shared<dielectric>(mat["index_of_refraction"]);
      }else if(m == "metal"){
        result = make_shared<metal>(color(list2vec3(col)), mat["fuzz"]);
      }
      return result;
    }

    bool initializer(string filename){
      // Image
      std::ifstream f(filename);
      json data = json::parse(f);
      json image_conf = data["image"];
      json world_conf = data["world"];
      json camera_conf = data["camera"];
      aspect_ratio = image_conf["aspect_ration"];
      image_width = image_conf["image_width"];
      image_height = static_cast<int>(image_width / aspect_ratio);
      samples_per_pixel = image_conf["sample_per_pixel"];
      max_depth = image_conf["max_depth"];

      // World
      for (auto& el : world_conf){
        auto sph = el["sphere"];
        auto mat = get_material(sph);
        json center = sph["center"];
        world.add(make_shared<sphere>(list2vec3(center), sph["r"], mat));
      }
      // Camera
      cam = new camera(list2vec3(camera_conf["lookfrom"]), list2vec3(camera_conf["lookat"]), list2vec3(camera_conf["vup"]), camera_conf["vfov"], aspect_ratio, camera_conf["aperture"], camera_conf["dist_to_focus"]);
      return true;
    }

    double hit_sphere(const point3& center, double radius, const ray& r) {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius*radius;
        auto discriminant = half_b*half_b - a*c;

        if (discriminant < 0) {
            return -1.0;
        } else {
            return (-half_b - sqrt(discriminant) ) / a;
        }
    }

    color ray_color(const ray& r, const hittable& world, int depth) {
        hit_record rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0,0,0);

        if (world.hit(r, 0.001, infinity, rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, world, depth-1);
            return color(0,0,0);
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
    }



    //image
    float aspect_ratio = 1.5;
    int image_width = 600;
    int image_height = 400;
    int samples_per_pixel = 30;
    int max_depth = 50;

    camera *cam = NULL;
    // World

    hittable_list world;

    void write_color(stringstream &out, color pixel_color, int samples_per_pixel) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // Divide the color by the number of samples and gamma-correct for gamma=2.0.
        auto scale = 1.0 / samples_per_pixel;
        r = sqrt(scale * r);
        g = sqrt(scale * g);
        b = sqrt(scale * b);

        // Write the translated [0,255] value of each color component.
        out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
    }
    string render(int j) {
        // insert code here...
        // printf(argv[0]);
        // ofstream myfile("img.ppm");
        stringstream myfile;
        // Image
    
        // Render

        // myfile << "P3\n" << image_width << " " << image_height << "\n255\n";

        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam->get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);            
            }
            write_color(myfile, pixel_color, samples_per_pixel);
        }

        std::cerr << "\nDone.\n";
        return myfile.str();
    }
};


#endif