function hot_one=hot_one_encoding(labels, min, max)
    hot_one = zeros(size(labels, 2), max-min);
    for i=1:size(labels, 2)
        hot_one(i, labels(i)-min+1) = 1;
    end
end