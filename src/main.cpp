#include "engine.h"
#include <iostream>


//SDL main
int main(int argc, char* argv[]) {
    
    Engine engine;

    if (engine.StartUp())
    {
        engine.Run();
    }
    else
    {
        std::cout << "Engine could not be initialised.\n";
    }

    engine.ShutDown();


    return 0;
}

