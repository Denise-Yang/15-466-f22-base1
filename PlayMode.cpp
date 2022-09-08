#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"
#include <istream>
#include <fstream>
#include "load_save_png.hpp"
#include <string>

#include <cstdlib> 



#include "read_write_chunk.hpp"
//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

int pres = 0;
glm::u8 convertToInt(unsigned char c1, unsigned char c2, unsigned char c3){
	std::string s;
	s.push_back(c1);
	s.push_back(c2);
	s.push_back(c3);
	return (glm::u8) stoi(s);
}
static std::ifstream mapsBinary( "maps.txt", std::ios::binary );
static std::ifstream tilesBinary( "tiles.txt", std::ios::binary);
static std::ifstream walkoBinary( "walko.txt", std::ios::binary);
static std::ifstream paletteBinary( "palettes.txt", std::ios::binary );
static std::vector<unsigned char>map(std::istreambuf_iterator<char>(mapsBinary), {});
static std::vector<unsigned char>tiles(std::istreambuf_iterator<char>(tilesBinary), {});
static std::vector<unsigned char>colors(std::istreambuf_iterator<char>(paletteBinary), {});
static std::vector<unsigned char>walkoTile(std::istreambuf_iterator<char>(walkoBinary), {});

	
PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.
	// int fdr = open( "maps.txt", O_RDONLY);
    // if (fdr >= 0) {


    // copies all data into buffer
	// std::vector<unsigned char>palettes;
	//read_chunk<unsigned char>(paletteBinary, "pal0", &palettes);
	//printf("suze %lu\n", palettes.size());
	int paletteIdx = 0;
	int palTableIdx = 0;
	for (int i = 0; i < colors.size();i+=10){
		if (paletteIdx >= 4){
			paletteIdx = 0;
			palTableIdx ++;

		}
		glm::u8 r = convertToInt(colors[i], colors[i+1],colors[i+2]);
		glm::u8  g = convertToInt(colors[i+3], colors[i+4],colors[i+5]);		
		glm::u8 b = convertToInt(colors[i+6], colors[i+7],colors[i+8]);
		glm::u8 ff = 255;
		glm::u8vec4 color = {r,g,b,ff};
		ppu.palette_table[palTableIdx][paletteIdx] = color;
		paletteIdx++;
		
		printf("(%d,%d,%d): r %d\n", palTableIdx, colors[i+1], colors[i+2], r);//do palette i-38
	}
	
	
	//store array of rooms =matrices,

	//Also, *don't* use these tiles in your game:

	{ //use tiles 0-16 as some weird dot pattern thing:

	
		for (uint32_t index = 0; index < 5; ++index) {
			uint32_t baseIndex = index*64*2 ;
			if (baseIndex > tiles.size()){
				break;
			}
			PPU466::Tile tile;
			for (uint32_t y = 0; y < 8; ++y) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (uint32_t x = 0; x < 8; ++x) {
					if (tiles[baseIndex + y*8+x]-48!=0) printf("%d",   tiles[baseIndex + y*8+x]-48);
					if (tiles[baseIndex + y*8+x]-48!=0) printf(" %d",   tiles[baseIndex + y*8+x]-48);

					bit0 = (bit0 << 1) | (tiles[baseIndex + y*8+x]-48);
					bit1 = (bit1 << 1) | (tiles[baseIndex + 64 + y*8+x]-48);
				}
				tile.bit0[y] = bit0;
				tile.bit1[y] = bit1;
				printf("\n");
			}
			ppu.tile_table[index] = tile;
		}
	}

	
	//use sprite 32 as a "bubbles":
	ppu.tile_table[32].bit0 = {
		0b01111110,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b01111110,
	};
	ppu.tile_table[32].bit1 = {
		0b00000000,
		0b00011000,
		0b00111100,
		0b00111100,
		0b00111100,
		0b00111100,
		0b00011000,
		0b00000000,
	};


	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0xff, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			playerAttr = 0;
			playerIdx = 1;
			return true;
		}
		
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE ) {
			playerAttr = 1;
			playerIdx = 1;
			return true;
		}
	}

	return false;
}

