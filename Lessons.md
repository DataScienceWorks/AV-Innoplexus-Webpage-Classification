# Lessons

* Problem Type : Supervised Text Classification Problem.

* Similar Problem Type with smaller dataset: [The 20 Newsgroup Dataset](https://www.kaggle.com/c/20-newsgroups). This comes as part of [scikit-learn package/library](http://scikit-learn.org/stable/datasets/twenty_newsgroups.html). 

* Install Github's [git-lfs](https://git-lfs.github.com/) for managing the versioning of large files. The train.zip file is of size 1.3 GB.

* The repository limit for files in LFS, is strictly 1GB, either in Github or Bitbucket. So this option of using git-lfs doesn't come to rescue in this use-case. Thus, copied the train.zip to OneDrive for future reference.

* `train.csv` has columns `Webpage_id, Domain, Url, Tag`
  `html_data.csv` has columns `Webpage_id, Html`

  * Should we merge these files into one?? The `html_data.csv` file size is ~7 GB.

  * For each id in html_data.csv, we should read the Html feature, classify it to one of the labels 9 predefined labels and add it to a new column titled `Tag`

  * But wait, even before  that, we need to split the train-dataset in train/test datasets per the problem statement.

  * Loading large CSV file (in the order of GBs) into memory is undoable business. So we load them in chunks in pandas and write it to RDBMS, for doing data munging later on it. Refer [Working with large CSV files in Python](http://pythondata.com/working-large-csv-files-python/) for more. Alternatively, you can also refer to [Dask – A better way to work with large CSV files in Python](http://pythondata.com/dask-large-csv-python/).

    ```python
    import pandas as pd
    from sqlalchemy import create_engine
    
    # To understand the structure of the csv file and make sure the data is formatted in a way that makes sense for your work.
    file = '/path/to/csv/file'
    print pd.read_csv(file, nrows=5) # to read in only 5 rows
    
    # Before we can actually work with the data, we need to do something with it so we can begin to filter it to work with subsets of the data.
    # But with large data files, we need to store the data somewhere else. In this case, we’ll set up a local sqllite database, read the csv file in chunks and then write those chunks to sqllite.
    csv_database = create_engine('sqlite:///csv_database.db')
    
    # Iterate through the CSV file in chunks and store the data into sqllite
    chunksize = 100000
    i = 0
    j = 1
    for df in pd.read_csv(file, chunksize=chunksize, iterator=True):
          df = df.rename(columns={c: c.replace(' ', '') for c in df.columns}) 
          df.index += j
          i+=1
          df.to_sql('table', csv_database, if_exists='append')
          j = df.index[-1] + 1
            
    # To access the data now, you can run commands like the following:
    # df = pd.read_sql_query('SELECT * FROM table', csv_database)
    
    # Of course, using ‘select *…’ will load all data into memory, which is the problem we are trying to get away from so you should throw from filters into your select statements to filter the data. For example:
    df = pd.read_sql_query('SELECT COl1, COL2 FROM table where COL1 = SOMEVALUE', csv_database)
    ```

* 



### Git LFS

* Good reads Atlassian Docs  - [Tutorials Git LFS](https://www.atlassian.com/git/tutorials/git-lfs) and [Git LFS with  Bitbucket](https://confluence.atlassian.com/bitbucket/git-large-file-storage-in-bitbucket-829078514.html).

* Features

  * Versioning : Version large files—even those as large as a couple GB in size—with Git. 
  * More repo space: Host more in your Git repositories. External file storage makes it easy to keep your repository at a manageable size. 
  * Faster cloning and fetching
  * Same git flow
  * Same access controls and permissions

* Getting started

  * To create a new Git LFS aware repository, you'll need to run `git lfs install` after you create the repository. You only have to setup Git LFS only once. This installs a special `pre-push` [Git hook](https://www.atlassian.com/git/tutorials/git-hooks) in your repository that will transfer Git LFS files to the server when you `git push`. 
    Note: Git LFS is automatically enabled for all [Bitbucket Cloud](https://bitbucket.org/)repositories. 

  * Select the file types you'd like Git LFS to manage (or directly edit  your .gitattributes). You can configure additional file extensions at  anytime with `git lfs track "*.zip"`. 

    Note: Make sure .gitattributes is tracked with `git add .gitattributes`.

  * Just commit and push to GitHub as you normally would. Yay!