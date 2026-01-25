#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "Triangle.h"
struct mesh{
    std::vector<triangle> tris;
    bool LoadFromObjectFile(std::string sFilename)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		std::vector<sf::Vector3f> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::stringstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				sf::Vector3f v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}

    bool generatePlane(int length, int height, float res){
        std::vector<sf::Vector3f> verts;
        for(float i = 0.0f; i < height; i++){ 
            for(float j = 0.0f; j < length; j++){
                sf::Vector3f v;
                v.x = j*res;
                v.y=i;
                v.z=i*res;
                verts.push_back(v);
            }
        }

        // Build two triangles per grid cell (quad) avoiding out-of-range accesses.
        // Iterate rows (height) and columns (length), but stop one short on each
        // dimension because each cell references the next row/column.
        if (length > 1 && height > 1) {
            for (int i = 0; i < height - 1; ++i) {
                for (int j = 0; j < length - 1; ++j) {
                    int idx = i * length + j;
                    // First triangle of the quad
                    tris.push_back({ verts[idx], verts[idx + length], verts[idx + length + 1] });
                    //Opposite Side (Counter Clockwise)
                    tris.push_back({ verts[idx], verts[idx + length + 1], verts[idx + length] });

                    // Second triangle of the quad
                    tris.push_back({ verts[idx], verts[idx + length + 1], verts[idx + 1] });
                    //CCW
                    tris.push_back({ verts[idx], verts[idx + 1], verts[idx + length + 1] });

                }
            }
        }


        return true;

    }
};