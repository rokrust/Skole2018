function output = network_forward_pass(network, input, current_layer, dropout)
    [n_inputs, n_nodes] = size(network{current_layer});
    n_layers = size(network, 2);
    
    %Calculate the output of every single node
    out = input * network{current_layer};
    out = logsig(out);
    
    %Dropout
    dropout_vec = double(rand(1, n_nodes) > dropout);
    out = ((1/(1-dropout))*dropout_vec) .* out;
    
    %Hidden layers propagate
    if current_layer < n_layers
        current_layer = current_layer + 1;
        output = network_forward_pass(network, out, current_layer, dropout);
        output{end+1} = out;
    
    %Final layer
    else
        out = input * network{current_layer};
        out = logsig(out);
        output = {out};
        %output = {input * network{current_layer}};
        
    end
        
end