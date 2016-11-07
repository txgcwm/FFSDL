#include <iostream>
#include <exception>
#include <string>
#include "SDL.h"

class InitError: public std::exception {
    public:
        InitError();
        InitError(const std::string&);
        virtual ~InitError() throw();
        virtual const char* what() const throw();
    private:
        std::string msg;
};

class SDL {
    public:
        SDL(Uint32 flags = 0) throw(InitError);
        bool createWindow();
        bool showWindow();
        static void sdlDelay(unsigned int delay);
        void createTextrue();
        void setVideoWidth(int height);
        void setVideoHeight(int width);
        void setVideoPixFormat(Uint32 format);
        void initRect();
        void setBuffer(void* pixels, int pitch);
        void showFrame(int mSec);
        virtual ~SDL();
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        SDL_Rect rect;
        int vWidth;
        int vHeight;
        void* vPixels;
        int vPitch;
        Uint32 vFormat;
};
