function network = network_add_layer(network, units)
    layer = normrnd(0, 0.05, units, 1);
    network{end+1} = layer;
end