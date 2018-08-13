import urllib2
from BeautifulSoup import BeautifulSoup
import time
start = time.time()

import tkFileDialog
from html.parser import HTMLParser
import pandas as pd

train_path_string = r'C:/Users/ROHIT/Desktop/Graphs/Analytics_Vidhya/train/train.csv'
train_df = pd.read_csv(train_path_string)
feature_df = train_df
tags = {'clinicalTrials': 0,'conferences': 1, 'forum': 2, 'guidelines': 3, 'news': 4, 'others': 5, 'profile': 6, 'publication': 7, 'thesis': 8}

train_df.Tag = [tags[item] for item in train_df.Tag]
def buildFeatures(df):
    isOrg = list()
    isArticle = list()
    isPDF = list()
    isBLOG = list()
    isNEWS = list()
    hasGuide = list()
    isEDU = list()
    isClinical = list()
    long = list()
    isForum = list()
    for i,row in df.iterrows():
        domain = row['Domain'].lower()
        url = row['Url']
        long.append(len(url))
        if ".org" in domain:
            isOrg.append(1)
        else:
            isOrg.append(0)

        if "clinical" in domain:
            isClinical.append(1)
        else:
            isClinical.append(0)

        if "article" in url:
            isArticle.append(1)
        else:
            isArticle.append(0)

        if "pdf" in url:
            isPDF.append(1)
        else:
            isPDF.append(0)

        if "blog" in domain:
            isBLOG.append(1)
        else:
            isBLOG.append(0)

        if ".edu" in domain:
            isEDU.append(1)
        else:
            isEDU.append(0)

        if "news" in url:
            isNEWS.append(1)
        else:
            isNEWS.append(0)

        if "guide" in url:
            hasGuide.append(1)
        else:
            hasGuide.append(0)

        if "forum" in url:
            isForum.append(1)
        else:
            isForum.append(0)

    df['isOrg'] = isOrg
    df['isArticle'] = isArticle
    df['isPDF'] = isPDF
    df['isBLOG'] = isBLOG
    df['isNEWS'] = isNEWS
    df['hasGuide'] = hasGuide
    df['isEDU'] = isEDU
    df['isClinical'] = isClinical
    df['how_long'] = long
    df['isForum'] = isForum
    return df

feature_df = buildFeatures(train_df)
feature_df.drop(['Webpage_id', 'Domain', 'Url'], axis = 1, inplace = 1)

print feature_df.columns

X_train=feature_df[['isOrg', 'isArticle', 'isPDF', 'isBLOG', 'isNEWS', 'hasGuide', 'isEDU', 'isClinical', 'how_long', 'isForum']]  # Features
y_train=feature_df['Tag']  # Labels


from sklearn.ensemble import RandomForestClassifier
clf=RandomForestClassifier(n_estimators=200)

#Train the model using the training sets y_pred=clf.predict(X_test)
clf.fit(X_train,y_train)

test_path_string = r'C:/Users/ROHIT/Desktop/Graphs/Analytics_Vidhya/test_nvPHrOx.csv'
test_df = pd.read_csv(test_path_string)
feature_df = buildFeatures(test_df)
print test_df.columns
webPageIDList = test_df['Webpage_id'].tolist()
DomainList = test_df['Domain'].tolist()
UrlList = test_df['Url'].tolist()
feature_df.drop(['Webpage_id', 'Domain', 'Url'], axis = 1, inplace = 1)
print test_df.columns
X_test=feature_df[['isOrg', 'isArticle', 'isPDF', 'isBLOG', 'isNEWS', 'hasGuide', 'isEDU', 'isClinical', 'how_long', 'isForum']]  # Features
#y_test=feature_df['Tag']  # Labels
y_pred=clf.predict(X_test)
tags = {0 : 'clinicalTrials',1 : 'conferences', 2 : 'forum', 3 : 'guidelines',4: 'news', 5 :'others', 6 :'profile', 7: 'publication',8: 'thesis'}
tagList = list()
for item in y_pred:
    item = int(item)
    tagList.append(tags[item])
test_df['Webpage_id'] = webPageIDList
test_df['Tag'] = tagList
test_df.drop(['isOrg', 'isArticle', 'isPDF', 'isBLOG', 'isNEWS', 'hasGuide', 'isEDU', 'isClinical', 'how_long', 'isForum'], axis = 1, inplace = 1)
test_df.set_index(['Webpage_id'], inplace=True)
test_df.to_csv('Test.csv')


print 'It took', time.time()-start, 'seconds.'