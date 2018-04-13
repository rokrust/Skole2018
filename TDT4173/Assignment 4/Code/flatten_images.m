function flattened_images=flatten_images(images)
    [n_images, rows, cols] = size(images);
    flattened_images = zeros(n_images, rows*cols);
    
    for i=1:n_images
        image = images(i, :, :);
        flattened_images(i, :) = reshape(image, [1, rows*cols]); 
    end

end