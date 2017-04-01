% % % % function predictTranslationAll(startXs, startYs, im0, im1) is for getting the feature points of 
% % % % the all image using the Kanade-Lucas-Tomasi tracking procedure method. 
% % % % @input args: feature pointsX,feture pointsY, initial image,next image of the sequence
% % % % @return val:2 matrices containing feture points loactions


function [newX,newY]=predictTranslationAll(startXs, startYs, im0, im1);

%initilally assign matrix values of startXs and startYs to return values
newX=startXs;
newY=startYs;

midRange=7;
ra=2;

imX=size(im0,1);
imY=size(im0,2);

tempX=startXs;
tempY=startYs;



 %taking the gradients of the image
[Gx,Gy]=imgradientxy(im0); 


%%considering the exactly 20 feature of the image, it considers the
%%variation of the given particular feature. considering the next image
%%frame, it tracks the path of the image. 
for i=1:20
     %check the validation of the indices
    
    if(floor(tempX(i)-midRange)>0 && floor(tempY(i)-midRange)>0 && ceil(tempX(i)+midRange)<imX && ceil(tempY(i)+midRange)<imY)  
        %take the X and Y coordinate vecotrs around the key point selected 
        %considering the given range variable. All the vectors are conside
        %red using the mid range value. used to create a mesh grid 
        indXVal=floor(tempX(i)-midRange):ceil(tempX(i)+midRange);                                      
        indYVal=floor(tempY(i)-midRange):ceil(tempY(i)+midRange);
        
        %taking the mesh grid of the cordinate and the query points
        [qX,qY]=meshgrid(tempX(i)-floor(tempX(i))+ 1:1:(2*midRange+1),tempY(i)-floor(tempY(i))+ 1:1:(2*midRange+1));
        [cX,cY]=meshgrid(1:numel(indXVal),1:numel(indYVal)); 
        
        %rearranging the meshgrid of qX and qY inorder to make it 15x15
        %matrix. otherwise it gives an grid of less than 225 elements.
        %Therefore it needs to validata the grid by adding another row and
        %column with 15 elements
        if ~(size(qX,1)==15)
            a=qX(end,:);
            qX=[qX;a];
            qX=[qX,15*ones(15,1)];

            qY=[qY,(1:1:14)'];
            b=15*ones(1,15);
            qY=[qY;b];
        end

        %Acquring the Intensity gradients of the query points using the 
        %exsisiting cordinates, cX and cY, and relevent pixel values in
        %each an every points in the Gx and Gy matrix
        Ix=interp2(cX,cY,Gx(indXVal,indYVal),qX,qY);
        Iy=interp2(cX,cY,Gy(indXVal,indYVal),qX,qY);
        
        %creating the column vectors containing Ix and Iy considering each
        %and every pixels of 15x15 matrix located around the considering 
        %feauture value
        Ix=reshape(Ix,[(2*midRange+1)^2 1]);
        Iy=reshape(Iy,[(2*midRange+1)^2 1]);
        
        %creatning the matrix A as giving in the lecture slides of Derek
        %Hoeim
        A=zeros(2,(2*midRange+1)^2);                       
        A=[Ix,Iy];
        
        %interpolate the intensity values of the intermediate points of the
        %meshgrid. This will return a meshgrid of dcimal values. also it
        %considers the cordinates of decimal values when creating the mesh
        %grid.
        pixelIntensity=reshape(interp2(cX,cY,double(im0(indXVal,indYVal)),qX,qY),[(2*midRange+1)^2 1]);
        
        %reach to the specific feature minimizing the error by iterating a
        %loop until the error becomes negeligible. This process is doing
        %for larger variations feature point variations. Some lines are
        %same as the lines in the previous segement of the code.
        
        for j=1:30
            if((floor(tempX(i))-midRange)>0 && (floor(tempY(i))-midRange)>0 && (ceil(tempX(i))+midRange)<imX && (ceil(tempY(i))+midRange)<imY)  
                          
                indXVal2=floor(tempX(i)-midRange):ceil(tempX(i)+midRange);                                      
                indYVal2=floor(tempY(i)-midRange):ceil(tempY(i)+midRange);
                
                [qX,qY]=meshgrid(tempX(i)-floor(tempX(i))+ 1:1:(2*midRange+1),tempY(i)-floor(tempY(i))+ 1:1:(2*midRange+1));
                [cX,cY]=meshgrid(1:numel(indXVal2),1:numel(indYVal2));   
                
                if ~(size(qX,1)==15)
                    a=qX(end,:);
                    qX=[qX;a];
                    qX=[qX,15*ones(15,1)];
                    
                    qY=[qY,(1:1:14)'];
                    b=15*ones(1,15);
                    qY=[qY;b];      
                end
                                
                newPixelIntensity=interp2(cX,cY,double(im1(indXVal2,indYVal2)),qX,qY);
                newPixelIntensity=reshape(newPixelIntensity,[(size(newPixelIntensity,1))^2 1] );
                
                %calculating the vector,b, which contains the pixel value
                %difference of the intensity values of the given two frames
                %of the image. 
                
                b=-(newPixelIntensity-pixelIntensity);
                
                %calculating the d=[u v]' vector by the method suggest in
                %the algo, Derek Hoiem. Here first it takes the second
                %moments matirx of the initial image and inverse of the
                %obtaines secomd moment matrix is multiplied with the,A'*b,
                %which contains the multiplication of the intennsity
                %gradient vaector and the intensity difference vector of
                %the given image.
         
                d=(inv(A'*A))*(A'*b);
               
                %add the revent u and v to the current values of the feature
                %locations.
       
                tempX(i)=tempX(i)+d(1,1);
                tempY(i)=tempY(i)+d(2,1);
                
                %if both u and v are less than the given values it takes
                %break the loop assigning the current values of the tempX(i)
                %and tempY(i) to the newX and newY
                if(abs(d(1))<0.0001 && abs(d(2)<0.0001))
                    break;
                end
            end
        end
        newX(i)=tempX(i);
        newY(i)=tempY(i);
    end
end

