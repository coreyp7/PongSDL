#include <SDL.h>
#include <SDL_image.h> // for loading pngs
#include <SDL_ttf.h> // for rendering text
#include <stdio.h>
#include <string> // TODO: look into how this shit works at all
#include <sstream>

// local object stuff
#include "Box.h" // api for box object stuff
#include "Text.h"

// Defining some global constants + other shit
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

// FPS Cap when rendering. 60 by default.
short int fpsCap = 60;

SDL_Window* mainWindow;
SDL_Renderer* renderer;

SDL_Texture* boxTexture;

TTF_Font* globalFont;

void close(){
    SDL_DestroyTexture( boxTexture );
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
    boxTexture = NULL;
    renderer = NULL;
    mainWindow = NULL;
}

// For loading all png/font stuff into their SDL_Texture fields.
bool loadMedia(){
    bool success = true;

    boxTexture = IMG_LoadTexture(renderer, "img/primitive_sandbox/box.png");
    if(boxTexture == NULL){
        printf("Couldn't load box texture. %s", IMG_GetError());
        success = false;
    }

    // load font into global font
    globalFont = TTF_OpenFont("img/primitive_sandbox/Lato-Black.ttf", 26);
    if(globalFont == NULL){
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        return -7;
    }

    return success;
}

/* 
    List of shit it does:
    - SDL_Init (no vsync)
    - create sdl window & renderer
    - initialize IMG/font stuff and load the files (boxTexture/globalFont/etc)
*/
int init(){
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return -1;
    }

    mainWindow = SDL_CreateWindow("Physics based off time", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );
    if(mainWindow == NULL){
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return -2;
    }

    // todo: create renderer
    renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return -3;
    }

    // todo: set renderer draw color to black/default
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // todo: initialize sdl_image
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init(imgFlags) & imgFlags ) ){ // TODO: look back at lazyfoo to see wtf this is
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return -4;
    }

    if( TTF_Init() == -1){
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
        return -5;
    }

    // load box texture
    if (!loadMedia()){
        return -6;
    }

    return 0;
}

int main( int argc, char* args[] ){

    int initValue = init();

    if(initValue != 0){
        printf("Problem when trying to initialize for main loop. error #%d", initValue);
        return initValue;
    }

    // Main loop starts
    bool quit = false; // Main loop flag

    SDL_Event e; // Event handler

    // FPS Stuff
    Uint32 frameStart = 0;
    // Keep track of the time of each frame.
    int frameTimeToComplete = 0;

    float avgFPS = 0.0;

    SDL_Color textColor = { 0, 0, 0, 255 }; // black

    // CHANGE: fps text stuff. Could put in an object or something.
    int countedFrames = 0; // TEMPORARY: experimenting with showing live frames with this
    std::stringstream countedFramesStr;

    // Text objects to show.
    Text avgFpsText = Text(renderer, globalFont, textColor);
    Text msText = Text(renderer, globalFont, textColor);
    Text fpsText = Text(renderer, globalFont, textColor);
    fpsText.changeText("FPS cap: " +std::to_string(fpsCap));

    // While game is running
    while(!quit){
        frameStart = SDL_GetTicks(); // mark time(in m/s) at start of this frame

        // Go through event queue
        while( SDL_PollEvent( &e ) != 0 ){
            
            // User requests quit
            if (e.type == SDL_QUIT){
                quit = true;
            }
        }

        //Calculate avg fps
        avgFPS = countedFrames / ( SDL_GetTicks() / 1000.f );
        if( avgFPS > 2000000 )
        {
            avgFPS = 0;
        }

        // Update avg fps
        countedFramesStr.str("");
        countedFramesStr << "Avg FPS: " << avgFPS;
        avgFpsText.changeText(countedFramesStr.str().c_str());

        // Rendering starts here.
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear(renderer);

        // show avg fps text & ms text
        avgFpsText.render(0, 0);
        msText.changeText("ms render frame: "+std::to_string(SDL_GetTicks() - frameStart));
        msText.render(0, avgFpsText.getHeight());
        fpsText.render(0, avgFpsText.getHeight() + msText.getHeight());

        SDL_RenderPresent(renderer);
        countedFrames++; // NOTE: not sure if this should be lower in loop

        // Finished rendering, cap framerate.
        // If frame is finished early, wait remaining time.
        frameTimeToComplete = SDL_GetTicks() - frameStart;
        if(1000 / fpsCap > frameTimeToComplete){
            SDL_Delay((1000/fpsCap) - frameTimeToComplete);
        }

    }

    //Free resources and close SDL
    close();

    return 0;

}