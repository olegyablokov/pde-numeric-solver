#ifndef PDE_SETTINGS_H
#define PDE_SETTINGS_H

#include <memory>
#include <QVector>
#include <functional>
#include <sys/types.h>

struct PdeSettings
{
public:
	std::function<float(QVector2D)> V1 = NULL;
	std::function<QVector2D(QVector2D)> V2 = NULL;

	float c = 2.0f; 
	float m= 1.0f;

	int countX = 100;
	int countY = 100; 
    int countT = 200;

	float minX = -10.0f; 
	float maxX = 10.0f;
	float minY = -10.0f; 
	float maxY = 10.0f; 
	float minT = 0.0f; 
    float maxT = 0.7f;

	float stepX;
	float stepY;
	float stepT;

	PdeSettings() 
	{
		stepX = (maxX - minX) / float(countX);
		stepY = (maxY - minY) / float(countY);
		stepT = (maxT - minT) / float(countT);
	}

	PdeSettings(QVariantMap& map) { reset(map); }

	void reset(QVariantMap& map)
	{
		if (map.contains("c")) countX = map["c"].value<float>();
		if (map.contains("m")) countX = map["m"].value<float>();

		if (map.contains("countX")) countX = map["countX"].value<int>();
		if (map.contains("countY")) countY = map["countY"].value<int>();
		if (map.contains("countT")) countT = map["countT"].value<int>();

		if (map.contains("minX")) minX = map["minX"].value<float>();
		if (map.contains("maxX")) maxX = map["maxX"].value<float>();
		if (map.contains("minY")) minY = map["minY"].value<float>();
		if (map.contains("maxY")) maxY = map["maxY"].value<float>();
		if (map.contains("minT")) minT = map["minT"].value<float>();
		if (map.contains("maxT")) maxT = map["maxT"].value<float>();

		stepX = (maxX - minX) / float(countX);
		stepY = (maxY - minY) / float(countY);
		stepT = (maxT - minT) / float(countT);
	}

	QVariantMap toQVariantMap()
	{
		QVariantMap map;
		map.insert("c", c);
		map.insert("m", m);

		map.insert("countX", countX);
		map.insert("countY", countY);
		map.insert("countT", countT);

		map.insert("minX", minX);
		map.insert("maxX", maxX);
		map.insert("minY", minY);
		map.insert("maxY", maxY);
		map.insert("minT", minT);
		map.insert("maxT", maxT);

		return map;
	}
};

#endif //PDE_SETTINGS_H
