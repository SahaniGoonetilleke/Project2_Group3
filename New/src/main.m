function main
  

    % read images
    dataDir = '../data2/testingImages/';
    files = dir(strcat(dataDir, '*.jpg'));

    disp('Start reading images');
    images = readImages(files, dataDir);
    disp('Finish reading images');

    
    f = size(images, 2) * 8.2 / 7.11; 
    % radial distortion constants
    k1 = -0.18;
    k2 = 0.21;
   
   
    projected = cylindrical(images, f, k1, k2);
   
    

    
    cropped = cropAll(projected);
    
    

    
    [stitched, yshift_total] = stitch(cropped);
   

    
    imwrite(stitched, '../result/stitched_raw.jpg')

   
    corrected = correctDrift(stitched, yshift_total);
   

    imshow(corrected);
    imwrite(corrected, '../result/final.jpg')
end