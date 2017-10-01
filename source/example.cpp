// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy,
// distribute, and modify this file as you see fit.

// Authored in 2015 by Dimitri Diakopoulos (http://www.dimitridiakopoulos.com)
// https://github.com/ddiakopoulos/tinyply

#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "tinyply.h"

using namespace tinyply;

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
std::chrono::high_resolution_clock c;

inline std::chrono::time_point<std::chrono::high_resolution_clock> now()
{
	return c.now();
}

inline double difference_millis(timepoint start, timepoint end)
{
	return (double)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

/*
void write_ply_example(const std::string & filename)
{
	std::vector<float> verts;
	std::vector<float> norms;
	std::vector<uint8_t> colors;

	std::vector<int32_t> vertexIndicies;
	std::vector<float> faceTexcoords;

	// Per-vertex elements
	verts = {
		0.f, -100.462f, -142.5f,
		123.409f, -100.462f, 71.25f,
		0.f, 100.462f, 0.f,
		-123.409f, -100.462f, 71.25f,
		0.f, -100.462f, -142.5f,
		0.f, -100.462f, -142.5f,
		123.409f, -100.462f, 71.25f,
		123.409f, -100.462f, 71.25f,
		0.f, 100.462f, 0.f,
		0.f, 100.462f, 0.f,
		-123.409f, -100.462f, 71.25f,
		-123.409f, -100.462f, 71.25f
	};

	norms = {
		0.853811f, 0.349609f, -0.492948f,
		0.853811f, 0.349609f, -0.492948f,
		0.0f, 0.350761f, 0.989145f,
		0.0f, 0.349609f, 0.985896f,
		-0.853811f, 0.349609f, -0.492948f,
		0.0f, -1.0472f, 0.0f,
		0.0f, 0.349609f, 0.985896f,
		0.0f, -1.0472f, 0.0f,
		0.856624f, 0.350761f, -0.494572f,
		-0.856624f, 0.350761f, -0.494572f,
		-0.853811f, 0.349609f, -0.492948f,
		0.0f, -1.0472f, 0.0f
	};

	colors = {
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 192, 192, 255,
		192, 193, 194, 255,
		195, 196, 197, 255
	};

	// Per-face elements
	vertexIndicies = { 6, 2, 3, 0, 8, 1, 10, 9, 4, 5, 7, 11 };
	faceTexcoords = {
		0.199362f, 0.679351f, 0.399522f, 
		0.333583f, 0.599682f, 0.679351f,
		0.000000f, 0.332206f, 0.399522f,
		0.333583f, 0.199362f, 0.679351f, 
		0.599682f, 0.679351f, 0.399522f,
		0.333583f, 0.799044f, 0.332206f, 
		0.799044f, 0.332206f, 1.000000f,
		0.678432f, 0.599682f, 0.679351f 
	};

	// Tinyply does not perform any file i/o internally
	std::filebuf fb;
	fb.open(filename, std::ios::out | std::ios::binary);
	std::ostream outputStream(&fb);

	PlyFile myFile;

	myFile.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
	myFile.add_properties_to_element("vertex", { "nx", "ny", "nz" }, norms);
	myFile.add_properties_to_element("vertex", { "red", "green", "blue", "alpha" }, colors);

	// List property types must also be created with a count and type of the list (data property type
	// is automatically inferred from the type of the vector argument). 
	myFile.add_properties_to_element("face", { "vertex_indices" }, vertexIndicies, 3, PlyProperty::Type::UINT8);
	myFile.add_properties_to_element("face", { "texcoord" }, faceTexcoords, 6, PlyProperty::Type::UINT8);

	myFile.comments.push_back("generated by tinyply");
	myFile.write(outputStream, true);

	fb.close();
}
*/

void read_ply_file(const std::string & filename)
{
	// Tinyply can and will throw exceptions at you!
	try
	{
		// Read the file and create a std::istringstream suitable
		// for the lib -- tinyply does not perform any file i/o.
		std::ifstream ss(filename, std::ios::binary);

        if (ss.fail()) 
        {
            throw std::runtime_error("failed to open " + filename);
        }

        PlyFile file;

        file.parse_header(ss);

        std::cout << "================================================================\n";

        for (auto c : file.get_comments()) std::cout << "Comment: " << c << std::endl;

		for (auto e : file.get_elements())
		{
			std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
			for (auto p : e.properties)
			{
				std::cout << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[p.propertyType].str << ")" << std::endl;
			}
		}

        std::cout << "================================================================\n";

        std::shared_ptr<PlyData> vertices, normals, colors, faces, texcoords;

        // The header information can be used to programmatically extract properties on elements
        // known to exist in the file prior to reading the data. For brevity of the sample, properties 
        // like vertex position are hard-coded below
        try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { faces = file.request_properties_from_element("face", { "vertex_indices" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { texcoords = file.request_properties_from_element("face", { "texcoord" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

		timepoint before = now();
        file.read(ss);
		timepoint after = now();

		// Good place to put a breakpoint!
		std::cout << "Parsing took " << difference_millis(before, after) << " ms: " << std::endl;
		if (vertices) std::cout << "\tRead " << vertices->count << " total vertices "<< std::endl;
        if (normals) std::cout << "\tRead " << normals->count << " total vertex normals " << std::endl;
        if (colors) std::cout << "\tRead " << colors->count << " total vertex colors "<< std::endl;
        if (faces) std::cout << "\tRead " << faces->count << " total faces (triangles) " << std::endl;
        if (texcoords) std::cout << "\tRead " << texcoords->count << " total texcoords " << std::endl;

        const size_t numVerticesBytes = vertices->count * tinyply::PropertyTable[vertices->t].stride;
        for (size_t i = 0; i < numVerticesBytes; i+=12)
        {
            std::cout << "1 " << *reinterpret_cast<float*>(&vertices->buffer.get()[i + 0]) << std::endl;
            std::cout << "2 " << *reinterpret_cast<float*>(&vertices->buffer.get()[i + 4]) << std::endl;
            std::cout << "3 " << *reinterpret_cast<float*>(&vertices->buffer.get()[i + 8]) << std::endl;
        }
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
	}
}

int main(int argc, char *argv[])
{
	//write_ply_example("example_tetrahedron.ply");
	read_ply_file("../assets/bunny.ply");
	return 0;
}
