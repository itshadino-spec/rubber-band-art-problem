#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define title "rubber band art"
#define height 576
#define width 720
#define sixtyfps 16

struct Node{  
    Sint32 xcoordinate;
    Sint32 ycoordinate;
    SDL_Texture *node;
};

struct Game{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    struct Node nodes [400];
    int count;
};


bool load(struct Game *game);
bool init(struct Game *game);
void delete(struct Game *game, int exitstatus);
bool addnode(struct Node *node, struct Game *game);

int main(void){
    bool clickflag = false;
    struct Game game = {
        .window = NULL,
        .renderer = NULL,
        .background = NULL,
        .nodes = NULL,
        .count = 0

    };

    struct Node node ={
        .xcoordinate = 0,
        .ycoordinate = 0,
        .node = NULL
    };

    if(init(&game)){
        delete(&game, EXIT_FAILURE);
    }
    if(load(&game)){
        delete(&game, EXIT_FAILURE);
    }

    while(true){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    delete(&game, EXIT_SUCCESS);
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_ESCAPE:
                            break;
                        default:
                            break;
                    }
                case SDL_MOUSEBUTTONDOWN:
                    clickflag = true;
                    node.xcoordinate = event.button.x;
                    node.ycoordinate = event.button.y;
                    addnode(&node, &game);
                    break;
                default:
                    break;
            }
        }
        SDL_RenderClear(game.renderer);
        SDL_RenderCopy(game.renderer, game.background, NULL,NULL);
        SDL_RenderPresent(game.renderer);

        SDL_Delay(sixtyfps);
        clickflag = false;
    }

    delete(&game, EXIT_SUCCESS);
    printf("test");
    return 0;
}

void delete(struct Game *game, int exitstatus){
    SDL_DestroyRenderer(game -> renderer);
    SDL_DestroyWindow(game -> window);
    SDL_DestroyTexture(game -> background);
    SDL_Quit();
    IMG_Quit();
    exit(exitstatus);
}

bool init(struct Game *game){
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        fprintf(stderr, "error initialising: %s \n", SDL_GetError());
        return true;
    }

    int img_init = IMG_Init(IMG_INIT_PNG);

    if((img_init & IMG_INIT_PNG) != (IMG_INIT_PNG)){
        fprintf(stderr, "error creating image: %s \n", SDL_GetError());
        return true;
    }

    game -> window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width, height,0);

    if(!game -> window){
        fprintf(stderr, "error creating window: %s \n", SDL_GetError());
        return true;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);

    if(!game -> renderer){
        fprintf(stderr, "error creating renderer: %s \n", SDL_GetError());
        return true;
    }
    return false;
}

bool load(struct Game *game){
    game -> background = IMG_LoadTexture(game ->renderer, "images/test.png");
    if(!game -> background){
        fprintf(stderr, "error creating image: %s \n", SDL_GetError());
        return true;
    }
    return false;
}

bool addnode(struct Node *node,struct Game *game){
    printf("xcoordinate: %i, y:coordinate: %i \n", node -> xcoordinate, node -> ycoordinate);   
}

