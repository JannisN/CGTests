#include "Matrix.h"
#include "Bitmap.h"

#include <random>
#include <thread>

using namespace cg;

namespace path2
{
	template <class T, unsigned int N>
	Vector<T, N> normalize(Vector<T, N> vec)
	{
		return vec / (::sqrt(vec * vec) + 0.00001);
	}

	template <class T>
	Vector<T, 3> cross(Vector<T, 3> v1, Vector<T, 3> v2)
	{
		return {
			v1(1) * v2(2) - v1(2) * v2(1),
			v1(2) * v2(0) - v1(0) * v2(2),
			v1(0) * v2(1) - v1(1) * v2(0)
		};
	}

	class RayTraceObject
	{
	public:
		virtual std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> getDistance(Vector<double, 3> origin, Vector<double, 3> direction) { return {}; };
		Vector<double, 3> color;
	};

	class TestPlane : public RayTraceObject
	{
	public:
		std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> getDistance(Vector<double, 3> origin, Vector<double, 3> direction)
		{
			std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> list;
			if (direction(1) != 0)
				list.push_back({ -(origin(1) / direction(1)), this, {0.0, 1.0, 0.0} });
			return list;
		};
	};

	class Sphere : public RayTraceObject
	{
	public:
		double size;
		Vector<double, 3> pos;
		std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> getDistance(Vector<double, 3> origin, Vector<double, 3> direction)
		{
			std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> list;

			double discriminant = (2.0 * direction * (origin - pos)) * (direction * (origin - pos) * 2) - (4.0 * direction * direction * (origin * origin + pos * pos - size * size - 2 * (origin * pos)));

			if (discriminant >= 0)
			{
				double distance1 = (-2.0 * direction * (origin - pos) + std::sqrt(discriminant)) / (2.0 * direction * direction);
				double distance2 = (-2.0 * direction * (origin - pos) - std::sqrt(discriminant)) / (2.0 * direction * direction);
				Vector<double, 3> normal1 = origin + distance1 * direction - pos;
				Vector<double, 3> normal2 = origin + distance2 * direction - pos;
				list.push_back({ distance1, this, normal1 });
				list.push_back({ distance2, this, normal2 });
			}

			return list;
		};
	};

	std::tuple<double, RayTraceObject*, Vector<double, 3>> getClosestObject(std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> list)
	{
		double distance = -1;
		RayTraceObject* closest = nullptr;
		Vector<double, 3> normal;
		for (auto e : list)
		{
			if ((std::get<0>(e) < distance && std::get<0>(e) > 0) || distance < 0)
			{
				distance = std::get<0>(e);
				closest = std::get<1>(e);
				normal = std::get<2>(e);
			}
		}
		if (distance > 0)
			return { distance, closest, normal };
		else
			return { distance, nullptr, normal };
	}


	std::tuple<double, RayTraceObject*, Vector<double, 3>> trace(Vector<double, 3> origin, Vector<double, 3> direction, std::vector<RayTraceObject*> scene)
	{
		std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> list;
		for (auto e : scene)
		{
			auto elems = e->getDistance(origin, direction);
			list.insert(list.end(), elems.begin(), elems.end());
		}
		return getClosestObject(list);
	}

	std::tuple<double, RayTraceObject*, Vector<double, 3>, Vector<double, 3>> tracePlus(Vector<double, 3> origin, Vector<double, 3> direction, std::vector<RayTraceObject*> scene)
	{
		auto [distance, object, normal] = trace(origin, direction, scene);
		normal = normalize(normal);
		Vector<double, 3> pos = origin + distance * direction;
		pos = pos + normal * 0.000001; // delta wert um nicht das gleiche objekt am gleichen ort wieder zu treffen
		return { distance, object, normal, pos };
	}

	double dot(Vector<double, 3> v1, Vector<double, 3> v2)
	{
		return v1(0)* v2(0) + v1(1) * v2(1) + v1(2) * v2(2);
	}

	// funktioniert noch nicht richtig
	Vector<double, 3> randomHemisphere(Vector<double, 3> normal)
	{
		const double pi = 3.141592653589793;
		static std::uniform_real_distribution<double> random(0, 1.0);
		static std::default_random_engine generator;

		double angle1 = random(generator) * pi * 2;
		double angle2 = random(generator) * pi;

		normal = normalize(normal);

		//Vector<double, 3> vec = { std::sin(angle1) * std::sin(angle2), std::cos(angle1) * std::sin(angle2), std::cos(angle2) };
		//
		//double angle = vec * normal;
		//if (angle < 0)
		//{
		//	// funktioniert nicht
		//	vec = vec - normal * angle * 2;
		//}
		//else

		Vector<double, 3> rand = { 2 * (random(generator) - 0.5), 2 * (random(generator) - 0.5), 2 * (random(generator) - 0.5) };
		Vector<double, 3> vec = normalize(normal + normalize(rand));

		return vec;
	}

