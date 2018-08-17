

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

* Bad assumptions and poor skills in Python led me to assume that the the Webpage_Id(s) from the test dataset doesn't exist in the html dataset. And because of this I ended up attempting to web-scrape all the URLs from test-dataset. This is what happens when you work too hard and don't take a step back, every now and then. Shame on me!

* My attempt to crawling Internet to read and persist the HTML page:

  ```python
  # OBJECTIVE: Crawl Internet to get web-page for persistence
  import requests
  from requests.adapters import HTTPAdapter
  from requests.packages.urllib3.util.retry import Retry
  from requests.packages.urllib3.util import make_headers
  import sys, traceback
  import pycurl
  from io import BytesIO
  
  
  import urllib3
  urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning) #When session.verify = False
  
  def curl_page(url):
      retry = Retry(connect=3, backoff_factor=0.5, total=10, redirect=5)
      adapter = HTTPAdapter(max_retries=retry)
      session = requests.Session()    
      session.mount('http://', adapter)
      session.mount('https://', adapter)
      session.verify = False # No more "SSL Error because of Bad Handshake on account of failure in certificate verification"    
      try:
          page = session.get(url)
          return page.content.strip()
      except Exception as err:
          print('Error accessing the URL:',url)
  #         traceback.print_exc()
          return curl_page2(url)
  
  # This method is called by curl_page() when exception raises
  def curl_page2(url):
      try:
          buffer = BytesIO()
          c = pycurl.Curl()
          c.setopt(c.URL, url)
          c.setopt(c.WRITEDATA, buffer)
          c.perform()
          c.close()
          body = buffer.getvalue()
          buffer.close()
          return body.strip()
      except Exception as ex:
          print('Error again, accessing the URL:',url)
          print(str(ex))
          return None
  
  # url = 'https://www.zacks.com/stock/news/253595/walmart-launches-new-venture-to-boost-ecommerce-business?cid=CS-NASDAQ-FT-253595'
  # curl_page2(url)
  
  # For every row in the dataframe, pick the URL, crawlthe web and persist it in DB
  for idx,row in testdf.iterrows():
      if(idx < 2590): continue # Last HTTTP Maxout error at 928
  #     if(idx >1000): break #For Testing purpose    
      conn = db_engine.connect() # Auto-commit = True, implicitly
      print('Crawling web-page for id:',idx)
      page = curl_page(row.Url)    
      try:        
          if(page is not None):
              conn.execute("UPDATE testdf SET Html=? WHERE Webpage_id=?", page,idx)            
      except Exception as err:
          print('Error updating record with id=',idx, "\nMore details:\t",err)
      finally:
          conn.close()
  print('Done!')
  ```

  * Issues with crawling URLs to get the web-page using the above method.
    * Not all are well-formed. 
    * There exists invalid URLs.
    * The host or web-site could be down/moved, making the URL void. Example [URL](http://investor.ariad.com/phoenix.zhtml?c=118422&p=RssLanding&cat=news&id=2246696).
    * The connection could be https. Now that means verification of certification validity is required ideally. In my case this was causing issues with some URLs, so had to configure to skip this certification verification and suppress API warnings asking me to do verify certificate.
    * Connection time-outs

* When working in a laptop, there are constraints of Memory (you can't load ~7GB of CSV data as pandas DataFrame - Jupyter Notebook crashes!). Cloud is the answer.

* Extracting domains from the URL is such hard stuff. This can however be made simple with the use of external library for this purpose like below:

  ```python
  import tldextract
  
  def extract_domain(url):
      return tldextract.extract(url).domain
  
  df.Domain = df.Domain.apply(extract_domain)
  df.Domain.value_counts().sort_index() # show in alphabetical order
  ```

* Extracting **title** from web-page like below:

  ```python
  def extract_title(page):
      if (page == None): 
          return None
      soup = BeautifulSoup(page, 'html.parser')
      title_tag = soup.find('title')
      if (title_tag == None):
          title = None
      else:
          title = title_tag.text.strip()
      return title
  
  #Test method definition
  print(extract_title("<html></html>"))
  print(extract_title(webpage))
  ```

* Extracting all text within text-container HTML tags like below:

  ```python
  import re
  from bs4 import BeautifulSoup
  from bs4.element import Comment
  
  def is_visible_content(element):
      if element.parent.name in ['style', 'script', 'head', 'title', 'meta', '[document]']:
          return False
      if isinstance(element, Comment):
          return False
      return True
  
  def remove_extra_spaces(str):
      return u" ".join(str.split())
  
  def extract_text(page):
      if (page == None): 
          return None
      soup = BeautifulSoup(page, 'html.parser') #, from_encoding="utf-8"
      texts = soup.findAll(text=True) # Extracts text from all HTML Markups, incl nested ones
      visible_texts = filter(is_visible_content, texts)
      # The u-prefix u" ".join() indicates Unicode and has been in python since v2.0
      # Ref. Read: https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/
      text = u" ".join(remove_extra_spaces(t.strip()) for t in visible_texts)
      text = text.replace(',','')
      text = text.replace('|','')
      text = re.sub(r'\s\s+',' ',text).strip()
      return text.encode('utf-8',errors='ignore').decode('utf-8').strip()
  
  text = extract_text(hdf.head(1)['Html'].values[0])
  text
  ```

* [Quora - How can I extract only text data from HTML pages?](https://www.quora.com/How-can-I-extract-only-text-data-from-HTML-pages)

* Merge columns of data-frames like below:

  ```python
  # OBJECTIVE : Merge the Title column of pdf dataframe into the df dataframe
  
  # A little code play for trying it out below:
  raw_data = {
      'subject_id': [11, 12, 13, 14, 15],
      'first_name': ['Alex', 'Amy', 'Allen', 'Alice', 'Ayoung'], 
      'last_name': ['Anderson', 'Ackerman', 'Ali', 'Aoni', 'Atiches']
  }
  df_a = pd.DataFrame(raw_data, columns = ['subject_id','first_name', 'last_name'])
  df_a.set_index('subject_id', inplace=True)
  df_a
  
  raw_data = {
          'subject_id': [11, 12, 13, 14, 15, 17, 18, 19, 20, 21],
          'test_id': [51, 15, 15, 61, 16, 14, 15, 1, 61, 16]
  }
  df_b = pd.DataFrame(raw_data, columns = ['subject_id','test_id'])
  df_b = df_b.set_index('subject_id')
  df_b
  
  pd.merge(df_a,df_b,on='subject_id')
  # df_a.merge(df_b, on='subject_id')
  # df_a.merge(df_b,how='inner', left_index=True, right_index=True)
  # df_a.merge(df_b,how='left', left_index=True, right_index=True)
  # df_a.merge(df_b,how='right', left_index=True, right_index=True)
  # df_a.merge(df_b,how='outer', left_index=True, right_index=True)
  # pd.concat([df_a,df_b], axis=1)
  ```

* [Joel On Software - The Absolute Minimum Every Software Developer Absolutely, Positively Must Know About Unicode and Character Sets (No Excuses!)](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/)

  * When non-english characters are extracted from HTML web-pages and later attempting to persist it to DB, the driver throws an error saying like: 
  * The solution to this is to use the `u` as prefix to string operation like `u''.join(..)` so that the characters are read as Unicode and is persisted the same way.



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