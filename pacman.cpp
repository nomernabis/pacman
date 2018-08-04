#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

const int BLOCK_SIZE = 20;
const int ROWS = 31;
const int COLS = 28;

struct map_position{
	int x, y;
	map_position(){
		x=0; y=0;
	}
	map_position(int x, int y){
		this->x = x;
		this->y = y;
	}
};


void renderMap(SDL_Renderer* r,SDL_Rect& block, string map[31]){
	string line;
	for(int i=0; i < ROWS; ++i){
		line = map[i];
		for(int j=0; j < line.length(); ++j){
			if(line[j] == '*'){
				block.y = i * block.h;
				block.x = j * block.w;
				SDL_RenderFillRect(r, &block);
			}
		}
	}
}

bool canMove(int x1, int y1, int x2, int y2, string map[31]){
	return map[y1/BLOCK_SIZE][x1/BLOCK_SIZE] != '*' && map[y2/BLOCK_SIZE][x2/BLOCK_SIZE] != '*';
}

bool check_pair(map_position* pair, string map[31]){
	return canMove(pair[0].x, pair[0].y, pair[1].x, pair[1].y, map);
}


int rect_right(SDL_Rect& rect){
	return rect.x + rect.w - 1;
}

int rect_left(SDL_Rect& rect){
	return rect.x;
}

int rect_top(SDL_Rect& rect){
	return rect.y;
}

int rect_down(SDL_Rect& rect){
	return rect.y + rect.h  - 1;
}

map_position* create_pair(int x1, int y1, int x2, int y2){
	map_position* pair = new map_position[2];
	map_position* a = new map_position(x1, y1);
	map_position* b = new map_position(x2, y2);
	pair[0] = *a;
	pair[1] = *b;
	return pair;
}

map_position* left_side(SDL_Rect& p, int speed){
	int left = rect_left(p);
	map_position* pair = create_pair(left - speed, rect_top(p), left - speed,  rect_down(p));
	return pair;
}

map_position* right_side(SDL_Rect& p, int speed){
	map_position* pair = create_pair(rect_right(p) + speed, rect_top(p), rect_right(p) + speed, rect_down(p));
	return pair;
}

map_position* top_side(SDL_Rect& p, int speed){
	return create_pair(rect_left(p), rect_top(p) - speed, rect_right(p), rect_top(p) - speed);
}

map_position* down_side(SDL_Rect& p, int speed){
	return create_pair(rect_left(p), rect_down(p) + speed, rect_right(p), rect_down(p) + speed);
}

int main(){
	const int WIDTH = BLOCK_SIZE * COLS;
	const int HEIGHT = BLOCK_SIZE * ROWS;
	string map[ROWS];
	//read file
	string line;
	ifstream file("map.txt");
	int cnt = 0;
	if(file.is_open()){
		while(getline(file, line)){
			map[cnt] = line;
			cnt++;	
		}
		file.close();
	}


	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	int dx=0, dy=0;
	bool run = true;
	SDL_Event e;
	int speed = 4;
	SDL_Rect p = { BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
	SDL_Rect block = {0, 0, BLOCK_SIZE, BLOCK_SIZE};

	const int NONE = -1;
	const int LEFT = 0;
	const int RIGHT = 1;
	const int UP = 2;
	const int DOWN = 3;

	double time = 0;
	double accumulator = 0;
	time = SDL_GetTicks();
	int direction_active = NONE;
	int direction_key = NONE;

	map_position* pair;
	while(run){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				run = false;
			} else 
			if(e.type == SDL_KEYDOWN){
				switch(e.key.keysym.sym){
					case SDLK_LEFT:
						direction_key = LEFT;
					break;
					case SDLK_RIGHT:
						direction_key = RIGHT;
					break;
					case SDLK_UP:
						direction_key = UP;
					break;
					case SDLK_DOWN:
						direction_key = DOWN;
					break;
					default:
						break;
				}
			} else 
			if(e.type = SDL_KEYUP){
				direction_key = NONE;
			}
		}

		if(accumulator >= 100/6){
			switch(direction_key){
				case LEFT:
					pair = left_side(p, speed);	
					break;
				case RIGHT:
					pair = right_side(p, speed);
					break;
				case UP:
					pair = top_side(p, speed);
					break;
				case DOWN:
					pair = down_side(p, speed);
					break;
				default:
					pair = NULL;
					break;
			}

			if(pair != NULL && check_pair(pair, map)){
				direction_active = direction_key;
			}

			if(pair != NULL){
				delete[] pair;
				pair = NULL;
			}

			int dx = 0, dy = 0;
			switch(direction_active){
				case LEFT:
					pair = left_side(p, speed);	
					dx = -speed;
					dy = 0;
					break;
				case RIGHT:
					pair = right_side(p, speed);
					dx = speed;
					dy = 0;
					break;
				case UP:
					pair = top_side(p, speed);
					dy = -speed;
					dx = 0;
					break;
				case DOWN:
					pair = down_side(p, speed);
					dy = speed;
					dx = 0;
					break;
				default:
					pair = NULL;
					break;
			}

			if(pair != NULL && check_pair(pair, map)){
				p.x += dx;
				p.y += dy;
			} else {
				dx = 0;
				dy = 0;
			}

			if(pair != NULL){
				delete[] pair;
				pair = NULL;
			}


			SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(render);

			SDL_SetRenderDrawColor(render, 0xFF, 0, 0, 0xFF);
			renderMap(render, block, map);

			SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0, 0xFF);
			SDL_RenderFillRect(render, &p);

			SDL_RenderPresent(render);
				accumulator = 0;
			}
			int diff = SDL_GetTicks() - time;
			accumulator += diff;
			time = SDL_GetTicks();
	}

	return 0;
}
