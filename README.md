# ultrafast_timelapse
Timelapse with fewer frames than days in source images.
For example: You have 500000 images taken at regular intervals in a period of 8 years and want to create a timelapse of 10 seconds. At 25fps this is 250 images.
Why not just randomly sample the required images? Because this will not be a "smooth" timelapse, because some images are more similar, mostly because of the weather and illumination.
One solution is to divide the images into packets and pick an initial image and search for the most similar in the first packet, then find the most similar in the next packet, and so on.
This eliminates the need to compare (determine some sort of distance) all images to every image, which would take too long, 
but would enable finding the optimal timelapse as a travelling salesman problem. 
Ideally, days could be used as packets for shorter timespans, but 8 years is 2920 days, which is a lot more than the 250 required images. 
So this program tries to find the optimal packet size (in number of days) and the number of images required from each packet,

Images have to be named as the unixtime of their creation (1493722274.jpg) or date_time (20190221_114151.jpg).
