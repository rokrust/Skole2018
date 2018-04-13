function output = network_forward_pass(network, input, current_layer)
    [n_inputs, n_nodes] = size(network{current_layer});
    n_layers = size(network, 2);
    
    %Calculate the output of every single node
    out = input * network{current_layer};
    out = logsig(out);
    
    %Hidden layers propagate
    if current_layer < n_layers
        current_layer = current_layer + 1;
        output = network_forward_pass(network, out, current_layer);
        output{end+1} = out;
    
    %Final layer
    else
        output = {out};
        %output = {input * network{current_layer}};
        
    end
        
end