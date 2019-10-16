#include <iostream>
#include <fstream>
#include <random>
#include <stdlib.h>
#include "vec3.h"
#include "ray.h"
#include "hitable_list.h"
#include "moving_sphere.h"
#include "float.h"
#include "sphere.h"
#include "Material.h"
#include "Camera.h"
#include "rnd.h"
#include <ctime>

using namespace std;


float hit_sphere(const vec3& center, float radius, const ray& r)
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a*c;
	if (discriminant < 0)
	{
		return -1.0;
	}
	else {
		return (-b - sqrt(discriminant)) / (2.0*a);
	}
}

vec3 color(const ray& r, hittable *world,int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 *(unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
	
}
hittable *random_scene() {
	int n = 50000;
	hittable **list = new hittable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(1,1,1)));
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random_double();
			vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					list[i++] = new moving_sphere(
						center,
						center + vec3(0, 0.5*random_double(), 0),
						0.0, 1.0, 0.2,
						new lambertian(
							vec3(random_double()*random_double(),
								random_double()*random_double(),
								random_double()*random_double())
						)
					);
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(
						center, 0.2,
						new metal(
							vec3(0.5*(1 + random_double()),
								0.5*(1 + random_double()),
								0.5*(1 + random_double())),
							0.5*random_double()
						)
					);
				}
				else {  // glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hittable_list(list, i);
}

int main()
{
	int nx = 1024;
	int ny = 700;
	int ns = 800;
	errno_t err;
	//err = fopen_s(&stream, "png.ppm","w+");
	ofstream myFile;
	std::clock_t    start;

	start = std::clock();
	// your test
	
	//(void) fprintf(fp, "P3\n%d, %d\n255\n", nx,ny);
	myFile.open("picture.ppm");
	myFile << "P3\n" << nx << " " << ny << "\n255\n";

	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0, 0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0, 0.0);

	//hittable *list = random_scene();
	float R = cos(std::_Pi  / 4);
	//list[0] = new sphere(vec3(-R, 0, -1), R, new lambertian(vec3(0, 0, 1)));
	//list[1] = new sphere(vec3(R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
	//list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
	//list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(1, 1, 0.8)));
	//list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.5f));
	//list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	hittable *world = random_scene();
	vec3 lookfrom(3, 3, 2);
	vec3 lookat(0, 0, -1);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 0.0;
	camera cam(lookfrom,lookat,vec3(0,1,0),45,float(nx)/float(ny),
			aperture,dist_to_focus,0.0,1.0);
	//vec3* line = (vec3*)malloc(sizeof(vec3)*nx * 3);
	for (int j = ny - 1; j >= 0; j--) {
		
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + random_double()) / float(nx);
				float v = float(j + random_double()) / float(ny);
				ray r = cam.get_ray(u,v);
				col += color(r, world,0);
			}
			col /= float(ns);
			
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);
			myFile << ir << " " << ig << " " << ib << "\n";
			//line[i] = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
		}
		cout << "Lines done: " << nx*(ny-j) << "/" << ny*nx << "  " << ((float(nx) * (float(ny) - float(j))) / (float(ny) * float(nx)))*100 << "%"<< endl;
		
	}
	myFile.close();
	std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
	int wait;
	std::cin >> wait;
	//(void)fclose(fp);
	return EXIT_SUCCESS;
}