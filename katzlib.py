month_abbreviations='JFMAYULGSOND'

def scan_directory(indir, cache=False):
 """Returns list of all files with path in a directory recursively."""
 import os
 d=indir
 if cache:
  import sqlite3
  cache_fn='%s/.nfs_directory_cache.db' % os.getenv("HOME")
  db = sqlite3.connect(cache_fn)
  dbc = db.cursor()
  dbc.execute('CREATE TABLE IF NOT EXISTS dirs (n TEXT, i INTEGER PRIMARY KEY AUTOINCREMENT)')
  dbc.execute('CREATE TABLE IF NOT EXISTS files (i INTEGER, f TEXT)')
  dbc.execute('CREATE INDEX IF NOT EXISTS files_idx ON files (i)')
  dbc.execute('SELECT i FROM dirs WHERE n = ?',(indir,))
  row=dbc.fetchone()
  if row:
   dbc.execute('SELECT f FROM files WHERE i = ? ORDER BY f',row)
   return zip(*dbc.fetchall())[0]
 d='%s/' % d.rstrip('/')
 rl=[]
 for fn in os.listdir(d):
  if os.path.isdir(d+fn): rl+=scan_directory(d+fn+'/')
  else: rl.append(d+fn)
 if not cache: return sorted(rl)
 import sqlite3
 dbc.execute('INSERT INTO dirs (n) VALUES (?)',(indir,))
 dbc.execute('SELECT last_insert_rowid() FROM dirs')
 i,=dbc.fetchone()
 for f in rl: 
  dbc.execute('INSERT INTO files (i,f) VALUES (?,?)',(i,f.decode('utf8')))
 db.commit()
 return sorted(rl)

def firstocc(inlist):
 """Return first occurrences of item in a list."""
 outlist,last=[inlist[0]],inlist[0]
 for i in inlist:
  if last!=i: outlist.append(i)
  last=i
 return outlist

def firstocc_index(inlist):
 """Return index of first occurrences of item in a list."""
 outlist,last=[],inlist[0]
 for n,i in enumerate(inlist):
  if last!=i: outlist.append(n)
  last=i
 return outlist

def create_month_score_matrix():
 """Creates a scoring matrix for aligning months."""
 match={}
 size=12
 for i in range(size):
  for j in range(size):
   key=month_abbreviations[i], month_abbreviations[j]
   score=size/2-abs(i-j)
   match[key]=score
 return match

def letter_encode(x):
 """Encodes month number as abbreviation letter."""
 return month_abbreviations[x-1]

def nicename(x):
 """Returns first 10 letters of basename of a path truncated to the first underscore."""
 import os
 return os.path.basename(x).split('_')[0][:10]

def fillstr(x):
 """Returns string filled up with spaces to 10 letters."""
 return '%s: %s' % (x,(10-len(x))*' ')

def utime(x):
 """Converts to int from filename without extension."""
 import os
 return int(os.path.splitext(os.path.basename(x))[0])

def prepare_outdir(odir,remove=True):
 import os
 if os.path.isdir(odir):
  if remove:
   oldfiles=scan_directory(odir)
   if oldfiles: print 'Removing %d old files from %s...' % (len(oldfiles),odir)
   for f in oldfiles: os.unlink(f)
 else: os.mkdir(odir)

# Find similar images using SURF compare from openCV2
cache={} # Cache for the surfDescriptors of the reference images

def cached_key2(ifn,key):
 global cache
 if cache.has_key(key): return cache[key]
 import cv2
 img=cv2.imread(ifn)
 surfDetector = cv2.FeatureDetector_create("SURF")
 surfDescriptorExtractor = cv2.DescriptorExtractor_create("SURF")
 keypoints = surfDetector.detect(img)
 kp, descriptors=surfDescriptorExtractor.compute(img,keypoints)
 cache[key] = descriptors
 return cache[key]

def cached_key1(ifn,key):
 global cache
 if cache.has_key(key): return cache[key]
 import cv2
 img = cv2.imread(ifn)
 surfDetector = cv2.FeatureDetector_create("SURF")
 surfDescriptorExtractor = cv2.DescriptorExtractor_create("SURF")
 keypoints = surfDetector.detect(img)
 kp, descriptors = surfDescriptorExtractor.compute(img,keypoints)
 cache[key] = kp, descriptors
 return cache[key]

def surf_compare(ifn1,ifn2,key1):
 import cv2,numpy
 kp, descriptors = cached_key1(ifn1,key1)
 samples = numpy.array(descriptors)
 responses = numpy.arange(len(kp),dtype = numpy.float32)
 knn = cv2.KNearest() # kNN training
 try: knn.train(samples,responses)
 except: return 0,0
 desc = cached_key2(ifn2,ifn2)
 ok=0
 for h,des in enumerate(desc):
  des = numpy.array(des,numpy.float32).reshape((1,128))
  retval, results, neigh_resp, dists = knn.find_nearest(des,1)
  res,dist =  int(results[0][0]),dists[0][0]
  if dist<0.1: ok+=1
 return float(ok)/float(len(desc))

def packets(values, pack_len):
 return [values[x : x + pack_len] for x in range(0, len(values), pack_len)]

def logg(x,log_fn):
 import time
 open(log_fn,'a').write('%s: %s\n' % (time.strftime('%Y-%m-%d %H:%M'),x))
 print x

def packet_average(values, pack_len):
 import numpy
 packs=[values[x : x + pack_len] for x in range(0, len(values), pack_len)]
 return map(lambda x : numpy.mean(x), packs)

def dhms(s): 
 """Converts seconds to days, hours, minutes, seconds"""
 n=60; d=int(s/n/n/24); s-=d*n*n*24; h=int(s/n/n); s-=h*n*n; m=int(s/n); s-=m*n; return d,h,m,s

def adaptive_time(x):
 labels='days','hours','min','sec'
 l=[]
 for k,v in zip(labels,dhms(x)):
  if v: l.append('%d %s' % (v,k))
 return ', '.join(l)

