%%The following script computes the SIFT features of the input images and 
%%display them on the images. SIFT features are calculated using vlfeat. A
%%predefined algorithm is available for matching which matches SIFT
%%features among the images.

%%

% SIFT features of the first image
Ia = imread('Images/frame_18.png');
Iax = single(rgb2gray(Ia));
%Iax = single(rgb2gray(Ia));
[fa, da] = vl_sift(Iax);

% SIFT features of the second image
Ib = imread('Images/frame_20.png');
Ibx = single(rgb2gray(Ib));
%Ibx = single(rgb2gray(Ib));
[fb, db] = vl_sift(Ibx);

% Matching SIFT features
%[matches, scores] = vl_ubcmatch(da, db) ;

% Displaying SIFT features on the image
% image(Ia);
% h1 = vl_plotframe(fa) ;
% h2 = vl_plotframe(fa) ;
% set(h1,'color','k','linewidth',3) ;
% set(h2,'color','y','linewidth',2) ;
% 
% figure;
% 
% image(Ib);
% h3 = vl_plotframe(fb) ;
% h4 = vl_plotframe(fb) ;
% set(h3,'color','k','linewidth',3) ;
% set(h4,'color','y','linewidth',2) ;

%Extract the matches and display only the matches
image(Ia);
h1 = vl_plotframe(fa(:,matches(1,:))) ;
h2 = vl_plotframe(fa(:,matches(1,:))) ;
set(h1,'color','k','linewidth',3) ;
set(h2,'color','y','linewidth',2) ;

figure
image(Ib);
h3 = vl_plotframe(fb(:,matches(2,:))) ;
h4 = vl_plotframe(fb(:,matches(2,:))) ;
set(h3,'color','k','linewidth',3) ;
set(h4,'color','y','linewidth',2) ;

