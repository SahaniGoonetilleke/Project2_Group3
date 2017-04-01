% % % % function predictTranslation(startX, startY, Ix, Iy, im0, im1) is for getting the feature points of 
% % % % the feature points according to the given Ix and Iy values using the Kanade-Lucas-Tomasi tracking procedure method. 
% % % % @input args: feature pointsX,feture pointsY, user defined Ix, user defined Iy,initial image,next image of the sequence
% % % % @return val:2 matrices containing feture points loactions



function [newX newY]=predictTranslation(startX, startY, Ix, Iy, im0, im1);

%initilally assign matrix values of startXs and startYs to return values
newX=startX;
newY=startY;

midRange=7;
ra=2;

imX=size(im0,1);
imY=size(im0,2);

tempX=startX;
tempY=startY;



 %taking the gradients of the image
[Gx,Gy]=imgradientxy(im0); 



    
    if(floor(tempX-midRange)>0 && floor(tempY-midRange)>0 && ceil(tempX+midRange)<imX && ceil(tempY+midRange)<imY)  

        indXVal=floor(tempX-midRange):ceil(tempX+midRange);                                      
        indYVal=floor(tempY-midRange):ceil(tempY+midRange);

        %taking the mesh grid of the cordinate and the query points
        [qX,qY]=meshgrid(tempX-floor(tempX)+ 1:1:(2*midRange+1),tempY-floor(tempY)+ 1:1:(2*midRange+1));
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

        
        %creating the column vectors containing Ix and Iy considering each
        %and every pixels of 15x15 matrix located around the considering 
        %feauture value

        
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
            if((floor(tempX)-midRange)>0 && (floor(tempY)-midRange)>0 && (ceil(tempX)+midRange)<imX && (ceil(tempY)+midRange)<imY)  
                          
                indXVal2=floor(tempX-midRange):ceil(tempX+midRange);                                      
                indYVal2=floor(tempY-midRange):ceil(tempY+midRange);
                
                [qX,qY]=meshgrid(tempX-floor(tempX)+ 1:1:(2*midRange+1),tempY-floor(tempY)+ 1:1:(2*midRange+1));
                [cX,cY]=meshgrid(1:numel(indXVal2),1:numel(indYVal2));   
                
                if ~(size(qX,1)==15)
                    a=qX(end,:);
                    qX=[qX;a];
                    qX=[qX,15*ones(15,1)];
                    
                    qY=[qY,(1:1:14)'];
                    b=15*ones(1,15);
                    qY=[qY;b];      
                end
                numel(indXVal2)
                
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
       
                tempX=tempX+d(1,1);
                tempY=tempY+d(2,1);
                
                %if both u and v are less than the given values it takes
                %break the loop assigning the current values of the tempX
                %and tempY to the newX and newY
                if(abs(d(1))<0.0001 && abs(d(2)<0.0001))
                    break;
                end
            end
        end
        newX=tempX;
        newY=tempY;
    end


