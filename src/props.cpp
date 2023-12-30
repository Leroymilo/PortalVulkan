#include <iostream>
#include <fstream>
#include <string>

#include "../include/props.hpp"


bool Props::generate_buffers(
	std::vector<Vertex> *vertices,
	std::vector<uint32_t> *indices
) {
	// the vertex index in indices where this brush starts
	vertex_offset = vertices->size();
	first_index = indices->size();

	std::fstream new_file;

	new_file.open("models/" + obj_name + ".obj", std::ios::in);
	// open a file to perform read operation using file object

	std::vector<glm::vec3> V;
	std::vector<glm::vec2> T;
	std::vector<glm::vec3> N;

	std::unordered_map<Vertex, uint32_t> used_vertices;
	
	if (new_file.is_open()) {
		std::string line;
		while(getline(new_file, line)) {
			switch (line[0]) {
				case 'v':
					float x, y, z;
					switch (line[1]) {
						case ' ':
							// vertex
							std::sscanf(line.c_str(), "v %f %f %f", x, y, z);
							V.emplace_back(x, y, z);
							break;
						
						case 'n':
							std::sscanf(line.c_str(), "vn %f %f %f", x, y, z);
							N.emplace_back(x, y, z);
							// normal
							break;

						case 't':
							std::sscanf(line.c_str(), "vt %f %f %f", x, y);
							T.emplace_back(x, y);
							// uv
							break;

						break;
					}

				case 'f':
					// face
					uint32_t v0, t0, n0, v1, t1, n1, v2, t2, n2;
					std::sscanf(
						line.c_str(),
						"f %i/%i/%i %i/%i/%i %i/%i/%i",
						v0, t0, n0, v1, t1, n1, v2, t2, n2
					);

					glm::vec3 V0 = V[v0-1], V1 = V[v1-1], V2 = V[v2-1];
					glm::vec2 T0 = T[t0-1], T1 = T[t1-1], T2 = T[t2-1];
					glm::vec3 N0 = N[n0-1], N1 = N[n1-1], N2 = N[n2-1];

					// computing face tangent and bitangent

					glm::mat2x3 TB = glm::mat2x3(V1-V0, V2-V0) * glm::inverse(glm::mat2x2(T1-T0, T2-T0));
					glm::vec3 T = TB[0], B = TB[1];

					std::array<Vertex, 3> verts = {{
						{V0, T, B, N0, T0},
						{V1, T, B, N1, T1},
						{V2, T, B, N2, T2},
					}};

					for (const Vertex &vert : verts) {
						if (used_vertices.count(vert) == 0) {
							// new vertex
							used_vertices.insert(std::make_pair(vert, used_vertices.size()));
							vertices->push_back(vert);
						}
						indices->push_back(used_vertices.at(vert));
					}

					break;
			}
		}
		new_file.close(); //close the file object.
	}

	return true;
}

void Props::set_descriptor_sets(std::vector<int> &set_indices) {
	this->descriptor_set_indices = set_indices;
}

void Props::cmd_draw_indexed(RenderInfo &render_info, uint32_t uboDynamicIndex) {
	int set_index = descriptor_set_indices[render_info.current_frame];
	VkDescriptorSet descriptor_set = render_info.descriptor_sets[set_index];

	uint32_t dynamicOffset = uboDynamicIndex * static_cast<uint32_t>(render_info.dynamicAlignment);

	vkCmdBindDescriptorSets(
		render_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		render_info.pipeline_layout, 0, 1, &descriptor_set,
		1, &dynamicOffset
	);

	vkCmdDrawIndexed(
		render_info.command_buffer, 36, 1,
		first_index, vertex_offset, 0
	);
}