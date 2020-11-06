#!/usr/bin/python
import os, sqlite3, sys
f1 = sys.argv[1]
f2 = sys.argv[2]

cache_fn='%s/.hist_dist_cache.db' % os.getenv("HOME")
db = sqlite3.connect(cache_fn)
dbc = db.cursor()
dbc.execute('CREATE TABLE IF NOT EXISTS distances (f1 TEXT, f2 TEXT, dist FLOAT)')
dbc.execute('CREATE INDEX IF NOT EXISTS distances_idx ON distances (f1, f2)')
dbc.execute('SELECT dist FROM distances WHERE f1 = ? AND f2 = ?',(f1, f2))
row = dbc.fetchone()
if row: 
	print row[0]
else:
	import cv2
	im1 = cv2.imread(f1)
	im2 = cv2.imread(f2)
	distlist = []
	for channel in range(3):
		hist1 = cv2.calcHist([im1],[channel],None,[256],[0,256])
		hist2 = cv2.calcHist([im2],[channel],None,[256],[0,256])
		distlist.append( cv2.compareHist(hist1,hist2,cv2.HISTCMP_BHATTACHARYYA) )
	dbc.execute('INSERT INTO distances (f1, f2, dist) VALUES (?,?,?)',(f1, f2, sum(distlist)))
	db.commit()
	print sum(distlist)
