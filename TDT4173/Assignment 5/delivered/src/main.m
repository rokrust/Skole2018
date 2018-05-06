%% Run the MLE algorithm and predict the supplied test images
n_principal_components = 60;
n_classes = 26;

% Read images
%[train_images, train_labels] = load_images('chars74k-lite');
test_images = load_test_images('test_images/learning');

index = 1:length(train_images);
index = index(randperm(length(index)));

train_end = floor(0.8*length(index));
train_index = index(1:train_end);
test_index = index(train_end+1:end);

%Train the MLE with respect to the training set
[pc, training_mean, proj_mean, proj_cov] = ...
        train(train_images(train_index, :), train_labels(train_index, :), ...
        n_principal_components, n_classes);

%Predict the test set
test_labels = predict(test_images, pc, training_mean, proj_mean, proj_cov);

%accuracy = sum(test_labels == train_labels(test_index)) / length(test_labels)
label = 'lnanaabgdssemdgrhecasicinereanoinane';
label = str2num(label) - str2num('a');
accuracy = sum(test_labels == label') / length(test_labels)
%{
%% Find images in the test files. This should be run separately from the above section
n_images = 4;
images = process_test_images(image, n_images);

for i=1:n_images
    imwrite(squeeze(images(i, :, :)), strcat('test_images/test/', num2str(i), '.jpg'))
end
%}
%Test 100%
%Learning 91.7%
%}