from sklearn.datasets import load_digits
from scipy.io import savemat

digits = load_digits()

#mat_list = [image for image in a.images]
#mat_labels = [label for label in a.targe]
mat_dict =  {"images" : digits.images,
             "labels" : digits.target
            }

savemat(file_name="digits", mdict=mat_dict)