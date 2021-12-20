// Instantiate the stb_image header into a cpp file so it can be compiled. This
// should be placed in a separate file to avoid unnecessary recompilation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"