	Vector<unsigned char, 3> getColor(Vector<double, 3> origin, Vector<double, 3> dest, std::vector<RayTraceObject*> scene)
	{
		Vector<double, 3> direction = normalize(dest - origin);

		auto [distance1, object1, normal1, pos1] = tracePlus(origin, direction, scene);

		Vector<unsigned char, 3> color;

		if (object1 == nullptr)
		{
			color = { 255, 255, 255 };
		}
		else
		{
			double lightIntensity = 0;
			double lightStrength = 0;
			const int samples = 16;

			for (int i = 0; i < samples; i++)
			{
				Vector<double, 3> randomDir = randomHemisphere(normal1);
				auto [distance2, object2, normal2, pos2] = tracePlus(pos1, randomDir, scene);
				if (object2 == nullptr)
				{
					lightStrength += 1.0;
					lightIntensity += 1.0;
					//lightStrength += normal1 * randomDir;
					//lightIntensity += normal1 * randomDir;// std::max(0.0, -(normal1 * lightVec));// *(normal2 * direction);
				}
				else
				{
					lightStrength += 1.0;
					lightIntensity += 0.0;
				}
			}

			lightIntensity /= lightStrength;
			color = { object1->color(0) * lightIntensity, object1->color(1) * lightIntensity, object1->color(2) * lightIntensity };
		}
		return color;
	}

	Vector<double, 3> tracePixel(Vector<double, 3> position, Vector<double, 3> normal, std::vector<RayTraceObject*> scene, unsigned int count, unsigned int countMax)
	{
		if (count >= countMax)
			return { 0.0, 0.0, 0.0 };

		auto [distance, object, normalObject, posObject] = tracePlus(position, normal, scene);

		if (object == nullptr)
			return { 1.0, 1.0, 1.0 };
		
		Vector<double, 3> randomDir = randomHemisphere(normalObject);
		Vector<double, 3> previousColor = tracePixel(posObject, randomDir, scene, count + 1, countMax);

		return { object->color(0) * previousColor(0), object->color(1) * previousColor(1), object->color(2) * previousColor(2) };
	}
	
	void run(unsigned int from, unsigned int to, Vector<double, 3> origin, Bitmap<unsigned char>* bitmap, std::vector<RayTraceObject*> scene)
	{
		for (int i = from; i < to; i++)
		{
			for (int j = 0; j < 1000; j++)
			{
				Vector<double, 3> color = { 0, 0, 0 };
				const int samples = 4;

				for (int k = 0; k < samples; k++)
				{
					Vector<double, 3> dest = { -i / 1000.0 + 0.5, 2.7 - j / 1000.0 + 0.5, 0 };
					color += tracePixel(origin, normalize(dest - origin), scene, 0, 3);
				}

				color *= 1.0 / samples;

				(*bitmap)(i, j, 0) = std::min(color(0) * 255, 255.0);
				(*bitmap)(i, j, 1) = std::min(color(1) * 255, 255.0);
				(*bitmap)(i, j, 2) = std::min(color(2) * 255, 255.0);

			}
		}
	}

	Bitmap<unsigned char> raytrace()
	{
		Bitmap<unsigned char> bitmap(1000, 1000, 3);
		bitmap.fill({ 0, 0, 0 });

		TestPlane plane;
		plane.color = { 1, 1, 1 };

		Sphere sphere;
		sphere.color = { 0.8, 0.8, 1 };
		sphere.size = 1;
		sphere.pos = { 0.15, 1, 6.5 };

		Sphere sphere2;
		sphere2.color = { 1, 0.8, 0.8 };
		sphere2.size = 0.75;
		sphere2.pos = { -1, 0.75, 5.1 };

		Sphere sphere3;
		sphere3.color = { 0.8, 1, 0.8 };
		sphere3.size = 0.5;
		sphere3.pos = { 0.5, 0.5, 5 };

		std::vector<RayTraceObject*> scene;
		scene.push_back(&plane);
		scene.push_back(&sphere);
		scene.push_back(&sphere2);
		scene.push_back(&sphere3);

		Vector<double, 3> origin = { 0, 3, -1 };
		std::vector<std::thread> threads;
		for (int i = 0; i < 8; i++)
		{
			threads.push_back(std::thread(run, i * 125, i * 125 + 125, origin, &bitmap, scene));
		}
		for (int i = 0; i < 8; i++)
		{
			threads[i].join();
		}

		return bitmap;
	}
}

int main()
{
	Bitmap<unsigned char> bitmap(1000, 1000, 3);
	bitmap.fill({ 100, 149, 237 });

	bitmap = path2::raytrace();
	//bitmap.saveAsPPM("test.ppm");
	bitmap.saveAsBMP("pathtrace2.bmp");

	return 0;
}