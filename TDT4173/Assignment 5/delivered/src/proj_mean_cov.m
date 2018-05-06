function [mean_proj, cov_proj] = proj_mean_cov(proj_data, labels, d, n_classes)
    mean_proj = zeros(n_classes, d);
    cov_proj = zeros(n_classes, d, d);
    
    for class = 1:n_classes

        classData = proj_data(labels == class, :);
        mean_proj(class, :) = mean(classData)';
        cov_proj(class, :, :) = cov(classData);
    end
end    