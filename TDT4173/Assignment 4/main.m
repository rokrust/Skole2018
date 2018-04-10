%% Hyper parameters
alpha = 0.1; %Learning rate
network_structure = [2, 1];
n_epochs = 10;

%% Create network
network = {};
for i=1:size(network_structure, 2)
   network = network_add_layer(network, network_structure(i)); 
end

%% Load samples and labels
inputs = [0, 0;
          0, 1;
          1, 0;
          1, 1];
labels = [0;
          1;
          1;
          0;];

%% Train network
for epoch=1:n_epochs
    network = network_train(network, inputs, labels, alpha);
end