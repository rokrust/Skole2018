from sklearn.datasets import load_digits
from sklearn.model_selection import train_test_split
from scipy.io import savemat

digits = load_digits()
train_digits, test_digits, train_labels, test_labels = train_test_split(digits.images, digits.target)

#mat_list = [image for image in a.images]
#mat_labels = [label for label in a.targe]
mat_dict =  {"images" : train_digits,
             "labels" : train_labels,
             "images_test" : test_digits,
             "labels_test" : test_labels
            }

savemat(file_name="digits", mdict=mat_dict)