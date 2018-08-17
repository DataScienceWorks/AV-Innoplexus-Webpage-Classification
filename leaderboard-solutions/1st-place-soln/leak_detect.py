import pandas as pd
import numpy as np
from sklearn.metrics import f1_score

train = pd.read_csv('../data/train/train.csv')
test = pd.read_csv('../data/test_nvPHrOx.csv')

avg_f1_score = []
for seed_value in range(10):
    print 'Seed value = {}'.format(seed_value)
    validation_lst = []
    for tag, domain in train.groupby('Tag')['Domain']:
        condition1 = (train['Tag'] == tag)
        condition2 = (train['Domain'].isin(domain.sample(frac=1., random_state=seed_value).unique()[2::3]))
        validation_lst.append(train[condition1 & condition2])

    validation_set = pd.concat(validation_lst)
    actual = validation_set['Tag'].values
    validation_set.drop('Tag', axis=1, inplace=True)
    training_set = train[~train['Webpage_id'].isin(validation_set['Webpage_id'])]
    
    combine = training_set.append(validation_set, sort=False)
    combine.sort_values('Webpage_id', inplace=True)
    combine['Tag'] = combine['Tag'].bfill().ffill()
    predicted = combine.loc[validation_set.index, 'Tag']
    cv_score = f1_score(actual, predicted, average='weighted')
    print 'Weighted F1 score {:.4f}\n'.format(cv_score)
    avg_f1_score.append(cv_score)
    
print '[CV] Avg. F1 score {:.4f}'.format(np.mean(avg_f1_score))