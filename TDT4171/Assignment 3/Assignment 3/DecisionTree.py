from random import randint, random
from math import log

class Data:
    attributes = []
    labels = []

    def __init__(self):
        self.attributes = []
        self.labels = []

    def __len__(self):
        return len(self.labels)

    def entropy(self, p):
        if p == 0 or p == 1:
            return 0

        return -(p * log(p, 2) + (1 - p) * log(1 - p, 2))

    def read(self, dir):
        with open(dir, "r") as f:
            for line in f:
                line = map(int, line.split())
                self.attributes.append(line[:-1])
                self.labels.append(line[-1])

    def best_attribute_split(self):
        highest_importance = 0
        best_attribute = 0

        for i in range(self.attributes_remaining()):
            current_importance = self.importance(i)
            if current_importance > highest_importance:
                best_attribute = i
                highest_importance = current_importance

        return best_attribute

    def split_by(self, attribute_index):
        left_data = Data()
        right_data = Data()

        if self.attributes_remaining() == 0:
            return left_data, right_data

        for i in range(len(self)):
            if (self.attributes[i][attribute_index] == 2):
                right_data.attributes.append(list(self.attributes[i]))
                right_data.labels.append(self.labels[i])
                del right_data.attributes[-1][attribute_index]

            else:
                left_data.attributes.append(list(self.attributes[i]))
                left_data.labels.append(self.labels[i])
                del left_data.attributes[-1][attribute_index]

        return left_data, right_data

    def positive_probability(self):
        label1_count = self.labels.count(1)
        label2_count = self.labels.count(2)

        return float(label2_count) / (label1_count + label2_count)

    def importance(self, attribute_index):

        left_data, right_data = self.split_by(attribute_index)
        p_left = left_data.positive_probability()
        p_right = right_data.positive_probability()
        p = self.positive_probability()

        rem = (len(left_data) * left_data.entropy(p_left) + len(right_data) * right_data.entropy(p_right)) / len(self)
        importance_value = self.entropy(p) - rem
        #importance_value = random()

        return importance_value

    def attributes_remaining(self):
        return len(self.attributes[0])

    def plurality(self):
        label1_count = self.labels.count(1)
        label2_count = self.labels.count(2)

        if label1_count > label2_count:
            return 1
        elif label2_count > label1_count:
            return 2
        else:
            return randint(1, 2)

    def is_classifiable(self):
        class_label = self.labels[0]
        for i in range(len(self)):
            if self.labels[i] != class_label:
                return False

        return True

class DecisionTree:
    classification = None
    attribute = None
    child_nodes = [None, None]

    def __init__(self, classification = None, attribute = None, child_nodes = []):
        self.classification = classification
        self.child_nodes = list(child_nodes)
        self.attribute = attribute

    def decision_tree_learning(self, parent_data, data, current_cuts):
        #Stopping conditions
        if len(data) == 0:
            self.classification = parent_data.plurality()
            return self

        if data.attributes_remaining() == 0:
            self.classification = data.plurality()
            return self

        if data.is_classifiable():
            self.classification = data.labels[0]
            return self

        # Correct relative cutting indexes
        attr = data.best_attribute_split()
        left_children, right_children = data.split_by(attr)
        for cut in current_cuts:
            if cut <= attr:
                attr += 1
        current_cuts.append(attr)
        self.attribute = attr

        self.child_nodes.append(DecisionTree().decision_tree_learning(data, left_children, list(current_cuts)))
        self.child_nodes.append(DecisionTree().decision_tree_learning(data, right_children, list(current_cuts)))

        return self

    def classify(self, sample):
        current_node = self
        for i in range(len(sample)):
            if(current_node.classification != None):
                return current_node.classification

            value = sample[current_node.attribute] - 1
            current_node = current_node.child_nodes[value]

    def accuracy(self, test_set, labels):
        n_correct_classifications = 0
        for i in range(len(test_data.attributes)):
            if tree.classify(test_set[i]) == labels[i]:
                n_correct_classifications += 1

        return float(n_correct_classifications) / len(labels)

if __name__ == "__main__":
    training_data = Data()
    test_data = Data()
    training_data.read("data/training.txt")
    test_data.read("data/test.txt")

    tree = DecisionTree()
    tree.decision_tree_learning(training_data, training_data, [])

    accuracy = tree.accuracy(test_data.attributes, test_data.labels)
    print "Test set accuracy: {0:.2f}%".format(accuracy*100)
