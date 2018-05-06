function pc = eigenPCA(images, d)
    %Find mean and cov
    zero_mean_images = images - mean(images);
    image_covariance = cov(zero_mean_images);
    
    [cov_eig_vec, ~] = eig(image_covariance);
    pc = cov_eig_vec(:, end-(d-1):end);

end