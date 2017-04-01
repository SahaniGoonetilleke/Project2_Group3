%geting the initial image
im0 = imread('hotel.seq0.png');

%define tau value for corner tracking in harris coner detection
tau=142/255;

startCX=[];
startCY=[];
endCX=[];
endCY=[];
ra=2;
numOfImage=10;
%calling the getKeyPoints(im0,tau) function to get the initial key values
%for the feature tracking 
[startXs, startYs]=getKeyPoints(im0,tau);

numOfKeyPoints=size(startXs,1);

%randomly selecting the indices for the feature points
r=randi([1 numOfKeyPoints],1,20);

%taking the randomly selected feature points
tempX=startXs(r);
tempY=startYs(r);
startCX=tempX;
startCY=tempY;

tx=tempX(1);
ty=tempY(1);
%%display the detected features on top of the initial. it is drawn a circle
%%for each and every selected key feauture
figure;
title('selected features ');
imshow(im0);
hold on;
for i=1:size(tempX,1)
    t=0:0.1:2*pi;
    x = tempX(i) + ra*sin(t);
    y = tempY(i)+ ra*cos(t);
    plot(x,y,'r');
   
end
hold off;
%%end of displaying the feautre points

%%%
%%%input the slightly vaiated iamge and tracking the features. here it has
%%%been used the predictTranslationAll(tempX, tempY, im0, im1) function to
%%%track the features. 
%%%function returns two column vectors with 20 elements which are tracked X
%%%and Y cordintes of slightly variated image. In each and every changing
%%%of the image function taked the previously tracked feature position to
%%%detect the next feature points of the image.   
%%% for-loop 1. Go to the specified end image bypassing intermediate images
%%% however this method is not good as this deviates from some basic
%%% assumptions of the algo.
% % % % % % for i=1:numOfImage
% % % % % %     if(i==numOfImage)
% % % % % %         x1=strcat('E:\Sem7\Machine vision\project 2\hw2_solution\prob_tracking\images\hotel.seq',int2str(i),'.png');
% % % % % %         im1=imread(x1);
% % % % % %         [newX,newY]=predictTranslationAll(tempX, tempY, im0, im1);
% % % % % %         tempX=newX;
% % % % % %         tempY=newY;
% % % % % %         endCX=[endCX;newX];
% % % % % %         endCY=[endCY;newY];
% % % % % %         startCX=[startCX;newX];
% % % % % %         startCY=[startCY;newY];
% % % % % %    end
% % % % % % end

%%%Go to the end image by step by step with intermediate images
for i=1:numOfImage
    x1=strcat('hotel.seq',int2str(i),'.png');
    im1=imread(x1);
    [newX,newY]=predictTranslationAll(tempX, tempY, im0, im1);
    tempX=newX;
    tempY=newY;
    endCX=[endCX;newX];
    endCY=[endCY;newY];
    startCX=[startCX;newX];
    startCY=[startCY;newY];
end
%%%end of getting the feaure points
%%%

%%%%
%%%%displaying the feature tracking path. There are 2 feature track
%%%%displaying methods.
%%%%1. straight line with the start and end points: with the first method
%%%%   of feature tracking
%%%%2. draw the dots indicating the tracking path: with the second method
%%%%   of feaure tracking
figure;
title('transformation');
imshow(im0);
hold on;

%%%%plotting the path with dots. Here it smooth the obtained tracking
%%%%points in order to a smoothe path.
startCX2=reshape(startCX,[20 numOfImage+1])';
startCY2=reshape(startCY,[20 numOfImage+1])';

startCX2(numOfImage+1,:)=smooth(startCX2(11,:),'loess');
startCY2(numOfImage+1,:)=smooth(startCY2(11,:),'loess');

startCX2=startCX2';
startCY2=startCY2';

startCX2=reshape(startCX2,[(numOfImage+1)*20 1]);
startCY2=reshape(startCY2,[(numOfImage+1)*20 1]);

plot(startCX2, startCY2, 'r+', 'MarkerSize',4, 'LineWidth', 1);

%%%%plotting the startting and end points
% % % % for i=1:11
% % % %     for j=1:20
% % % %         line([startCX2((i-1)*20+j) endCX((i-1)*20+j)],[startCY2((i-1)*20+j) endCY((i-1)*20+j)],'LineWidth',1);
% % % %     end
% % % % end
hold off;




%%%%implement the predictTransalation function

%%%%randomly generate the Ix and Iy values to obtain the new position of
%%%%the feautre
newIx = -150+ (150+150)*rand(225,1);
newIy = -150+ (150+1)*rand(225,1);

im1=imread('hotel.seq20.png');
[newXpoint,newYpoint]=predictTranslation(tx, ty, newIx, newIy, im0, im1);

figure;
title('transformation');
imshow(im0);
hold on;
plot(tempX(1), tempY(1), 'r+', 'MarkerSize',10, 'LineWidth', 1);
plot(newXpoint, newYpoint, 'yx', 'MarkerSize',10, 'LineWidth', 1);
hold off
