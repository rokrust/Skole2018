function [images, labels] = load_images(folderDir)
    %dir = chars74k-lite
    alphabet = 'abcdefghijklmnopqrstuvwxyz';
    imageSize1d = 20*20;
    images = [];
    labels = [];
    
    for i = 1:length(alphabet)
        current_dir = char(fullfile(folderDir, alphabet(i)))
        search_dir = char(fullfile(current_dir, '*.jpg'));
        
        jpegFiles = dir(search_dir);
        n_images = length(jpegFiles);
        
        images = [images; zeros([n_images, imageSize1d])];
        new_labels = ones(n_images, 1)*i;
        labels = [labels; new_labels];
        
        for j = 1:n_images
            file = fullfile(current_dir, jpegFiles(j).name);
            newImage = im2double(imread(file));
            images(end-n_images+j, :) = newImage(:)';
        end
    end
    
end