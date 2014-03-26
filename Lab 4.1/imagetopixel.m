pixeldata=imread('bean_image.jpg'); %Enter the name of the image. The size of the array can be seen in the workspace
pixeldata=permute(pixeldata,[3,1,2]);
pixeldata=uint32(pixeldata);
dlmwrite('bean_data.txt',pixeldata, '\t');%Enter the name of the file where the pixel data is to be written