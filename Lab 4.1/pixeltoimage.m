pixelarray=dlmread('', '\t'); %Enter the name of the output file
pixelarray=reshape(uint8(pixelarray), []); %Enter the pixel data dimensions in [] in the following order. For 640x480 image with RGB values,reshape(uint8(pixelarray), [480,640,3])
%^For 640x480 image with one grayscale value for each pixel,reshape(uint8(pixelarray), [480,640,1])
imshow(pixelarray);