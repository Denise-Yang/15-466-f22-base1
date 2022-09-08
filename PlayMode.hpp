#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	bool overlappingPlayer(float x,float y);

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	//some weird background animation:
	float background_fade = 0.0f;

	int walkoDir = 0; //0 = right, 1 = down, 2 = left, 3 yp

	int playerAttr = 1;
	int playerIdx = 0;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);
	glm::vec2 walko_at = glm::vec2(0.0f);


	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
