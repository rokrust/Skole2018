load('digits.mat')

[n_images, n_rows, n_cols] = size(images);
flattened_images = flatten_images(images);
flattened_images(flattened_images>0) = 20;
labels_hot_one = hot_one_encoding(double(labels), 0, 9);

labels_hot_one_test = hot_one_encoding(double(labels_test), 0, 9);
flattened_images_test = flatten_images(images_test);
flattened_images_test(flattened_images_test>0) = 20;

%% Hyper parameters
alpha = 0.01; %Learning rate
network_structure = [64, 32, 10];
n_epochs = 100;

%% Create network
network = {};
input_size = 64;
for i=1:size(network_structure, 2)
   network = network_add_layer(network, network_structure(i), input_size);
   input_size = network_structure(i);
end

loss = [];
      
%% Train network
for epoch=1:n_epochs
    current_loss = 0;
    %{
    for sample=1:size(labels_hot_one_test, 1)
        output = network_forward_pass(network, flattened_images(sample, :), 1, 0);
        current_loss = current_loss + sum(0.5*(output{1} - labels_hot_one(sample)).^2);
    end
    %}
    loss = [loss current_loss/size(labels_hot_one, 1)];
    network = network_train(network, flattened_images, labels_hot_one, alpha, 0.2);
end

plot(loss);
predictions = zeros(size(labels_test));
for i=1:size(labels_test, 2)
    output = network_forward_pass(network, flattened_images_test(i, :), 1, 0);
    predictions(i) = reverse_hot_one(output{1});
end

confusion_matrix = confusionmat(double(labels_test), predictions);
latex_table = latex(sym(confusion_matrix));