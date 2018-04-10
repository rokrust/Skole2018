function network = network_train(network, inputs, labels, alpha)
    
    %For each sample
    for i=1:size(labels)
        
        %Calculate weight gradients
        delta_w = network_calculate_delta_w(network, inputs(i, :), labels(i));
        
        %Update weights
        for j=1:size(network)
            network{j} = network{j} - alpha * delta_w{j};
        end
        
    end
end

function delta_w = network_calculate_delta_w(network, input, label)
    n_layers = size(network, 2);
    delta_w = cell(size(network));
    
    %Calculate output of every single node
    output = network_forward_pass(network, input, 1);
    
    delta = (label - output{1}) * output{1}*(1 - output{1});
    for i=1:n_layers - 1
        j = n_layers - i + 1;
        input = output{i+1};
        delta_w{i} = delta * input;
        
        delta = (delta * network{j}) * output{i}*(1 - output{i});
    end
    
    delta_w{end} = delta * input;%output{end};
end