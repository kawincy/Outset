/*
  ==============================================================================

    Colors.cpp
    Created: 8 Apr 2025 1:27:11pm
    Author:  Race Williams

  ==============================================================================
*/

#include "Colors.h"

Colors &colors() {
    static Colors instance;
    return instance;
}
