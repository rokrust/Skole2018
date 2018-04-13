function label=reverse_hot_one(hot_one)
    max = 0;
    label = 0;
    
    for i=1:size(hot_one, 2)
       if hot_one(i) > max 
           max = hot_one(i);
           label = i-1;
       end
    end
end