from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.datasets import load_digits
from sklearn.metrics import accuracy_score, classification_report

digits = load_digits()

x, y = digits.data, digits.target

X_train, X_test, y_train, y_test = train_test_split(x, y, test_size=0.2, random_state=42)

clf = RandomForestClassifier(n_estimators=100, random_state=42)
clf.fit(X_train, y_train)


predictions = clf.predict(X_test)

accuracy = accuracy_score(y_test, predictions)

print(accuracy)

report = classification_report(y_test, predictions, output_dict=True)
precision = report['weighted avg']['precision']
recall = report['weighted avg']['recall']
print(f'Precyzja: {precision:.4f}')
print(f'Czułość: {recall:.4f}')
