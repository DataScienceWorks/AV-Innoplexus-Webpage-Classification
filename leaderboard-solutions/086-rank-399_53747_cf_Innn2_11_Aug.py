
# coding: utf-8

# In[3]:

import pandas as pd
import nltk, string
import numpy as np
nltk.download('punkt')
nltk.download('wordnet')
import matplotlib.pyplot as plt
get_ipython().magic('matplotlib inline')
import seaborn as sns
nltk.download('stopwords')
from nltk.corpus import stopwords


# In[7]:

web_train_df = pd.read_csv("E:\\Datathom\\inno2\\train\\train.csv")
from sklearn import preprocessing
le = preprocessing.LabelEncoder()
 
dom_le_fit = le.fit(web_train_df['Domain'])
#dom_lab = dom_le_fit.transform(web_train_sample_df2['Domain'])

#web_train_df["domain_nw"] =
#web_train_df.
#print(web_train_df["Tag"].value_counts())
#sns.countplot(data=web_train_df,x='Tag')


# In[9]:

web_train_df[web_train_df["Tag"] == "news"]["Webpage_id"]

web_train_df["Tag"].value_counts()/400

sample_sz = 100

web_train_sample_df = web_train_df[web_train_df["Tag"] == "others"].sample(sample_sz)
web_train_sample_df = web_train_sample_df.append(web_train_df[web_train_df["Tag"] == "news"].sample(sample_sz), ignore_index=True)
web_train_sample_df = web_train_sample_df.append(web_train_df[web_train_df["Tag"] == "publication"].sample(sample_sz), ignore_index=True)
web_train_sample_df = web_train_sample_df.append(web_train_df[web_train_df["Tag"] == "profile"].sample(sample_sz), ignore_index=True)
web_train_sample_df = web_train_sample_df.append(web_train_df[web_train_df["Tag"] == "forum"].sample(sample_sz), ignore_index=True)
web_train_sample_df = web_train_sample_df.append(web_train_df[web_train_df["Tag"] == "clinicalTrials"].sample(sample_sz), ignore_index=True)
web_train_sample_df = web_train_sample_df.append(web_train_df[web_train_df["Tag"] == "thesis"].sample(sample_sz), ignore_index=True)
web_train_sample_df = web_train_sample_df.append(web_train_df[web_train_df["Tag"] == "guidelines"].sample(sample_sz), ignore_index=True)

#web_test_sample_df = web_train_df[web_train_df["Tag"] == "others"].sample(43)
#web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "news"].sample(20), ignore_index=True)
#web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "publication"].sample(20), ignore_index=True)
#web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "profile"].sample(13), ignore_index=True)
#web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "forum"].sample(12), ignore_index=True)
#web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "clinicalTrials"].sample(7), ignore_index=True)
#web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "thesis"].sample(5), ignore_index=True)
#web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "guidelines"].sample(4), ignore_index=True)

#print(temp_df)
#web_test_sample_df.Tag.value_counts()


# In[11]:

web_test_sample_df_new = pd.read_csv("E:\\Datathom\\inno2\\test_nvPHrOx.csv")
web_test_sample_df_old = web_test_sample_df_new.copy(deep=True)
#web_test_sample_df = web_test_sample_df[1:100]
#web_test_sample_df.head()
web_test_sample_df_new.shape


# In[13]:

web_test_sample_df = web_train_df[web_train_df["Tag"] == "others"].sample(43)
web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "news"].sample(20), ignore_index=True)
web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "publication"].sample(20), ignore_index=True)
web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "profile"].sample(13), ignore_index=True)
web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "forum"].sample(12), ignore_index=True)
web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "clinicalTrials"].sample(7), ignore_index=True)
web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "thesis"].sample(5), ignore_index=True)
web_test_sample_df = web_test_sample_df.append(web_train_df[web_train_df["Tag"] == "guidelines"].sample(4), ignore_index=True)

