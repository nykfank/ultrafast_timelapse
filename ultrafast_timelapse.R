args <- commandArgs(trailingOnly=TRUE)
if (!exists("indir")) indir <- args[1]
if (!exists("filtermode")) filtermode <- args[2] # full / summer
if (!exists("nb_seconds")) nb_seconds <- as.integer(args[3]) # 10

nb_frames <- 25 * nb_seconds
noon_hour <- 13
camID <- basename(indir)
outdir <- sprintf("tl_%s_%s", filtermode, camID)
writeLines(sprintf("Outdir: %s (%d seconds)", outdir, nb_seconds))
dir.create(outdir, showWarnings = FALSE)
images <- data.frame(datei = list.files(indir, pattern="jpg"), stringsAsFactors=FALSE)
images$pfad <- sprintf("%s/%s", indir, images$datei)
images$filesize <- file.info(images$pfad)$size
if (sum(grepl("_", images$datei)) > 0) {
	images$timestamp <- as.POSIXct(sub("netcam", "", images$datei), tz="", "%Y%m%d_%H%M%S")
	images$unixtime <- as.numeric(images$timestamp)
} else {
	images$unixtime <- as.integer(sub(".jpg", "", images$datei))
	images$timestamp <- as.POSIXct(images$unixtime, origin="1970-01-01")
}
images$date <- as.Date(images$timestamp)
images$stunde <- as.numeric(format(images$timestamp, "%H")) + as.numeric(format(images$timestamp, "%M")) / 60
images$jahr <- as.numeric(format(images$timestamp, "%Y"))
images$monat <- as.numeric(format(images$timestamp, "%m"))
images$midiff <- abs(noon_hour - images$stunde)
images <- images[images$date < Sys.Date(),] # Images from the future are not plausible
if (filtermode == "summer") subimg <- images[images$monat %in% 4:10 & images$midiff < 2,]
if (filtermode == "noon") subimg <- images[images$midiff < 2,]
if (filtermode == "none") subimg <- images
writeLines(sprintf("images = %d, days = %d, filtered_images = %d, filtered_days = %d", nrow(images), length(unique(images$date)), nrow(subimg), length(unique(subimg$date))))
print(table(images$jahr))
days <- sort(unique(subimg$date))
nb_days <- length(days)

# Determine number of blocks

suitable_divisors <- function(number) {
	blocksize <- numbers::divisors(nb_days)
	blocksize[blocksize > 2 & blocksize < 30]
}

blocksize <- suitable_divisors(nb_days)
while (length(blocksize) == 0) {
	nb_days <- nb_days - 1
	blocksize <- suitable_divisors(nb_days)
}
blocksize <- max(blocksize)
nb_blocks <- nb_days / blocksize

# Remove days with least number of images
if (nb_days != length(days)) {
	dayocc <- as.data.frame.table(table(subimg$date))
	dayocc <- dayocc[order(dayocc$Freq),]
	nb_remdays <- length(days) - nb_days
	remdays <- as.Date(dayocc[1:nb_remdays, "Var1"])
	nb_remimgs <- sum(dayocc[1:nb_remdays, "Freq"])
	subimg <- subimg[!subimg$date %in% remdays,]
	days <- sort(unique(subimg$date))
	writeLines(sprintf("Removed %d days containing %d images", nb_remdays, nb_remimgs))
}

# Determine number of images per block
imgs_per_block <- round(nb_frames / nb_blocks)
if (imgs_per_block < 1) imgs_per_block <- 1
nb_frames_new <- nb_blocks * imgs_per_block
writeLines(sprintf("nb_days = %d, blocksize = %d days, nb_blocks = %d, nb_frames_target = %d, nb_frames_real = %d, imgs_per_block = %d", nb_days, blocksize, nb_blocks, nb_frames, nb_frames_new, imgs_per_block))

