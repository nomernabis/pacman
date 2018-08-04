#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

const int BLOCK_SIZE = 20;
const int ROWS = 31;
const int COLS = 28;

string map[ROWS];

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

bool canMove(int x1, int y1, int x2, int y2){
	cout << map[y1/BLOCK_SIZE][x1/BLOCK_SIZE] << " " << y2/BLOCK_SIZE << " " << x2/BLOCK_SIZE << "\n";
	return map[y1/BLOCK_SIZE][x1/BLOCK_SIZE] != '*' && map[y2/BLOCK_SIZE][x2/BLOCK_SIZE] != '*';
}
int main(){
	const int WIDTH = BLOCK_SIZE * COLS;
	const int HEIGHT = BLOCK_SIZE * ROWS;
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
	Pos pos, pb;

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

			//can we move in that direction
			int x1=0, y1=0, x2=0, y2=0;
			if(direction_key != NONE){
				if(direction_key == LEFT){
					x1 = p.x - speed;
					y1 = p.y;

					x2 = x1;
					y2 = p.y + p.h - 1;
					if(map[y1/BLOCK_SIZE][x1/BLOCK_SIZE] != '*' && map[y2/BLOCK_SIZE][x2/BLOCK_SIZE] != '*'){
						cout << "can move left\n";
						direction_active = LEFT;
					} else {
						cout << "can't move left\n";
					}
				} else if(direction_key == RIGHT){
					x1 = p.x + p.w + speed - 1;
					y1 = p.y;
					x2 = x1;
					y2 = p.y + p.h - 1;
					if(map[y1/BLOCK_SIZE][x1/BLOCK_SIZE] != '*' && map[y2/BLOCK_SIZE][x2/BLOCK_SIZE] != '*'){
						cout << "can move right\n";
						direction_active = RIGHT;
					} else {
						cout << "can't move right\n";
					}
				} else if(direction_key == DOWN){
					x1 = p.x;
					y1 = p.y + p.h + speed;
					x2 = p.x + p.w - 1;
					y2 = y1;
					if(canMove(x1, y1, x2, y2)){
						direction_active = DOWN;
					} else {
						cout << "can't move down\n";
					}
				} else if(direction_key == UP){
					x1 = p.x;
					y1 = p.y - speed;
					x2 = p.x + p.w - 1;
					y2 = y1;
					if(canMove(x1, y1, x2, y2)){
						direction_active = UP;
					}
				}
			}

			if(direction_active == RIGHT){
				x1 = p.x + p.w + speed - 1;
				y1 = p.y;
				x2 = x1;
				y2 = p.y + p.h - 1;
				if(map[y1/BLOCK_SIZE][x1/BLOCK_SIZE] != '*' && map[y2/BLOCK_SIZE][x2/BLOCK_SIZE] != '*'){
					p.x += speed;
				}
			} else if(direction_active == LEFT){
				x1 = p.x - speed;
				y1 = p.y;
				x2 = x1;
				y2 = p.y + p.h - 1;
				if(map[y1/BLOCK_SIZE][x1/BLOCK_SIZE] != '*' && map[y2/BLOCK_SIZE][x2/BLOCK_SIZE] != '*'){
					p.x -= speed;
				}
			} else if(direction_active == DOWN){
				x1 = p.x;
				y1 = p.y + p.h + speed - 1;
				x2 = p.x + p.w - 1;
				y2 = y1;
				if(canMove(x1, y1, x2, y2)){
					p.y += speed;
				}
			} else if(direction_active == UP){
				x1 = p.x;
				y1 = p.y - speed;
				x2 = p.x + p.w - 1;
				y2 = y1;
				if(canMove(x1, y1, x2, y2)){
					p.y -= speed;
				}
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
