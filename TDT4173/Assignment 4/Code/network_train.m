function network = network_train(network, inputs, labels, alpha, dropout)
    
    %For each sample
    for i=1:size(labels, 1)
        
        %Calculate weight gradients
        delta_w = network_calculate_delta_w(network, inputs(i, :), labels(i, :), dropout);
        
        %Update weights
        for j=1:size(network, 2)
              network{j} = network{j} - alpha * delta_w{j};
        end
        
    end
end

function delta_w = network_calculate_delta_w(network, input, label, dropout)
    n_layers = size(network, 2);
    delta_w = cell(size(network));
    
    %Calculate output of every single node
    output = network_forward_pass(network, input, 1, dropout);
    
    %add model input as the output of the input layer
    output = [output input];
    
    %delta at output layer
    delta = (output{1} - label) .* (output{1} .* (1 - output{1}));
    %delta = (output{1} - label) * output{1};
    
    %Iterate through each layer
    for i=1:n_layers
        j = n_layers - i + 1; %j from back, i from front
        current_input = output{i+1};
        
        %Iterate through each node in layer
        delta_w{j} = current_input' * delta;
        delta = (delta * network{j}') .* (current_input .* (1 - current_input));
    end
end