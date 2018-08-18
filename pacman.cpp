#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>

using namespace std;

const int speed = 3;
const int BLOCK_SIZE = 7 * speed;
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

#include <map>
#include <algorithm>

vector<int> bfs(SDL_Rect rect, string m[31], int aim){
    queue<int> to_visit;
    set<int> visited;
    map<int, int> parents;
    vector<int> path;

    int root = (rect.y / BLOCK_SIZE) * COLS + (rect.x / BLOCK_SIZE);

    to_visit.push(root);
    visited.insert(root);

    parents[root] = -1;

    int node;
    int r,c;

    while(!to_visit.empty()){
        node = to_visit.front();
        to_visit.pop();
        r = node / COLS;
        c = node % COLS;
        
        if(node != aim){
            //get adjacent nodes
            if(c - 1 >= 0 && m[r][c-1] != '*' && visited.find(r * COLS + c - 1) == visited.end()){
                to_visit.push(r * COLS + c - 1);
                visited.insert(r * COLS + c - 1);
                parents[r*COLS + c - 1] = node;
            }
            if(c + 1 < COLS && m[r][c+1] != '*' && visited.find(r * COLS + c + 1) == visited.end()){
                to_visit.push(r * COLS + c + 1);
                visited.insert(r * COLS + c + 1);
                parents[r * COLS + c + 1] = node;
            }
            if(r - 1 >= 0 && m[r-1][c] != '*' && visited.find((r-1)* COLS + c) == visited.end()){
                to_visit.push((r-1)*COLS + c);
                visited.insert((r-1)*COLS + c);
                parents[(r-1)*COLS + c] = node;
            }
            if(r + 1 < ROWS && m[r+1][c] != '*'  && visited.find((r+1)* COLS + c) == visited.end()){
                to_visit.push((r+1) * COLS + c);
                visited.insert((r+1) * COLS + c);
                parents[(r+1) * COLS + c] = node;
            }
        } else {
            //create path
            path.push_back(node);
            int cur = parents[node];
            while(cur != -1){
                path.push_back(cur);
                cur = parents[cur];
            }
            for(int i:path){
                printf("%i i \n", i); 
            }
            reverse(path.begin(), path.end());
            break;
        }
    }
    return path;
}

void moveTo(SDL_Rect& r, vector<int>& path, int speed, string map[31]){
    if(path.size() < 1) return;
    
    
    int next = -1;
    if(path.size() > 1){
        next = path[1];
     } else {
        next = path[0];
     }
   

    int y = (next/COLS) * BLOCK_SIZE; 
    int x = (next%COLS) * BLOCK_SIZE;

    int dx = x - r.x;
    int dy = y - r.y;

    int s = speed;
    map_position* pair = NULL;
    int s_x = 0, s_y = 0;
 
    if(dx != 0){
        if(dx < 0){
            pair = left_side(r, s);
            if(check_pair(pair, map)){
                s_x = -s;
            } 
        } else {
            pair = right_side(r, s);
            if(check_pair(pair, map)){
                s_x = s;
            } 
        }
    } 
    if(dy != 0){
        if(dy < 0){
            pair = top_side(r, s);
            if(check_pair(pair, map)){
                s_y = -s;
            } 
        } else {
            pair = down_side(r, s);
            if(check_pair(pair, map)){
                s_y = s;
            }
        }
    }
    r.x+=s_x;
    r.y+=s_y;
    if(pair != NULL){
         delete[] pair;
    }
}

void correct(int& r, int& c, string map[31], int dx, int dy){
    if(map[r][c] == '*' || map[r][c] == 'h'){
        c+=dx;
        r+=dy;
        correct(r, c, map, dx, dy);
    }
    return;
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
    SDL_Rect p = { BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };

    SDL_Rect enemy = {WIDTH -  2 * BLOCK_SIZE, HEIGHT - 2*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};

    SDL_Rect green_enemy = {2 * BLOCK_SIZE, HEIGHT - 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};

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
    map[p.y/BLOCK_SIZE][p.x/BLOCK_SIZE] = 'p';
    bool pause = false;
    vector<int> path;
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
                    case SDLK_SPACE:
                        pause = !pause;
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
            if(!pause){
                if(pair != NULL && check_pair(pair, map)){
                    map[p.y/BLOCK_SIZE][p.x/BLOCK_SIZE] = ' ';
                    p.x += dx;
                    p.y += dy;
                    //move(p, direction, speed);
                    map[p.y/BLOCK_SIZE][p.x/BLOCK_SIZE] = 'p';
                } else {
                    dx = 0;
                    dy = 0;
                }

                if(pair != NULL){
                    delete[] pair;
                    pair = NULL;
                }

                path = bfs(enemy, map, (p.y / BLOCK_SIZE) * COLS + (p.x / BLOCK_SIZE));
                moveTo(enemy, path, speed, map);
                //target for green
                int target = -1;
                int t_x = p.x/BLOCK_SIZE, t_y = p.y/BLOCK_SIZE;
                if(direction_active == RIGHT){
                    if(t_x + 4 < COLS){
                        t_x = t_x + 4;
                        correct(t_y, t_x, map, -1, 0);
                    }
                } else 
                if(direction_active == LEFT){
                    if(t_x - 4 >= 0){
                        t_x = t_x - 4;
                        correct(t_y, t_x, map, 1, 0);
                    }
                } else 
                if(direction_active == UP){
                    if(t_y - 4 >= 0){
                        t_y = t_y - 4;
                        correct(t_y, t_x, map, 0, 1);
                    }
                } else 
                if(direction_active == DOWN){
                    if(t_y + 4 < ROWS){
                        t_y = t_y + 4;
                        correct(t_y, t_x, map, 0, -1);
                    }
                }
                
                target = t_y * COLS + t_x;
                cout << "the target is \n";
                cout << "target " << target << "\n";
                path = bfs(green_enemy, map, target);
                moveTo(green_enemy, path, speed, map);
           }

            //
            SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(render);

            SDL_SetRenderDrawColor(render, 0xFF, 0, 0, 0xFF);
            renderMap(render, block, map);

            int node_r, node_c;
            SDL_SetRenderDrawColor(render, 0, 0xFF, 0, 0xFF);

            SDL_SetRenderDrawColor(render, 0, 0, 0, 0xFF);
            for(int i:path){
                int row = i / COLS;
                int col = i % COLS;
                block.x = col * BLOCK_SIZE;
                block.y = row * BLOCK_SIZE;
                SDL_RenderFillRect(render, &block);
            }

            SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0, 0xFF);
            SDL_RenderFillRect(render, &p);

            SDL_SetRenderDrawColor(render, 0, 0, 0xFF, 0xFF);
            SDL_RenderFillRect(render, &enemy);

            SDL_SetRenderDrawColor(render, 0, 0xFF, 0, 0xFF);
            SDL_RenderFillRect(render, &green_enemy);

            SDL_SetRenderDrawColor(render, 0, 0, 0xFF, 0xFF);
            SDL_RenderFillRect(render, &enemy);


            SDL_RenderPresent(render);
            accumulator = 0;
        }
        int diff = SDL_GetTicks() - time;
        accumulator += diff;
        time = SDL_GetTicks();
    }

    return 0;   
}
