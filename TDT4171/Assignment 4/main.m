%% Hyper parameters
alpha = 0.01; %Learning rate
network_structure = [64, 32, 10];
n_epochs = 100;
x = linspace(-6, 6);
y = linspace(-6, 6);

x = repelem(x, 100);
y = repmat(y, 1, 100);
E = l_simple([x; y]);

plot3(x, y, E, '*');
%{
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
%}