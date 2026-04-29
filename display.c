#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define title "rubber band art"
#define height 640
#define width 800
#define sixtyfps 16

struct Node{  
    Sint32 xCoordinates;
    Sint32 yCoordinatesS;
    SDL_Texture *node;
};

typedef struct{
    Sint32 xStartingNode;
    Sint32 yStartingNode;
    Sint32 xEndingNode;
    Sint32 yEndingNode;
    SDL_Rect bands [100];
    char label [100];
    int angle;
    int length;
}RubberBand;

typedef struct{
    SDL_Rect area;
    SDL_Color colour;

    void (*leftClick)(void* self, void* game);
    void (*rightClick)(void* self, void* game);
}Button;

struct Game{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    SDL_Texture *nodesprite;
    SDL_Texture *rubberBandSprite;
    SDL_Texture *taskBar;
    SDL_Texture *addButton;
    SDL_Rect nodeRects [400];
    RubberBand bands[500];
    Button buttons[500];
    bool creatingBand;
    int bandCount;
    int nodeCount;
    int xTempStartNode;
    int yTempStartNode;
    int nodeTextureHeight;
    int nodeTextureWidth;
    int rubberBandTextureHeight;
    int rubberBandTextureWidth;
};


bool load(struct Game *game);
bool init(struct Game *game);
void delete(struct Game *game, int exitstatus);
void addnode(struct Node *node, struct Game *game);
void leftClickFunc(void *buttonPointer, void *gamePointer);
void rightClickFunc(void *buttonPointer, void *gamePointer);
void addLabelFunc();

