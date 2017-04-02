% Cylindrical projection of images
function projected = cylindrical(images, f, k1, k2)
    projected = zeros(size(images));

    W = size(images, 2);
    H = size(images, 1);
    Xcyl = W / 2;
    Ycyl = H / 2;

    % formula from the lecture notes
    for X = 1 : W
        alpha = (X - Xcyl) / f;
        for Y = 1 : H
            h = (Y - Ycyl) / f;
            Xhat = sin(alpha);
            Yhat = h;
            Zhat = cos(alpha);
            
           
            r = (Xhat / Zhat)^2 + (Yhat / Zhat)^2;
            
%             Xd = (Xhat / Zhat) * (1 + k1 * r + k2 * r^2);
%             Yd =(Yhat / Zhat) * (1 + k1 * r + k2 * r^2);

             Xd = (Xhat / Zhat) * (1 + k1 * r^2 + k2 * r^4);
             Yd = (Yhat / Zhat) * (1 + k1 * r^2 + k2 * r^4);

             
            
            Xi = int64(f * Xd + Xcyl);
            Yi = int64(f * Yd + Ycyl);
            
            if (Xi > 0 && Xi <= W && Yi > 0 && Yi <= H)
                projected(Y, X, :, :) = uint8(images(Yi, Xi, :, :));
            end
        end
    end

    projected = uint8(projected);
end