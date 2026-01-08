// #include "../CommonInterfaces/CommonExampleInterface.h"
// #include "../CommonInterfaces/CommonGUIHelperInterface.h"
// #include "Bridge.h" // Your bridge header
// #include "../StandaloneMain/main_opengl_single_example.h" 

// int main(int argc, char* argv[])
// {
//     // This helper function starts the GL window and runs your BridgeExample
//     return example_main(argc, argv, ET_BridgeCreateFunc);
// }

#include "Bridge.h"
#include "main_opengl_single_example.h" 

int main(int argc, char* argv[]) {
    return example_main(argc, argv, ET_BridgeCreateFunc);
}