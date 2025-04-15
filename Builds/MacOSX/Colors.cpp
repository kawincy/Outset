//
//  Colors.cpp
//  Outset
//
//  Created by Race Williams on 4/8/25.
//

#include "Colors.h"

Colors &colors() {
    static Colors instance;
    return instance;
}
