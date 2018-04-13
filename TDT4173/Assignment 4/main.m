load('digits.mat')

[n_images, n_rows, n_cols] = size(images);
flattened_images = flatten_images(images);
labels = double(labels);

%% Hyper parameters
%alpha = 0.7; %Learning rate
%network_structure = [2, 1];
%n_epochs = 10000;

alpha = 0.01; %Learning rate
network_structure = [128, 64, 20, 1];
n_epochs = 10000;

%% Create network
network = {};
input_size = 64;
for i=1:size(network_structure, 2)
   network = network_add_layer(network, network_structure(i), input_size);
   input_size = network_structure(i);
end

%% Load samples and labels
%inputs = [1, 1;0, 1;1, 0;0, 0];
%labels = [0;1;1;0];

loss = [];
      
%% Train network
for epoch=1:n_epochs
    current_loss = 0;
    for sample=1:size(labels, 1)
        output = network_forward_pass(network, flattened_images(sample, :), 1);
        current_loss = current_loss + 0.5*(output{1} - labels(sample))^2;
    end
    
    loss = [loss current_loss/size(labels, 1)];
    network = network_train(network, flattened_images, labels, alpha);
end

plot(loss);
predictions = zeros(size(labels));
for i=1:size(labels)
    output = network_forward_pass(network, flattened_images(i, :), 1);
    predictions(i) = round(output{1})
end

%{
for epoch=1:n_epochs
    current_loss = 0;
    for sample=1:size(inputs, 1)
        output = network_forward_pass(network, inputs(sample, :), 1);
        current_loss = current_loss + 0.5*(output{1} - labels(sample))^2;
    end
    
    loss = [loss current_loss/size(labels, 1)];
    network = network_train(network, inputs, labels, alpha);
end

plot(loss);
predictions = zeros(size(labels));
for i=1:size(labels)
    output = network_forward_pass(network, inputs(i, :), 1);
    predictions(i) = round(output{1})
end
%}
confusion_matrix = confusionmat(labels, predictions);