bool PlayMode::overlappingPlayer(float x,float y){
	if ((x >= player_at.x - 8 && x <= player_at.x+8) && (y >= player_at.y-8 && y <= player_at.y + 8)) printf("overlap \n");
	return (x >= player_at.x - 8 && x <= player_at.x+8) && (y >= player_at.y-8 && y <= player_at.y + 8);
}
void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	constexpr float PlayerSpeed = 30.0f;
	constexpr float WalkoSpeed = 18.0f;
	float dx = 0;
	float dy = 0;
	float diff = WalkoSpeed*elapsed;
	if(walkoDir == 0) dx += WalkoSpeed*elapsed;
	if(walkoDir == 2) dx -= WalkoSpeed*elapsed;
	if(walkoDir == 1) dy -= WalkoSpeed*elapsed;
	if(walkoDir == 3) dy += WalkoSpeed*elapsed;
	if(!(overlappingPlayer(walko_at.x-4,walko_at.y+4)&& playerAttr == 0)){
		if (left.pressed) player_at.x += -PlayerSpeed * elapsed;
		if (right.pressed) player_at.x += PlayerSpeed * elapsed;
		if (down.pressed) player_at.y += -PlayerSpeed * elapsed;
		if (up.pressed) player_at.y += PlayerSpeed * elapsed;
	}

	float x= (walko_at.x-4 + dx+8)/8;
	float y= (walko_at.y+4 + dy+8)/8;
	bool onPlayer0 = overlappingPlayer(walko_at.x-4+diff,walko_at.y+4);
	bool onPlayer1 = overlappingPlayer(walko_at.x-4,walko_at.y+4-diff);
	bool onPlayer2 = overlappingPlayer(walko_at.x-4-diff,walko_at.y+4);
	bool onPlayer3 = overlappingPlayer(walko_at.x-4,walko_at.y+4+diff);
	int currVal = map[(int)((((int)y+2)%PPU466::BackgroundHeight)*PPU466::BackgroundWidth+x)]-48;

	int rightVal = map[(int)((((int)y+2)%PPU466::BackgroundHeight)*PPU466::BackgroundWidth+x+diff)]-48;
	int leftVal = map[(int)((((int)y+2)%PPU466::BackgroundHeight)*PPU466::BackgroundWidth+x-diff)]-48;
	int upVal = map[(int)((((int)(y+2+diff))%PPU466::BackgroundHeight)*PPU466::BackgroundWidth+x)]-48;
	int downVal = map[(int)((((int)(y+2-diff))%PPU466::BackgroundHeight)*PPU466::BackgroundWidth+x)]-48;
	if (currVal == 2){
		//red tiles mean death of Walko
		exit(1);
	}
	if (walkoDir ==0) currVal = rightVal;
	if (walkoDir ==1) currVal = downVal;
	if (walkoDir ==2) currVal = leftVal;
	if (walkoDir ==3) currVal = upVal;
	
	if (!(currVal == 0) || (onPlayer0 && playerAttr == 0)){
		walko_at.y += dy*2;
		walko_at.x += dx*2;
	} else if (!(rightVal == 0) || (onPlayer0 && playerAttr == 0)){
		walko_at.x += dx*2;
		walkoDir = 0;
	} else if (!(downVal == 0) || (onPlayer1 && playerAttr == 0)){
		walko_at.y -= dy*2;
		walkoDir = 1;
	} else if (!(leftVal == 0) || (onPlayer2 && playerAttr == 0)){
		walko_at.x -= dx*2;
		walkoDir = 2;
	}else if (!(upVal == 0) || (onPlayer3 && playerAttr == 0)){
		walko_at.y += dy*2;
		walkoDir = 3;
	}
	



	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	space.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	// ppu.background_color = glm::u8vec4(
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
	// 	0xff
	// );

	ppu.background_color = ppu.palette_table[7][0];

	//tilemap gets recomputed every frame as some weird plasma thing:
	//NOTE: don't do this in your game! actually make a map or something :-)
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			//TODO: make weird plasma thing
			//if (map[x+PPU466::BackgroundWidth*y]-48==3) printf("map %d \n", map[x+PPU466::BackgroundWidth*y]-48);
			ppu.background[x+PPU466::BackgroundWidth*y] = map[x+PPU466::BackgroundWidth*((y+2)%PPU466::BackgroundHeight)]-48;
		}
	}

	//background scroll:
	// ppu.background_position.x = int32_t(-0.5f * player_at.x);
	// ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//walko sprite:
	ppu.sprites[0].x = int8_t(walko_at.x);
	ppu.sprites[0].y = int8_t(walko_at.y+8);
	ppu.sprites[0].index = 4;
	ppu.sprites[0].attributes = 1;

	//player sprite:
	ppu.sprites[1].x = int8_t(player_at.x+16);
	ppu.sprites[1].y = int8_t(player_at.y+8);
	ppu.sprites[1].index = playerIdx;
	ppu.sprites[1].attributes = playerAttr;

	//some other misc sprites:
	for (uint32_t i = 2; i < 63; ++i) {
		float amt = (i + 2.0f * background_fade) / 62.0f;
		ppu.sprites[i].x = int8_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].y = int8_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].index = 32;
		ppu.sprites[i].attributes = 6;
		if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}