queryString_test = "select DISTINCT Webpage_id, Html from 'table' where Webpage_id in "+str(tuple(web_test_sample_df["Webpage_id"]))
test_sam_html_df = pd.read_sql_query( queryString_test, con)
web_test_sample_df2 = web_test_sample_df
test_sam_html_df = test_sam_html_df[["Webpage_id","Html"]].drop_duplicates()

web_test_sample_df2 = web_test_sample_df2.join(test_sam_html_df[["Webpage_id","Html"]].set_index("Webpage_id"), on="Webpage_id", lsuffix="l", rsuffix="ht")


# In[12]:

## Feching Data
#print(web_html_data_df)
import sqlite3 
con = sqlite3.connect('csv_database.db')
cursor = con.cursor()
#cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
#print(cursor.fetchall())

### Train Data
queryString_train = "select DISTINCT Webpage_id, Html from 'table' where Webpage_id in "+str(tuple(web_train_sample_df["Webpage_id"]))

train_sam_html_df = pd.read_sql_query( queryString_train, con)
print(train_sam_html_df.shape)
web_train_sample_df2 = web_train_sample_df
train_sam_html_df = train_sam_html_df[["Webpage_id","Html"]].drop_duplicates()
web_train_sample_df2 = web_train_sample_df2.join(train_sam_html_df[["Webpage_id","Html"]].set_index("Webpage_id"), on="Webpage_id", lsuffix="l", rsuffix="ht")

#### Test Data
#queryString_test = "select DISTINCT Webpage_id, Html from 'table' where Webpage_id in "+str(tuple(web_test_sample_df["Webpage_id"]))
#test_sam_html_df = pd.read_sql_query( queryString_test, con)
#web_test_sample_df2 = web_test_sample_df
#test_sam_html_df = test_sam_html_df[["Webpage_id","Html"]].drop_duplicates()

#web_test_sample_df2 = web_test_sample_df2.join(test_sam_html_df[["Webpage_id","Html"]].set_index("Webpage_id"), on="Webpage_id", lsuffix="l", rsuffix="ht")



# In[14]:

stemmer = nltk.stem.porter.PorterStemmer()
def StemTokens(tokens):
     return [stemmer.stem(token) for token in tokens]
remove_punct_dict = dict((ord(punct), None) for punct in string.punctuation + string.digits)
def StemNormalize(text):
    return StemTokens(nltk.word_tokenize(text.lower().translate(remove_punct_dict)))

lemmer = nltk.stem.WordNetLemmatizer()
def LemTokens(tokens):
     return [lemmer.lemmatize(token) for token in tokens]
remove_punct_dict = dict((ord(punct), None) for punct in string.punctuation + string.digits)
def LemNormalize(text):
    return LemTokens(nltk.word_tokenize(text.lower().translate(remove_punct_dict)))

def remove_html_tags(text):
    """Remove html tags from a string"""
    import re
    clean = re.compile('<.*?>')         
    return re.sub(clean, ' ', text)

from sklearn.feature_extraction.text import CountVectorizer
LemVectorizer = CountVectorizer(tokenizer=LemNormalize, stop_words='english', max_df=0.5, min_df = 0.05, max_features= 50000, ngram_range=(1, 1))

from sklearn.feature_extraction.text import TfidfTransformer
tfidfTran = TfidfTransformer(use_idf=False, norm="l2")


# In[15]:


LemVectorizer_ft = LemVectorizer.fit(web_train_sample_df2.Html.apply(lambda x: remove_html_tags(x)))
tf_matrix = LemVectorizer_ft.transform(web_train_sample_df2.Html.apply(lambda x: remove_html_tags(x)))


web_html_matrix_tfidf = tfidfTran.fit_transform(tf_matrix)
print("done stemming --->")


# In[17]:

import lightgbm as lgbm
from lightgbm import LGBMClassifier, LGBMRegressor

