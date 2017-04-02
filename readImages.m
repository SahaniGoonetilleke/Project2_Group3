
cd Synthetic
imageFiles = dir('*.jpg');
nFiles = length(imageFiles);
numFeat = zeros(1, nFiles);
numMatches = zeros(1, size(numFeat, 2));

for i=1:nFiles
    currentFileName = imageFiles(i).name;
    currentImage = imread(currentFileName);
    images{i} = currentImage;
    Ix = single(rgb2gray(currentImage));
    [f, ~] = vl_sift(Ix);
    numFeat(i) = size(f, 2);
end

cd ..

[~, maxSIFTFeatInd] = max(numFeat);
initImage = single(rgb2gray(images{maxSIFTFeatInd}));
[f, d] = vl_sift(initImage);

for i = 1:nFiles
    if i ~= maxSIFTFeatInd
        I = single(rgb2gray(images{i}));
        [fi, di] = vl_sift(I);
        [matches, scores] = vl_ubcmatch(d, di);
        numMatches(i) = size(matches, 2);
    else 
        numMatches(i) = 0;
    end
end

[~, maxMatchedInd] = max(numMatches);
mosaic = mosaicFunc(images{maxSIFTFeatInd}, images{maxMatchedInd});

images([maxMatchedInd maxSIFTFeatInd]) = [];
numFeat([maxMatchedInd maxSIFTFeatInd]) = [];
numMatches([maxMatchedInd maxSIFTFeatInd]) = [];

for j = 1:(nFiles - 2)
    temp = single(rgb2gray(mosaic));
    [f, d] = vl_sift(temp);
    for k = 1:size(images, 1)
        J = single(rgb2gray(images{k}));
        [fk, dk] = vl_sift(J);
        [matches, scores] = vl_ubcmatch(d, dk);
        numMatches(k) = size(matches, 2);
    end
    
    [~, maxMatchedInd] = max(numMatches);
 
    mosaic = mosaicFunc(mosaic, images{maxMatchedInd});
    images{maxMatchedInd} = [];
    images(cellfun(@(images) isempty(images),images))=[];
    numMatches(maxMatchedInd) = [];
end



