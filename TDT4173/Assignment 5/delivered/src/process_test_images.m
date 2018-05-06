function [ test_images ] = process_test_images( input_image, k )
    sliding_window_size = [20, 20];
    means = zeros(180, 180);
    
    for i=1:(size(input_image, 1) - sliding_window_size(1))
        for j=1:(size(input_image, 2) - sliding_window_size(2))
            
            window = get_window(input_image, i, j, sliding_window_size);
            means(i, j) = mean(window(:));
        end
    end
    
    test_images = find_k_windows(means, k, input_image, sliding_window_size);
end

function window = get_window(image, i, j, sliding_window_size)
    window = image(i:i+sliding_window_size(1)-1, j:j+sliding_window_size(2)-1);
end

function window = set_window(means, i, j, sliding_window_size)
    image(i:i+sliding_window_size(1)-1, j:j+sliding_window_size(2)-1) = ones(sliding_window_size)*255;
end

function windows = find_k_windows(means, k, image, sliding_window_size)
    x = sliding_window_size(1);
    y = sliding_window_size(2);
    windows = zeros(k, x, y);
    
    for l=1:k
        minMatrix = min(means(:));
        [i,j] = find(means==minMatrix);
        means(i:i+x-1, j:j+y-1) = ones(sliding_window_size)*255;
        means(i-x+1:i, j-y+1:j) = ones(sliding_window_size)*255;
        means(i:i+x-1, j-y+1:j) = ones(sliding_window_size)*255;
        means(i-x+1:i, j:j+y-1) = ones(sliding_window_size)*255;
        
        windows(l, :, :) = get_window(image, i, j, sliding_window_size) / 255;
    end
end