# Select largest image in first block as initial frame.
subimg <- subimg[order(subimg$unixtime),]
seldays <- days[(0*blocksize+1):(0*blocksize+blocksize)]
selimgs <- subimg[subimg$date %in% seldays,]
selimgs <- selimgs[order(selimgs$filesize, decreasing=TRUE),]
selimgs <- selimgs[selimgs$filesize >= mean(selimgs$filesize),] # optional
init_img <- selimgs[1, "pfad"]
writeLines(sprintf("Initial image: %s", init_img))
subimg[subimg$pfad == init_img, "Auswahl"] <- TRUE
writeLines(sprintf("First pack: %d images", nrow(selimgs)))
for (i in 2:nrow(selimgs)) {
	cmd <- sprintf("image_histogram_distance_rgb.py %s %s", init_img, selimgs[i, "pfad"])
	d <- as.numeric(system(cmd, intern=TRUE))
	if (length(d) == 1) selimgs[i, "distance"] <- d
	#writeLines(sprintf("%d/%d: %f", i, nrow(selimgs), selimgs[i, "distance"]))
}

# Select n with lowest distance to initial frame.
selimgs <- selimgs[order(selimgs$distance),]
top_imgs <- selimgs[1:(imgs_per_block-1), "unixtime"]
subimg[subimg$unixtime %in% top_imgs, "Auswahl"] <- TRUE
last_img <- tail(subimg[!is.na(subimg$Auswahl), "pfad"], n=1)

# Copy files
for (filename in subimg[subimg$Auswahl == TRUE, "datei"]) {
	f1 <- sprintf("%s/%s", indir, filename)
	f2 <- sprintf("%s/%s", outdir, filename)
	if (!file.exists(f2)) file.copy(f1, f2)
}

# Loop:	Select n with lowest distance to sequentially last selected of previous block.
for (packi in 1:(nb_blocks-1)) {
	seldays <- days[(packi*blocksize+1):(packi*blocksize+blocksize)]
	selimgs <- subimg[subimg$date %in% seldays,]
	selimgs <- selimgs[selimgs$filesize >= mean(selimgs$filesize),] # optional
	if (sum(selimgs$Auswahl, na.rm=TRUE) > 0) {
		last_img <- tail(selimgs[!is.na(selimgs$Auswahl), "pfad"], n=1)
		next
	}
	writeLines(sprintf("%d/%d: %d images", packi, nb_blocks-1, nrow(selimgs)))
	for (i in 1:nrow(selimgs)) {
		cmd <- sprintf("image_histogram_distance_rgb.py %s %s", last_img, selimgs[i, "pfad"])
		d <- as.numeric(system(cmd, intern=TRUE))
		if (length(d) == 1) selimgs[i, "distance"] <- d
		#writeLines(sprintf("%d/%d -> %d/%d: %f", packi, nb_blocks-1, i, nrow(selimgs), selimgs[i, "distance"]))
	}
	selimgs <- selimgs[order(selimgs$distance),]
	top_imgs <- selimgs[1:imgs_per_block, "unixtime"]
	subimg[subimg$unixtime %in% top_imgs, "Auswahl"] <- TRUE
	last_img <- tail(subimg[!is.na(subimg$Auswahl), "pfad"], n=1)
	# Copy files
	for (filename in subimg[subimg$Auswahl == TRUE, "datei"]) {
		f1 <- sprintf("%s/%s", indir, filename)
		f2 <- sprintf("%s/%s", outdir, filename)
		if (!file.exists(f2)) file.copy(f1, f2)
	}
}

cmd <- sprintf("median_images.py %s 3", outdir)
writeLines(cmd)
system(cmd)

cmd <- sprintf("date_to_timelapse_monyear.py %s_median3", outdir)
writeLines(cmd)
system(cmd)

cmd <- sprintf("ffmpeg -hide_banner -loglevel panic -y -framerate 25 -pattern_type glob -i '%s_median3/*.jpg' -codec:v libx264 -crf 18 -profile:v main %s.mp4", outdir, outdir)
writeLines(cmd)
system(cmd)

# 1280x960 -> 960x540: 320 420 1280 960
# crop_images.py /mnt/big/nick/cams/lauterbrunnen 320 420 1280 960
# crop_images.py /mnt/big/nick/cams/scheidegg 160 150 1120 690
