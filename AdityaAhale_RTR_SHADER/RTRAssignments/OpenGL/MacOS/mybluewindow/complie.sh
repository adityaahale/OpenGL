mkdir -p bluewindow.app/Contents/MacOS

clang++ -o bluewindow.app/Contents/MacOS/bluewindow.mm -framework Cocoa -framework QuartzCore -framework OpenGL
