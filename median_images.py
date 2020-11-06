#!/usr/bin/python
# 2011-10-26: using imgtools module
# 2014-03-24: user katzlib, added path_remap to use high resolution images for rotaZoom

import sys, os, time, imgtools, katzlib
data_dir = os.path.abspath(sys.argv[1]).rstrip('/') + '/' # input directory
num_hdr = int(sys.argv[2]) # default 3
do_remap = 0
if len(sys.argv)>3: do_remap = int(sys.argv[3])
out_dir = '%s_median%d' % (data_dir.rstrip('/'),num_hdr)
if do_remap: out_dir += 'high'
if not os.path.isdir(out_dir): os.mkdir(out_dir)

def sliding(inlist,pack_len):
 packs = [inlist[x:x+pack_len] for x in range(0,len(inlist))]
 return filter(lambda x : len(x)==pack_len,packs)
 
def path_remap(x):
 """2014-03-24: scheidegg_crop_scaled_ok_smooth20_median3 -> scheidegg"""
 rl = []
 b = os.path.basename(x)
 for i in os.path.dirname(x).split('_'):
  ok = True
  for j in ('crop', 'scaled', 'ok', 'smooth', 'median'):
   if j in i: ok = False
  if ok: rl.append(i)
 y = '%s/%s' % ('_'.join(rl), b)
 return y
 
errors = ['', 'output exists', 'unable to open', 'not jpeg']
selected = map(str, katzlib.scan_directory(data_dir))
if do_remap: selected = map(path_remap, selected)
start_time = time.time()
for pack in sliding(selected,num_hdr):
 bfn = os.path.splitext(os.path.basename(pack[0]))[0]
 jpg_fn = '%s/%s.jpg' % (out_dir,bfn)
 if os.path.isfile(jpg_fn): continue
 filelist = pack + [jpg_fn]
 r = imgtools.median_merge(len(filelist),filelist)
 if r: print 'ERROR %s: %s' % (errors[int(r)],', '.join(filelist))
total_time = time.time() - start_time
print 'median_images: %d images in %f seconds' % (len(selected),total_time)