int main(void){
    bool clickflag = false;
    struct Game game = {0};
    struct Node node ={
        .xCoordinates = 0,
        .yCoordinatesS = 0,
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
            bool hitButton = false;
            SDL_Point mousePos = {event.button.x, event.button.y};
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

                    for (int i = 0; i < game.nodeCount; i++) {
                        if (SDL_PointInRect(&mousePos, &game.buttons[i].area)) {
                            hitButton = true;
                            if (i == 0 && event.button.button == SDL_BUTTON_LEFT ){
                                game.buttons[i].leftClick(&game.buttons[i], &game);
                            }
                            else if (event.button.button == SDL_BUTTON_LEFT) {
                                game.buttons[i].leftClick(&game.buttons[i], &game);
                            }
                            else if (event.button.button == SDL_BUTTON_RIGHT) {
                                game.buttons[i].rightClick(&game.buttons[i], &game);
                            }
                        }   
                    }
                    if (!hitButton && event.button.button == SDL_BUTTON_LEFT && mousePos.x >= 160){
                        node.xCoordinates = event.button.x;
                        node.yCoordinatesS = event.button.y;
                        addnode(&node, &game);
                    }
                    break;

                case SDL_MOUSEBUTTONUP:

                    for(int i = 0; i<game.nodeCount; i++){
                        if (SDL_PointInRect(&mousePos, &game.buttons[i].area)) {
                            hitButton = true;
                        if(event.button.button == SDL_BUTTON_RIGHT && game.creatingBand){
                            game.bands[game.bandCount].xStartingNode = game.xTempStartNode;
                            game.bands[game.bandCount].yStartingNode = game.yTempStartNode;
                            game.bands[game.bandCount].xEndingNode = game.buttons[i].area.x;
                            game.bands[game.bandCount].yEndingNode = game.buttons[i].area.y;

                            int xDelta = (game.bands[game.bandCount].xEndingNode) - (game.bands[game.bandCount].xStartingNode);
                            int yDelta = (game.bands[game.bandCount].yEndingNode) - (game.bands[game.bandCount].yStartingNode);
                            double rads = atan2(yDelta, xDelta);
                            double degrees = rads * (180/3.14);
                            double length = sqrt(pow(xDelta, 2) + pow(yDelta, 2));

                            game.bands[game.bandCount].length = length;
                            game.bands[game.bandCount].angle = degrees;
                            printf("degrees: %lf\nlength: %lf\n", degrees, length);

                            game.bandCount ++;
                            game.creatingBand = false;
                            break;
                        }
                        break;
                    }
                }
                    break;
                default:
                    break;
            
            }

        }
        SDL_RenderClear(game.renderer);
        SDL_Rect backGroundRect = {160,0,640,640};
        SDL_Rect taskBar = {0,0,160,640};
        SDL_Rect addButton = {0,0,160,32};
        SDL_RenderCopy(game.renderer, game.background, NULL, &backGroundRect);
        SDL_RenderCopy(game.renderer,game.taskBar, NULL,&taskBar);
        SDL_RenderCopy(game.renderer,game.addButton, NULL,&addButton);
        for(int i = 0; i < game.bandCount; i++){
            SDL_Rect destinationRect;
            destinationRect.x = game.bands[i].xStartingNode + 16;
            destinationRect.y = game.bands[i].yStartingNode + 16;
            destinationRect.w = game.bands[i].length;             
            destinationRect.h = 4;

            SDL_Point centre = {0,2};
            SDL_RenderCopyEx(
                game.renderer, 
                game.rubberBandSprite, 
                NULL,           
                &destinationRect,      
                game.bands[i].angle, 
                &centre,        
                SDL_FLIP_NONE   
        );
        }
        for (int i = 0; i < game.nodeCount; i++) {
            SDL_RenderCopy(game.renderer, game.nodesprite, NULL, &game.nodeRects[i]);
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
    if(game->rubberBandSprite) SDL_DestroyTexture(game->rubberBandSprite);
    if(game->taskBar) SDL_DestroyTexture(game->taskBar);
    if(game->addButton) SDL_DestroyTexture(game->addButton);

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
    game -> nodesprite = IMG_LoadTexture(game ->renderer, "images/nodes.png");
    if(SDL_QueryTexture(game -> nodesprite, NULL, NULL, &game -> nodeTextureWidth, &game->nodeTextureHeight)){
        fprintf(stderr, "error creating node's texture: %s \n", SDL_GetError());
        return true;
    }

    game -> rubberBandSprite = IMG_LoadTexture(game ->renderer, "images/line.png");
    if(SDL_QueryTexture(game -> rubberBandSprite, NULL, NULL, &game -> rubberBandTextureWidth, &game->rubberBandTextureHeight)){
        fprintf(stderr, "error creating rubberband texture: %s \n", SDL_GetError());
        return true;
    }
    game -> taskBar = IMG_LoadTexture(game ->renderer, "images/taskbar.png");
    if (!game->taskBar) {
        fprintf(stderr, "Failed to load taskbar: %s\n", IMG_GetError());
        return true;
    }
    game -> addButton = IMG_LoadTexture(game ->renderer, "images/addbutton.png");
    if (!game->addButton) {
        fprintf(stderr, "Failed to load add button: %s\n", IMG_GetError());
        return true;
    }

    game->buttons[0].area = (SDL_Rect){0,0,160,32};
    game->buttons[0].leftClick = addLabelFunc;
    game->buttons[0].rightClick = addLabelFunc;
    game->nodeCount ++;

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
    SDL_Rect nodeRect;
    nodeRect.h = game->nodeTextureHeight;
    nodeRect.w = game->nodeTextureWidth;
    nodeRect.x = (int)round((float)node->xCoordinates / 64.0f) *64 - 48;
    nodeRect.y = (int)round((float)node ->yCoordinatesS / 64.0f) *64 -16;

    game->nodeRects[game->nodeCount] = nodeRect;
    game->buttons[game->nodeCount].area = nodeRect; 
    game->buttons[game->nodeCount].leftClick = leftClickFunc;
    game->buttons[game->nodeCount].rightClick = rightClickFunc;

    game->nodeRects[game->nodeCount] = nodeRect; 
    game->nodeCount ++;   
}

void leftClickFunc(void *buttonPointer, void *gamePointer){
    printf("leftclick\n");
    Button *clickedButton = (Button *)buttonPointer;
    struct Game *game = (struct Game *)gamePointer;
    int index = -1;
    for (int i = 0; i < game->nodeCount; i++) {
        if (&game->buttons[i] == clickedButton) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        game->nodeRects[index] = game->nodeRects[game->nodeCount - 1];
        game->buttons[index] = game->buttons[game->nodeCount - 1];
        game->nodeCount--;
    }
}
void rightClickFunc(void *self, void *gamePointer){
    printf("rightclick\n");
    struct Game *game = (struct Game *)gamePointer;
    game->creatingBand = true;

    int x = ((Button *)self)-> area.x;
    int y = ((Button *)self)-> area.y;

    game->xTempStartNode = x;
    game -> yTempStartNode = y;
}

void addLabelFunc(){
    printf("testing\n");
}