#  Test Data
tf_tst_matrix = LemVectorizer_ft.transform(web_test_sample_df2.Html.apply(lambda x: remove_html_tags(x)))
web_html_matrix_test_tfidf = tfidfTran.transform(tf_tst_matrix)

print(web_train_sample_df2.shape)
print(web_html_matrix_tfidf.shape)
print(web_test_sample_df2.shape)
print(web_html_matrix_test_tfidf.shape)



params = {
    'boosting_type':'gbdt',
    'objective':'multiclass',
    'learning_rate': 0.02,
    'num_class': 9,
    'early_stopping': 100,
    'num_iteration': 2000, 
    'num_leaves': 31,
    'is_enable_sparse': 'true',
    'tree_learner': 'data',
    'max_depth': 7, 
    'n_estimators': 50  
    }

tag_le_fit = le.fit(web_train_sample_df2['Tag'])
tag_lab = tag_le_fit.transform(web_train_sample_df2['Tag'])

test_tag_lab = tag_le_fit.transform(web_test_sample_df2['Tag'])

#print(tag_lab)
#print(test_tag_lab)

d_train = lgbm.Dataset(web_html_matrix_tfidf, tag_lab)
d_valid = lgbm.Dataset(web_html_matrix_test_tfidf, test_tag_lab)

lgbm_train = [d_train, d_valid]
#lgbm_eval = lgbm.Dataset(tf_tst_matrix, test_tag_lab, reference=lgbm_train)
#, 
#type(pd.DataFrame(web_html_matrix_tfidf.toarray()))
clf_gbm = lgbm.train(params, 
                     train_set=d_train, 
                     valid_sets=lgbm_train, 
                     verbose_eval=10)

predicted_LGBM = clf_gbm.predict(web_html_matrix_test_tfidf)
predictions = []

for x in predicted_LGBM:
    predictions.append(np.argmax(x))
    
print(len(predictions))
from sklearn.metrics import f1_score
from sklearn.metrics import confusion_matrix

print(f1_score(test_tag_lab, predictions, average='macro'))
#print(accuracy_score(test_tag_lab.Tag, predicted_LGBM))


# In[21]:

#web_test_sample_df_new

### Get Data complete
print(web_test_sample_df_new.shape)
web_train_out_df_new = pd.DataFrame({'Webpage_id': range(1, 3), 'Tag': np.random.randn(2)})

## Get Chunk Data
chunksize_tst = 1000
i =0;
for i in range(0, 25787, chunksize_tst):

    df_new = web_test_sample_df_new[i:i+chunksize_tst]
    print("chunk number ==>",i,df_new.shape)    
    ## Fetch HTML Data for Chunk
    queryString_test = "select DISTINCT Webpage_id, Html from 'table' where Webpage_id in "+str(tuple(df_new["Webpage_id"]))
    test_sam_html_df = pd.read_sql_query( queryString_test, con)
    test_sam_html_df = test_sam_html_df[["Webpage_id","Html"]].drop_duplicates()
    df_new = df_new.join(test_sam_html_df[["Webpage_id","Html"]].set_index("Webpage_id"), on="Webpage_id", lsuffix="l", rsuffix="ht")
    #print(test_sam_html_df.shape)
    ## Transform Chunk Data 
    tf_tst_matrix = LemVectorizer_ft.transform(df_new.Html.apply(lambda x: remove_html_tags(x)))
    web_html_matrix_test_tfidf = tfidfTran.transform(tf_tst_matrix)
    #print(web_html_matrix_test_tfidf.shape)
    ## Get Prediction Lables for Chunk
    predicted_LGBM = clf_gbm.predict(web_html_matrix_test_tfidf)    
    predictions = []
    for x in predicted_LGBM:
        predictions.append(np.argmax(x))
    #print(predictions)    
    ## Store Chunk data and Tag in Data Frame
    web_train_out_df_part = pd.DataFrame( {'Webpage_id': df_new["Webpage_id"], 'Tag':le.inverse_transform(predictions)})
    #web_train_out_df_part['Webpage_id'] = df_new["Webpage_id"]
    #web_train_out_df_part['Tag'] = le.inverse_transform(predictions)
    #print(web_train_out_df_part)
    web_train_out_df_new = web_train_out_df_new.append( web_train_out_df_part, ignore_index=True)

