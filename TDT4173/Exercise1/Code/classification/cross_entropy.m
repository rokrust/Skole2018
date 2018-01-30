function E = cross_entropy(y, z)
    %E = sum( -(1 - y)' * z - log(1 + exp(z)) );
    n = size(y, 1);
    E = -(y'*log(sigmoid(z))+(1-y)'*log(1-sigmoid(z)))/n;

end