function network = network_add_layer(network, units, input_size)
    %layer = normrnd(1, 0.5, input_size, units);
    layer = normrnd(0, 0.05, input_size, units);
    network{end+1} = layer;
end