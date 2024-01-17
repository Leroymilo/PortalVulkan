#include <iostream>
#include <fstream>

#include "../include/props.hpp"

#include <glm/gtx/string_cast.hpp>


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
void PropInstance<Collider>::set_matrix(const glm::mat4 &new_matrix) {
	matrix = new_matrix;
	collider.set_transform(matrix);
}

template <class Collider>
void PropInstance<Collider>::set_rot_pos(const glm::vec3 &new_pos, const glm::vec3 &new_rot) {
	position = new_pos;
	rotation = new_rot;
	glm::mat4 new_matrix = glm::mat4(1.f);
	new_matrix = glm::translate(new_matrix, position);
	if (glm::length(rotation) > 0)
		new_matrix = glm::rotate(new_matrix, glm::length(rotation), glm::normalize(rotation));
	set_matrix(new_matrix);
}


template <class Collider>
void PropInstance<Collider>::process_physics(float delta) {
	speed += 9.81f * mass * delta * glm::vec3(0, 0, -1);

	// std::cout << "speed : " << speed.x << ", " << speed.y << ", " << speed.z << std::endl;

	float abs_speed = glm::length(speed);
	float abs_rot_sp = glm::length(rot_speed);
	if (abs_speed > 0)
		speed -= 0.1f * glm::normalize(speed) * powf(glm::length(speed), 2.f);
	if (abs_rot_sp > 0)
		rot_speed -= 0.1f * glm::normalize(rot_speed) * powf(glm::length(rot_speed), 2.f);

	// std::cout << "speed : " << speed.x << ", " << speed.y << ", " << speed.z << std::endl;

	position += speed * delta;
	rotation += rot_speed * delta;	// according to SO

	glm::mat4 new_matrix = glm::mat4(1.f);
	new_matrix = glm::translate(new_matrix, position);
	if (glm::length(rotation) > 0)
		new_matrix = glm::rotate(new_matrix, glm::length(rotation), glm::normalize(rotation));
	set_matrix(new_matrix);

	// std::cout << glm::to_string(matrix) << std::endl;
}

template <class Collider>
void PropInstance<Collider>::resolve(const glm::vec4 &dir_dist, float delta) {
	glm::vec3 dA = - dir_dist * dir_dist.w;
	glm::vec3 A = collider.support(glm::vec3(dir_dist));
	glm::vec3 G = collider.get_center();

	glm::vec3 d_rot_speed = glm::cross(dA, G - A) / delta;

	rot_speed += d_rot_speed;
	speed += dA / delta + glm::cross(A - G, d_rot_speed);

	// this change in speed will be applied on the next physics process
}

Cube::Cube(Model *model, float size) :
	PropInstance<Collision::Cube>(model, Collision::Cube(size)) {}

template Collision::Cube *PropInstance<Collision::Cube>::get_collider_p();
template const glm::mat4 &PropInstance<Collision::Cube>::get_matrix();
// template void PropInstance<Collision::Cube>::set_matrix(const glm::mat4&);
template void PropInstance<Collision::Cube>::set_rot_pos(const glm::vec3&, const glm::vec3&);
template void PropInstance<Collision::Cube>::process_physics(float);
template void PropInstance<Collision::Cube>::resolve(const glm::vec4&, float);
template void PropInstance<Collision::Cube>::cmd_draw_indexed(RenderInfo&, unsigned int);