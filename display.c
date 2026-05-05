#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define title "rubber band art"
#define height 640
#define width 800
#define sixtyfps 16
#define MAXLABELS 17
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

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
    SDL_Color colour;
    SDL_Rect bands [100];
    char label [100];
    double angle;
    int length;
}RubberBand;

typedef struct{
    SDL_Texture *pallete;
    int r;
    int g;
    int b;
}Label;

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
    SDL_Texture *label;
    SDL_Texture *up;
    SDL_Texture *down;
    SDL_Texture *openingCap;
    SDL_Texture *closingCap;
    SDL_Color activeColour;
    SDL_Rect nodeRects [400];
    RubberBand bands[500];
    Button buttons[500];
    Button labelButton[100];
    Label labels[100];
    bool creatingBand;
    int offset;
    int bandCount;
    int labelCount;
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
void addLabelFunc(void *self, void *gamePointer);
void colour(int rgb[3] ,struct Game *game);
void pickColour(void *self,void *gamePointer);
void upPage(void *self,void *gamePointer);
void downPage(void *self,void *gamePointer);

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
        printf("init error\n");
        delete(&game, EXIT_FAILURE);
    }
    if(load(&game)){
        printf("load error\n");
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
                            else if(i == 1 && event.button.button == SDL_BUTTON_LEFT){
                                game.buttons[i].leftClick(&game.buttons[i], &game);
                            }
                            else if(i == 2 && event.button.button == SDL_BUTTON_LEFT){
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
                    for (int i=game.offset; (i<game.offset+MAXLABELS) && (i<game.labelCount); i++){
                        SDL_Rect currRect = {0,((i-game.offset)*32 +32),160,32};
                        if(SDL_PointInRect(&mousePos, &currRect)){
                            hitButton = true;
                            pickColour(&game.labels[i],&game);
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
                            game.bands[game.bandCount].colour = game.activeColour; 

                            int xDelta = (game.bands[game.bandCount].xEndingNode) - (game.bands[game.bandCount].xStartingNode);
                            int yDelta = (game.bands[game.bandCount].yEndingNode) - (game.bands[game.bandCount].yStartingNode);
                            double rads = atan2(yDelta, xDelta);
                            double degrees = rads * (180.0 / M_PI);
                            double length = sqrt(pow(xDelta, 2) + pow(yDelta, 2));

                            game.bands[game.bandCount].length = length;
                            game.bands[game.bandCount].angle = degrees;

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
        SDL_Rect up = {0, 640 - 64, 160, 32};
        SDL_Rect down = {0, 640 - 32, 160, 32};
        SDL_RenderCopy(game.renderer, game.background, NULL, &backGroundRect);
        SDL_RenderCopy(game.renderer,game.taskBar, NULL,&taskBar);
        SDL_RenderCopy(game.renderer,game.addButton, NULL,&addButton);
        SDL_RenderCopy(game.renderer,game.up, NULL,&up);
        SDL_RenderCopy(game.renderer,game.down, NULL,&down);
        for(int i = 0; i < game.bandCount; i++){
            double rad = game.bands[i].angle*(M_PI/180);
            int y=game.bands[i].yStartingNode+32;
            int x=game.bands[i].xStartingNode+32;

            SDL_Rect capRect;
            capRect.x=game.bands[i].xStartingNode;
            capRect.y=game.bands[i].yStartingNode;
            capRect.w=64;
            capRect.h=64;
            SDL_Rect closeRect;
            closeRect.x=game.bands[i].xEndingNode;
            closeRect.y=game.bands[i].yEndingNode;
            closeRect.w=64;
            closeRect.h=64;
            SDL_Rect rightLine;
            rightLine.x=x+(int)(32*(cos(rad+(M_PI/2))));
            rightLine.y=y+(int)(32*(sin(rad+(M_PI/2))));
            rightLine.w=game.bands[i].length;
            rightLine.h=4;
            SDL_Rect leftLine;
            leftLine.x=x+(int)(32*(cos(rad-(M_PI/2))));
            leftLine.y=y+(int)(32*(sin(rad-(M_PI/2))));
            leftLine.w=game.bands[i].length;
            leftLine.h=4;
            SDL_SetTextureColorMod(game.openingCap,game.bands[i].colour.r,game.bands[i].colour.g,game.bands[i].colour.b);
            SDL_SetTextureColorMod(game.closingCap,game.bands[i].colour.r,game.bands[i].colour.g,game.bands[i].colour.b);
            SDL_SetTextureColorMod(game.rubberBandSprite,game.bands[i].colour.r,game.bands[i].colour.g, game.bands[i].colour.b);
            SDL_Point fulcrum={24,24};
            SDL_Point pivot={0,2};
            SDL_RenderCopyEx(game.renderer, game.openingCap,NULL,&capRect,game.bands[i].angle,&fulcrum,SDL_FLIP_NONE);
            SDL_RenderCopyEx(game.renderer, game.closingCap,NULL,&closeRect,game.bands[i].angle,&fulcrum,SDL_FLIP_NONE);
            SDL_RenderCopyEx(game.renderer, game.rubberBandSprite,NULL,&rightLine,game.bands[i].angle,&pivot,SDL_FLIP_NONE);
            SDL_RenderCopyEx(game.renderer, game.rubberBandSprite,NULL,&leftLine,game.bands[i].angle,&pivot,SDL_FLIP_NONE);
        }
        for (int i= game.offset; (i<game.labelCount)&&((i-game.offset)<MAXLABELS); i++ ){
            int index = i-game.offset;
            SDL_Rect labelButton = {0,(32 + 32*index),160,32};
            SDL_Rect labelColour ={64, (32 + 32*index) + 2, 32, 28};
            SDL_SetTextureColorMod(game.labels[i].pallete,
                                    game.labels[i].r,
                                    game.labels[i].g,
                                    game.labels[i].b
                                    );
            SDL_RenderCopy(game.renderer, game.labels[i].pallete,NULL, &labelColour);
            SDL_RenderCopy(game.renderer,game.label,NULL, &labelButton);
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
    if(game->up) SDL_DestroyTexture(game->up);
    if(game->down) SDL_DestroyTexture(game->down);
    if(game->openingCap) SDL_DestroyTexture(game->openingCap);
    if(game->closingCap) SDL_DestroyTexture(game->closingCap);
    for(int i = 0; i < game->labelCount; i++){
        SDL_DestroyTexture(game->labels[i].pallete);
    }
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

    game -> rubberBandSprite = IMG_LoadTexture(game ->renderer, "images/line.bmp");
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
    game -> label = IMG_LoadTexture(game -> renderer, "images/label.png");
    if (!game->label){
        fprintf(stderr, "failed to load label button: %s\n", IMG_GetError());
        return true;
    }
    game -> up = IMG_LoadTexture(game -> renderer, "images/up.png");
    if (!game->up){
        fprintf(stderr, "failed to load up arrow button: %s\n", IMG_GetError());
        return true;
    }
    game -> down = IMG_LoadTexture(game -> renderer, "images/down.png");
    if (!game->down){
        fprintf(stderr, "failed to load down arrow button: %s\n", IMG_GetError());
        return true;
    }
    game -> openingCap = IMG_LoadTexture(game -> renderer, "images/openingcap.png");
    if (!game->openingCap){
        fprintf(stderr, "failed to load down arrow button: %s\n", IMG_GetError());
        return true;
    }
    game -> closingCap = IMG_LoadTexture(game -> renderer, "images/closingcap.png");
    if (!game->closingCap){
        fprintf(stderr, "failed to load down arrow button: %s\n", IMG_GetError());
        return true;
    }

    game->buttons[0].area = (SDL_Rect){0,0,160,32};
    game->buttons[0].leftClick = addLabelFunc;
    game->buttons[0].rightClick = addLabelFunc;
    

    game->buttons[1].area = (SDL_Rect){0, 640 - 64, 160, 32};
    game->buttons[1].leftClick = upPage;
    game->buttons[1].rightClick = upPage;

    game->buttons[2].area = (SDL_Rect) {0, 640 - 32, 160, 32};
    game->buttons[2].leftClick = downPage;
    game->buttons[2].rightClick = downPage;

    game->nodeCount = 3;
    srand((unsigned)time(NULL));

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
    Button *clickedButton = (Button *)buttonPointer;
    struct Game *game = (struct Game *)gamePointer;
    int xCoord = clickedButton->area.x;
    int yCoord = clickedButton->area.y;
    int index = -1;

    for (int i=0;i<game->bandCount;i++){
        if(((game->bands[i].xStartingNode == xCoord) && (game->bands[i].yStartingNode == yCoord)) || ((game->bands[i].xEndingNode == xCoord) && (game->bands[i].yEndingNode == yCoord))){
            game->bands[game->bandCount-1];
            game->bandCount--;
            i--;
        }
    }

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
    struct Game *game = (struct Game *)gamePointer;
    game->creatingBand = true;

    int x = ((Button *)self)-> area.x;
    int y = ((Button *)self)-> area.y;

    game->xTempStartNode = x;
    game->yTempStartNode = y;
}
void addLabelFunc(void *self, void *gamePointer){
    struct Game *game = (struct Game *)gamePointer;
    game->activeColour.r = rand() % 256;
    game->activeColour.g = rand() % 256;
    game->activeColour.b = rand() % 256;
    int rgb[3] = {game->activeColour.r,game->activeColour.g,game->activeColour.b}; 
    colour(rgb, game);
}
void colour(int rgb[3], struct Game *game){
    game->labels[game->labelCount].pallete = game->rubberBandSprite;
    game->labels[game->labelCount].r = rgb[0];
    game->labels[game->labelCount].g = rgb[1];
    game->labels[game->labelCount].b = rgb[2]; 

    Button label;
    label.area = (SDL_Rect){64, (32 + 32 * game->labelCount) + 2, 32, 28};
    label.leftClick = pickColour;
    label.rightClick = pickColour;
    game->labelButton[game->labelCount] = label;
    game->labelCount ++;
}
void pickColour(void *self,void *gamePointer){
    Label *clickedPallete = (Label *)self;
    struct Game *game = (struct Game *)gamePointer;
    game->activeColour.r = clickedPallete->r;
    game->activeColour.g = clickedPallete->g;
    game->activeColour.b = clickedPallete->b;
}
void upPage(void *self,void *gamePointer){
    struct Game *game = (struct Game *)gamePointer;
    if(game->offset > 0){
        game->offset--;
    }
}
void downPage(void *self,void *gamePointer){
    struct Game *game = (struct Game *)gamePointer;
    if((game->offset + MAXLABELS)<game->labelCount){
        game->offset++;
    }
}
