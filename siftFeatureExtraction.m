%%The following script computes the SIFT features of the input images and 
%%display them on the images. SIFT features are calculated using vlfeat. A
%%predefined algorithm is available for matching which matches SIFT
%%features among the images.

%%
%--------------------------SIFT Feature Extraction-------------------------

% SIFT features of the first image
Ia = imread('Images/panorama_image1.jpg');
Iax = single(rgb2gray(Ia));
%Iax = single(rgb2gray(Ia));
[fa, da] = vl_sift(Iax);

% SIFT features of the second image
Ib = imread('Images/panorama_image2.jpg');
Ibx = single(rgb2gray(Ib));
%Ibx = single(rgb2gray(Ib));
[fb, db] = vl_sift(Ibx);

%---------------------------SIFT Feature Matching--------------------------

% Matching SIFT features
[matches, scores] = vl_ubcmatch(da, db) ; %Matches contain 2 columns which 
                                          %store index of original and 
                                          %estimated match 

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
% image(Ia);
% h1 = vl_plotframe(fa(:,matches(1,:)));
% h2 = vl_plotframe(fa(:,matches(1,:)));
% set(h1,'color','k','linewidth',3);
% set(h2,'color','y','linewidth',2);
% 
% figure
% image(Ib);
% h3 = vl_plotframe(fb(:,matches(2,:)));
% h4 = vl_plotframe(fb(:,matches(2,:)));
% set(h3,'color','k','linewidth',3);
% set(h4,'color','y','linewidth',2);

%---------------------------Calculate Homography---------------------------

numMatches = size(matches, 2);

%Converting the co-ordinates to homographic co-ordinates by adding 1 as the
%third element
X1 = fa(1:2,matches(1,:));   
X1(3,:) = 1;
X2 = fb(1:2,matches(2,:)); 
X2(3,:) = 1;

clear H score ok;

%Run 100 iterations to compute homography

for t = 1:100
    %Perform Direct Linear Transformation based on 4 random points
    randPoints = vl_colsubset(1:numMatches, 4); 
    A = [];
    for i = randPoints
    A = cat(1, A, kron(X1(:,i)', vl_hat(X2(:,i))));
    end
    
    %Singular Value Decomposition such that A = UDV'
    [U,D,V] = svd(A);
    
    %Convert vector of homographic constants to form tansformation matrix
    H{t} = reshape(V(:,9),3,3);
    
    %Fit the model 
    X2Calc = H{t} * X1;
    
    %Calculate absolute co-ordinates from homographic co-ordinates and
    %compute the error
    uError = X2Calc(1,:)./X2Calc(3,:) - X2(1,:)./X2(3,:);
    vError = X2Calc(2,:)./X2Calc(3,:) - X2(2,:)./X2(3,:);
    
    ok{t} = (uError.*uError + vError.*vError) < 6*6;
    score(t) = sum(ok{t});
end

[score, best] = max(score);
H = H{best};
ok = ok{best};





