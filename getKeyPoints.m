% % % % function getKeypoints(imageInit,tau) is for getting the feature points of 
% % % % the image using the harris corner detection method. 
% % % % @input args: initial image, tau
% % % % @return val:2 matrices containing feture points

function [row, col]=getKeyPoints(imageInit,tau);

sigma=0.3;
alpha=0.04;

rad=5;

%convert the image to gray scale if it is RGB
if size(imageInit,3) > 1
    imageGray = rgb2gray(imageInit) ;
else imageGray = imageInit ; 
end

%generate the gaussian kernel for smoothing process    
gaussKernal=fspecial('gaussian',[5 5],sigma);


%definring the sobel kernel in X and Y directions
Gx = [-1 0 1; -2 0 2; -1 0 1];
Gy = Gx';

%%%taking the derivative of the each pixel of the image
Ix=imfilter(imageGray,Gx);
Iy=imfilter(imageGray,Gy);

%%%taking the product of the derivative of the image
Ixx=Ix.*Ix;
Iyy=Iy.*Iy;
Ixy=Ix.*Iy;

%%%filter all the dervative products using the gaussina fitler
IxxFiltered=imfilter(Ixx,gaussKernal);
IyyFiltered=imfilter(Iyy,gaussKernal);
IxyFiltered=imfilter(Ixy,gaussKernal);

%getting the second moment matrix of the harris cornet detection
har=(IxxFiltered.*IyyFiltered-IxyFiltered.^2)-alpha*((IxxFiltered+IyyFiltered).^2);


%non maximu supression inorder to get a clear point of feature
har = har/(max(har(:)));
n = ordfilt2(har, rad^2, ones([rad rad]));

%if the pixel value is greater than a given threshold value and if that
%point is exsist in the given matirx, that point is taken as corner point
corners = (har>tau).* double(n==har);
[row col]=find(corners);

  


