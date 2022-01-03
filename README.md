# Enlighten
Automatically brighten dark areas in high dynamic range images

This code shows how to automatically brighten the darker areas of high dynamic range images.

To compile (only tested on macOS) just type 

% make

To test it 

% make test

This image processing method is related to Homomorphic Image Processing but avoids the use of log space processing.

&nbsp;&nbsp;&nbsp;&nbsp;[Homomorphic Filtering - wikipedia](https://en.wikipedia.org/wiki/Homomorphic_filtering#Image_enhancement)

&nbsp;&nbsp;&nbsp;&nbsp;[Homomorphic Filtering - faadooengineers](http://www.faadooengineers.com/online-study/post/ece/digital-image-processing/1123/homomorphic-filtering)

This method is used in my photo painting app for iOS called "[PhotoViva](https://apps.apple.com/us/app/photoviva-paintings-from-your-photos/id459007515)"

The image blurring is implemented by resizing an image to a small size and then resizing it big again.

Enlighten sees images like your eyes see the world.

Creditis: Uses https://github.com/nothings/stb to read, write and resize images.

