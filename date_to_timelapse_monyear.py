#!/usr/bin/python
import Image,os,sys,string,time,ImageDraw,time,ImageFont
startDir=os.path.abspath(sys.argv[1]).rstrip('/')
getTime=lambda x : int(filter(lambda y : y in string.digits,os.path.basename(x)))

def load_font(fontSize):
 """Load the arial TTF font from the default location for gentoo and debian linux"""
 f1='/usr/share/fonts/corefonts/arialbd.ttf' 
 f2='/usr/share/fonts/truetype/msttcorefonts/Arial_Bold.ttf'
 if os.path.isfile(f1): font=ImageFont.truetype(f1,fontSize)
 if os.path.isfile(f2): font=ImageFont.truetype(f2,fontSize)
 return font

def draw_date(img,txs):
 """Write date and time in the image"""
 fontSize=img.size[1]/20
 draw = ImageDraw.Draw(img) # new draw instance 
 arial=load_font(fontSize)
 tsize=draw.textsize(txs,font=arial)
 #draw.text((img.size[0]-tsize[0]-10,img.size[1]-(fontSize+5)),txs,fill=(200,200,255),font=arial)
 draw.text((10,img.size[1]-(fontSize+5)),txs,fill=(50,75,255),font=arial)
 return img

start_time=time.time()
is_jpeg=lambda x : x.endswith('.jpg')
photos=sorted(filter(is_jpeg,os.listdir(startDir)))
if '_' in photos[0]: getTime=lambda x : int(time.mktime(time.strptime(x, '%Y%m%d_%H%M%S')))
else: getTime=lambda x : int(filter(lambda y : y in string.digits,os.path.basename(x)))
for f in photos:
 fn='%s/%s' % (startDir,f)
 t=getTime(os.path.splitext(f)[0])
 #txs=time.strftime('%Y-%m-%d %H:%M',time.gmtime(t))
 txs=time.strftime('%Y-%m',time.localtime(t))
 i=Image.open(fn)
 i2=draw_date(i,txs)
 i2.save(fn)
total_time=time.time() - start_time
print 'date_to_timelapse: %d images in %f seconds' % (len(photos),total_time)

 
