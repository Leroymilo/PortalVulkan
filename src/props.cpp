#include <iostream>
#include <fstream>

#include "../include/props.hpp"


template <class Collider>
PropInstance<Collider>::PropInstance(Model *model, Collider collider):
	model(model), collider(collider) {}

template <class Collider>
const glm::mat4 &PropInstance<Collider>::get_matrix() {
	return matrix;
}

template <class Collider>
Collider *PropInstance<Collider>::get_collider_p() {
	return &collider;
}

template <class Collider>
void PropInstance<Collider>::cmd_draw_indexed(
	RenderInfo &render_info, uint32_t ubo_dyna_index
) {
	model->cmd_draw_indexed(render_info, ubo_dyna_index);
}

template <class Collider>
void PropInstance<Collider>::process_physics(float delta) {
	// TODO
}

template <class Collider>
void PropInstance<Collider>::nudge(const glm::vec4 &dir_dist) {
	glm::vec3 delta = dir_dist * dir_dist.w;
	matrix = glm::translate(matrix, delta);

}

Cube::Cube(Model *model, float size) :
	PropInstance<Collision::Cube>(model, Collision::Cube(size)) {}

template void PropInstance<Collision::Cube>::nudge(const glm::vec4&);
template const glm::mat4 &PropInstance<Collision::Cube>::get_matrix();
template void PropInstance<Collision::Cube>::cmd_draw_indexed(RenderInfo&, unsigned int);