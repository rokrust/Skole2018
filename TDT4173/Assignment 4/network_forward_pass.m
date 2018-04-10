function output = network_forward_pass(network, input, current_layer)
    n_inputs = size(input, 1);
    n_nodes = size(network{current_layer}, 1);
    n_layers = size(network, 2);
    out = zeros(n_nodes, 1);
    
    %Calculate output of each node
    for i=1:n_inputs
        out = out + network{current_layer} .* input(i);
    end
    out = logsig(out);
    
    %Hidden layers propagate
    if current_layer < n_layers
        current_layer = current_layer + 1;
        output = network_forward_pass(network, out, current_layer);
        output{end+1} = out;
    
    %Final layer
    else
        output = {out};
        
    end
        
end