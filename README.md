# Enlighted
Automatically fix high dynamic range images

This code shows how to automatically brighten the darker areas of high dynamic range images.

To compile, just type 

% make

To test it 

% make test

This is used in my photo painting app for iOS called "PhotoViva"

This uses https://github.com/nothings/stb to read, write and resize images.

The image blurring is implemented by resizing an image to a small size and then resizing it big again.

