#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

#include "brushes.hpp"

class World {
	private:
		std::vector<SimpleBrush> simple_brushes;
		

	public:
		World();
};

#endif //WORLD_HPP