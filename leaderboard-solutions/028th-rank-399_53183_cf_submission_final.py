import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.metrics import f1_score
import seaborn as sns
import gc
from sklearn.preprocessing import LabelEncoder
from sklearn.model_selection import train_test_split
from sklearn.decomposition import TruncatedSVD
from sklearn.linear_model import LogisticRegression
from sklearn.ensemble import RandomForestClassifier
from sklearn.feature_extraction.text import CountVectorizer, TfidfVectorizer, HashingVectorizer
from sklearn.multiclass import OneVsRestClassifier
from sklearn.svm import SVC
from sklearn.naive_bayes import MultinomialNB
from sklearn.svm import LinearSVC

df_train = pd.read_csv("train.csv")
df_test = pd.read_csv('test_nvPHrOx.csv')

def clean_final(my_str):
    my_str = my_str.replace("http://", '').replace("www.", '').replace("https://", '').replace('.html', '').replace('.htm', '')
   
    return my_str

df_final = df_train[['Webpage_id', 'Tag', 'Url']]
df_final_test = df_test[['Webpage_id', 'Url']]

df_final['clean_url'] = df_final['Url'].apply(clean_final)
df_final_test['clean_url'] = df_final_test['Url'].apply(clean_final)

id_test = df_final_test['Webpage_id']
y_train = df_final['Tag'].values.tolist()
le = LabelEncoder()
y_train = le.fit_transform(y_train)

cv = TfidfVectorizer(analyzer='char', ngram_range=(3,10), max_features=50000)
cv.fit(df_final['clean_url'].values.tolist())

train_tfidf = cv.transform(df_final['clean_url'].values.tolist())
test_tfidf = cv.transform(df_final_test['clean_url'].values.tolist())

clf = OneVsRestClassifier(LogisticRegression(random_state=2017))
clf.fit(train_tfidf, y_train)
y_pred = clf.predict(test_tfidf)

sub = pd.DataFrame()
sub['Webpage_id'] = pd.Series(id_test)
sub['Tag'] = pd.Series(le.inverse_transform(y_pred))

print(sub.head())

sub.to_csv('first_one.csv', index=False)