### Final Join

web_train_out_df_new[['Webpage_id','Tag']].to_csv("E:\\Datathom\\inno2\\out_new.csv", index=False)
web_train_out_df_new.head(10)


# In[20]:

web_train_out_df_part
web_test_sample_df_new[:101].to_csv("E:\\Datathom\\inno2\\out.csv", index=False)


# In[ ]:

web_train_out_df = web_test_sample_df2.copy(deep=True)
web_train_out_df['Tag'] = le.inverse_transform(predictions)
web_train_out_df[['Webpage_id','Tag']].to_csv("E:\\Datathom\\inno2\\out.csv", index=False)


# In[ ]:

#!pip install -U xgboost 
predicted_LGBM.shape
#import xgboost as xgb
#print(tag_lab)
#LemVectorizer_ft.get_feature_names()
#print(string.punctuation + string.digits)
#len(predicted)
#web_train_out_df.shape

#stop_words = stopwords.words('english')
#print(stop_words)
#import string
#from string import maketrans
#ranslator = str.maketrans(' ', ' ', string.punctuation)
#fset_text1.lower().translate(ranslator)
#print(web_html_matrix_tfidf.shape)
#from sklearn.metrics import accuracy_score
#predictions = [round(value) for value in predicted_LGBM]
#print(accuracy_score(test_tag_lab, predicted_LGBM))
#print(f1_score(web_test_sample_df2.Tag, predicted, average='macro'))

print(web_train_sample_df2.shape)
print(web_html_matrix_tfidf.shape)
print(web_test_sample_df2.shape)
print(web_html_matrix_test_tfidf.shape)

predictions = []

for x in predicted_LGBM:
    predictions.append(np.argmax(x))
    
len(predictions)
print(f1_score(test_tag_lab, predictions, average='macro'))
confusion_matrix(test_tag_lab, predictions)

le.classes_
le.inverse_transform(predictions)


# In[ ]:

print(web_html_matrix_tfidf.shape)
#print(tf_matrix.shape)

from sklearn.naive_bayes import MultinomialNB
clf = MultinomialNB().fit(web_html_matrix_tfidf, web_train_sample_df2.Tag)

#  Test Data
tf_tst_matrix = LemVectorizer_ft.transform(web_test_sample_df2.Html.apply(lambda x: remove_html_tags(x)))
web_html_matrix_test_tfidf = tfidfTran.transform(tf_tst_matrix)
#dom_test_lab = dom_le_fit.transform(web_test_sample_df2['Domain'])
#web_html_matrix_test_tfidf2 = hstack((web_html_matrix_test_tfidf,dom_test_lab[:,None]))
#  Test Data

predicted = clf.predict(web_html_matrix_test_tfidf)

from sklearn.metrics import f1_score
print(f1_score(web_test_sample_df2.Tag, predicted, average='macro'))

from sklearn.metrics import confusion_matrix
confusion_matrix(web_test_sample_df2.Tag, predicted)


# In[ ]:

#import random
#random.shuffle( web_train_df[web_train_df["Tag"] == "news"]["Webpage_id"])


#web_html_data_df = pd.read_csv("E:\\Datathom\\inno2\\train\\html_data.csv", nrows=5)
fset_text1 = web_html_data_df["Html"][1]

#fset_text = fset_text1.replace('articleBody\"', ' articleBody\" id=\"kst_12345\"')
#fset = fset_text1.get_element_by_id("block-tektitegenericarticlebottomadblock")

fset_text = fset_text1.replace('\n', ' ')
print(fset_text1)

