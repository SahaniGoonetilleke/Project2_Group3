function mosaic = mosaicFunc(Ia, Ib)
%--------------------------SIFT Feature Extraction-------------------------

% SIFT features of the first image
Iax = single(rgb2gray(Ia));
%Iax = single(rgb2gray(Ia));
[fa, da] = vl_sift(Iax);

% SIFT features of the second image
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


% --------------------------------------------------------------------
%                                                         Show matches
% --------------------------------------------------------------------
% 
% dh1 = max(size(Ib,1)-size(Ia,1),0) ;
% dh2 = max(size(Ia,1)-size(Ib,1),0) ;
% 
% figure(1) ; clf ;
% subplot(2,1,1) ;
% imagesc([padarray(Ia,dh1,'post') padarray(Ib,dh2,'post')]) ;
% o = size(Ia,2) ;
% line([fa(1,matches(1,:));fb(1,matches(2,:))+o], ...
%      [fa(2,matches(1,:));fb(2,matches(2,:))]) ;
% title(sprintf('%d tentative matches', numMatches)) ;
% axis image off ;
% 
% subplot(2,1,2) ;
% imagesc([padarray(Ia,dh1,'post') padarray(Ib,dh2,'post')]) ;
% o = size(Ia,2) ;
% line([fa(1,matches(1,ok));fb(1,matches(2,ok))+o], ...
%      [fa(2,matches(1,ok));fb(2,matches(2,ok))]) ;
% title(sprintf('%d (%.2f%%) inliner matches out of %d', ...
%               sum(ok), ...
%               100*sum(ok)/numMatches, ...
%               numMatches)) ;
% axis image off ;
% 
% drawnow ;

% --------------------------------------------------------------------
%                                                               Mosaic
% --------------------------------------------------------------------

box2 = [1  size(Ib,2) size(Ib,2)  1 ;
        1  1           size(Ib,1)  size(Ib,1) ;
        1  1           1            1 ] ;
box2_ = inv(H) * box2 ;
box2_(1,:) = box2_(1,:) ./ box2_(3,:) ;
box2_(2,:) = box2_(2,:) ./ box2_(3,:) ;
ur = min([1 box2_(1,:)]):max([size(Ia,2) box2_(1,:)]) ;
vr = min([1 box2_(2,:)]):max([size(Ia,1) box2_(2,:)]) ;

[u,v] = meshgrid(ur,vr) ;
Ia_ = vl_imwbackward(im2double(Ia),u,v) ;

z_ = H(3,1) * u + H(3,2) * v + H(3,3) ;
u_ = (H(1,1) * u + H(1,2) * v + H(1,3)) ./ z_ ;
v_ = (H(2,1) * u + H(2,2) * v + H(2,3)) ./ z_ ;
Ib_ = vl_imwbackward(im2double(Ib),u_,v_) ;

mass = ~isnan(Ia_) + ~isnan(Ib_) ;
Ia_(isnan(Ia_)) = 0 ;
Ib_(isnan(Ib_)) = 0 ;
mosaic = (Ia_ + Ib_) ./ mass ;

figure ; clf ;
imagesc(mosaic) ; axis image off ;
title('Mosaic') ;