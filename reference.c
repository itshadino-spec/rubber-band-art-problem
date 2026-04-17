#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>

#define title "01 open window"
#define height 576
#define width 720
#define sixtyfps 16

struct Game{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
};

bool loading(struct Game *game);
bool initialise(struct Game *game);
void clean(struct Game *game, int exitstatus);

int main(void){
    printf("Hello, world!\n");
    struct Game game = {
        .window = NULL,
        .renderer = NULL,
        .background = NULL,
    };
    if(initialise(&game)){
        clean(&game,EXIT_FAILURE);
    }
    if(loading(&game)){
        clean(&game, EXIT_FAILURE);
    }
    while(true){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    clean(&game, EXIT_SUCCESS);
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_ESCAPE:
                            clean(&game, EXIT_SUCCESS);
                            break;
                        case SDL_SCANCODE_SPACE:
                            SDL_SetRenderDrawColor(game.renderer, rand() % 256 ,rand() % 256, rand() % 256,255);
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        SDL_RenderClear(game.renderer);
        SDL_RenderCopy(game.renderer, game.background, NULL, NULL);
        SDL_RenderPresent(game.renderer);

        SDL_Delay(sixtyfps);
    }
    

    clean(&game, EXIT_SUCCESS);
    printf("testest");
    return 0;
}

void clean(struct Game *game, int exitstatus){
    SDL_DestroyRenderer(game -> renderer);
    SDL_DestroyWindow(game -> window);
    SDL_DestroyTexture(game -> background);
    SDL_Quit();
    IMG_Quit();
    exit(exitstatus);
}

bool initialise(struct Game *game){
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        fprintf(stderr, "error initlialising: %s \n", SDL_GetError());
        return true;
    }

    int img_init = IMG_Init(IMG_INIT_PNG);

    if((img_init & IMG_INIT_PNG) != (IMG_INIT_PNG)){
        fprintf(stderr, "error initlialising image: %s \n", SDL_GetError());
        return true;
    }
    game ->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,0);
    
    if(!game -> window){
        fprintf(stderr, "error creating window: %s \n", IMG_GetError());
        return true;
    }
    game -> renderer = SDL_CreateRenderer(game -> window, -1,0);
    if(!game -> renderer){
        fprintf(stderr, "error creating renderer: %s \n", SDL_GetError());
        return true;
    }
    srand((unsigned)time(NULL));
    return false;
}

bool loading(struct Game *game){
    game -> background = IMG_LoadTexture(game->renderer,"images/test.png");
    if(!game -> background){
        fprintf(stderr, "error creating texture: %s \n", IMG_GetError());
        return true;
    }
    return false;
}