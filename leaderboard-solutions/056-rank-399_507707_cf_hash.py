import pandas as pd
from sklearn.feature_extraction.text import HashingVectorizer
import pickle
from sklearn.model_selection import train_test_split 
from sklearn.ensemble import RandomForestClassifier
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.metrics import accuracy_score
from sklearn.metrics import f1_score
from sklearn.metrics import recall_score
from sklearn.metrics import precision_score
from sklearn.metrics import confusion_matrix
from nltk.corpus import stopwords
import string
import nltk
import re
from sklearn.feature_extraction.text import TfidfVectorizer

clean = re.compile('<.*?>')

nltk.download('stopwords')
stop = stopwords.words('english')
# list of text documents
data=pd.read_csv('wow_text.csv')#nrows=100)
print(data)

# create the transform

data['NTag']=0
data.NTag[data.Tag == 'news'] = 1
data.NTag[data.Tag == 'clinicalTrials'] = 2
data.NTag[data.Tag == 'conferences'] = 3
data.NTag[data.Tag == 'forum'] = 4
data.NTag[data.Tag == 'guidelines'] = 5
data.NTag[data.Tag == 'others'] = 6
data.NTag[data.Tag == 'profile'] = 7
data.NTag[data.Tag == 'publication'] = 8
data.NTag[data.Tag == 'thesis'] = 9


data.Html = data.Html.str.replace('<[^<]+?>', '')
data.Html = data.Html.str.replace('[^a-zA-Z ]+', '')
data.Html = data.Html.apply(lambda x: x.lower())
data.Html = data.Html.apply(lambda x: x.translate(string.punctuation))
data.Html = data.Html.apply(lambda x: x.translate(string.digits))
#re.sub(clean, '', text)

#data.Html=data.Html.apply(lambda x: [item for item in x if item not in stop])

data_total=data[data.NTag != 0]
data_predict=data[data.NTag == 0]
#data_total.drop('Tag',axis=1)
data_total.drop('Webpage_id',axis=1)
print(data_total)
print('Meh')
print(data_predict)


train_x = data_total \
    .drop("NTag", axis = 1) \
    .fillna(0) \
    .values

train_y = data_total.NTag.values



vectorizer = HashingVectorizer()#(sublinear_tf=True, max_df=0.5, max_features=10000, norm='l1')
#vectorizer = HashingVectorizer()
X_train = vectorizer.fit_transform(data_total.Html.values)


train_x_1, test_x_1, train_y_1, test_y_1 = train_test_split(X_train, train_y, test_size=0.1)


test_x = data_predict \
    .drop("NTag", axis = 1) \
    .fillna(0) \
    .values

print(data_predict)
#vectorizer = HashingVectorizer()
print(data_predict.Html.values)
X_test = vectorizer.transform(data_predict.Html.values)
#vectorizer.transform(X_valid)

'''
with open('Vectorized.pkl', 'wb') as f:
    pickle.dump(vectorizer, f)
'''
clf = RandomForestClassifier(n_estimators=300, verbose=3,n_jobs=20)
clf.fit(X_train, train_y)
data_predict['New']=clf.predict(X_test)

#loaded_model = pickle.load(open('clf.pkl', 'rb'))
#result = loaded_model.predict(X_test)

#data_predict['Prediction']=result

data_predict=data_predict.drop("Html", axis = 1)
data_predict=data_predict.drop('Unnamed: 0',axis=1)
data_predict=data_predict.drop('Tag',axis=1)
data_predict=data_predict.drop('NTag',axis=1)

#data_predict['New']=0
data_predict.New[data_predict.New == 1] = 'news'
data_predict.New[data_predict.New == 2] = 'clinicalTrials'
data_predict.New[data_predict.New == 3] = 'conferences'
data_predict.New[data_predict.New == 4] = 'forum'
data_predict.New[data_predict.New == 5] = 'guidelines'
data_predict.New[data_predict.New == 6] = 'others'
data_predict.New[data_predict.New == 7] = 'profile'
data_predict.New[data_predict.New == 8] = 'publication'
data_predict.New[data_predict.New == 9] = 'thesis'


#data_predict.to_csv('upload_1.csv')
data_predict.to_csv('upload_me_1.csv')
print(data_predict)

print(clf.score(test_x_1, test_y_1))

