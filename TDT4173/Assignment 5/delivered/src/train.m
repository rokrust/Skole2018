function [pc, training_mean, proj_mean, proj_cov] = train(images, labels, d, n_classes)
    %Perform PCA
    pc = eigenPCA(images, d);
    training_mean = mean(images);
    proj_images = (images - training_mean)*pc;

    %Find mean and cov of each class of the projected data
    [proj_mean, proj_cov] = proj_mean_cov(proj_images, labels, d, n_classes);
    %{
    hits = 0;
    for i = 1:length(images)
        
        likelihood = zeros(n_classes, 1);
        for class = 1:n_classes
            likelihood(class) = mvnpdf(proj_images(i, :), proj_mean(class, :), squeeze(proj_cov(class, : , :)));
        end

        [val, predictedLabels(i)] = max(likelihood);

        %Hit
        if predictedLabels(i) == labels(i)
            hits = hits + 1;
        end
    end

    
    total_score = hits / length(images)
    %}
end
