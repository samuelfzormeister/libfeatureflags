//
//  main.cpp
//  featurectl
//
//  Created by Samuel Zormeister on 1/1/26.
//  Copyright © 2026 PureDarwin. All rights reserved.
//

#include <string>

extern "C" void FFDumpFlags(void);

/*
 * featurectl enable org.puredarwin.xyz/ExampleFeature
 * featurectl disable org.puredarwin.xyz/ExampleFeature
 * featurectl show
 * featurectl version
 */
bool parse_args(int argc, const char *argv[])
{
    for (int i = 0; i < argc; i++) {
        std::string str = argv[i];
    }
}

int main(int argc, const char *argv[])
{
    
    
    return 0;
}
