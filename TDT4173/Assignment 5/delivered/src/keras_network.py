from keras.models import Sequential
from keras.layers import Dense, Dropout, Conv2D, MaxPooling2D, Flatten, Conv1D
from keras.utils import to_categorical
import numpy as np
from PIL import Image
import glob
import string
import pickle
from tensorflow.python.client import device_lib


def read_data(directory):
    image_list = []
    alphabet = list(string.ascii_lowercase)
    labels = []

    for letter in alphabet:
        images_in_dir = glob.glob('{}/{}/*.jpg'.format(directory, letter))
        print("At letter", letter)

        for file in images_in_dir:
            image = Image.open(file)
            image_list.append(np.asarray(image))
            labels.append(ord(letter) - ord('a'))

    return np.expand_dims(np.array(image_list), axis=3), to_categorical(labels)

def build_network(droprate):
    model = Sequential()
    model.add(Conv2D(filters=8, kernel_size=(3, 3), activation='relu', input_shape=(20, 20, 1)))
    model.add(Conv2D(filters=16, kernel_size=(3, 3), activation='relu'))
    model.add(Conv2D(filters=32, kernel_size=(5, 5), activation='relu'))
    #model.add(MaxPooling2D((2, 2)))
    model.add(Dropout(droprate))

    model.add(Conv2D(filters=48, kernel_size=(10, 10), activation='relu'))
    model.add(MaxPooling2D((2, 2)))
    model.add(Dropout(droprate))

    #model.add(Conv2D(filters=128, kernel_size=(3, 3), activation='relu'))
    #model.add(Dropout(droprate))

    model.add(Flatten())
    model.add(Dense(units=256, activation='relu'))
    model.add(Dropout(droprate))

    model.add(Dense(units=128, activation='relu'))
    model.add(Dropout(droprate))

    model.add(Dense(units=64, activation='relu'))
    model.add(Dropout(droprate))
    model.add(Dense(units=26, activation='softmax'))

    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])

    model.summary()

    return model



if __name__ == '__main__':
    print(device_lib.list_local_devices())
    images, labels = read_data('Exercise2/chars74k-lite')
    model = build_network(0.5)
    history = model.fit(images, labels, batch_size=32, epochs=100, validation_split=0.2)

    model.save("Conv_keras_model.h5")
    with open('trainHistoryDict.pickle', 'wb') as file_pi:
        pickle.dump(history.history, file_pi, protocol=pickle.HIGHEST_PROTOCOL)