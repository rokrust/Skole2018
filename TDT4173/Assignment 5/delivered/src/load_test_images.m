function images = load_test_images(folderDir)
    imageSize1d = 400;
    search_dir = char(fullfile(folderDir, '*.jpg'));

    jpegFiles = dir(search_dir);
    n_images = length(jpegFiles);

    images = zeros([n_images, imageSize1d]);

    for j = 1:n_images
        jpegFiles(j).name
        file = fullfile(folderDir, jpegFiles(j).name);
        newImage = im2double(imread(file));
        images(j, :) = newImage(:)';
    end
end