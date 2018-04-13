function y = network_predict(network, input)
    output = network_forward_pass(network, input);
    y = output(1);
end