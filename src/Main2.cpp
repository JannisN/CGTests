#include "Matrix.h"
#include "Bitmap.h"

using namespace cg;

class RayTraceObject
{
public:
	virtual std::vector<std::tuple<double, RayTraceObject*, Vector<double, 3>>> getDistance(Vector<double, 3> origin, Vector<double, 3> direction) { return {}; };
	Vector<unsigned char, 3> color;
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

template <class T, unsigned int N>
Vector<T, N> normalize(Vector<T, N> vec)
{
	return vec / ::sqrt(vec * vec);
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
	auto[distance, object, normal] = trace(origin, direction, scene);
	normal = normalize(normal);
	Vector<double, 3> pos = origin + distance * direction;
	pos = pos + normal * 0.00001; // delta wert um nicht das gleiche objekt am gleichen ort wieder zu treffen
	return { distance, object, normal, pos };
}

double getAOLine(double max, Vector<double, 3> normal, Vector<double, 3> pos, std::vector<RayTraceObject*> scene)
{
	auto[distance, object0, normal0, pos0] = tracePlus(pos, normal, scene);
	if (object0 == nullptr)
		return max;
	if (distance < 0)
		return 0;

	double realDistance = (std::min)(max, distance);
	return realDistance;
}

Bitmap<unsigned char> raytrace()
{
	Bitmap<unsigned char> bitmap(1000, 1000, 3);
	bitmap.fill({ 100, 149, 237 });

	TestPlane plane;
	plane.color = { 240, 240, 240 };

	Sphere sphere;
	sphere.color = { 200, 200, 255 };
	sphere.size = 1;
	sphere.pos = { 0, 1, 7 };

	Sphere sphere2;
	sphere2.color = { 255, 200, 200 };
	sphere2.size = 0.75;
	sphere2.pos = { -1, 0.75, 5.5 };

	Sphere sphere3;
	sphere3.color = { 200, 255, 200 };
	sphere3.size = 0.5;
	sphere3.pos = { 0.5, 0.5, 3 };

	std::vector<RayTraceObject*> scene;
	scene.push_back(&plane);
	scene.push_back(&sphere);
	scene.push_back(&sphere2);
	scene.push_back(&sphere3);

	Vector<double, 3> origin = { 0, 1, -1 };

	for (int i = 0; i < 1000; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			Vector<double, 3> dest = { -i / 1000.0 + 0.5, 1 - j / 1000.0 + 0.5, 0 };
			Vector<double, 3> direction = dest - origin;

			auto[distance, object, normal, pos] = tracePlus(origin, direction, scene);

			if (object != nullptr)
			{
				double shadowVolumeSize = 1.5;
				double dist = (getAOLine(shadowVolumeSize, normal, pos, scene) / shadowVolumeSize);
				double shadow = (1 - std::pow(1 - dist, 8)) * 1 + 0.0;
				Vector<double, 3> lightVec = { 0.5, -1.0, 0.5 };
				lightVec = normalize(lightVec);
				double directLight = std::acos(normal * lightVec) / 3.14159265;
				directLight *= 0.5;
				directLight += 0.5;
				Vector<unsigned char, 3> color = object->color;
				bitmap(i, j, 0) = shadow * directLight * color(0);
				bitmap(i, j, 1) = shadow * directLight * color(1);
				bitmap(i, j, 2) = shadow * directLight * color(2);
				//bitmap(i, j, 0) = color(0);
				//bitmap(i, j, 1) = color(1);
				//bitmap(i, j, 2) = color(2);
				//bitmap(i, j, 0) *= shadow;
				//bitmap(i, j, 1) *= shadow;
				//bitmap(i, j, 2) *= shadow;
				//bitmap(i, j, 0) = distance * 3;
				//bitmap(i, j, 1) = distance * 3;
				//bitmap(i, j, 2) = distance * 3;
				//bitmap(i, j, 0) = 100 * (1 + normal(0));
				//bitmap(i, j, 1) = 100 * (1 + normal(1));
				//bitmap(i, j, 2) = 100 * (1 + normal(2));
			}
		}
	}

	return bitmap;
}

std::tuple<double, double, double> hsvToRgb(double h, double s, double v)
{
	int hi = std::floor(h / 60);
	double f = h / 60.0 - hi;
	double p = v * (1 - s);
	double q = v * (1 - s * f);
	double t = v * (1 - s * (1 - f));
	switch (hi)
	{
	case 0:
	case 6:
		return { v, t, p };
	case 1:
		return { q, v, p };
	case 2:
		return { p, v, t };
	case 3:
		return { p, q, v };
	case 4:
		return { t, p, v };
	case 5:
		return { v, p, q };
	}
	return {};
}

int main()
{
	Bitmap<unsigned char> bitmap(1000, 1000, 3);
	bitmap.fill({ 100, 149, 237 });

	bitmap = raytrace();
	bitmap.saveAsPPM("test.ppm");
	bitmap.saveAsBMP("test.bmp");
	
	return 0;
}