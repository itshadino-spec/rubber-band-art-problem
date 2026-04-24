#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define title "rubber band art"
#define height 640
#define width 640
#define sixtyfps 16

struct Node{  
    Sint32 xcoordinate;
    Sint32 ycoordinate;
    SDL_Texture *node;
};

typedef struct{
    SDL_Rect area;
    SDL_Color colour;

    void (*leftClick)(void* self, void* game);
    void (*rightClick)(void* self);
}Button;

struct Game{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    SDL_Texture *nodesprite;
    SDL_Rect noderects [400];
    Button buttons[500];
    int count;
    int textureheight;
    int texturewidth;
};


bool load(struct Game *game);
bool init(struct Game *game);
void delete(struct Game *game, int exitstatus);
void addnode(struct Node *node, struct Game *game);
void leftClickFunc(void *buttonPointer, void *gamePointer);
void rightClickFunc(void *self);

int main(void){
    bool clickflag = false;
    struct Game game = {0};
    struct Node node ={
        .xcoordinate = 0,
        .ycoordinate = 0,
        .node = NULL
    };


    Button button;
    button.area = (SDL_Rect){0};
    button.leftClick = leftClickFunc;
    button.rightClick = rightClickFunc;

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
                            delete(&game, EXIT_SUCCESS);
                            break;
                        default:
                            break;
                    }
                case SDL_MOUSEBUTTONDOWN:
                    SDL_Point mousePos = {event.button.x, event.button.y};
                    bool hitButton = false;

                    for (int i = 0; i < game.count; i++) {
                    if (SDL_PointInRect(&mousePos, &game.buttons[i].area)) {
                            hitButton = true;
                    if (event.button.button == SDL_BUTTON_LEFT) {
                    game.buttons[i].leftClick(&game.buttons[i], &game);
                }
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                    game.buttons[i].rightClick(&game.buttons[i]);
                }
            break;
        }
    }
            if (!hitButton && event.button.button == SDL_BUTTON_LEFT) {
                node.xcoordinate = event.button.x;
                node.ycoordinate = event.button.y;
                addnode(&node, &game);
            }
            break;
                default:
                    break;
            
            }

        }
        SDL_RenderClear(game.renderer);

        SDL_RenderCopy(game.renderer, game.background, NULL, NULL);
        for (int i = 0; i < game.count; i++) {
            SDL_RenderCopy(game.renderer, game.nodesprite, NULL, &game.noderects[i]);
        }
        SDL_RenderPresent(game.renderer);

        SDL_Delay(sixtyfps);
        clickflag = false;
    }

    delete(&game, EXIT_SUCCESS);
    printf("test");
    return 0;
}

void delete(struct Game *game, int exitstatus){
    if(game->background) SDL_DestroyTexture(game->background);
    if(game->nodesprite) SDL_DestroyTexture(game->nodesprite);

    SDL_DestroyRenderer(game -> renderer);
    SDL_DestroyWindow(game -> window);
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
    game -> nodesprite = IMG_LoadTexture(game ->renderer, "images/node.png");
    if(SDL_QueryTexture(game -> nodesprite, NULL, NULL, &game -> texturewidth, &game->textureheight)){
        fprintf(stderr, "error creating node's texture: %s \n", SDL_GetError());
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

void addnode(struct Node *node,struct Game *game){
    SDL_Rect noderect;
    noderect.h = game->textureheight;
    noderect.w = game->texturewidth;
    noderect.x = (int)round((float)node->xcoordinate / 64.0f) *64 -16;
    noderect.y = (int)round((float)node ->ycoordinate / 64.0f) *64 -16;

    game->noderects[game->count] = noderect;
    game->buttons[game->count].area = noderect; 
    game->buttons[game->count].leftClick = leftClickFunc;
    game->buttons[game->count].rightClick = rightClickFunc;

    game->noderects[game->count] = noderect; 
    game->count ++;   
}

void leftClickFunc(void *buttonPointer, void *gamePointer){
    printf("leftclick\n");
    Button *clickedButton = (Button *)buttonPointer;
    struct Game *game = (struct Game *)gamePointer;
    int index = -1;
    for (int i = 0; i < game->count; i++) {
        if (&game->buttons[i] == clickedButton) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        game->noderects[index] = game->noderects[game->count - 1];
        game->buttons[index] = game->buttons[game->count - 1];
        game->count--;
    }
}
void rightClickFunc(void *self){
    printf("rightclick\n");
    int x = ((Button *)self)->area.x;
    int y = ((Button *)self)->area.y;
}

