function predictedLabels=predict(images, pc, training_mean, proj_mean, proj_cov)
    n_classes = size(proj_mean, 1);
    proj_images = (images - training_mean)*pc;
    predictedLabels = zeros(size(images, 1), 1);
    
    for i = 1:size(images, 1)
        
        likelihood = zeros(n_classes, 1);
        for class = 1:n_classes
            likelihood(class) = mvnpdf(proj_images(i, :), proj_mean(class, :), squeeze(proj_cov(class, : , :)));
        end

        [val, predictedLabels(i)] = max(likelihood);
    end
end