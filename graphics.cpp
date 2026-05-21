#include <SDL3/SDL.h>
#include <cmath>

class Platform{
    public:
        Platform(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
            SDL_Init(SDL_INIT_VIDEO);
            window = SDL_CreateWindow(title, windowWidth, windowHeight, 0);
            if(!window){
                SDL_Log("Failed to create window: %s", SDL_GetError());
            }

            renderer = SDL_CreateRenderer(window, nullptr);
            if(!renderer){
                SDL_Log("Failed to create renderer: %s", SDL_GetError());
            }

            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
            if(!texture){
                SDL_Log("Failed to create texture: %s", SDL_GetError());
            }
        }

        ~Platform(){
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        void Update(void const* buffer, int pitch){
            SDL_UpdateTexture(texture, nullptr, buffer, pitch);
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        bool processInput(uint8_t& action, uint8_t& dir){
            bool quit = false;
            SDL_Event event;
            action = 0x0F;
            dir = 0x0F;

            const bool* keys = SDL_GetKeyboardState(nullptr);

            if(keys[SDL_SCANCODE_RIGHT]) dir &= ~0x01;
            if(keys[SDL_SCANCODE_LEFT]) dir &= ~0x02;
            if(keys[SDL_SCANCODE_UP]) dir &= ~0x04;
            if(keys[SDL_SCANCODE_DOWN]) dir &= ~0x08;

            if(keys[SDL_SCANCODE_Z]) action &= ~0x01;
            if(keys[SDL_SCANCODE_X]) action &= ~0x02;
            if(keys[SDL_SCANCODE_RSHIFT]) action &= ~0x04;
            if(keys[SDL_SCANCODE_RETURN]) action &= ~0x08;

            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_EVENT_QUIT:
                        quit = true;
                        break;

                    case SDL_EVENT_KEY_DOWN:
                        switch(event.key.scancode){
                            case SDL_SCANCODE_ESCAPE: quit = true; break;
                        }
                        break;



                }
            }
            return quit;
        }



    private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
};