import re
fset_text1 = web_html_data_df["Html"][1]
fset_text = fset_text1.replace('\n', ' ')
regex = "<body(.+?)</body"

project_description2 = re.findall(regex, fset_text)

def remove_html_tags(text):
    """Remove html tags from a string"""
    import re
    clean = re.compile('<.*?>')         
    return re.sub(clean, ' ', text)

project_description =remove_html_tags(fset_text)


print(project_description2)

#train_web_id = web_train_sample_df["Webpage_id"].values
#print((train_web_id))


# In[ ]:

from sqlalchemy import create_engine
csv_database = create_engine('sqlite:///csv_database.db')
#web_html_data_df = pd.read_csv("E:\\Datathom\\inno2\\train\\html_data.csv")

chunksize = 1000
i = 0
j = 1
for df in pd.read_csv("E:\\Datathom\\inno2\\train\\html_data.csv", chunksize=chunksize, iterator=True):
    df = df.rename(columns={c: c.replace(' ', '') for c in df.columns}) 
    df.index += j
    i+=1
    df.to_sql('table', csv_database, if_exists='append')
    j = df.index[-1] + 1
        


# In[ ]:

#web_train_sample_df = web_train_sample_df.join(train_sam_html_df, on="Webpage_id", rsuffix="ht")
#web_test_sample_df = web_test_sample_df.join(test_sam_html_df, on="Webpage_id")
#web_train_sample_df2 = web_train_sample_df
#web_test_sample_df2 = web_test_sample_df

#train_sam_html_df = train_sam_html_df[["Webpage_id","Html"]].drop_duplicates()

#web_train_sample_df2 = web_train_sample_df2.join(train_sam_html_df[["Webpage_id","Html"]].set_index("Webpage_id"), on="Webpage_id", lsuffix="l", rsuffix="ht")
#web_test_sample_df2 = web_test_sample_df2.join(test_sam_html_df[["Webpage_id","Html"]].set_index("Webpage_id"), on="Webpage_id", lsuffix="l", rsuffix="ht")
#web_test_sample_df2 = web_test_sample_df2.join(test_sam_html_df[["Webpage_id","Html"]], on="Webpage_id", rsuffix="ht")


#print(train_sam_html_df.loc[train_sam_html_df["Webpage_id"] == 65123])
#train_sam_html_df[["Webpage_id","Html"]].head()
print(test_sam_html_df.shape)
print(web_test_sample_df2.shape)
print(train_sam_html_df.shape)
print(queryString_test)
#train_sam_html_df["Webpage_id"]


# In[ ]:

#web_train_sample_df2.Html[1].as_matrix()



#remove_html_tags(web_train_sample_df2.Html.as_matrix())
#type(web_train_sample_df2.Html(lambda x: remove_html_tags(x)))

#web_train_sample_df2.Html.apply(lambda x: remove_html_tags(x))
print(web_html_matrix_tfidf.shape)
print(web_train_sample_df2.shape)
#web_html_matrix_test_tfidf.shape

#from sklearn import preprocessing
#le = preprocessing.LabelEncoder()

#dom_le_fit = le.fit(web_train_sample_df2['Domain'])
#dom_lab = dom_le_fit.transform(web_train_sample_df2['Domain'])
#from scipy.sparse import hstack
#web_html_matrix_tfidf2 = hstack((web_html_matrix_tfidf,dom_lab[:,None]))

#web_html_matrix_tfidf2 = sparse.hstack((web_html_matrix_tfidf,web_train_sample_df2["Domain"].values)[:,None])
print(web_html_matrix_tfidf2.shape)
print(web_train_sample_df2.shape)
print(type(web_train_sample_df2["Domain"].values))
print(type(web_html_matrix_tfidf))

#web_train_sample_df2["Domain"].values
web_html_matrix_tfidf
dom_lab


# In[ ]:

type(predicted_LGBM2)
np.argmax(predicted_LGBM2[:1])

